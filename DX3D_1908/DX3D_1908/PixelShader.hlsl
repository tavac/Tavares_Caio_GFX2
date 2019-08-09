cbuffer constBuff : register(b0)
{
	matrix vWorld;
	matrix vView;
	matrix vProj;
	// lights and color floats
	float4 vAmbLight;
}

cbuffer Dir_LightBuff : register(b1)
{
	float4 DL_dir;
	float4 DL_color;
}

struct PS_Input
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL0;
	float4 color : COLOR0;
};

float4 main(PS_Input psIn) : SV_Target
{
	float4 outie;
	// Directional Light
	float LR = saturate(dot((-DL_dir) , psIn.norm));
	outie = (LR * DL_color * psIn.color);
	// Ambient Light
	outie += vAmbLight*psIn.color;
	return outie;
}