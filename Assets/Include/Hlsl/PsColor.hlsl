#include <ShaderStructDefinitions>
#include <ShaderConstantBuffers>
#include <ShaderStructuredBufferDefinitions>

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return _cbDiffuseColor;
}
