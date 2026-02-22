#include "Common.hlsli"

float4 main(PixelInput input) : SV_TARGET
{
    float4 finalColor = input.color;    
    return finalColor;
}