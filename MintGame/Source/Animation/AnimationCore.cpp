#include <stdafx.h>
#include <MintGame/Include/Animation/AnimationCore.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/StringUtil.hpp>

#include <MintRendering/Include/InstantRenderer.h>


namespace mint
{
    namespace Game
    {
        SkeletonJoint::SkeletonJoint()
            : _name{}
            , _parentIndex{ kJointIndexInvalid }
        {
            __noop;
        }

        SkeletonJoint::~SkeletonJoint()
        {
            __noop;
        }

        const char* SkeletonJoint::getName() const noexcept
        {
            return _name;
        }

        const bool SkeletonJoint::hasParent() const noexcept
        {
            return 0 <= _parentIndex;
        }

        const JointIndexType SkeletonJoint::getParentIndex() const noexcept
        {
            return _parentIndex;
        }

        const mint::Float4x4& SkeletonJoint::getBindPoseLocalTm() const noexcept
        {
            return _bindPoseLocalTm;
        }

        const mint::Float4x4& SkeletonJoint::getBindPoseModelTm() const noexcept
        {
            return _bindPoseModelTm;
        }

        Skeleton::Skeleton()
        {
            __noop;
        }

        Skeleton::~Skeleton()
        {
            __noop;
        }

        JointIndexType Skeleton::createJoint(const JointIndexType parentJointIndex, const char* const jointName, const mint::Float4x4& bindPoseLocalTm) noexcept
        {
            if (_joints.empty() == false && parentJointIndex < 0)
            {
                MINT_LOG_ERROR("김장원", "이미 RootJoint 가 있는데 또 만들 수 없습니다!");
                return SkeletonJoint::kJointIndexInvalid;
            }

            if (_joints.empty() == false && _joints.size() <= static_cast<uint32>(parentJointIndex))
            {
                MINT_LOG_ERROR("김장원", "Parent Joint Index 가 잘못되었습니다!");
                return SkeletonJoint::kJointIndexInvalid;
            }

            SkeletonJoint newJoint;
            mint::StringUtil::strcpy(newJoint._name, jointName);
            newJoint._parentIndex = parentJointIndex;
            newJoint._bindPoseLocalTm = bindPoseLocalTm;
            _joints.push_back(newJoint);
            return static_cast<JointIndexType>(_joints.size() - 1);
        }

        const SkeletonJoint* Skeleton::getJoint(const JointIndexType jointIndex) const noexcept
        {
            return (static_cast<uint32>(jointIndex) < _joints.size()) ? &_joints[jointIndex] : nullptr;
        }

        void Skeleton::calculateBindPoseModelTms() noexcept
        {
            const JointIndexType jointCount = static_cast<JointIndexType>(_joints.size());
            for (JointIndexType jointIndex = 0; jointIndex < jointCount; jointIndex++)
            {
                SkeletonJoint& joint = _joints[jointIndex];
                joint._bindPoseModelTm = joint._bindPoseLocalTm;

                JointIndexType parentAt = joint._parentIndex;
                while (0 <= parentAt)
                {
                    SkeletonJoint& parentJoint = _joints[parentAt];
                    joint._bindPoseModelTm.mulAssignReverse(parentJoint._bindPoseLocalTm);
                    parentAt = parentJoint._parentIndex;
                }
            }
        }

        void Skeleton::renderSkeleton(Rendering::InstantRenderer* const instantRenderer, const mint::Float4x4& worldTm) const noexcept
        {
            if (_joints.empty() == true)
            {
                return;
            }

            const JointIndexType jointCount = static_cast<JointIndexType>(_joints.size());
            for (JointIndexType jointIndex = 0; jointIndex < jointCount; jointIndex++)
            {
                const SkeletonJoint& joint = _joints[jointIndex];
                mint::Float4x4 jointWorldTm = worldTm;
                jointWorldTm *= joint._bindPoseModelTm;

                if (joint.hasParent() == true)
                {
                    const SkeletonJoint& parentJoint = _joints[joint._parentIndex];
                    mint::Float4x4 parentJointWorldTm = worldTm;
                    parentJointWorldTm *= parentJoint._bindPoseModelTm;

                    instantRenderer->drawLine(jointWorldTm.getTranslation(), parentJointWorldTm.getTranslation(), mint::RenderingBase::Color::kCyan);
                }

                instantRenderer->drawSphere(jointWorldTm.getTranslation(), 0.03125f, 1, mint::RenderingBase::Color::kMagenta);
            }
        }

    }
}
