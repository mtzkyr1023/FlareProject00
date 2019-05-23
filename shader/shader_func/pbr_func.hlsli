#include "../shader_constant.h"


float GGX(float roughness, float3 N, float3 H) {
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float NH = saturate(dot(H, N));
//	float t = (1 - NH2) / NH2;
	float t = ((NH * NH) * (alpha2 - 1.0f) + 1.0f);
	
	return alpha2 / (PI * t * t);
//	return (1.0f / PI) * sqrt(alpha / (NH2 * (alpha2 + t)));
}

float Flesnel(float metalness, float3 V, float3 H) {
	float VH = saturate(dot(V, H));
	float F = pow(1.0f - VH, 5);
	F *= (1.0f - metalness);
	F += metalness;
	return F;
}

float G_CookTorrance(float3 L, float3 V, float3 H, float3 N) {
    float NH = saturate(dot(N, H));
    float NL = saturate(dot(N, L));
    float NV = saturate(dot(N, V));
    float VH = saturate(dot(V, H));

    float NH2 = 2.0 * NH;
    float g1 = (NH2 * NV) / VH;
    float g2 = (NH2 * NL) / VH;
    float G = min(1.0f, min(g1, g2));
    return G;
}

float4 CubeMapBlur(TextureCube tex, SamplerState Sampler, float3 refV, float level) {
	float3 blurPower[10];
	float4 ret = (float4)0;
	int i;
	
	[unroll]
	for (i = -5; i < 5; i++) {
		blurPower[i + 5] = refV + float3((float)i * level, -(float)i * level, (float)i * level);
	}
	
	for (i = 0; i < 10; i++) {
		ret += tex.Sample(Sampler, blurPower[i]);
	}
	
	return ret / 10.0f * (1.0f - level);
}