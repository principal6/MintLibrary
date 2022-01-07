#pragma once


#ifndef MINT_IID_H
#define MINT_IID_H


namespace mint
{
    class IID abstract
    {
        static constexpr uint32         kInvalidRawId{ kUint32Max };

    public:
                                        IID()                              = default;
                                        IID(const IID& rhs)                = default;
                                        IID(IID&& rhs)                     = default;
        virtual                         ~IID()                             = default;

    public:
        IID&                            operator=(const IID& rhs) noexcept = default;
        IID&                            operator=(IID&& rhs) noexcept      = default;
    
    public:
        const bool                      operator==(const IID& rhs) const noexcept;
        const bool                      operator!=(const IID& rhs) const noexcept;
        const bool                      operator<(const IID& rhs) const noexcept;
        const bool                      operator>(const IID& rhs) const noexcept;

    public:
        const bool                      isValid() const noexcept;

    protected:
        virtual void                    assignRawId(const uint32 rawId) noexcept abstract;

    protected:
        uint32                          _rawId{ kInvalidRawId };
    };
}


#include <MintContainer/Include/IID.inl>


#endif // !MINT_IID_H
