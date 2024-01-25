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
#include <MintPhysics/Include/CollisionShape.h>
#include <MintPhysics/Include/PhysicsObjectPool.h>


namespace mint
{
	namespace Physics
	{
		class World;
		struct GJK2DInfo;
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

		struct CollisionManifold2D
		{
			struct DistanceComparator
			{
				bool operator()(const CollisionManifold2D& lhs, const CollisionManifold2D& rhs) const
				{
					return lhs._signedDistance < rhs._signedDistance;
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
			// TODO: try not to store Body2D
			struct BodySnapshot
			{
				Body2D _body;
				Vector<CollisionManifold2D> _collisionManifolds;
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
			BodyID CreateBody(const Body2DCreationDesc& body2DCreationDesc);
			Body2D& AccessBody(BodyID bodyID);
			const Body2D& GetBody(BodyID bodyID) const;

		public:
			void Step(float deltaTime);
			uint64 GetTotalStepCount() const { return _totalStepCount; }
			uint64 GetCurrentStepIndex() const;

		public:
			void RenderDebug(Rendering::ShapeRendererContext& shapeRendererContext) const;

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
			bool StepCollide_NarrowPhase_CCD(float deltaTime, const Body2D& bodyA, const Body2D& bodyB, Physics::GJK2DInfo& gjk2DInfo, SharedPtr<CollisionShape2D>& outShapeA, SharedPtr<CollisionShape2D>& outShapeB);
			void StepCollide_NarrowPhase_GenerateCollision(const Body2D& bodyA, const CollisionShape2D& bodyShapeA, const Body2D& bodyB, const CollisionShape2D& bodyShapeB, const Physics::GJK2DInfo& gjk2DInfo, CollisionManifold2D& outCollisionManifold2D) const;
			void StepSolve(float deltaTime);
			void StepSolveIntegrate(float deltaTime);
			void StepSolveAssignCollisionSectors();
			void StepRecordSnapshot();

		private:
			void ComputeCollisionSectorIndices(const Physics::AABBCollisionShape2D& aabb, const Float2& worldMin, const Float2& collisionSectorSize, Vector<uint32>& outIndices) const;
			uint32 ComputeCollisionSectorIndex(const Physics::AABBCollisionShape2D& aabb, const Float2& worldMin, const Float2& collisionSectorSize) const;
			uint32 ComputeCollisionSectorIndex(const Int2& collisionSectorIndex2) const;
			CollisionSector* GetCollisionSector(const Int2& collisionSectorIndex2);
			void GetAdjacentCollisionSectors(const CollisionSector& collisionSector, CollisionSector* (outAdjacentCollisionSectors)[8]);
			Transform2D PredictBodyTransform(const Body2D& body, float deltaTime) const;
			Transform2D PredictTransform(const Transform2D& transform2D, const Float2& linearVelocity, const Float2& linearAcceleration, float angularVelocity, float angularAcceleration, float deltaTime) const;

		private:
			void RenderDebugBody(Rendering::ShapeRendererContext& shapeRendererContext, const Body2D& body) const;
			void RenderDebugCollisionManifold(Rendering::ShapeRendererContext& shapeRendererContext, const CollisionManifold2D& collisionManifold) const;

		public:
			PhysicsObjectPool<Body2D> _bodyPool;

		private:
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
			HashMap<BodyID::RawType, Vector<CollisionManifold2D>> _collisionManifold2DsMap;

		private:
			static constexpr const uint32 kWorldHistoryCapacity = 128;
			WorldHistory _worldHistory;
		};
	}
}


#endif // !_MINT_PHYSICS_PHYSICS_WORLD_H_
