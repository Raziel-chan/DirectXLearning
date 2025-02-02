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
    output.position = float4(input.position, 1.0f);
    // pass the color through
    output.color = input.color;
    return output;
}

/*float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}*/