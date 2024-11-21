#pragma once


#ifndef _MINT_GAME_DELTA_TIMER_H_
#define _MINT_GAME_DELTA_TIMER_H_


#include <mutex>

#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	class DeltaTimer final
	{
	private:
		DeltaTimer();

	public:
		~DeltaTimer();

	public:
		static const DeltaTimer& GetInstance() noexcept;
		void ComputeDeltaTime(uint64 frameNumber) const noexcept;
		MINT_INLINE float GetDeltaTimeMs() const noexcept { return _deltaTimeMs; }
		MINT_INLINE float GetDeltaTimeS() const noexcept { return _deltaTimeS; }

	private:
		mutable uint64 _lastFrameNumber;
		mutable float _deltaTimeMs;
		mutable float _deltaTimeS;
		mutable uint64 _prevTimePointUs;
	};
}


#endif // !_MINT_GAME_DELTA_TIMER_H_
