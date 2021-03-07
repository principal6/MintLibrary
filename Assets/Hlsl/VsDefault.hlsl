#include <ShaderStructDefinitions>
#include <ShaderConstantBuffers>
#include <ShaderStructuredBufferDefinitions>


VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 inputPosition = float4(input._positionU.xyz, 1.0);
	output._worldPosition = mul(inputPosition, _cbWorldMatrix);

	float4x4 finalTm = mul(_cbWorldMatrix, _cbPerspectiveProjectionMatrix);
	output._screenPosition = mul(inputPosition, finalTm);
	output._screenPosition /= output._screenPosition.w;

	float4 inputTangent = float4(input._tangentV.xyz, 0.0);
	float4 inputBitangent = float4(input._bitangentW.xyz, 0.0);

	output._worldTangent = normalize(mul(inputTangent, finalTm));
	output._worldBitangent = normalize(mul(inputBitangent, finalTm));
	output._worldNormal = float4(normalize(cross(output._worldTangent.xyz, output._worldBitangent.xyz)), 0.0);
	output._texCoord = float4(input._positionU.w, input._tangentV.w, input._bitangentW.w, 0.0);
	return output;
}
