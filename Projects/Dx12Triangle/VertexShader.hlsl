// Link to: Specifying Root Signatures in HLSL
// https://learn.microsoft.com/en-us/windows/win32/direct3d12/specifying-root-signatures-in-hlsl
#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)"    // Input assembler is required for this shader
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

[RootSignature(ROOT_SIG)]
PixelInput main(VertexInput input)
{
    PixelInput output;
    output.pos = float4(input.pos, 0.0f, 1.0f);
    output.color = input.color;
    
    return output;
}