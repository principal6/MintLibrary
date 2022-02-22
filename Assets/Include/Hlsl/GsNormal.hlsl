#include <ShaderStructDefinitions>
#include <ShaderConstantBuffers>
#include <ShaderStructuredBufferDefinitions>


[maxvertexcount(4)]
void main(line VS_OUTPUT input[2], inout LineStream<VS_OUTPUT> output)
{
    static const float  kNormalLengthScale  = 0.125;
    static const float4 kNormalRootColor    = float4(0.25, 0.875, 0.125, 1.0);
    static const float4 kNormalTipColor     = float4(0.125, 0.5, 0.125, 1.0);

    VS_OUTPUT element = (VS_OUTPUT)0;
    for (int i = 0; i < 2; ++i)
    {
        const float4 worldPosition = input[i]._worldPosition;
        const float4 worldNormal = input[i]._worldNormal;
        const float4 viewProjectionNormal = mul(worldNormal, _cbViewProjectionMatrix);
        if (viewProjectionNormal.z >= 0.0) // Cull back-facing normals
        {
            return;
        }

        element._worldPosition = worldPosition;
        element._screenPosition = mul(element._worldPosition, _cbViewProjectionMatrix);
        element._texCoord = kNormalRootColor;
        output.Append(element);

        element._worldPosition = worldPosition + (worldNormal * kNormalLengthScale);
        element._screenPosition = mul(element._worldPosition, _cbViewProjectionMatrix);
        element._texCoord = kNormalTipColor;
        output.Append(element);

        output.RestartStrip();
    }
}
