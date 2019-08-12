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

struct VS_Input
{
	float4 pos : POSITION;
	float4 norm : NORMAL0;
	float2 uv : TEXCOORD0;
};

struct VS_Output
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL0;
	float2 uv : TEXCOORD0;
};

VS_Output main(VS_Input vsIn)
{
	VS_Output vsOut = (VS_Input)0;
	vsOut.pos = mul(vsIn.pos, vWorld);
	vsOut.norm = mul(vsIn.norm, vWorld);
	vsOut.pos = mul(vsOut.pos, vView);
	vsOut.pos = mul(vsOut.pos, vProj);
	vsOut.uv = vsIn.uv;
	return vsOut;
}