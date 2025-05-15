#include <MintGame/Include/Animation/AnimationCore.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/StringUtil.hpp>

#include <MintRendering/Include/Utilities/InstantRenderer.h>


namespace mint
{
	namespace Game
	{
		const SkeletonJoint SkeletonJoint::kInvalidSkeletonJoint = SkeletonJoint("INVALID_JOINT");
		SkeletonJoint::SkeletonJoint()
			: _name{}
			, _parentIndex{ kInvalidJointIndex }
		{
			__noop;
		}

		SkeletonJoint::SkeletonJoint(const char* const jointName)
			: _parentIndex{ kInvalidJointIndex }
		{
			StringUtil::Copy(_name, jointName);
		}

		SkeletonJoint::~SkeletonJoint()
		{
			__noop;
		}

		const char* SkeletonJoint::GetName() const noexcept
		{
			return _name;
		}

		bool SkeletonJoint::HasParent() const noexcept
		{
			return (_parentIndex >= 0);
		}

		JointIndexType SkeletonJoint::GetParentIndex() const noexcept
		{
			return _parentIndex;
		}

		const Float4x4& SkeletonJoint::GetBindPoseLocalMatrix() const noexcept
		{
			return _bindPoseLocalMatrix;
		}

		const Float4x4& SkeletonJoint::GetBindPoseModelMatrix() const noexcept
		{
			return _bindPoseModelMatrix;
		}


		SkeletonGenerator::SkeletonGenerator()
		{
			__noop;
		}

		SkeletonGenerator::~SkeletonGenerator()
		{
			__noop;
		}

		JointIndexType SkeletonGenerator::CreateJoint(const JointIndexType parentJointIndex, const char* const jointName, const Float4x4& bindPoseLocalMatrix) noexcept
		{
			if (_joints.IsEmpty() == false && parentJointIndex < 0)
			{
				MINT_LOG_ERROR("이미 RootJoint 가 있는데 또 만들 수 없습니다!");
				return SkeletonJoint::kInvalidJointIndex;
			}

			if (_joints.IsEmpty() == false && _joints.Size() <= static_cast<uint32>(parentJointIndex))
			{
				MINT_LOG_ERROR("Parent Joint Index 가 잘못되었습니다!");
				return SkeletonJoint::kInvalidJointIndex;
			}

			SkeletonJoint newJoint;
			StringUtil::Copy(newJoint._name, jointName);
			newJoint._parentIndex = parentJointIndex;
			newJoint._bindPoseLocalMatrix = bindPoseLocalMatrix;
			_joints.PushBack(newJoint);
			return static_cast<JointIndexType>(_joints.Size() - 1);
		}

		const SkeletonJoint* SkeletonGenerator::GetJoint(const JointIndexType jointIndex) const noexcept
		{
			return (static_cast<uint32>(jointIndex) < _joints.Size()) ? &_joints[jointIndex] : nullptr;
		}

		const Vector<SkeletonJoint>& SkeletonGenerator::GetJoints() const noexcept
		{
			return _joints;
		}

		void SkeletonGenerator::BuildBindPoseModelSpace() noexcept
		{
			const JointIndexType jointCount = static_cast<JointIndexType>(_joints.Size());
			for (JointIndexType jointIndex = 0; jointIndex < jointCount; ++jointIndex)
			{
				SkeletonJoint& joint = _joints[jointIndex];
				joint._bindPoseModelMatrix = joint._bindPoseLocalMatrix;

				if (joint.HasParent())
				{
					SkeletonJoint& parentJoint = _joints[joint._parentIndex];
					joint._bindPoseModelMatrix.MulAssignReverse(parentJoint._bindPoseModelMatrix);
				}
			}
		}


		Skeleton::Skeleton()
		{
			__noop;
		}

		Skeleton::Skeleton(const SkeletonGenerator& skeletonGenerator)
		{
			CreateFromGenerator(skeletonGenerator);
		}

		Skeleton::~Skeleton()
		{
			__noop;
		}

		bool Skeleton::CreateFromGenerator(const SkeletonGenerator& skeletonGenerator) noexcept
		{
			_joints = skeletonGenerator.GetJoints();
			return true;
		}

		const SkeletonJoint& Skeleton::GetJoint(const JointIndexType jointIndex) const noexcept
		{
			return (static_cast<uint32>(jointIndex) < _joints.Size()) ? _joints[jointIndex] : SkeletonJoint::kInvalidSkeletonJoint;
		}

		void Skeleton::RenderSkeleton(Rendering::InstantRenderer& instantRenderer, const Float4x4& worldMatrix) const noexcept
		{
			if (_joints.IsEmpty() == true)
			{
				return;
			}

			const JointIndexType jointCount = static_cast<JointIndexType>(_joints.Size());
			for (JointIndexType jointIndex = 0; jointIndex < jointCount; ++jointIndex)
			{
				const SkeletonJoint& joint = _joints[jointIndex];
				Float4x4 jointWorldMatrix = worldMatrix;
				jointWorldMatrix *= joint._bindPoseModelMatrix;

				if (joint.HasParent() == true)
				{
					const SkeletonJoint& parentJoint = _joints[joint._parentIndex];
					Float4x4 parentJointWorldMatrix = worldMatrix;
					parentJointWorldMatrix *= parentJoint._bindPoseModelMatrix;

					instantRenderer.DrawLine(jointWorldMatrix.GetTranslation(), parentJointWorldMatrix.GetTranslation(), Color::kCyan);
				}

				instantRenderer.DrawGeoSphere(jointWorldMatrix.GetTranslation(), 0.03125f, 1, Color::kMagenta);
			}
		}

	}
}
