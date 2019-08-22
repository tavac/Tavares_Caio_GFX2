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
    float4 DL_pos;
	float4 DL_dir;
	float4 DL_color;
}

cbuffer Pnt_LightBuff : register(b2)
{
    float4 PL_pos;
    float4 PL_dir;
    float4 PL_color;
}

cbuffer Spt_LightBuff : register(b3)
{
    float4 SL_pos;
    float4 SL_dir;
    float4 SL_color;
};

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
    float4 wPos : PosInWorld;
};

VS_Output main(VS_Input vsIn)
{
	VS_Output vsOut = (VS_Output)0;
	vsOut.pos = mul(vsIn.pos, vWorld);
	vsOut.wPos = mul(vsIn.pos, vWorld);

    vsOut.norm = normalize(mul(vsIn.norm, vWorld));
    vsOut.norm.w = 0;
    vsOut.pos = mul(vsOut.pos, vView);
	vsOut.pos = mul(vsOut.pos, vProj);
    vsOut.color = vsIn.color;
	vsOut.uv = vsIn.uv;
	return vsOut;
}