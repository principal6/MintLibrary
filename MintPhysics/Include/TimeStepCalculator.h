#pragma once


#ifndef _MINT_PHYSICS_TIME_STEP_CALCULATOR_H_
#define _MINT_PHYSICS_TIME_STEP_CALCULATOR_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	namespace Physics
	{
		class FixedTimeStepCalculator
		{
		public:
			FixedTimeStepCalculator(const float fixedDeltaTime);

		public:
			void update(const float deltaTime);
		
		public:
			uint32 getSubstepCount() const { return _substepCount; }
			float getLeftOverTime() const { return _leftOverTime; }

		private:
			const float kFixedDeltaTime;

		private:
			float _leftOverTime = 0.0f;
			uint32 _substepCount = 0;
		};
	}
}


#endif // !_MINT_PHYSICS_TIME_STEP_CALCULATOR_H_
