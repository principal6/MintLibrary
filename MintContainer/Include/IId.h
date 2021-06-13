#pragma once


#ifndef MINT_IID_H
#define MINT_IID_H


namespace mint
{
    class IId abstract
    {
        static constexpr uint32         kInvalidRawId{ kUint32Max };

    public:
                                        IId()                              = default;
                                        IId(const IId& rhs)                = default;
                                        IId(IId&& rhs)                     = default;
        virtual                         ~IId()                             = default;

    public:
        IId&                            operator=(const IId& rhs) noexcept = default;
        IId&                            operator=(IId&& rhs) noexcept      = default;
    
    public:
        const bool                      operator==(const IId& rhs) const noexcept;
        const bool                      operator!=(const IId& rhs) const noexcept;
        const bool                      operator<(const IId& rhs) const noexcept;
        const bool                      operator>(const IId& rhs) const noexcept;

    public:
        const bool                      isValid() const noexcept;

    protected:
        virtual void                    assignRawId(const uint32 rawId) noexcept abstract;

    protected:
        uint32                          _rawId{ kInvalidRawId };
    };
}


#include <MintContainer/Include/IId.inl>


#endif // !MINT_IID_H
