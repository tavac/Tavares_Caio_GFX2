Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer constBuff : register(b0)
{
	matrix vWorld;
	matrix vView;
	matrix vPersProj;
	matrix vOrthProj;
	// lights and color floats
	float4 vAmbLight;
    float vDTime;
}

cbuffer Dir_LightBuff : register(b1)
{
    //matrix DL_space;
    //matrix DL_orthoProj;
    //float DL_ZBuff[] = { 0 };
    float4 DL_pos;
    float4 DL_dir;
    float4 DL_color;
}

cbuffer Pnt_LightBuff : register(b2)
{
    //matrix PL_space;
    //matrix PL_orthoProj;
    //float PL_ZBuff[] = { 0 };
    float4 PL_pos;
    float4 PL_dir;
    float4 PL_color;
}

cbuffer Spt_LightBuff : register(b3)
{
    //matrix SL_space;
    //matrix SL_orthoProj;
    //float SL_ZBuff[] = { 0 };
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
    //matrix _DL_space = DL_space;
    //matrix _DL_orthoProj = DL_orthoProj;
    //float _DL_ZBuff[] = DL_ZBuff;
    //float4 _DL_pos = DL_pos;
    //float4 _DL_dir = DL_dir;
    //float4 _DL_color = DL_color;

	VS_Output vsOut = (VS_Output)0;
	vsOut.pos = mul(vsIn.pos, vWorld);
	vsOut.wPos = mul(vsIn.pos, vWorld);

    //vsIn.norm = clamp(vsIn.norm, -1.0f, 1.0f);
    vsIn.norm.w = 0;
    vsOut.norm = mul(vsIn.norm, vWorld);
    vsOut.norm = normalize(vsOut.norm);
    //vsOut.norm.w = 0;
    vsOut.pos = mul(vsOut.pos, vView);
	vsOut.pos = mul(vsOut.pos, vPersProj);
    vsOut.color = vsIn.color;
	vsOut.uv = vsIn.uv;
	return vsOut;
}