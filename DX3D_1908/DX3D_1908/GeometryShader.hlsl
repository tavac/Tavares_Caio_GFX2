Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer constBuff : register(b0)
{
    matrix vWorld;
    matrix vView;
    matrix vCam;
    matrix vPersProj;
    matrix vOrthProj;
	// lights and color floats
    float4 vAmbLight;
    float vDTime;
}

struct GS_Input
{
    float4 pos : SV_POSITION;
    float4 norm : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
    float4 wPos : PosInWorld;
};

struct GS_Output
{
    float4 pos : SV_POSITION;
    float4 norm : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
    float4 wPos : PosInWorld;
};

[maxvertexcount(3)]
void main(
	point GS_Input input[1], 
	inout TriangleStream< GS_Output > output
)
{
    GS_Output verts[3] = { input[0], input[0], input[0] };

    for (uint v = 0; v < 3u; v++)
    {
        verts[v].pos = mul(verts[v].pos, vView);
        verts[v].pos = mul(verts[v].pos, vPersProj);
    }
    //verts[0].color = float4(1, 0, 0, 1);
    //verts[0].pos = float4(0, 0, 0, 1);
    //verts[1].color = float4(0, 1, 0, 1);
    //verts[1].pos = float4(0, 0, 0, 1);
    //verts[2].color = float4(0, 0, 1, 1);
    //verts[2].pos = float4(0, 0, 0, 1);

    //verts[0].pos.xyz = input[0].xyz;
    //verts[0].pos.x -= 0.5f;

    //verts[1].pos.xyz = input[1].xyz;
    //verts[1].pos.x += 0.5f;

    //verts[2].pos = verts[0].pos;
    //verts[2].pos.x += 10.0f;


    //float4x4 mVP = mul(vView, vPersProj);
        for (uint i = 0; i < 3; i++)
        {
            output.Append(verts[0]);
            output.Append(verts[1]);
            output.Append(verts[2]);
            output.RestartStrip();

        }
}