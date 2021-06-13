#pragma once


#ifndef MINT_I_DX_BASE_H
#define MINT_I_DX_BASE_H


#include <atomic>

#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>


namespace mint
{
    namespace RenderingBase
    {
        class GraphicDevice;
        class IDxObject;


        enum class DxShaderType
        {
            VertexShader,
            GeometryShader,
            PixelShader,

            COUNT,
        };


        enum class DxObjectType
        {
            None,
            Pool,
            Resource,
            Shader,
        };


        class DxObjectId final
        {
            friend IDxObject;
            static constexpr uint32         kDxInvalidObjectRawId{ kUint32Max };

        public:
                                            DxObjectId() = default;
                                            DxObjectId(const DxObjectId& rhs) = default;
                                            DxObjectId(DxObjectId&& rhs) = default;
                                            ~DxObjectId() = default;

        public:
            DxObjectId&                     operator=(const DxObjectId& rhs) noexcept = default;
            DxObjectId&                     operator=(DxObjectId&& rhs) noexcept = default;
    
        public:
            MINT_INLINE const bool            operator==(const DxObjectId& rhs) const noexcept
            {
                return _rawId == rhs._rawId;
            }

            MINT_INLINE const bool            operator!=(const DxObjectId& rhs) const noexcept
            {
                return _rawId != rhs._rawId;
            }

            MINT_INLINE const bool            operator<(const DxObjectId& rhs) const noexcept
            {
                return _rawId < rhs._rawId;
            }

            MINT_INLINE const bool            operator>(const DxObjectId& rhs) const noexcept
            {
                return _rawId > rhs._rawId;
            }

        public:
            MINT_INLINE const bool            isValid() const noexcept
            {
                return _rawId != kDxInvalidObjectRawId;
            }

            MINT_INLINE const DxObjectType    getObjectType() const noexcept
            {
                return _objectType;
            }

            MINT_INLINE const bool            isObjectType(const DxObjectType objectType) const noexcept
            {
                return _objectType == objectType;
            }

        private:
            MINT_INLINE void                  assignIdXXX() noexcept
            {
                ++_lastRawId;
                _rawId = _lastRawId;
            }

            MINT_INLINE void                  setObjectTypeXXX(const DxObjectType objectType) noexcept
            {
                _objectType = objectType;
            }

        private:
            uint32                          _rawId{ kDxInvalidObjectRawId };
            DxObjectType                    _objectType{ DxObjectType::None };

        private:
            static std::atomic<uint32>      _lastRawId;
    
        public:
            static const DxObjectId         kInvalidObjectId;
        };


        class IDxObject abstract
        {
        public:
                                        IDxObject(GraphicDevice* const graphicDevice, const DxObjectType objectType) : _graphicDevice{ graphicDevice} { _objectId.setObjectTypeXXX(objectType); }
            virtual                     ~IDxObject() = default;

        public:
            MINT_INLINE const bool        operator==(const DxObjectId& objectId) const noexcept
            {
                return _objectId == objectId;
            }

            MINT_INLINE const bool        operator<(const DxObjectId& objectId) const noexcept
            {
                return _objectId < objectId;
            }

            MINT_INLINE const bool        operator>(const DxObjectId& objectId) const noexcept
            {
                return _objectId > objectId;
            }

        public:
            MINT_INLINE const DxObjectId& getId() const noexcept
            {
                return _objectId;
            }

        protected:
            MINT_INLINE void              assignIdXXX() noexcept
            {
                _objectId.assignIdXXX();
            }

        protected:
            GraphicDevice*              _graphicDevice{ nullptr };
            DxObjectId                  _objectId{};
        };
    }
}


#endif // !MINT_I_DX_BASE_H
