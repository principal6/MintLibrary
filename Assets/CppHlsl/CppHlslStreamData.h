#pragma once


#ifndef MINT_CPP_HLSL_STRUCTS_H
#define MINT_CPP_HLSL_STRUCTS_H


#include <MintRenderingBase/Include/CppHlsl/CppHlslTypes.h>


namespace mint
{
    namespace RenderingBase
    {
        // Position
        // Tangent & Bitangent => Normal
        // TexCoord
        struct VS_INPUT
        {
            float4              _positionU;       // position  + texCoord[0]
            float4              _tangentV;        // tangent   + texCoord[1]
            float4              _bitangentW;      // bitangent + texCoord[2]
            uint                _materialId{ 0 };
            uint                _instanceId{ 0 };
        };

        //struct VS_INPUT1
        //{
        //    uint                _materialIdTest{ 0 };
        //    uint                _instanceIdTest{ 0 };
        //};

        struct VS_INPUT_SHAPE
        {
            float4              _position;
            float4              _color;
            float4              _texCoord;
            float4              _info;
        };


        struct VS_OUTPUT
        {
            float4              _screenPosition CPP_HLSL_SEMANTIC_NAME(SV_POSITION);
            float4              _worldPosition;
            float4              _worldNormal;
            float4              _worldTangent;
            float4              _worldBitangent;
            float4              _texCoord;
            uint                _materialId{ 0 };
        };

        struct VS_OUTPUT_SHAPE
        {
            float4              _position CPP_HLSL_SEMANTIC_NAME(SV_POSITION);
            float4              _color;
            float4              _texCoord;
            float4              _info;
            uint                _viewportIndex CPP_HLSL_SEMANTIC_NAME(SV_ViewportArrayIndex);
        };
    }
}


#endif // !MINT_CPP_HLSL_STRUCTS_H
