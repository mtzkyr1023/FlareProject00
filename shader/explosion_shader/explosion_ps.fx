struct PS_IN {
	float4 pos : SV_POSITION;
	float3 nor : NORMAL0;
	float3 wnor : NORMAL1;
	float2 tex : TEXCOORD0;
	float linearZ : TEXCOORD1;
};

struct PS_OUT {
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
};

PS_OUT ps_main(PS_IN input) {
	PS_OUT output = (PS_OUT)0;
	float3 color = float3(1.0f, 1.0f, 1.0f);
	float3 lightDir = float3(1.0f, 1.0f, 1.0f);
	float intensity = dot(normalize(lightDir), input.wnor) * 0.5f + 0.5f;
	intensity *= intensity;
	
	output.color = float4(color * intensity, 1.0f);
//	output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	output.normal = float4(input.nor, input.linearZ);
	
	return output;
}