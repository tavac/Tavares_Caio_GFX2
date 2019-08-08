struct PS_Input
{
	float4 pos : SV_POSITION;
	// float4 norm;
	// float4 color;
};

float4 main(PS_Input psIn) : SV_Target
{

	return float4(1.0f,0.25f,0.67f,1.0f);
}