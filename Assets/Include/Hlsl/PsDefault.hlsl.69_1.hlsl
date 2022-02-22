#include <ShaderStructDefinitions>
#include <ShaderConstantBuffers>
#include <ShaderStructuredBufferDefinitions>

StructuredBuffer<SB_InstantMaterial> sbInstantMaterials : register(t1);

float4 main(VS_OUTPUT input) : SV_TARGET
{
    const float4 toLight = normalize(float4(2.0, 3.0, -1.0, 0.0));
    const float ambientIntensity = 0.25;
    const float dotProduct = dot(input._worldNormal, toLight);
    const float3 testDiffuseColor = sbInstantMaterials[0]._diffuseColor.xyz; //float3(0.0, 0.5, 1.0);
    return float4((saturate(dotProduct) + ambientIntensity) * testDiffuseColor, 1.0);
}
