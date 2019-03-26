struct PS_IN {
	float4 pos : SV_POSITION;
	float3 nor : NORMAL0;
	float2 tex : TEXCOORD0;
};

struct PS_OUT {
	float4 color : SV_Target0;
	float3 normal : SV_Target1;
};

PS_OUT ps_main(PS_IN input) {
	PS_OUT output = (PS_OUT)0;
	output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	output.normal = float3(1.0f, 0.0f, 0.0f);
	return output;
}