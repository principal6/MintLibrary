#pragma once


#ifndef _MINT_GUI_CONTROL_DATA_HPP_
#define _MINT_GUI_CONTROL_DATA_HPP_


#include <MintRenderingBase/Include/GUI/ControlData.h>

#include <MintContainer/Include/Hash.hpp>


namespace mint
{
    template<>
    inline const uint64 Hasher<Rendering::ControlID>::operator()(const Rendering::ControlID& value) const noexcept
    {
        return computeHash(value.getRawID());
    }
}


#endif // !_MINT_GUI_CONTROL_DATA_HPP_
