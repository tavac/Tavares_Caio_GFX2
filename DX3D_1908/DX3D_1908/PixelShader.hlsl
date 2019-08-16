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

cbuffer gSptLightBuff : register(b3)
{
    float4 SL_pos;
    float4 SL_coneDir;
    float4 SL_color;
    float4 SL_coneWidth;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float4 norm : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
    float4 wPos : PosInWorld;
};

float4 main(PS_Input psIn) : SV_Target
{
    float4 outie = { 0, 0, 0, 0 };
    if (PL_color.w < 1.0f)
    {
        psIn.uv.x += sin(vDTime) * cos(vDTime) * (3.1415f / 180);
        psIn.uv.y += (sin(vDTime) * 0.1f);
    }

    outie += vAmbLight;

   /////////////// Direction Light /////////////
   // for (int d = 0; d < 2; d++)
   // {
   //     float LR = saturate(dot((-DL_dir[d]), psIn.norm));
   //     outie += (LR * DL_color[d] /** sin(vDTime * 2)*/);
   // }
   /////////////////////////////////////////////
   //
   //////////////// Point Light //////////////
   ////for (int p = 0; p < 1; p++)
   ////{
   // float4 ptLightDir = normalize(PL_pos - psIn.wPos);
   // float LightRatio = saturate(dot(ptLightDir, psIn.norm));
   // float attenutation = 1.0f - saturate((length(PL_pos - psIn.wPos) / 100.0f));
   // LightRatio = (attenutation * attenutation) * LightRatio;
   // outie += lerp(float4(0, 0, 0, 0), PL_color /** sin(vDTime * 2)*/, LightRatio);
   ////}
   ////////////////////////////////////////////

    ////////////// Spot Light ////////////////
    //float4 spLightDir = normalize(SL_pos - psIn.wPos);
    //float surfaceRatio = saturate(dot(spLightDir, SL_coneDir));
    //float intensity = (surfaceRatio > SL_coneWidth.x) ? 1 : 0;
    //float lightRatio = saturate(dot(spLightDir, psIn.norm));
    //psIn.color += intensity * lightRatio * SL_color * psIn.color;
    //outie += intensity * lightRatio * SL_color * psIn.color;
    //////////////////////////////////////////
    //////////////////// Spot Light ////////////////
    //float4 toLight = SL_pos - psIn.wPos; // vector from pixel to Light
    //float distance = length(toLight); // length of vector
    //toLight = (toLight / distance); // normalizing 'toLight'
    //float AngAtten = saturate(dot(toLight, psIn.norm));
    //float spotDot = saturate(dot(-toLight, -SL_coneDir));
    ////--------------------------(spotdot - outer) / (innner - outer)
    //float spotAtten = saturate((spotDot - (SL_coneWidth.x*.25f)) / ((SL_coneWidth.x*.28f) / (SL_coneWidth.x*.25f)));
    //outie += (spotAtten * AngAtten * SL_color * psIn.color) * 2.0f;
    ////////////////////////////////////////////////
    float3 toLight = SL_pos.xyz - psIn.wPos.xyz; // vector from pixel to Light
    float distance = length(toLight); // length of vector
    toLight = (toLight / distance); // normalizing the toLight vector
    float AngAtten = saturate(dot(toLight, psIn.norm.xyz));
    float spotDot = saturate(dot(-toLight, -SL_coneDir.xyz));
    float spotAtten = saturate((SL_coneWidth.x - 0.1f) - (spotDot) / ((SL_coneWidth.x - 0.12f) / (SL_coneWidth.x - 0.1f)));
    outie += (spotAtten * AngAtten * SL_color * outie) * 5.0f;

    outie = (txDiffuse.Sample(samLinear, psIn.uv)) * outie ;

    return outie;
}