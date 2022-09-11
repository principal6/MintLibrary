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
        class SkeletonGenerator;
        class Skeleton;


        class SkeletonJoint
        {
            friend SkeletonGenerator;
            friend Skeleton;

        public:
            static constexpr uint32         kJointNameLengthMax = 32;
            static constexpr JointIndexType kInvalidJointIndex = -1;

        public:
                                        SkeletonJoint();
                                        SkeletonJoint(const char* const jointName);
                                        ~SkeletonJoint();

        public:
            const char*                 getName() const noexcept;
            bool                        hasParent() const noexcept;
            JointIndexType              getParentIndex() const noexcept;
            const Float4x4&             getBindPoseLocalMatrix() const noexcept;
            const Float4x4&             getBindPoseModelMatrix() const noexcept;

        private:
            char                        _name[kJointNameLengthMax];
            JointIndexType              _parentIndex;
            Float4x4                    _bindPoseLocalMatrix;
            Float4x4                    _bindPoseModelMatrix; // TODO: 여기 있는 게 맞을지 Skeleton 으로 옮길지?

        public:
            static const SkeletonJoint  kInvalidSkeletonJoint;
        };


        class SkeletonGenerator
        {
        public:
                                            SkeletonGenerator();
                                            ~SkeletonGenerator();

        public:
            JointIndexType                  createJoint(const JointIndexType parentJointIndex, const char* const jointName, const Float4x4& bindPoseLocalMatrix) noexcept;
            const SkeletonJoint*            getJoint(const JointIndexType jointIndex) const noexcept;
            const Vector<SkeletonJoint>&    getJoints() const noexcept;
            void                            buildBindPoseModelSpace() noexcept;

        private:
            Vector<SkeletonJoint>           _joints;
        };


        class Skeleton final
        {
        public:
                                            Skeleton();
                                            Skeleton(const SkeletonGenerator& skeletonGenerator);
                                            ~Skeleton();

        public:
            bool                            createFromGenerator(const SkeletonGenerator& skeletonGenerator) noexcept;

        public:
            const SkeletonJoint&            getJoint(const JointIndexType jointIndex) const noexcept;

        public:
            void                            renderSkeleton(Rendering::InstantRenderer& instantRenderer, const Float4x4& worldMatrix) const noexcept;

        private:
            Vector<SkeletonJoint>           _joints;
        };
    }
}


#endif // !MINT_GAME_ANIMATION_CORE_H
