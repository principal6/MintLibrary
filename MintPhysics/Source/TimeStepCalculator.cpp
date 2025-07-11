#include <MintPhysics/Include/TimeStepCalculator.h>


namespace mint
{
	namespace PhysicsCommon
	{
		FixedTimeStepCalculator::FixedTimeStepCalculator(const float fixedDeltaTime)
			: kFixedDeltaTime{ fixedDeltaTime }
		{
			__noop;
		}

		void FixedTimeStepCalculator::Update(const float deltaTime)
		{
			_leftOverTime += deltaTime;
			_substepCount = static_cast<uint32>(_leftOverTime / kFixedDeltaTime);
			_leftOverTime -= _substepCount * kFixedDeltaTime;
		}
	}
}
