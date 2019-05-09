Texture2D tex : register(t0);
Texture2D<float> shadowTex : register(t1);
SamplerState SampleType : register(s0);

struct PS_IN {
	float4 pos : SV_POSITION;
	float3 nor : NORMAL0;
	float2 tex : TEXCOORD0;
	float linearZ : TEXCOORD1;
	float4 shadowPos : TEXCOORD2;
	float3 wnor : NORMAL1;
};

struct PS_OUT {
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
};

PS_OUT ps_main(PS_IN input) {
	PS_OUT output = (PS_OUT)0;
	float4 texColor = tex.Sample(SampleType, input.tex);


	float intensity = dot(input.wnor, normalize(float3(1.0f, 1.0f, 1.0f))) * 0.5f + 0.5f;
	intensity *= intensity;

	float2 shadowUV;
	shadowUV.x = (1.0f + (input.shadowPos.x)) * 0.5f;
	shadowUV.y = (1.0f + (input.shadowPos.y)) * -0.5f;

	float depth = shadowTex.Sample(SampleType, shadowUV);


	float realZ = input.shadowPos.z;

	if (realZ > depth + 0.0005f) {
		depth = 0.5f;
	}
	else {
		depth = 1.0f;
	}

	output.color = tex.Sample(SampleType, input.tex) * intensity * depth;

	output.normal = float4(input.nor, input.linearZ);
	return output;
}