cbuffer constBuff : register(b0)
{
	matrix vWorld;
	matrix vView;
	matrix vProj;
	// lights and color floats
	float4 vAmbLight;
}

struct PS_Input
{
	float4 pos : SV_POSITION;
	// float4 norm;
	float4 color : COLOR0;
};

float4 main(PS_Input psIn) : SV_Target
{
	float4 outie = vAmbLight*psIn.color;
	return outie;
}