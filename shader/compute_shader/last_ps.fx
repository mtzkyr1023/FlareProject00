
Texture2D tex : register(t0);
SamplerState Sampler : register(s0);

struct PS_IN {
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 ps_main(PS_IN input) {
	return tex.Sample(Sampler, input.tex);
}