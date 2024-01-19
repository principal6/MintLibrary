#include <MintPhysics/Include/PhysicsWorld.h>
#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/HashMap.hpp>
#include <MintContainer/Include/Color.h>
#include <MintContainer/Include/Algorithm.hpp>
#include <MintRenderingBase/Include/ShapeRendererContext.h>
#include <MintPhysics/Include/Intersection.hpp>


namespace mint
{
	namespace Physics
	{
#pragma region Body2D
		Body2D::Body2D()
			: _isDynamic{ false }
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
			: _worldMin{ -8.0, -8.0f }
			, _worldMax{ +8.0, +8.0f }
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
				body._isDynamic = body2DCreationDesc._isDynamic;
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
			// Broad phase
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
						if (Intersect2D_AABB_AABB(*GetBody(bodyIDA)._bodyAABB, *GetBody(bodyIDB)._bodyAABB) == false)
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

			// Narrow phase
			for (const auto& iter : _broadPhaseBodyPairs)
			{
				const Body2D& bodyA = GetBody(iter._bodyIDA);
				const Body2D& bodyB = GetBody(iter._bodyIDB);

				Transform2D transformBInASpace = bodyB._transform2D;
				transformBInASpace *= bodyA._transform2D.GetInverted();
				if (Intersect2D_GJK(*bodyA._shape._collisionShape, ConvexCollisionShape2D(*bodyB._shape._collisionShape, transformBInASpace)))
				{
					// TODO: COLLIDED!!!
				}
			}
		}

		void World::StepSolve(float deltaTime)
		{
			_worldSizePreStepSolve = _worldSize;

			const uint32 collisionSectorBufferSize = _collisionSectorSideCount * _collisionSectorSideCount;
			for (uint32 i = 0; i < collisionSectorBufferSize; ++i)
			{
				_collisionSectors[i]._bodyIDs.Clear();
			}

			const Float2 worldMinPreStepSolve = _worldMin;
			const Float2 worldMaxPreStepSolve = _worldMax;
			const Float2 worldSizePreStepSolve = (worldMaxPreStepSolve - worldMinPreStepSolve);
			const Float2 collisionSectorSize = worldSizePreStepSolve / static_cast<float>(1 + _collisionSectorDepth);

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

				if (body._isDynamic)
				{
					// integrate acceleration
					body._linearVelocity += body._linearAcceleration * deltaTime;
					body._angularVelocity += body._angularAcceleration * deltaTime;

					// integrate velocity
					body._transform2D._translation += body._linearVelocity * deltaTime;
					body._transform2D._rotation += body._angularVelocity * deltaTime;
				}
				body._bodyAABB->Set(*body._shape._shapeAABB, body._transform2D);

				ComputeCollisionSectorIndices(*body._bodyAABB, worldMinPreStepSolve, collisionSectorSize, collisionSectorIndices);
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

				_worldMin = Float2::Min(_worldMin, body._transform2D._translation);
				_worldMax = Float2::Max(_worldMax, body._transform2D._translation);
			}

			_worldSize = (_worldMax - _worldMin);
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

		void World::RenderDebug(Rendering::ShapeRendererContext& shapeRendererContext) const
		{
			const uint32 bodyCount = _bodyPool.GetObjects().Size();
			for (uint32 i = 0; i < bodyCount; ++i)
			{
				const Body2D& body = _bodyPool.GetObject_(i);
				if (body.IsValid() == false)
				{
					continue;
				}

				body._bodyAABB->DebugDrawShape(shapeRendererContext, ByteColor(255, 255, 0), Transform2D());

				body._shape._collisionShape->DebugDrawShape(shapeRendererContext, ByteColor(255, 0, 0), body._transform2D);
			}

			shapeRendererContext.Render();
		}
#pragma endregion
	}
}
