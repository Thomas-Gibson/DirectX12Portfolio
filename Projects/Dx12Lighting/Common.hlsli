struct VertexInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 normal : NORMAL;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 normal : NORMAL;
};

cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix viewProjection;
    float time;
}