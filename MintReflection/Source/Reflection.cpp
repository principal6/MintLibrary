#include <stdafx.h>
#include <MintReflection/Include/Reflection.h>

#include <MintContainer/Include/Vector.hpp>


namespace mint
{
    void ReflectionTesterInner::onConstruction() noexcept
    {
        __noop;
    }

    void ReflectionTesterInner::onDestruction() noexcept
    {
        __noop;
    }


    void ReflectionTesterOuter::onConstruction() noexcept
    {
        __noop;
    }

    void ReflectionTesterOuter::onDestruction() noexcept
    {
        __noop;
    }
}
