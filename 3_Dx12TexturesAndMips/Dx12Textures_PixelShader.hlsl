#include "Common.hlsli"

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

float4 main(PixelInput input) : SV_TARGET
{    
    float4 finalColor = gTexture.Sample(gSampler, input.uv);
    
    
    return finalColor;
}