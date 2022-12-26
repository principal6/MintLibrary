#pragma once


#ifndef _MINT_RENDERING_BASE_CPP_HLSL_STRUCTS_H_
#define _MINT_RENDERING_BASE_CPP_HLSL_STRUCTS_H_


#include <MintRenderingBase/Include/CppHlsl/CppHlslTypes.h>


namespace mint
{
    namespace Rendering
    {
        // Position
        // Tangent & Bitangent => Normal
        // TexCoord
        struct VS_INPUT
        {
            float4              _positionU;       // position  + texCoord[0]
            float4              _tangentV;        // tangent   + texCoord[1]
            float4              _bitangentW;      // bitangent + texCoord[2]
            uint                _materialID{ 0 };
        };

        // ### VS_INPUT on input slot 1
        //struct VS_INPUT1
        //{
        //    uint                _materialIDTest{ 0 };
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
            uint                _materialID{ 0 };
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


#endif // !_MINT_RENDERING_BASE_CPP_HLSL_STRUCTS_H_
