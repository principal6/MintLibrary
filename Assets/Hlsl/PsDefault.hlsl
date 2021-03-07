#include <ShaderStructDefinitions>
#include <ShaderConstantBuffers>
#include <ShaderStructuredBufferDefinitions>


float4 main(VS_OUTPUT input) : SV_TARGET
{
	const float4 toLight = normalize(float4(0.0, 1.0, -1.0, 0.0));
	const float ambientIntensity = 0.125;
	const float dotProduct = dot(input._worldNormal, toLight);
	return float4((saturate(dotProduct) + ambientIntensity) * float3(0.0, 0.5, 1.0), 1.0);
}
