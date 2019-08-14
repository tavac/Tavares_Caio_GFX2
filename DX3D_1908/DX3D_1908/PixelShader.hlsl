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
    float4 PL_pos;
    float4 PL_color;
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
    if (PL_color.w < 1.0f)
    {
        psIn.uv.x += sin(vDTime) * cos(vDTime) * (3.1415f / 180);
        psIn.uv.y += (sin(vDTime) * 0.1f);
    }
        float4 outie = (txDiffuse.Sample(samLinear, psIn.uv)) * psIn.color * 5.0f;

        return outie;
    }