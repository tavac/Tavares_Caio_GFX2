TextureCube txDiffuse : register(t0);
SamplerState samLinear : register(s0);
cbuffer constBuff : register(b0)
{
    matrix vWorld;
    matrix vView;
    matrix vProj;
    float4 vAmbLight;
    float vDTime;
}

struct PS_Input
{
    float4 pos : SV_POSITION;
    float3 uv : TEXCOORD0;
};

float4 main(PS_Input psIn) : SV_TARGET
{
    
    return (txDiffuse.Sample(samLinear, psIn.uv.xyz));
}