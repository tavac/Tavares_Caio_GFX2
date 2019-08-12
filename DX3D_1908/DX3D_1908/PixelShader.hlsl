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

struct PS_Input
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL0;
	float2 uv : TEXCOORD0;
};

float4 main(PS_Input psIn) : SV_Target
{
    float4 outie = (txDiffuse.Sample(samLinear, psIn.uv));
    outie += vAmbLight * (txDiffuse.Sample(samLinear, psIn.uv));
    for (int i = 0; i < 2; i++)
    {
        float LR = saturate(dot((-DL_dir[i]), psIn.norm));
        outie += (LR * DL_color[i] * (txDiffuse.Sample(samLinear, psIn.uv)) * sin(vDTime*2));
    }
        return outie;
}