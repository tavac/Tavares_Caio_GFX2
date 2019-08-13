Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer constBuff : register(b0)
{
    matrix vWorld;
    matrix vView;
    matrix vProj;
	// lights and color floats
    float4 vAmbLight;
    float vDTime;
}

cbuffer Dir_LightBuff : register(b1)
{
    float4 DL_dir[2];
    float4 DL_color[2];
}

cbuffer Pnt_LightBuff : register(b2)
{
    float4 PL_pos[1];
    float4 PL_color[1];
}

struct PS_Input
{
    float4 pos : SV_POSITION;
    float4 norm : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

float4 main(PS_Input psIn) : SV_Target
{
    float4 outie = (txDiffuse.Sample(samLinear, psIn.uv)) * psIn.color;

    return outie;
}