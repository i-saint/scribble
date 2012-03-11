#define FXAA_PC 1
//#define FXAA_HLSL_4 1
//#define FXAA_QUALITY__PRESET 12


#include "Fxaa3_11.h"


Texture2D g_TexRGBL : register( t0 );
SamplerState g_Sampler : register( s0 );

cbuffer FXAAParams : register( b0 )
{
    float2 g_RcpFrame;
    float g_Subpix;            // default: 0.75f
    float g_EdgeThreshold;     // default: 0.166f
    float g_EdgeThresholdMin;  // default: 0.0833f
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};

float4 PS( PS_INPUT input) : SV_Target
{
    float4 not_used = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float2 texcoord = input.Pos.xy * g_RcpFrame.xy;
    FxaaTex RGBL;
    RGBL.tex = g_TexRGBL;
    RGBL.smpl = g_Sampler;

    return FxaaPixelShader(
        texcoord, not_used, RGBL, RGBL, RGBL, g_RcpFrame.xy, not_used, not_used, not_used,
        g_Subpix, g_EdgeThreshold, g_EdgeThresholdMin, not_used.x, not_used.x, not_used.x, not_used);
}
