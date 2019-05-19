
#define RAY_NUM 30
#define NOISE_POWER 49813

#include "../shader_defines.h"

float noise(float2 seed) {
    return frac(sin(dot(seed.xy, float2(18.80f, 24.0f))) * NOISE_POWER);
}

Texture2D<float4> colorTex : register(t0);
texture2D<float4> normalTex : register(t1);
Texture2D<float> depthTex : register(t2);
TextureCube cubeTex : register(t3);
RWTexture2D<float4> dstTex : register(u0);

SamplerState Sampler : register(s0);

cbuffer ProjMatrix : register(b0) {
	matrix g_proj;
}

cbuffer InvViewMatrix : register(b1) {
	matrix g_transView;
}

cbuffer ViewMatrix : register(b2) {
	matrix g_view;
}

cbuffer VPMatrix : register(b3) {
	matrix g_vpMatrix;
}


[numthreads(16,16,1)]
void cs_main(uint3 threadId : SV_DispatchThreadID) {
	float2 spos;
	float3 vpos;
	float4 wpos;
	float4 vnorZ;
	float4 vpPos;
	float3 wnor;
	float4 color = (float4)0;
	float3 viewDir;
	float3 vReflect;
	
	float3 viewPos;
	viewPos.x = -dot(g_view[0], g_view[3]);
	viewPos.y = -dot(g_view[1], g_view[3]);
	viewPos.z = -dot(g_view[2], g_view[3]);
	
	float depth = depthTex[threadId.xy];
	vnorZ = normalTex[threadId.xy];
	
	spos = float2(threadId.xy) / (TEX_SIZE) * 2 - float2(1, 1);
	spos.y*=-1.0f;
	vpos.z = -g_proj._43 / (g_proj._33 - depth);
	vpos.x = spos.x * vpos.z / g_proj._11;
	vpos.y = spos.y * vpos.z / g_proj._22;
	
	wpos = mul(float4(vpos, 1), g_transView);
	wnor = mul(vnorZ.xyz, (float3x3)g_transView);
	
	viewDir = wpos.xyz - viewPos;
	
	vReflect = reflect(viewDir, wnor);
	
	float3 step = 1.0f / RAY_NUM * vReflect;
	float thicness = 0.1f / RAY_NUM;
	
	for (int i = 1; i <= RAY_NUM; i++) {
		float3 rayPos = wpos.xyz + step * ((float)i + noise(spos));
//		float3 rayPos = wpos.xyz + step * ((float)i);
		float4 vpPos = mul(float4(rayPos, 1.0f), g_vpMatrix);
		vpPos /= vpPos.w;
		vpPos.x = (1.0f + (vpPos.x)) * 0.5f;
		vpPos.y = (1.0f - (vpPos.y)) * 0.5f;
		float gbufDepth = depthTex.SampleLevel(Sampler, vpPos.xy, 0);
		if (vpPos.z - gbufDepth > 0.0f && vpPos.z - gbufDepth < thicness) {
			color += colorTex.SampleLevel(Sampler, vpPos.xy, 0);
			break;
		}
		
		if (i == RAY_NUM) {
//			color += cubeTex.SampleLevel(Sampler, vReflect, 0);
		}
	}
	
//	color *= rcp(RAY_NUM) * 4.0f;
	
//	dstTex[threadId.xy] = cubeTex.SampleLevel(Sampler, vReflect, 0);;
	dstTex[threadId.xy] = color;
}