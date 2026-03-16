// Input for the vertex shader.
struct VertexInput
{
    float2 pos : POSITION;
    float4 color : COLOR;
};

// Output of the vertex shader and input for the pixel shader.
struct PixelInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

float4 main(PixelInput input) : SV_TARGET
{
    float4 finalColor = input.color;    
    return finalColor;
}