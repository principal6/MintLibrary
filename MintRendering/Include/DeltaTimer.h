#pragma once


#ifndef MINT_DELTA_TIMER_H
#define MINT_DELTA_TIMER_H


#include <mutex>

#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
    namespace Rendering
    {
        class DeltaTimer final
        {
        private:
                                        DeltaTimer();

        public:
                                        ~DeltaTimer();
        
        public:
            static const DeltaTimer&    getDeltaTimer() noexcept;
            const float                 computeDeltaTimeS() const noexcept;
            const float                 getDeltaTimeS() const noexcept;

        private:
            mutable float               _deltaTimeS;
            mutable uint64              _prevTimePointMs;
            mutable std::mutex          _mutex;
        };
    }
}


#endif // !MINT_DELTA_TIMER_H
