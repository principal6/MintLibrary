#pragma once


#ifndef MINT_GAME_ANIMATION_CORE_H
#define MINT_GAME_ANIMATION_CORE_H


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
        class Skeleton;


        class SkeletonJoint
        {
            friend Skeleton;

            static constexpr uint32         kBoneNameLengthMax = 32;
            static constexpr JointIndexType kJointIndexInvalid = -1;

        public:
                                        SkeletonJoint();
                                        ~SkeletonJoint();

        public:
            const char*                 getName() const noexcept;
            const bool                  hasParent() const noexcept;
            const JointIndexType        getParentIndex() const noexcept;
            const mint::Float4x4&       getBindPoseLocalTm() const noexcept;
            const mint::Float4x4&       getBindPoseModelTm() const noexcept;

        private:
            char                        _name[kBoneNameLengthMax];
            JointIndexType              _parentIndex;
            mint::Float4x4              _bindPoseLocalTm;
            mint::Float4x4              _bindPoseModelTm; // TODO: 여기 있는 게 맞을지 Skeleton 으로 옮길지?
        };


        class Skeleton final
        {
        public:
                                                Skeleton();
                                                ~Skeleton();

        public:
            JointIndexType                      createJoint(const JointIndexType parentJointIndex, const char* const jointName, const mint::Float4x4& bindPoseLocalTm) noexcept;
            const SkeletonJoint*                getJoint(const JointIndexType jointIndex) const noexcept;

        public:
            void                                calculateBindPoseModelTms() noexcept;

        public:
            void                                renderSkeleton(Rendering::InstantRenderer* const instantRenderer, const mint::Float4x4& worldTm) const noexcept;

        private:
            mint::Vector<SkeletonJoint>         _joints;
        };
    }
}


#endif // !MINT_GAME_ANIMATION_CORE_H
