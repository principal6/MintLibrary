#include <ShaderStructDefinitions>
#include <ShaderConstantBuffers>
#include <ShaderStructuredBufferDefinitions>

StructuredBuffer<SB_Material> sbMaterials : register(t1);

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return sbMaterials[input._materialID]._diffuseColor;
}
