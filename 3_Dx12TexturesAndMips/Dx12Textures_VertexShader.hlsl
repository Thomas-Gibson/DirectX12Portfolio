// Link to: Specifying Root Signatures in HLSL
// https://learn.microsoft.com/en-us/windows/win32/direct3d12/specifying-root-signatures-in-hlsl
#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
                 "CBV(b0)," \
                 "DescriptorTable(SRV(t0), visibility=SHADER_VISIBILITY_PIXEL),"\
                 "StaticSampler(s0)"

#include "Common.hlsli"

cbuffer ConstantBuffer : register(b0)
{
    matrix wvp;
}

[RootSignature(ROOT_SIG)]
PixelInput main(VertexInput input)
{
    PixelInput output;
    output.pos = mul(wvp, float4(input.pos, 0.0f, 1.0f));
    output.uv = input.uv;
    
    return output;
}