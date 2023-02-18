#pragma once


#ifndef _MINT_GAME_ANIMATION_CORE_H_
#define _MINT_GAME_ANIMATION_CORE_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float4x4.h>


namespace mint
{
	namespace Rendering
	{
		class InstantRenderer;
	}


	namespace Game
	{
		// ### TERMINOLOGY ###
		// Skeleton (Joint)
		// Space: Local, Model, World

		using JointIndexType = int16;

		class SkeletonJoint;
		class SkeletonGenerator;
		class Skeleton;


		class SkeletonJoint
		{
			friend SkeletonGenerator;
			friend Skeleton;

		public:
			static constexpr uint32 kJointNameLengthMax = 32;
			static constexpr JointIndexType kInvalidJointIndex = -1;

		public:
			SkeletonJoint();
			SkeletonJoint(const char* const jointName);
			~SkeletonJoint();

		public:
			const char* GetName() const noexcept;
			bool HasParent() const noexcept;
			JointIndexType GetParentIndex() const noexcept;
			const Float4x4& GetBindPoseLocalMatrix() const noexcept;
			const Float4x4& GetBindPoseModelMatrix() const noexcept;

		private:
			char _name[kJointNameLengthMax];
			JointIndexType _parentIndex;
			Float4x4 _bindPoseLocalMatrix;
			Float4x4 _bindPoseModelMatrix; // TODO: 여기 있는 게 맞을지 Skeleton 으로 옮길지?

		public:
			static const SkeletonJoint kInvalidSkeletonJoint;
		};


		class SkeletonGenerator
		{
		public:
			SkeletonGenerator();
			~SkeletonGenerator();

		public:
			JointIndexType CreateJoint(const JointIndexType parentJointIndex, const char* const jointName, const Float4x4& bindPoseLocalMatrix) noexcept;
			const SkeletonJoint* GetJoint(const JointIndexType jointIndex) const noexcept;
			const Vector<SkeletonJoint>& GetJoints() const noexcept;
			void BuildBindPoseModelSpace() noexcept;

		private:
			Vector<SkeletonJoint> _joints;
		};


		class Skeleton final
		{
		public:
			Skeleton();
			Skeleton(const SkeletonGenerator& skeletonGenerator);
			~Skeleton();

		public:
			bool CreateFromGenerator(const SkeletonGenerator& skeletonGenerator) noexcept;

		public:
			const SkeletonJoint& GetJoint(const JointIndexType jointIndex) const noexcept;

		public:
			void RenderSkeleton(Rendering::InstantRenderer& instantRenderer, const Float4x4& worldMatrix) const noexcept;

		private:
			Vector<SkeletonJoint> _joints;
		};
	}
}


#endif // !_MINT_GAME_ANIMATION_CORE_H_
