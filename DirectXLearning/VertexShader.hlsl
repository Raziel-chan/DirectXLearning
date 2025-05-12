//define our constant buffer
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};

//vertex shader
struct VertexInput
{
    float3 position : POSITION; // Vertex position
    float4 color : COLOR; // Vertex color
};

struct VertexOutput
{
    float4 position : SV_POSITION; // Screen position (SV means System Value)
    float4 color : COLOR; // Vertex color
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    
    // transform position from 3D to screen coordinates
    float4 pos = float4(input.position, 1.0f);
    
    // apply the world, view, and projection matrices
    pos = mul(pos, World);
    pos = mul(pos, View);
    pos = mul(pos, Projection);
    // pass the color through
    output.position = pos;
    output.color = input.color;
    return output;
}