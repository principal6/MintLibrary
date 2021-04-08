#include <ShaderStructDefinitions>
#include <ShaderConstantBuffers>
#include <ShaderStructuredBufferDefinitions>


[maxvertexcount(4)]
void main(triangle VS_OUTPUT input[3], inout LineStream<VS_OUTPUT> output)
{
    static const float kDepthOffset = -0.001;
    static const float4 kEdgeColorA = float4(0.875, 0.875, 0.875, 1.0);
    static const float4 kEdgeColorB = float4(0.75 , 0.75 , 0.75 , 1.0);

    VS_OUTPUT element = (VS_OUTPUT)0;
    float4 viewProjectionNormalAverage = 0.0;
    for (int i = 0; i < 3; ++i)
    {
        const float4 worldPosition = input[i]._worldPosition;
        const float4 worldNormal = input[i]._worldNormal;
        const float4 viewProjectionNormal = mul(worldNormal, _cbViewProjectionMatrix);
        viewProjectionNormalAverage += viewProjectionNormal;
    }
    viewProjectionNormalAverage = normalize(viewProjectionNormalAverage);

    if (viewProjectionNormalAverage.z >= 0.0) // Cull back-facing normals
    {
        return;
    }

    for (int i = 0; i < 3; ++i)
    {
        const int index0 = i + 0;
        const int index1 = (2 == index0) ? 0 : index0 + 1;

        element._worldPosition = input[index0]._worldPosition;
        element._screenPosition = mul(element._worldPosition, _cbViewProjectionMatrix);
        element._screenPosition.z += kDepthOffset;
        element._texCoord = kEdgeColorA;
        output.Append(element);

        element._worldPosition = input[index1]._worldPosition;
        element._screenPosition = mul(element._worldPosition, _cbViewProjectionMatrix);
        element._screenPosition.z += kDepthOffset;
        element._texCoord = kEdgeColorB;
        output.Append(element);

        output.RestartStrip();
    }
}
