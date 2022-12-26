#pragma once


#ifndef _MINT_RENDERING_DELTA_TIMER_H_
#define _MINT_RENDERING_DELTA_TIMER_H_


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
            float                       computeDeltaTimeS() const noexcept;
            float                       getDeltaTimeS() const noexcept;

        private:
            mutable float               _deltaTimeS;
            mutable uint64              _prevTimePointMs;
            mutable std::mutex          _mutex;
        };
    }
}


#endif // !_MINT_RENDERING_DELTA_TIMER_H_
