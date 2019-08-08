cbuffer constBuff : register(b0)
{
	matrix vWorld;
	matrix vView;
	matrix vProj;
	// lights and color floats
}

struct VS_Input
{
	float4 pos : POSITION;
	// float4 norm;
	// float4 color;
};

float4 main(VS_Input vsIn) : SV_POSITION
{
	VS_Input vsOut = (VS_Input)0;
	vsOut.pos = mul(vsIn.pos, vWorld);
	vsOut.pos = mul(vsOut.pos, vView);
	vsOut.pos = mul(vsOut.pos, vProj);
	return vsOut.pos;
}