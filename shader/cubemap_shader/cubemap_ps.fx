TextureCube TextureMap : register(t0);
SamplerState samLinear : register(s0);

struct PS_IN {
	float4 pos : SV_POSITION;
	float3 tex : TEXCOORD0;
};

float4 ps_main(PS_IN input) : SV_Target {
	return TextureMap.Sample(samLinear, input.tex);
}