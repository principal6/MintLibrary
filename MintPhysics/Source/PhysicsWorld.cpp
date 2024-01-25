#include <MintPhysics/Include/PhysicsWorld.h>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/HashMap.hpp>
#include <MintContainer/Include/Color.h>
#include <MintContainer/Include/Algorithm.hpp>
#include <MintContainer/Include/StringUtil.hpp>
#include <MintRenderingBase/Include/ShapeRendererContext.h>
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

#pragma region World
		World::World()
			: _worldMin{ -128.0, -128.0f }
			, _worldMax{ +128.0, +128.0f }
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
			StepCollide(deltaTime);
			StepSolve(deltaTime);
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

			_narrowPhaseCollisionInfosMap.Clear();

			GJK2DInfo gjk2DInfo;
			for (const auto& iter : _broadPhaseBodyPairs)
			{
				const Body2D& bodyA = GetBody(iter._bodyIDA);
				const Body2D& bodyB = GetBody(iter._bodyIDB);

				NarrowPhaseCollisionInfo narrowPhaseCollisionInfo;
				const Float2 relativeLinearVelocity = bodyA._linearVelocity - bodyB._linearVelocity;
				if (relativeLinearVelocity != Float2::kZero)
				{
					// Continuous collision detection
					SharedPtr<CollisionShape2D> transformedShapeA;
					SharedPtr<CollisionShape2D> transformedShapeB;
					if (StepCollide_NarrowPhase_CCD(deltaTime, bodyA, bodyB, gjk2DInfo, transformedShapeA, transformedShapeB))
					{
						StepCollide_NarrowPhase_GenerateCollision(bodyA, *transformedShapeA, bodyB, *transformedShapeB, gjk2DInfo, narrowPhaseCollisionInfo);
					}
				}
				else
				{
					// Discrete collision detection
					SharedPtr<CollisionShape2D> transformedShapeA{ CollisionShape2D::MakeTransformed(bodyA._shape._collisionShape, bodyA._transform2D) };
					SharedPtr<CollisionShape2D> transformedShapeB{ CollisionShape2D::MakeTransformed(bodyB._shape._collisionShape, bodyB._transform2D) };
					if (Intersect2D_GJK(*transformedShapeA, *transformedShapeB, &gjk2DInfo))
					{
						StepCollide_NarrowPhase_GenerateCollision(bodyA, *transformedShapeA, bodyB, *transformedShapeB, gjk2DInfo, narrowPhaseCollisionInfo);
					}
				}

				if (narrowPhaseCollisionInfo.IsValid() == true)
				{
					KeyValuePair found = _narrowPhaseCollisionInfosMap.Find(narrowPhaseCollisionInfo.GetKey());
					if (found.IsValid())
					{
						found._value->PushBack(narrowPhaseCollisionInfo);
					}
					else
					{
						_narrowPhaseCollisionInfosMap.Insert(narrowPhaseCollisionInfo.GetKey(), { narrowPhaseCollisionInfo });
					}
				}
			}
		}

		void World::StepCollide_NarrowPhase_GenerateCollision(const Body2D& bodyA, const CollisionShape2D& bodyShapeA, const Body2D& bodyB, const CollisionShape2D& bodyShapeB, const Physics::GJK2DInfo& gjk2DInfo, NarrowPhaseCollisionInfo& outNarrowPhaseCollisionInfo) const
		{
			outNarrowPhaseCollisionInfo._bodyIDA = bodyA._bodyID;
			outNarrowPhaseCollisionInfo._bodyIDB = bodyB._bodyID;

			EPA2DInfo epa2DInfo;
			Float2 normal;
			float distance = 0.0f;
			ComputePenetration_EPA(bodyShapeA, bodyShapeB, gjk2DInfo, normal, distance, epa2DInfo);

			Float2 edgeVertex0;
			Float2 edgeVertex1;
			bodyShapeB.ComputeSupportEdge(normal, edgeVertex0, edgeVertex1);
			Float2 edgeDirection = edgeVertex0 - edgeVertex1;
			edgeDirection.Normalize();
			outNarrowPhaseCollisionInfo._collisionNormal = Float2(-edgeDirection._y, edgeDirection._x);

			const Float2 bodyAPoint = bodyShapeA.ComputeSupportPoint(-outNarrowPhaseCollisionInfo._collisionNormal);
			outNarrowPhaseCollisionInfo._collisionPosition = bodyAPoint;
			outNarrowPhaseCollisionInfo._signedDistance = outNarrowPhaseCollisionInfo._collisionNormal.Dot(outNarrowPhaseCollisionInfo._collisionPosition - edgeVertex0);
		}

		void World::StepSolve(float deltaTime)
		{
			_worldSizePreStepSolve = _worldSize;

			// Resolve Collisions
			GJK2DInfo gjk2DInfo;
			for (Vector<NarrowPhaseCollisionInfo>& narrowPhaseCollisionInfos : _narrowPhaseCollisionInfosMap)
			{
				QuickSort(narrowPhaseCollisionInfos, NarrowPhaseCollisionInfo::DistanceComparator());

				for (NarrowPhaseCollisionInfo& narrowPhaseCollisionInfo : narrowPhaseCollisionInfos)
				{
					Body2D* bodyA = &AccessBody(narrowPhaseCollisionInfo._bodyIDA);
					Body2D* bodyB = &AccessBody(narrowPhaseCollisionInfo._bodyIDB);
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

						// TODO ...
						if (narrowPhaseCollisionInfo._signedDistance < 0.0f)
						{
							const Float2& relativeVelocity = bodyA->_linearVelocity;
							bodyA->_linearVelocity -= relativeVelocity.Dot(narrowPhaseCollisionInfo._collisionNormal) * narrowPhaseCollisionInfo._collisionNormal;

							SharedPtr<CollisionShape2D> transformedShapeA{ CollisionShape2D::MakeTransformed(bodyA->_shape._collisionShape, bodyA->_transform2D) };
							SharedPtr<CollisionShape2D> transformedShapeB{ CollisionShape2D::MakeTransformed(bodyB->_shape._collisionShape, bodyB->_transform2D) };
							if (Intersect2D_GJK(*transformedShapeA, *transformedShapeB, &gjk2DInfo))
							{
								NarrowPhaseCollisionInfo newNarrowPhaseCollisionInfo;
								StepCollide_NarrowPhase_GenerateCollision(*bodyA, *transformedShapeA, *bodyB, *transformedShapeB, gjk2DInfo, newNarrowPhaseCollisionInfo);
								bodyA->_transform2D._translation += newNarrowPhaseCollisionInfo._collisionNormal * -newNarrowPhaseCollisionInfo._signedDistance;
							}
						}
					}
				}
			}

			// Integrate
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
					body._transform2DPrevStep = body._transform2D;
					body._transform2D._translation += body._linearVelocity * deltaTime;
					body._transform2D._rotation += body._angularVelocity * deltaTime;

					body._linearVelocity *= (1.0f - body._linearDamping);
				}

				body._bodyAABB->Set(*body._shape._shapeAABB, body._transform2D);

				_worldMin = Float2::Min(_worldMin, body._transform2D._translation);
				_worldMax = Float2::Max(_worldMax, body._transform2D._translation);
			}
			_worldSize = (_worldMax - _worldMin);

			const Float2 collisionSectorSize = _worldSize / static_cast<float>(1 + _collisionSectorDepth);
			const uint32 collisionSectorBufferSize = _collisionSectorSideCount * _collisionSectorSideCount;
			for (uint32 i = 0; i < collisionSectorBufferSize; ++i)
			{
				_collisionSectors[i]._bodyIDs.Clear();
			}
			Vector<uint32> collisionSectorIndices;
			collisionSectorIndices.Resize(4);
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

		void World::RenderDebug(Rendering::ShapeRendererContext& shapeRendererContext) const
		{
			StackStringW<256> buffer;
			const uint32 bodyCount = _bodyPool.GetObjects().Size();
			for (uint32 i = 0; i < bodyCount; ++i)
			{
				const Body2D& body = _bodyPool.GetObject_(i);
				if (body.IsValid() == false)
				{
					continue;
				}

				body._bodyAABB->DebugDrawShape(shapeRendererContext, ByteColor(255, 255, 0), Transform2D());

				body._shape._collisionShape->DebugDrawShape(shapeRendererContext, ByteColor(128, 128, 128), body._transform2D);

				FormatString(buffer, L"[%d]", body._bodyID.Value());
				shapeRendererContext.DrawDynamicText(buffer.CString(), Float4(body._transform2D._translation), Rendering::FontRenderingOption());
			}

			const float kNormalLength = 64.0f;
			const float kNormalThickness = 2.0f;
			const float kPositionCircleRadius = 4.0f;
			for (const Vector<NarrowPhaseCollisionInfo>& narrowPhaseCollisionInfos : _narrowPhaseCollisionInfosMap)
			{
				for (const NarrowPhaseCollisionInfo& narrowPhaseCollisionInfo : narrowPhaseCollisionInfos)
				{
					shapeRendererContext.SetColor(ByteColor(0, 128, 255));
					shapeRendererContext.SetPosition(Float4(narrowPhaseCollisionInfo._collisionPosition));
					shapeRendererContext.DrawCircle(kPositionCircleRadius);
					shapeRendererContext.DrawLine(narrowPhaseCollisionInfo._collisionPosition, narrowPhaseCollisionInfo._collisionPosition + narrowPhaseCollisionInfo._collisionNormal * kNormalLength, kNormalThickness);

					shapeRendererContext.SetColor(ByteColor(128, 0, 255));
					shapeRendererContext.SetPosition(Float4(narrowPhaseCollisionInfo._collisionPosition + narrowPhaseCollisionInfo._collisionNormal * ::abs(narrowPhaseCollisionInfo._signedDistance)));
					shapeRendererContext.DrawCircle(kPositionCircleRadius);
				}
			}

			shapeRendererContext.Render();
		}
#pragma endregion
	}
}
