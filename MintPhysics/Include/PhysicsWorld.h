#pragma once


#ifndef _MINT_PHYSICS_PHYSICS_WORLD_H_
#define _MINT_PHYSICS_PHYSICS_WORLD_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/Queue.h>
#include <MintContainer/Include/HashMap.h>
#include <MintContainer/Include/SharedPtr.h>
#include <MintContainer/Include/ID.h>
#include <MintMath/Include/Float3.h>
#include <MintMath/Include/Transform.h>
#include <MintPhysics/Include/PhysicsCommon.h>
#include <MintPhysics/Include/CollisionShape.h>
#include <MintPhysics/Include/PhysicsObjectPool.h>


namespace mint
{
	namespace Physics2D
	{
		struct GJKInfo;
	}
}

namespace mint
{
	namespace Physics2D
	{
		struct BodyShape
		{
			SharedPtr<CollisionShape> _collisionShape;
			SharedPtr<AABBCollisionShape> _shapeAABB;
		};

		class Body
		{
		public:
			Body();
			~Body();

		public:
			bool IsValid() const;
			void Invalidate() { _bodyID.Invalidate(); }

		public:
			BodyID _bodyID;
			BodyShape _shape;
			SharedPtr<AABBCollisionShape> _bodyAABB;
			Transform2D _transform2D;

			BodyMotionType _bodyMotionType;

			// TODO
			float _inverseMass = 1.0f;
			Float2 _linearVelocity;
			Float2 _linearAcceleration;
			float _linearDamping = 0.0f;
			float _angularVelocity;
			float _angularAcceleration;
		};

		struct BodyCreationDesc
		{
			SharedPtr<CollisionShape> _collisionShape;
			Transform2D _transform2D;
			BodyMotionType _bodyMotionType = BodyMotionType::Static;
			float _inverseMass = 1.0f;
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

		struct CollisionManifold
		{
			struct AbsoluteDistanceComparator
			{
				bool operator()(const CollisionManifold& lhs, const CollisionManifold& rhs) const
				{
					return ::abs(lhs._signedDistance) < ::abs(rhs._signedDistance);
				}
			};
			using Key = uint64;
			Key GetKey() const
			{
				return (static_cast<Key>(_bodyIDA.Value()) << 32) | _bodyIDB.Value();
			}
			bool IsValid() const { return _collisionNormal != Float2::kZero; }
			BodyID _bodyIDA;
			BodyID _bodyIDB;
			Float2 _collisionPosition;
			Float2 _collisionNormal = Float2::kZero;
			float _signedDistance = 0.0f;
		};

		struct StepSnapshot
		{
			// TODO: try not to store Body
			struct BodySnapshot
			{
				Body _body;
				Vector<CollisionManifold> _collisionManifolds;
			};
			uint64 _stepIndex = 0;
			Vector<BodySnapshot> _bodySnapshots;
		};

		struct WorldHistory
		{
			void BeingPlaying();
			void EndPlaying();
			bool IsPlaying() const { return _currentSnapshotIndex != kInvalidIndexUint32; }
			void StepPlay(bool isForward);
			const StepSnapshot& GetStepSnapshot() const;
			uint32 GetCurrentSnapshotIndex() const { return _currentSnapshotIndex; }

			bool _isRecording = false;
			Queue<StepSnapshot> _stepSnapshots;

		private:
			uint32 _currentSnapshotIndex = kInvalidIndexUint32;
		};

		class World
		{
		public:
			World();
			~World();

		public:
			BodyID CreateBody(const BodyCreationDesc& bodyCreationDesc);
			Body& AccessBody(BodyID bodyID);
			const Body& GetBody(BodyID bodyID) const;

		public:
			void Step(float deltaTime);
			uint64 GetTotalStepCount() const { return _totalStepCount; }
			uint64 GetCurrentStepIndex() const;
		
		public:
			const Float2& GetGravity() const { return _gravity; }

		public:
			void RenderDebug(Rendering::ShapeRenderer& shapeRenderer) const;

		public:
			void BeginHistoryRecording();
			void EndHistoryRecording();
			[[nodiscard]] bool BeginHistoryPlaying();
			void EndHistoryPlaying();
			uint32 GetHistorySize() const;
			uint32 GetCurrentHistoryIndex() const;

		private:
			void StepCollide(float deltaTime);
			void StepCollide_BroadPhase(float deltaTime);
			void StepCollide_NarrowPhase(float deltaTime);
			bool StepCollide_NarrowPhase_CCD(float deltaTime, const Body& bodyA, const Body& bodyB, Physics2D::GJKInfo& gjkInfo, SharedPtr<CollisionShape>& outShapeA, SharedPtr<CollisionShape>& outShapeB);
			void StepCollide_NarrowPhase_GenerateCollision(const Body& bodyA, const CollisionShape& bodyShapeA, const Body& bodyB, const CollisionShape& bodyShapeB, const Physics2D::GJKInfo& gjkInfo, CollisionManifold& outCollisionManifold) const;
			void StepSolve(float deltaTime);
			void StepSolveResolveCollisions(float deltaTime);
			void StepSolveSolveConstraints(float deltaTime);
			void StepSolveIntegrate(float deltaTime);
			void StepSolveAssignCollisionSectors();
			void StepRecordSnapshot();

		private:
			void ComputeCollisionSectorIndices(const Physics2D::AABBCollisionShape& aabb, const Float2& worldMin, const Float2& collisionSectorSize, Vector<uint32>& outIndices) const;
			uint32 ComputeCollisionSectorIndex(const Physics2D::AABBCollisionShape& aabb, const Float2& worldMin, const Float2& collisionSectorSize) const;
			uint32 ComputeCollisionSectorIndex(const Int2& collisionSectorIndex2) const;
			CollisionSector* GetCollisionSector(const Int2& collisionSectorIndex2);
			void GetAdjacentCollisionSectors(const CollisionSector& collisionSector, CollisionSector* (outAdjacentCollisionSectors)[8]);
			Transform2D PredictBodyTransform(const Body& body, float deltaTime) const;
			Transform2D PredictTransform(const Transform2D& transform2D, const Float2& linearVelocity, const Float2& linearAcceleration, float angularVelocity, float angularAcceleration, float deltaTime) const;

		private:
			void RenderDebugBody(Rendering::ShapeRenderer& shapeRenderer, const Body& body) const;
			void RenderDebugCollisionManifold(Rendering::ShapeRenderer& shapeRenderer, const CollisionManifold& collisionManifold) const;

		public:
			PhysicsObjectPool<Body> _bodyPool;

		private:
			Float2 _gravity;
			Float2 _worldMin;
			Float2 _worldMax;
			Float2 _worldSize;
			Float2 _worldSizePreStepSolve;
			uint64 _totalStepCount;

		private:
			static constexpr const uint32 kCollisionSectorTessellationPerSide = 2; // 4 = 2 * 2
			uint32 _collisionSectorDepth = 1;
			uint32 _collisionSectorSideCount = kCollisionSectorTessellationPerSide;
			Vector<CollisionSector> _collisionSectors;
			HashMap<BroadPhaseBodyPair::Key, BroadPhaseBodyPair> _broadPhaseBodyPairs;
			HashMap<BodyID::RawType, Vector<CollisionManifold>> _collisionManifoldMap;

		private:
			static constexpr const uint32 kWorldHistoryCapacity = 128;
			WorldHistory _worldHistory;
		};
	}
}


#endif // !_MINT_PHYSICS_PHYSICS_WORLD_H_
