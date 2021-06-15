#include <ShaderStructDefinitions>
#include <ShaderConstantBuffers>
#include <ShaderStructuredBufferDefinitions>

StructuredBuffer<SB_InstantMaterial> sbInstantMaterials : register(t1);

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return sbInstantMaterials[0]._diffuseColor;
}
