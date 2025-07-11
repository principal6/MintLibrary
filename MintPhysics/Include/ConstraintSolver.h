#pragma once


#ifndef _MINT_PHYSICS_CONSTRAINT_SOLVER_H_
#define _MINT_PHYSICS_CONSTRAINT_SOLVER_H_

#include <MintPhysics/Include/PhysicsCommon.h>

namespace mint
{
	namespace Physics2D
	{
		struct ParticleDistanceConstraint
		{
			BodyID _bodyAID;
			BodyID _bodyBID;
			float _distance; // Desired distance between the two particles
			float _stiffness; // Stiffness of the constraint
		};

		void SolveParticleDistanceConstraint(World& world, float timeStep, const ParticleDistanceConstraint& particleDistanceConstraint);
	}
}

#endif // !_MINT_PHYSICS_CONSTRAINT_SOLVER_H_
