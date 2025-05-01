#include <MintPhysics/Include/PhysicsWorld.h>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/Queue.hpp>
#include <MintContainer/Include/HashMap.hpp>
#include <MintContainer/Include/Color.h>
#include <MintContainer/Include/Algorithm.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintRenderingBase/Include/ShapeRenderer.h>
#include <MintPhysics/Include/Intersection.hpp>


namespace mint
{
	namespace Physics
	{
#pragma region Body2D
		Body2D::Body2D()
			: _bodyMotionType{ BodyMotionType::Static }
			, _angularVelocity{ 0.0f }
			, _angularAcceleration{ 0.0f }
		{
			__noop;
		}

		Body2D::~Body2D()
		{
			__noop;
		}

		bool Body2D::IsValid() const
		{
			return _bodyID.IsValid();
		}
#pragma endregion

#pragma region WorldHistory
		void WorldHistory::BeingPlaying()
		{
			_currentSnapshotIndex = 0;
		}

		void WorldHistory::EndPlaying()
		{
			_currentSnapshotIndex = kInvalidIndexUint32;
		}

		void WorldHistory::StepPlay(bool isForward)
		{
			if (IsPlaying() == false)
			{
				return;
			}

			if (isForward)
			{
				++_currentSnapshotIndex;
				if (_currentSnapshotIndex >= _stepSnapshots.Size())
				{
					_currentSnapshotIndex = 0;
				}
			}
			else
			{
				if (_currentSnapshotIndex > 0)
				{
					--_currentSnapshotIndex;
				}
				else
				{
					_currentSnapshotIndex = _stepSnapshots.Size() - 1;
				}
			}
		}

		const StepSnapshot& WorldHistory::GetStepSnapshot() const
		{
			return _stepSnapshots.Get(_currentSnapshotIndex);
		}
#pragma endregion

#pragma region World
		World::World()
			: _worldMin{ -2048.0, -2048.0f }
			, _worldMax{ +2048.0, +2048.0f }
			, _totalStepCount{ 0 }
		{
			_collisionSectorSideCount = kCollisionSectorTessellationPerSide;
			for (uint32 i = 0; i < _collisionSectorDepth; ++i)
			{
				_collisionSectorSideCount *= kCollisionSectorTessellationPerSide;
			}

			const uint32 collisionSectorBufferSize = _collisionSectorSideCount * _collisionSectorSideCount;
			_collisionSectors.Resize(collisionSectorBufferSize);
			for (uint32 i = 0; i < collisionSectorBufferSize; ++i)
			{
				_collisionSectors[i]._index = i;
			}
		}

		World::~World()
		{
			__noop;
		}

		BodyID World::CreateBody(const Body2DCreationDesc& body2DCreationDesc)
		{
			_bodyPool.GrowIfFull();

			const uint32 slotIndex = _bodyPool.GetNextSlotIndex();
			BodyID bodyID;
			bodyID.Assign(slotIndex);
			{
				Body2D body;
				body._bodyID = bodyID;
				body._shape._collisionShape = body2DCreationDesc._collisionShape2D;
				body._shape._shapeAABB = MakeShared<AABBCollisionShape2D>(AABBCollisionShape2D(*body._shape._collisionShape));
				body._transform2D = body2DCreationDesc._transform2D;
				body._bodyAABB = MakeShared<AABBCollisionShape2D>(AABBCollisionShape2D(*body._shape._shapeAABB, body._transform2D));
				body._bodyMotionType = body2DCreationDesc._bodyMotionType;
				_bodyPool.Create(slotIndex, std::move(body));

				_worldMin = Float2::Min(_worldMin, body._bodyAABB->_center - body._bodyAABB->_halfSize);
				_worldMax = Float2::Max(_worldMax, body._bodyAABB->_center + body._bodyAABB->_halfSize);
				_worldSize = (_worldMax - _worldMin);
			}
			_collisionSectors[0]._bodyIDs.PushBack(bodyID);
			return bodyID;
		}

		Body2D& World::AccessBody(BodyID bodyID)
		{
			MINT_ASSERT(bodyID.IsValid(), "!!!");
			return _bodyPool.GetObject_(bodyID.Value());
		}

		const Body2D& World::GetBody(BodyID bodyID) const
		{
			MINT_ASSERT(bodyID.IsValid(), "!!!");
			return _bodyPool.GetObject_(bodyID.Value());
		}

		void World::Step(float deltaTime)
		{
			if (_worldHistory.IsPlaying())
			{
				_worldHistory.StepPlay((deltaTime >= 0.0f));

				for (const StepSnapshot::BodySnapshot& bodySnapshot : _worldHistory.GetStepSnapshot()._bodySnapshots)
				{
					AccessBody(bodySnapshot._body._bodyID) = bodySnapshot._body;
				}
				return;
			}

			++_totalStepCount;

			StepCollide(deltaTime);
			StepSolve(deltaTime);

			if (_worldHistory._isRecording == true)
			{
				StepRecordSnapshot();
			}
		}

		uint64 World::GetCurrentStepIndex() const
		{
			if (_worldHistory.IsPlaying())
			{
				return _worldHistory.GetStepSnapshot()._stepIndex;
			}

			return _totalStepCount - 1;
		}

		void World::StepCollide(float deltaTime)
		{
			StepCollide_BroadPhase(deltaTime);
			StepCollide_NarrowPhase(deltaTime);
		}

		void World::StepCollide_BroadPhase(float deltaTime)
		{
			// Continuous collision detection
			const uint32 bodyCount = _bodyPool.GetObjects().Size();
			for (uint32 i = 0; i < bodyCount; ++i)
			{
				Body2D& body = _bodyPool.GetObject_(i);
				if (body.IsValid() == false || body._bodyMotionType != BodyMotionType::Dynamic)
				{
					continue;
				}

				const Float2 displacement = (body._linearVelocity + body._linearAcceleration * deltaTime) * deltaTime;
				body._bodyAABB->SetExpanded(*body._shape._shapeAABB, body._transform2D, displacement);
			}

			_broadPhaseBodyPairs.Clear();
			const uint32 collisionSectorCount = _collisionSectors.Size();
			for (uint32 i = 0; i < collisionSectorCount; ++i)
			{
				const uint32 collisionSectorBodyCount = _collisionSectors[i]._bodyIDs.Size();
				for (uint32 indexA = 0; indexA < collisionSectorBodyCount; ++indexA)
				{
					for (uint32 indexB = 0; indexB < collisionSectorBodyCount; ++indexB)
					{
						if (indexA == indexB)
						{
							continue;
						}

						BodyID bodyIDA = _collisionSectors[i]._bodyIDs[indexA];
						BodyID bodyIDB = _collisionSectors[i]._bodyIDs[indexB];
						const Body2D& bodyA = GetBody(bodyIDA);
						const Body2D& bodyB = GetBody(bodyIDB);
						if (bodyA._bodyMotionType != BodyMotionType::Dynamic && bodyB._bodyMotionType != BodyMotionType::Dynamic)
						{
							continue;
						}

						if (Intersect2D_AABB_AABB(*bodyA._bodyAABB, *bodyB._bodyAABB) == false)
						{
							continue;
						}

						if (bodyIDA.Value() > bodyIDB.Value())
						{
							BodyID temp = bodyIDA;
							bodyIDA = bodyIDB;
							bodyIDB = temp;
						}

						BroadPhaseBodyPair bodyPair{ bodyIDA, bodyIDB };
						const KeyValuePair found = _broadPhaseBodyPairs.Find(bodyPair.GetKey());
						if (found.IsValid() == false)
						{
							_broadPhaseBodyPairs.Insert(bodyPair.GetKey(), bodyPair);
						}
					}
				}
			}
		}

		bool World::StepCollide_NarrowPhase_CCD(float deltaTime, const Body2D& bodyA, const Body2D& bodyB, Physics::GJK2DInfo& gjk2DInfo, SharedPtr<CollisionShape2D>& outShapeA, SharedPtr<CollisionShape2D>& outShapeB)
		{
			float fraction = 1.0f;
			float range = 2.0f;
			Float2 relativeLinearVelocity = bodyA._linearVelocity - bodyB._linearVelocity;
			Float2 separatingDirection = relativeLinearVelocity;
			separatingDirection.Normalize();
			while (true)
			{
				float fractionDeltaTime = deltaTime * fraction;
				Transform2D predictedBodyTransformA = PredictBodyTransform(bodyA, fractionDeltaTime);
				Transform2D predictedBodyTransformB = PredictBodyTransform(bodyB, fractionDeltaTime);

				outShapeA = CollisionShape2D::MakeTransformed(bodyA._shape._collisionShape, predictedBodyTransformA);
				outShapeB = CollisionShape2D::MakeTransformed(bodyB._shape._collisionShape, predictedBodyTransformB);
				const bool intersected = Intersect2D_GJK(*outShapeA, *outShapeB, &gjk2DInfo);
				if (intersected)
				{
					EPA2DInfo epa2DInfo;
					Float2 normal;
					float distance = 0.0f;
					ComputePenetration_EPA(*outShapeA, *outShapeB, gjk2DInfo, normal, distance, epa2DInfo);
					if (distance < 1.0f || range < 0.0625f)
					{
						return true;
					}

					range *= 0.5f;
					fraction -= range * 0.5f;
				}
				else
				{
					if (fraction == 1.0f)
					{
						return false;
					}

					range *= 0.5f;
					fraction += range * 0.5f;
				}
			}
		}

		void World::StepCollide_NarrowPhase(float deltaTime)
		{
			const float deltaTimeSq = deltaTime * deltaTime;

			_collisionManifold2DsMap.Clear();

			GJK2DInfo gjk2DInfo;
			for (const auto& iter : _broadPhaseBodyPairs)
			{
				const Body2D& bodyA = GetBody(iter._bodyIDA);
				const Body2D& bodyB = GetBody(iter._bodyIDB);

				CollisionManifold2D collisionManifold2D;
				const Float2 relativeLinearVelocity = bodyA._linearVelocity - bodyB._linearVelocity;
				if (relativeLinearVelocity != Float2::kZero)
				{
					// Continuous collision detection
					SharedPtr<CollisionShape2D> transformedShapeA;
					SharedPtr<CollisionShape2D> transformedShapeB;
					if (StepCollide_NarrowPhase_CCD(deltaTime, bodyA, bodyB, gjk2DInfo, transformedShapeA, transformedShapeB))
					{
						StepCollide_NarrowPhase_GenerateCollision(bodyA, *transformedShapeA, bodyB, *transformedShapeB, gjk2DInfo, collisionManifold2D);
					}
				}
				else
				{
					// Discrete collision detection
					SharedPtr<CollisionShape2D> transformedShapeA{ CollisionShape2D::MakeTransformed(bodyA._shape._collisionShape, bodyA._transform2D) };
					SharedPtr<CollisionShape2D> transformedShapeB{ CollisionShape2D::MakeTransformed(bodyB._shape._collisionShape, bodyB._transform2D) };
					if (Intersect2D_GJK(*transformedShapeA, *transformedShapeB, &gjk2DInfo))
					{
						StepCollide_NarrowPhase_GenerateCollision(bodyA, *transformedShapeA, bodyB, *transformedShapeB, gjk2DInfo, collisionManifold2D);
					}
				}

				if (collisionManifold2D.IsValid() == true)
				{
					KeyValuePair found = _collisionManifold2DsMap.Find(bodyA._bodyID.Value());
					if (found.IsValid())
					{
						found._value->PushBack(collisionManifold2D);
					}
					else
					{
						_collisionManifold2DsMap.Insert(bodyA._bodyID.Value(), { collisionManifold2D });
					}
				}
			}
		}

		void World::StepCollide_NarrowPhase_GenerateCollision(const Body2D& bodyA, const CollisionShape2D& bodyShapeA, const Body2D& bodyB, const CollisionShape2D& bodyShapeB, const Physics::GJK2DInfo& gjk2DInfo, CollisionManifold2D& outCollisionManifold2D) const
		{
			outCollisionManifold2D._bodyIDA = bodyA._bodyID;
			outCollisionManifold2D._bodyIDB = bodyB._bodyID;

			EPA2DInfo epa2DInfo;
			Float2 normal;
			float distance = 0.0f;
			ComputePenetration_EPA(bodyShapeA, bodyShapeB, gjk2DInfo, normal, distance, epa2DInfo);

			Float2 edgeVertex0;
			Float2 edgeVertex1;
			bodyShapeB.ComputeSupportEdge(normal, edgeVertex0, edgeVertex1);
			Float2 edgeDirection = edgeVertex0 - edgeVertex1;
			edgeDirection.Normalize();
			outCollisionManifold2D._collisionNormal = Float2(-edgeDirection._y, edgeDirection._x);

			const Float2 bodyAPoint = bodyShapeA.ComputeSupportPoint(-outCollisionManifold2D._collisionNormal);
			outCollisionManifold2D._collisionPosition = bodyAPoint;
			outCollisionManifold2D._signedDistance = outCollisionManifold2D._collisionNormal.Dot(outCollisionManifold2D._collisionPosition - edgeVertex0);
		}

		void World::StepSolve(float deltaTime)
		{
			_worldSizePreStepSolve = _worldSize;

			StepSolveResolveCollisions(deltaTime);
			StepSolveIntegrate(deltaTime);
			StepSolveAssignCollisionSectors();
		}

		void World::StepSolveResolveCollisions(float deltaTime)
		{
			// Resolve Collisions
			GJK2DInfo gjk2DInfo;
			for (Vector<CollisionManifold2D>& collisionManifold2Ds : _collisionManifold2DsMap)
			{
				QuickSort(collisionManifold2Ds, CollisionManifold2D::AbsoluteDistanceComparator());

				for (CollisionManifold2D& collisionManifold2D : collisionManifold2Ds)
				{
					Body2D* bodyA = &AccessBody(collisionManifold2D._bodyIDA);
					Body2D* bodyB = &AccessBody(collisionManifold2D._bodyIDB);
					if (bodyA->_bodyMotionType == BodyMotionType::Dynamic && bodyB->_bodyMotionType == BodyMotionType::Dynamic)
					{
						// TODO
						MINT_NEVER;
					}
					else
					{
						// Make sure bodyA is Dynamic!
						if (bodyB->_bodyMotionType == BodyMotionType::Dynamic)
						{
							Body2D* bodyTemp = bodyA;
							bodyA = bodyB;
							bodyB = bodyTemp;
						}

						// TODO: Resolve Penetration!
						if (collisionManifold2D._signedDistance < 0.0f)
						{
							SharedPtr<CollisionShape2D> transformedShapeA{ CollisionShape2D::MakeTransformed(bodyA->_shape._collisionShape, bodyA->_transform2D) };
							SharedPtr<CollisionShape2D> transformedShapeB{ CollisionShape2D::MakeTransformed(bodyB->_shape._collisionShape, bodyB->_transform2D) };
							if (Intersect2D_GJK(*transformedShapeA, *transformedShapeB, &gjk2DInfo))
							{
								CollisionManifold2D newCollisionManifold2D;
								StepCollide_NarrowPhase_GenerateCollision(*bodyA, *transformedShapeA, *bodyB, *transformedShapeB, gjk2DInfo, newCollisionManifold2D);
								const Float2 separatingVector = newCollisionManifold2D._collisionNormal * -newCollisionManifold2D._signedDistance;
								bodyA->_transform2D._translation += separatingVector;

								//bodyA->_linearVelocity += separatingVector / deltaTime;
							}
						}
					}
				}
			}
		}

		void World::StepSolveIntegrate(float deltaTime)
		{
			const uint32 bodyCount = _bodyPool.GetObjects().Size();
			for (uint32 i = 0; i < bodyCount; ++i)
			{
				Body2D& body = _bodyPool.GetObject_(i);
				if (body.IsValid() == false)
				{
					continue;
				}

				if (body._bodyMotionType == BodyMotionType::Dynamic)
				{
					// integrate acceleration
					body._linearVelocity += body._linearAcceleration * deltaTime;
					body._angularVelocity += body._angularAcceleration * deltaTime;

					// integrate velocity
					body._transform2D._translation += body._linearVelocity * deltaTime;
					body._transform2D._rotation += body._angularVelocity * deltaTime;

					body._linearVelocity *= (1.0f - body._linearDamping);
				}

				body._bodyAABB->Set(*body._shape._shapeAABB, body._transform2D);

				_worldMin = Float2::Min(_worldMin, body._transform2D._translation);
				_worldMax = Float2::Max(_worldMax, body._transform2D._translation);
			}
			_worldSize = (_worldMax - _worldMin);
		}

		void World::StepSolveAssignCollisionSectors()
		{
			const Float2 collisionSectorSize = _worldSize / static_cast<float>(1 + _collisionSectorDepth);
			const uint32 collisionSectorBufferSize = _collisionSectorSideCount * _collisionSectorSideCount;
			for (uint32 i = 0; i < collisionSectorBufferSize; ++i)
			{
				_collisionSectors[i]._bodyIDs.Clear();
			}
			Vector<uint32> collisionSectorIndices;
			collisionSectorIndices.Resize(4);
			const uint32 bodyCount = _bodyPool.GetObjects().Size();
			for (uint32 i = 0; i < bodyCount; ++i)
			{
				Body2D& body = _bodyPool.GetObject_(i);
				if (body.IsValid() == false)
				{
					continue;
				}

				ComputeCollisionSectorIndices(*body._bodyAABB, _worldMin, collisionSectorSize, collisionSectorIndices);
				QuickSort(collisionSectorIndices, ComparatorAscending<uint32>());
				_collisionSectors[collisionSectorIndices[0]]._bodyIDs.PushBack(body._bodyID);
				for (uint32 j = 1; j < 4; ++j)
				{
					if (collisionSectorIndices[j] == collisionSectorIndices[j - 1])
					{
						continue;
					}
					_collisionSectors[collisionSectorIndices[j]]._bodyIDs.PushBack(body._bodyID);
				}
			}
		}

		void World::StepRecordSnapshot()
		{
			while (_worldHistory._stepSnapshots.Size() > kWorldHistoryCapacity)
			{
				_worldHistory._stepSnapshots.Pop();
			}

			StepSnapshot stepSnapshot;
			stepSnapshot._stepIndex = _totalStepCount - 1;
			const uint32 bodyCount = _bodyPool.GetObjects().Size();
			for (uint32 i = 0; i < bodyCount; ++i)
			{
				const Body2D& body = _bodyPool.GetObject_(i);
				if (body.IsValid() == false)
				{
					continue;
				}

				StepSnapshot::BodySnapshot bodySnapshot;
				bodySnapshot._body = body;
				KeyValuePair found = _collisionManifold2DsMap.Find(body._bodyID.Value());
				if (found.IsValid())
				{
					bodySnapshot._collisionManifolds = *found._value;
				}
				stepSnapshot._bodySnapshots.PushBack(std::move(bodySnapshot));
			}
			_worldHistory._stepSnapshots.Push(stepSnapshot);
		}

		void World::ComputeCollisionSectorIndices(const Physics::AABBCollisionShape2D& aabb, const Float2& worldMin, const Float2& collisionSectorSize, Vector<uint32>& outIndices) const
		{
			const Float2 aabbMax = aabb._center + aabb._halfSize;
			const Float2 aabbMin = aabb._center - aabb._halfSize;
			Float2 p0 = Float2(aabbMin._x, aabbMax._y);
			const Float2& p1 = aabbMin;
			Float2 p2 = Float2(aabbMax._x, aabbMin._y);
			const Float2& p3 = aabbMax;

			const Float2 p0FromMin = p0 - worldMin;
			const Float2 p1FromMin = p1 - worldMin;
			const Float2 p2FromMin = p2 - worldMin;
			const Float2 p3FromMin = p3 - worldMin;
			outIndices[0] = ComputeCollisionSectorIndex(Int2(p0FromMin / collisionSectorSize));
			outIndices[1] = ComputeCollisionSectorIndex(Int2(p1FromMin / collisionSectorSize));
			outIndices[2] = ComputeCollisionSectorIndex(Int2(p2FromMin / collisionSectorSize));
			outIndices[3] = ComputeCollisionSectorIndex(Int2(p3FromMin / collisionSectorSize));
		}

		uint32 World::ComputeCollisionSectorIndex(const Physics::AABBCollisionShape2D& aabb, const Float2& worldMin, const Float2& collisionSectorSize) const
		{
			const Float2 centerFromMin = aabb._center - worldMin;
			return ComputeCollisionSectorIndex(Int2(centerFromMin / collisionSectorSize));
		}

		uint32 World::ComputeCollisionSectorIndex(const Int2& collisionSectorIndex2) const
		{
			return collisionSectorIndex2._y * _collisionSectorSideCount + collisionSectorIndex2._x;
		}

		CollisionSector* World::GetCollisionSector(const Int2& collisionSectorIndex2)
		{
			if (collisionSectorIndex2._x < 0 || collisionSectorIndex2._x >= static_cast<int32>(_collisionSectorSideCount))
			{
				return nullptr;
			}
			else if (collisionSectorIndex2._y < 0 || collisionSectorIndex2._y >= static_cast<int32>(_collisionSectorSideCount))
			{
				return nullptr;
			}
			return &_collisionSectors[ComputeCollisionSectorIndex(collisionSectorIndex2)];
		}

		void World::GetAdjacentCollisionSectors(const CollisionSector& collisionSector, CollisionSector* (outAdjacentCollisionSectors)[8])
		{
			// 5 6 7
			// 3   4
			// 0 1 2

			const int32 x = collisionSector._index % _collisionSectorSideCount;
			const int32 y = collisionSector._index / _collisionSectorSideCount;
			const Int2 index2(x, y);
			outAdjacentCollisionSectors[0] = GetCollisionSector(index2 + Int2(-1, -1));
			outAdjacentCollisionSectors[1] = GetCollisionSector(index2 + Int2(0, -1));
			outAdjacentCollisionSectors[2] = GetCollisionSector(index2 + Int2(+1, -1));

			outAdjacentCollisionSectors[3] = GetCollisionSector(index2 + Int2(-1, 0));
			outAdjacentCollisionSectors[4] = GetCollisionSector(index2 + Int2(+1, 0));

			outAdjacentCollisionSectors[5] = GetCollisionSector(index2 + Int2(-1, +1));
			outAdjacentCollisionSectors[6] = GetCollisionSector(index2 + Int2(0, +1));
			outAdjacentCollisionSectors[7] = GetCollisionSector(index2 + Int2(+1, +1));
		}

		Transform2D World::PredictBodyTransform(const Body2D& body, float deltaTime) const
		{
			return PredictTransform(body._transform2D, body._linearVelocity, body._linearAcceleration, body._angularVelocity, body._angularAcceleration, deltaTime);
		}

		Transform2D World::PredictTransform(const Transform2D& transform2D, const Float2& linearVelocity, const Float2& linearAcceleration, float angularVelocity, float angularAcceleration, float deltaTime) const
		{
			const float deltaTimeSq = deltaTime * deltaTime;
			Transform2D resultTransform2D = transform2D;
			resultTransform2D._translation += linearVelocity * deltaTime;
			resultTransform2D._translation += linearAcceleration * deltaTimeSq;
			resultTransform2D._rotation = angularVelocity * deltaTime;
			resultTransform2D._rotation += angularAcceleration * deltaTimeSq;
			return resultTransform2D;
		}

		void World::RenderDebug(Rendering::ShapeRenderer& shapeRenderer) const
		{
			if (_worldHistory.IsPlaying() == true)
			{
				const StepSnapshot& stepSnapshot = _worldHistory.GetStepSnapshot();
				for (const StepSnapshot::BodySnapshot& bodySnapshot : stepSnapshot._bodySnapshots)
				{
					RenderDebugBody(shapeRenderer, bodySnapshot._body);

					for (const CollisionManifold2D& collisionManifold : bodySnapshot._collisionManifolds)
					{
						RenderDebugCollisionManifold(shapeRenderer, collisionManifold);
					}
				}
			}
			else
			{
				const uint32 bodyCount = _bodyPool.GetObjects().Size();
				for (uint32 i = 0; i < bodyCount; ++i)
				{
					const Body2D& body = _bodyPool.GetObject_(i);
					if (body.IsValid() == false)
					{
						continue;
					}

					RenderDebugBody(shapeRenderer, body);
				}

				for (const Vector<CollisionManifold2D>& collisionManifold2Ds : _collisionManifold2DsMap)
				{
					for (const CollisionManifold2D& collisionManifold2D : collisionManifold2Ds)
					{
						RenderDebugCollisionManifold(shapeRenderer, collisionManifold2D);
					}
				}
			}

			shapeRenderer.Render();
		}

		void World::RenderDebugBody(Rendering::ShapeRenderer& shapeRenderer, const Body2D& body) const
		{
			MINT_ASSERT(body.IsValid() == true, "Caller must guarantee this!");

			// TEMP
			//body._bodyAABB->DebugDrawShape(shapeRenderer, ByteColor(255, 255, 0), Transform2D::GetIdentity());

			body._shape._collisionShape->DebugDrawShape(shapeRenderer, ByteColor(128, 128, 128), body._transform2D);

			// TEMP
			//StackStringW<256> buffer;
			//FormatString(buffer, L"[%d]", body._bodyID.Value());
			//shapeRenderer.DrawDynamicText(buffer.CString(), Float4(body._transform2D._translation), Rendering::FontRenderingOption());
		}

		void World::RenderDebugCollisionManifold(Rendering::ShapeRenderer& shapeRenderer, const CollisionManifold2D& collisionManifold) const
		{
			const float kNormalLength = 64.0f;
			const float kNormalThickness = 2.0f;
			const float kPositionCircleRadius = 4.0f;

			shapeRenderer.SetColor(ByteColor(0, 128, 255));
			shapeRenderer.DrawCircle(Float3(collisionManifold._collisionPosition), kPositionCircleRadius);
			shapeRenderer.DrawLine(collisionManifold._collisionPosition, collisionManifold._collisionPosition + collisionManifold._collisionNormal * kNormalLength, kNormalThickness);

			shapeRenderer.SetColor(ByteColor(128, 0, 255));
			shapeRenderer.DrawCircle(Float3(collisionManifold._collisionPosition + collisionManifold._collisionNormal * ::abs(collisionManifold._signedDistance)), kPositionCircleRadius);
		}

		void World::BeginHistoryRecording()
		{
			_worldHistory._stepSnapshots.Flush();
			_worldHistory._isRecording = true;
		}

		void World::EndHistoryRecording()
		{
			_worldHistory._isRecording = false;
		}

		bool World::BeginHistoryPlaying()
		{
			if (_worldHistory._isRecording == true)
			{
				return false;
			}

			if (_worldHistory._stepSnapshots.IsEmpty() == true)
			{
				return false;
			}

			_worldHistory.BeingPlaying();
			return true;
		}

		void World::EndHistoryPlaying()
		{
			_worldHistory.EndPlaying();
		}

		uint32 World::GetHistorySize() const
		{
			return _worldHistory._stepSnapshots.Size();
		}

		uint32 World::GetCurrentHistoryIndex() const
		{
			return _worldHistory.GetCurrentSnapshotIndex();
		}
#pragma endregion
	}
}
