#pragma once


#ifndef MINT_I_DX_BASE_H
#define MINT_I_DX_BASE_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>


namespace mint
{
    namespace Rendering
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


        class DxObjectID final
        {
            friend IDxObject;
            static constexpr uint32         kDxInvalidObjectRawID{ kUint32Max };

        public:
                                            DxObjectID() = default;
                                            DxObjectID(const DxObjectID& rhs) = default;
                                            DxObjectID(DxObjectID&& rhs) = default;
                                            ~DxObjectID() = default;

        public:
            DxObjectID&                     operator=(const DxObjectID& rhs) noexcept = default;
            DxObjectID&                     operator=(DxObjectID&& rhs) noexcept = default;
    
        public:
            MINT_INLINE const bool            operator==(const DxObjectID& rhs) const noexcept
            {
                return _rawID == rhs._rawID;
            }

            MINT_INLINE const bool            operator!=(const DxObjectID& rhs) const noexcept
            {
                return _rawID != rhs._rawID;
            }

            MINT_INLINE const bool            operator<(const DxObjectID& rhs) const noexcept
            {
                return _rawID < rhs._rawID;
            }

            MINT_INLINE const bool            operator>(const DxObjectID& rhs) const noexcept
            {
                return _rawID > rhs._rawID;
            }

        public:
            MINT_INLINE const bool            isValid() const noexcept
            {
                return _rawID != kDxInvalidObjectRawID;
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
            MINT_INLINE void                  assignIDXXX() noexcept
            {
                ++_lastRawID;
                _rawID = _lastRawID;
            }

            MINT_INLINE void                  setObjectTypeXXX(const DxObjectType objectType) noexcept
            {
                _objectType = objectType;
            }

        private:
            uint32                          _rawID{ kDxInvalidObjectRawID };
            DxObjectType                    _objectType{ DxObjectType::None };

        private:
            static std::atomic<uint32>      _lastRawID;
    
        public:
            static const DxObjectID         kInvalidObjectID;
        };


        class IDxObject abstract
        {
        public:
                                            IDxObject(GraphicDevice& graphicDevice, const DxObjectType objectType) 
                                                : _graphicDevice{ graphicDevice }
                                                , _objectID{}
                                            {
                                                _objectID.setObjectTypeXXX(objectType); 
                                            }
            virtual                         ~IDxObject() = default;

        public:
            MINT_INLINE const bool          operator==(const DxObjectID& objectID) const noexcept
            {
                return _objectID == objectID;
            }

            MINT_INLINE const bool          operator<(const DxObjectID& objectID) const noexcept
            {
                return _objectID < objectID;
            }

            MINT_INLINE const bool          operator>(const DxObjectID& objectID) const noexcept
            {
                return _objectID > objectID;
            }

        public:
            MINT_INLINE const DxObjectID&   getID() const noexcept
            {
                return _objectID;
            }

        protected:
            MINT_INLINE void                assignIDXXX() noexcept
            {
                _objectID.assignIDXXX();
            }

        protected:
            GraphicDevice&              _graphicDevice;
            DxObjectID                  _objectID;
        };
    }
}


#endif // !MINT_I_DX_BASE_H
