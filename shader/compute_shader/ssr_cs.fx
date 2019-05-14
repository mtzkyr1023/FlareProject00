
#include "../shader_defines.h"

Texture2D<float4> colorTex : register(t0);
texture2D<float4> normalTex : register(t1);
Texture2D<float> depthTex : register(t2);
TextureCube cubeTex : register(t3);
RWTexture2D<float4> dstTex : register(u0);

SamplerState cubeSampler : register(s0);

cbuffer ProjMatrix : register(b0) {
	matrix g_proj;
}

cbuffer InvViewMatrix : register(b1) {
	matrix g_transView;
}


[numthreads(16,16,1)]
void cs_main(uint3 threadId : SV_DispatchThreadID) {
	float2 spos;
	float3 vpos;
	float3 wpos;
	float4 vnorZ;
	float3 wnor;
	float3 viewDir;
	float3 reflectVec;
	
	float depth = depthTex[threadId.xy];
	vnorZ = normalTex[threadId.xy];
	
	spos = float2(threadId.xy) / (TEX_SIZE) * 2 - float2(1, 1);
	vpos.z = -g_proj._43 / (g_proj._33 - depth);
	vpos.x = spos.x * vpos.z / g_proj._11;
	vpos.y = spos.y * vpos.z / g_proj._22;
	
	viewDir = mul(float4(vpos, 1), g_transView).xyz;
	wnor = mul(float4(vnorZ.xyz, 1), g_transView).xyz;
	
	reflectVec = normalize(reflect(viewDir, wnor));
	
	dstTex[threadId.xy] = cubeTex.SampleLevel(cubeSampler, reflectVec, 0);
}