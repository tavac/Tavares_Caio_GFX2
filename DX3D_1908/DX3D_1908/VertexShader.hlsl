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

struct VS_Input
{
	float4 pos : POSITION;
	float4 norm : NORMAL0;
	float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

struct VS_Output
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL0;
	float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

VS_Output main(VS_Input vsIn)
{
	VS_Output vsOut = (VS_Input)0;
	vsOut.pos = mul(vsIn.pos, vWorld);
	vsOut.norm = mul(vsIn.norm, vWorld);
    
    vsOut.color = vAmbLight;

    for (int d = 0; d < 2; d++)
    {
        float LR = saturate(dot((-DL_dir[d]), vsOut.norm));
        vsOut.color += (LR * DL_color[d] * sin(vDTime * 2));
    }
    //for (int p = 0; p < 1; p++)
    //{
    float4 LightDir = normalize(PL_pos - vsOut.pos);
    float LightRatio = saturate(dot(LightDir, vsOut.norm));
    float attenutation = 1.0f - saturate((length(PL_pos - vsOut.pos) / 10.0f));
    LightRatio = (attenutation * attenutation) * LightRatio;
    vsOut.color += lerp(float4(0, 0, 0, 0), PL_color, LightRatio);
    //}

	vsOut.pos = mul(vsOut.pos, vView);
	vsOut.pos = mul(vsOut.pos, vProj);
	vsOut.uv = vsIn.uv;
	return vsOut;
}