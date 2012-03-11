Texture2D g_TexRGBA : register( t0 );

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};

float4 PS( PS_INPUT input) : SV_Target
{
    float4 rgba = g_TexRGBA.Load(int3(input.Pos.xy, 0));
    float4 result = float4(rgba.rgb, dot(rgba.rgb, float3(0.299f, 0.587f, 0.114f)));
    return result;
}
