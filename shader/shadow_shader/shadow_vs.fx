
cbuffer ShadowViewMatrix : register(b0) {
	matrix g_shadowView;
	matrix g_shadowProj;
}


cbuffer WorldMatrix : register(b1) {
	matrix g_world;
}


float4 vs_static_main(float3 pos : POSITION0) : SV_POSITION {
	float4 output = (float4)0;

	output = mul(float4(pos, 1.0f), g_world);
	output = mul(output, g_shadowView);
	output = mul(output, g_shadowProj);

	return output;
}