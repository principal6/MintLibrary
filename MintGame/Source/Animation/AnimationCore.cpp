﻿#include <stdafx.h>
#include <MintGame/Include/Animation/AnimationCore.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/StringUtil.hpp>

#include <MintRendering/Include/InstantRenderer.h>


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
            StringUtil::copy(_name, jointName);
        }

        SkeletonJoint::~SkeletonJoint()
        {
            __noop;
        }

        const char* SkeletonJoint::getName() const noexcept
        {
            return _name;
        }

        bool SkeletonJoint::hasParent() const noexcept
        {
            return (_parentIndex >= 0);
        }

        JointIndexType SkeletonJoint::getParentIndex() const noexcept
        {
            return _parentIndex;
        }

        const Float4x4& SkeletonJoint::getBindPoseLocalMatrix() const noexcept
        {
            return _bindPoseLocalMatrix;
        }

        const Float4x4& SkeletonJoint::getBindPoseModelMatrix() const noexcept
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

        JointIndexType SkeletonGenerator::createJoint(const JointIndexType parentJointIndex, const char* const jointName, const Float4x4& bindPoseLocalMatrix) noexcept
        {
            if (_joints.empty() == false && parentJointIndex < 0)
            {
                MINT_LOG_ERROR("이미 RootJoint 가 있는데 또 만들 수 없습니다!");
                return SkeletonJoint::kInvalidJointIndex;
            }

            if (_joints.empty() == false && _joints.size() <= static_cast<uint32>(parentJointIndex))
            {
                MINT_LOG_ERROR("Parent Joint Index 가 잘못되었습니다!");
                return SkeletonJoint::kInvalidJointIndex;
            }

            SkeletonJoint newJoint;
            StringUtil::copy(newJoint._name, jointName);
            newJoint._parentIndex = parentJointIndex;
            newJoint._bindPoseLocalMatrix = bindPoseLocalMatrix;
            _joints.push_back(newJoint);
            return static_cast<JointIndexType>(_joints.size() - 1);
        }

        const SkeletonJoint* SkeletonGenerator::getJoint(const JointIndexType jointIndex) const noexcept
        {
            return (static_cast<uint32>(jointIndex) < _joints.size()) ? &_joints[jointIndex] : nullptr;
        }

        const Vector<SkeletonJoint>& SkeletonGenerator::getJoints() const noexcept
        {
            return _joints;
        }

        void SkeletonGenerator::buildBindPoseModelSpace() noexcept
        {
            const JointIndexType jointCount = static_cast<JointIndexType>(_joints.size());
            for (JointIndexType jointIndex = 0; jointIndex < jointCount; ++jointIndex)
            {
                SkeletonJoint& joint = _joints[jointIndex];
                joint._bindPoseModelMatrix = joint._bindPoseLocalMatrix;

                if (joint.hasParent())
                {
                    SkeletonJoint& parentJoint = _joints[joint._parentIndex];
                    joint._bindPoseModelMatrix.mulAssignReverse(parentJoint._bindPoseModelMatrix);
                }
            }
        }


        Skeleton::Skeleton()
        {
            __noop;
        }

        Skeleton::Skeleton(const SkeletonGenerator& skeletonGenerator)
        {
            createFromGenerator(skeletonGenerator);
        }

        Skeleton::~Skeleton()
        {
            __noop;
        }

        bool Skeleton::createFromGenerator(const SkeletonGenerator& skeletonGenerator) noexcept
        {
            _joints = skeletonGenerator.getJoints();
            return true;
        }

        const SkeletonJoint& Skeleton::getJoint(const JointIndexType jointIndex) const noexcept
        {
            return (static_cast<uint32>(jointIndex) < _joints.size()) ? _joints[jointIndex] : SkeletonJoint::kInvalidSkeletonJoint;
        }

        void Skeleton::renderSkeleton(Rendering::InstantRenderer& instantRenderer, const Float4x4& worldMatrix) const noexcept
        {
            if (_joints.empty() == true)
            {
                return;
            }

            const JointIndexType jointCount = static_cast<JointIndexType>(_joints.size());
            for (JointIndexType jointIndex = 0; jointIndex < jointCount; ++jointIndex)
            {
                const SkeletonJoint& joint = _joints[jointIndex];
                Float4x4 jointWorldMatrix = worldMatrix;
                jointWorldMatrix *= joint._bindPoseModelMatrix;

                if (joint.hasParent() == true)
                {
                    const SkeletonJoint& parentJoint = _joints[joint._parentIndex];
                    Float4x4 parentJointWorldMatrix = worldMatrix;
                    parentJointWorldMatrix *= parentJoint._bindPoseModelMatrix;

                    instantRenderer.drawLine(jointWorldMatrix.getTranslation(), parentJointWorldMatrix.getTranslation(), Rendering::Color::kCyan);
                }

                instantRenderer.drawGeoSphere(jointWorldMatrix.getTranslation(), 0.03125f, 1, Rendering::Color::kMagenta);
            }
        }

    }
}
