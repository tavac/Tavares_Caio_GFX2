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
};

float4 main(PS_Input psIn) : SV_Target
{
    float4 outie = (txDiffuse.Sample(samLinear, psIn.uv));
    outie += vAmbLight * (txDiffuse.Sample(samLinear, psIn.uv));

    for (int d = 0; d < 2; d++)
    {
        float LR = saturate(dot((-DL_dir[d]), psIn.norm));
        outie += (LR * DL_color[d] * (txDiffuse.Sample(samLinear, psIn.uv)) * sin(vDTime * 2));
    }
    //for (int p = 0; p < 1; p++)
    //{
        float4 LightDir = normalize(PL_pos[0] - psIn.pos);
        float LightRatio = saturate(dot(LightDir, psIn.norm));
        float attenutation = 1.0f - saturate((length(PL_pos[0] - psIn.pos) / 5.0f));
        LightRatio = (attenutation * attenutation) * LightRatio;
        outie += LightRatio * PL_color[0] * (txDiffuse.Sample(samLinear, psIn.uv));
    //}
    return outie;
}