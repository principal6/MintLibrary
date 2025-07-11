#include <MintPhysics/Include/ConstraintSolver.h>
#include <MintPhysics/Include/PhysicsWorld.h>
#include <MintMath/Include/VectorR.hpp>
#include <MintMath/Include/Matrix.hpp>


namespace mint
{
	namespace Physics2D
	{
		void SolveParticleDistanceConstraint(World& world, float timeStep, const ParticleDistanceConstraint& particleDistanceConstraint)
		{
			MINT_ASSERT(particleDistanceConstraint._bodyAID.IsValid(), "!!!");
			MINT_ASSERT(particleDistanceConstraint._bodyBID.IsValid(), "!!!");

			const float kBeta = 0.2f; // or 0.1 ~ 0.4, tuning parameter

			Body& bodyA = world.AccessBody(particleDistanceConstraint._bodyAID);
			Body& bodyB = world.AccessBody(particleDistanceConstraint._bodyBID);
			Transform2D& transformA = bodyA._transform2D;
			Transform2D& transformB = bodyB._transform2D;

			VectorF<2> xA{ transformA._translation._x, transformA._translation._y };
			VectorF<2> xB{ transformB._translation._x, transformB._translation._y };
			VectorF<2> r = xB - xA;
			const float C = r.Dot(r) - particleDistanceConstraint._distance * particleDistanceConstraint._distance;

			VectorF<2> gradC_xA = -2.0f * r;
			VectorF<2> gradC_xB = 2.0f * r;
			MatrixF<4, 1> JT; // == gradC
			JT.SetElement(0, 0, gradC_xA.X());
			JT.SetElement(1, 0, gradC_xA.Y());
			JT.SetElement(2, 0, gradC_xB.X());
			JT.SetElement(3, 0, gradC_xB.Y());

			MatrixF<1, 4> J = JT.Transpose();
			VectorF<2> vA{ bodyA._linearVelocity._x, bodyA._linearVelocity._y };
			VectorF<2> vB{ bodyB._linearVelocity._x, bodyB._linearVelocity._y };
			VectorF<4> v;
			v.SetComponent(0, vA.X());
			v.SetComponent(1, vA.Y());
			v.SetComponent(2, vB.X());
			v.SetComponent(3, vB.Y());

			const float dCdt = J * v;

			const float inverseMassA = bodyA._inverseMass;
			const float inverseMassB = bodyB._inverseMass;
			MatrixF<4, 4> inverseM;
			inverseM.SetElement(0, 0, inverseMassA);
			inverseM.SetElement(1, 1, inverseMassA);
			inverseM.SetElement(2, 2, inverseMassB);
			inverseM.SetElement(3, 3, inverseMassB);

			// P = M * delta_v 이므로
			// => delta_v = inverseM * P
			// 
			// 또한 P = J^T * lambda 이므로
			// delta_v = inverseM * J^T * lambda

			// impulse 를 가한 뒤에도 Constraint 가 유지되어야 하므로
			// J(v + delta_v) = 0 이어야 한다.
			// => J * v + J * delta_v = 0
			// => J * delta_v = -J * v
			// => J * inverseM * J^T * lambda = -J * v
			// lambda = (-J * v) / (J * inverseM * J^T)

			float numerator = -J * v;
			// Baumgarte Stabilization
			const float kPositionalCorrection = kBeta * C / timeStep;
			numerator -= kPositionalCorrection;

			const float denominator = (J * inverseM * JT);
			if (denominator == 0.0f)
			{
				return;
			}
			const float lambda = numerator / denominator;

			const auto delta_v = inverseM * JT * lambda;
			VectorF<2> delta_vA;
			VectorF<2> delta_vB;
			delta_vA.SetComponent(0, delta_v.GetElement(0, 0));
			delta_vA.SetComponent(1, delta_v.GetElement(1, 0));
			delta_vB.SetComponent(0, delta_v.GetElement(2, 0));
			delta_vB.SetComponent(1, delta_v.GetElement(3, 0));

			bodyA._linearVelocity += Float2(delta_vA.X(), delta_vA.Y());
			bodyB._linearVelocity += Float2(delta_vB.X(), delta_vB.Y());
		}
	}
}