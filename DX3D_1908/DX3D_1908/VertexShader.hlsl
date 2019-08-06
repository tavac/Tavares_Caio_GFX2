float4 main(float4 pos : POSITION) : SV_POSITION
{
	pos.x = pos.x + 0.25f;
	pos.y = pos.y - 0.25f;
	return pos;
}