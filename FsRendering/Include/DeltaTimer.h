#pragma once


#ifndef FS_DELTA_TIMER_H
#define FS_DELTA_TIMER_H


namespace fs
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


#endif // !FS_DELTA_TIMER_H
