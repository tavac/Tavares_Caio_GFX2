Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer constBuff : register(b0)
{
    matrix vWorld;
    matrix vView;
    matrix vProj;
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
struct PS_Input
{
    float4 pos : SV_POSITION;
    float4 norm : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
    float4 wPos : PosInWorld;
};

float4 main(PS_Input psIn) : SV_TARGET
{
    float4 outie = { 0, 0, 0, 0 };
    if (PL_color.w < 1.0f)
    {
        psIn.uv.x += sin(vDTime) * cos(vDTime) * (3.1415f / 180);
        psIn.uv.y += (sin(vDTime) * 0.1f);
    }

    outie += vAmbLight;

   ///////////// Direction Light /////////////
    //for (int d = 0; d < 2; d++)
    //{
    float3 lightDir = DL_dir.xyz;
        //lightDir.y = clamp(cos(vDTime), -1, 1);
    float _dot = dot((lightDir), psIn.norm.xyz);
    float LR = saturate(_dot);
    outie += (LR * DL_color /** sin(vDTime * 2)*/);
    //}
   ///////////////////////////////////////////
   
   //////////// Point Light //////////////
    //for (int p = 0; p < 1; p++)
    //{
    float3 ptLightDir = normalize(PL_pos.xyz - psIn.wPos.xyz);
    float LightRatio = saturate(dot(-ptLightDir, psIn.norm.xyz));
    float attenutation = 1.0f - saturate((length(PL_pos.xyz - psIn.wPos.xyz) / 50.0f));
    LightRatio = (attenutation * attenutation) * LightRatio;
   //outie += lerp(float4(0, 0, 0, 0), PL_color /** sin(vDTime * 2)*/, LightRatio);
    //outie += LightRatio * PL_color;
    //}
    /////////////// Specular Formula ///////////////
    float3 viewdir = normalize(vView._14_24_34 - psIn.wPos.xyz);
    float3 halfVec = normalize((-PL_pos.xyz) + viewdir);
    float intensity = max(clamp(psIn.norm.xyz, normalize(halfVec), 2.0f), 0);
    outie += PL_color * 2.5f * intensity * LightRatio;
    ////////////////////////////////////////////////
   ////////////////////////////////////////

    ////////////// Spot Light ////////////////
    float innerRatio = SL_dir.w;
    float outerRatio = innerRatio - 0.05f;
    float3 toLight = SL_pos.xyz - psIn.wPos.xyz; // vector from pixel to Light
    float distance = length(toLight); // length of vector
    toLight = (toLight / distance); // normalizing the toLight vector
    float AngAtten = saturate(dot(toLight, psIn.norm.xyz)); // angle between vector to the light from surface and the normal of the surface where 1.0 is directly at the light
    float spotDot = saturate(dot(-toLight, SL_dir.xyz)); // 
    float spotAtten = saturate(((innerRatio) - (spotDot)) / ((innerRatio) - (outerRatio)));
    spotAtten -= 1.0f;
    outie += (spotAtten * spotAtten * AngAtten * SL_color);
    ////////////////////////////////////////////////


    outie = (txDiffuse.Sample(samLinear, psIn.uv)) * outie;

    return outie;
}