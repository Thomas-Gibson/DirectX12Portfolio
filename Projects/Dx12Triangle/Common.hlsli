#pragma once

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