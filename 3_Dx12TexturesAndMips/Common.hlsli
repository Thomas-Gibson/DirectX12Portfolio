struct VertexInput
{
    float2 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 uv : COLOR;
};