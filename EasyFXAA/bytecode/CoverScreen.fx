struct VS_INPUT
{
    uint VertexIndex: SV_VertexID;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};


PS_INPUT VS( VS_INPUT input )
{
    const float4 vertices[3] = {
        float4( 3.0f, 1.0f, 0.0f, 1.0f),
        float4(-1.0f, 1.0f, 0.0f, 1.0f),
        float4(-1.0f,-3.0f, 0.0f, 1.0f),
    };

    PS_INPUT output;
    output.Pos = vertices[input.VertexIndex];
    return output;
}
