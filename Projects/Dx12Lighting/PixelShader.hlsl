#include "Common.hlsli"

static float3 WHITE = float3(1.0f, 1.0f, 1.0f);

float4 main(PixelInput input) : SV_Target
{
    float3 ObjectColor = WHITE;
    
    float angle = time * 1.0f;
    float3 LightDirection = normalize(float3(sin(angle), 0.5f, cos(angle)));
    
    float DiffuseStrength = max(dot(normalize(input.normal), LightDirection), 0.0f);
    
    float AmbientStrength = 0.15f;
    float3 Ambient = AmbientStrength * ObjectColor;
    
    float3 FinalColor = (Ambient + DiffuseStrength) * ObjectColor;
    
    return float4(FinalColor, 1.0f);
}
