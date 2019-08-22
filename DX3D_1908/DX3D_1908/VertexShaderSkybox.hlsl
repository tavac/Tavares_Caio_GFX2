
cbuffer constBuff : register(b0)
{
    matrix vWorld;
    matrix vView;
    matrix vProj;
	// lights and color floats
    float4 vAmbLight;
    float vDTime;
}

struct skybox_IN
{
    float4 pos : POSITION;
};

struct skybox_OUT
{
    float4 pos : SV_POSITION;
    float3 uv : TEXCOORD0;
};

skybox_OUT main( skybox_IN sb_in )
{
    skybox_OUT sb_out = (skybox_OUT) 0;
    sb_out.pos = mul(sb_in.pos, vWorld);
    sb_out.pos = mul(sb_out.pos, vView);
    sb_out.pos = mul(sb_out.pos, vProj);
    sb_out.uv.xyz = sb_in.pos.xyz;

	return sb_out;
}