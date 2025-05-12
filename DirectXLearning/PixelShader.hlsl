// Pixel shader for rendering 
struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main(PixelInput input) : SV_TARGET
{    
    // return the color of the pixel
	return input.color;
}