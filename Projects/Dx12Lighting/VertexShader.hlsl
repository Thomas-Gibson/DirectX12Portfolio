// Link to: Specifying Root Signatures in HLSL
// https://learn.microsoft.com/en-us/windows/win32/direct3d12/specifying-root-signatures-in-hlsl
#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
                    "CBV(b0)"

#include "Common.hlsli"

[RootSignature(ROOT_SIG)]
PixelInput main(VertexInput input)
{
    PixelInput output;
    
    float4 worldPos = mul(world, float4(input.position, 1.0f));
    output.position = mul(viewProjection, worldPos);
    output.uv = input.uv;
    
    float4 normal = mul(view, float4(input.normal, 1.0f));
    output.normal = normalize(normal).xyz;
    
    return output;
}