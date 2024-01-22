#pragma once


#ifndef _MINT_PHYSICS_PHYSICS_WORLD_H_
#define _MINT_PHYSICS_PHYSICS_WORLD_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/HashMap.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintContainer/Include/ID.h>
#include <MintMath/Include/Float3.h>
#include <MintMath/Include/Transform.h>
#include <MintPhysics/Include/CollisionShape.h>
#include <MintPhysics/Include/PhysicsObjectPool.h>


namespace mint
{
	namespace Physics
	{
		class World;
	}
}

namespace mint
{
	namespace Physics
	{
		class BodyID : public ID32
		{
			friend World;
			BodyID(uint32 value) : ID32() { Assign(value); }

		public:
			BodyID() = default;
		};

		struct BodyShape2D
		{
			SharedPtr<CollisionShape2D> _collisionShape;
			SharedPtr<AABBCollisionShape2D> _shapeAABB;
		};

		enum class BodyMotionType
		{
			Static,
			Dynamic,
			KeyFramed,
		};
		class Body2D
		{
		public:
			Body2D();
			~Body2D();

		public:
			bool IsValid() const;
			void Invalidate() { _bodyID.Invalidate(); }

		public:
			BodyID _bodyID;
			BodyShape2D _shape;
			SharedPtr<AABBCollisionShape2D> _bodyAABB;
			Transform2D _transform2D;
			Transform2D _transform2DPrevStep;

			BodyMotionType _bodyMotionType;
			// TODO
			float _mass = 1.0f;
			Float2 _linearVelocity;
			Float2 _linearAcceleration;
			float _linearDamping = 1.0f;
			float _angularVelocity;
			float _angularAcceleration;
		};

		struct Body2DCreationDesc
		{
			SharedPtr<CollisionShape2D> _collisionShape2D;
			Transform2D _transform2D;
			BodyMotionType _bodyMotionType = BodyMotionType::Static;
		};

		struct CollisionSector
		{
			uint32 _index = kInvalidIndexUint32;
			Vector<BodyID> _bodyIDs;
		};

		struct BroadPhaseBodyPair
		{
			using Key = uint64;
			Key GetKey() const
			{
				return (static_cast<Key>(_bodyIDA.Value()) << 32) | _bodyIDB.Value();
			}
			// A must be smaller than B
			BodyID _bodyIDA;
			BodyID _bodyIDB;
		};

		struct NarrowPhaseCollisionInfo
		{
			using Key = uint64;
			Key GetKey() const
			{
				return (static_cast<Key>(_bodyIDA.Value()) << 32) | _bodyIDB.Value();
			}
			BodyID _bodyIDA;
			BodyID _bodyIDB;
			Float2 _collisionPosition;
			Float2 _collisionNormal;
			Float2 _collisionEdgeVertex0;
			Float2 _collisionEdgeVertex1;
			float _signedDistance = 0.0f;
		};

		class World
		{
		public:
			World();
			~World();

		public:
			BodyID CreateBody(const Body2DCreationDesc& body2DCreationDesc);
			Body2D& AccessBody(BodyID bodyID);
			const Body2D& GetBody(BodyID bodyID) const;

		public:
			void Step(float deltaTime);

		public:
			void RenderDebug(Rendering::ShapeRendererContext& shapeRendererContext) const;

		private:
			void StepCollide(float deltaTime);
			void StepCollide_BroadPhase(float deltaTime);
			void StepCollide_NarrowPhase(float deltaTime);
			void StepSolve(float deltaTime);

		private:
			void ComputeCollisionSectorIndices(const Physics::AABBCollisionShape2D& aabb, const Float2& worldMin, const Float2& collisionSectorSize, Vector<uint32>& outIndices) const;
			uint32 ComputeCollisionSectorIndex(const Physics::AABBCollisionShape2D& aabb, const Float2& worldMin, const Float2& collisionSectorSize) const;
			uint32 ComputeCollisionSectorIndex(const Int2& collisionSectorIndex2) const;
			CollisionSector* GetCollisionSector(const Int2& collisionSectorIndex2);
			void GetAdjacentCollisionSectors(const CollisionSector& collisionSector, CollisionSector* (outAdjacentCollisionSectors)[8]);

		public:
			PhysicsObjectPool<Body2D> _bodyPool;

		private:
			Float2 _worldMin;
			Float2 _worldMax;
			Float2 _worldSize;
			Float2 _worldSizePreStepSolve;

		private:
			static constexpr const uint32 kCollisionSectorTessellationPerSide = 2; // 4 = 2 * 2
			uint32 _collisionSectorDepth = 1;
			uint32 _collisionSectorSideCount = kCollisionSectorTessellationPerSide;
			Vector<CollisionSector> _collisionSectors;
			HashMap<BroadPhaseBodyPair::Key, BroadPhaseBodyPair> _broadPhaseBodyPairs;
			//HashMap<NarrowPhaseCollisionInfo::Key, NarrowPhaseCollisionInfo> _narrowPhaseCollisionInfos;
			Vector<NarrowPhaseCollisionInfo> _narrowPhaseCollisionInfos;
		};
	}
}


#endif // !_MINT_PHYSICS_PHYSICS_WORLD_H_
