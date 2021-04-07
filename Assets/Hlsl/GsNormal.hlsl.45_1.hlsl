#include <ShaderStructDefinitions>
#include <ShaderConstantBuffers>
#include <ShaderStructuredBufferDefinitions>


[maxvertexcount(4)]
void main(line VS_OUTPUT input[2], inout LineStream<VS_OUTPUT> output)
{
	float4x4 finalTm = mul(_cbWorldMatrix, _cbViewProjectionMatrix);
	
	VS_OUTPUT element = (VS_OUTPUT)0;
	for (int i = 0; i < 2; ++i)
	{
		element._worldPosition = input[i]._worldPosition;
		element._screenPosition = mul(element._worldPosition, finalTm);
		element._texCoord = float4(1, 1, 0, 1); // Use _texCoord as color
		output.Append(element);

		element._worldPosition = input[i]._worldPosition + input[i]._worldNormal;
		element._screenPosition = mul(element._worldPosition, finalTm);
		output.Append(element);

		output.RestartStrip();
	}
}
