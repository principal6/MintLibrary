﻿#pragma once


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
		float ComputeDeltaTimeSec() const noexcept;
		float GetDeltaTimeSec() const noexcept;

	private:
		mutable float _deltaTimeS;
		mutable uint64 _prevTimePointUs;
		mutable std::mutex _mutex;
	};
}


#endif // !_MINT_GAME_DELTA_TIMER_H_
