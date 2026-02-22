// Link to: Specifying Root Signatures in HLSL
// https://learn.microsoft.com/en-us/windows/win32/direct3d12/specifying-root-signatures-in-hlsl
#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)"    // Input assembler is required for this shader
#include "Common.hlsli"

[RootSignature(ROOT_SIG)]
PixelInput main(VertexInput input)
{
    PixelInput output;
    output.pos = float4(input.pos, 0.0f, 1.0f);
    output.color = input.color;
    
    return output;
}