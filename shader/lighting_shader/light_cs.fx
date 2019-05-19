
#define GROUP_WIDTH 16
#define TILE_SIZE (GROUP_WIDTH * GROUP_WIDTH)


#include "../shader_defines.h"
#include "../shader_structues.h"
#include "../shader_func/pbr_func.hlsli"

RWTexture2D<float4> g_dst : register(u0);
Texture2D<float4> g_color : register(t0);
Texture2D<float4> g_normal : register(t1);
Texture2D<float> g_depth : register(t2);
Texture2D<float> g_roughness : register(t3);
Texture2D<float> g_metalness : register(t4);

StructuredBuffer<PointLight> g_pointLight : register(t5);

cbuffer ProjMatrix : register(b0) {
	matrix g_projMatrix;
}

groupshared uint s_minZ;
groupshared uint s_maxZ;
groupshared uint s_minD;
groupshared uint s_maxD;

groupshared uint s_tileLightIndices[POINT_LIGHT_NUM];
groupshared uint s_tileNumLights;

[numthreads(GROUP_WIDTH, GROUP_WIDTH, 1)]
void cs_main(uint3 threadId :  SV_DispatchThreadID, uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID) {
	uint groupIndex = groupThreadId.y * GROUP_WIDTH + groupThreadId.x;
	
	if (groupIndex == 0) {
		s_tileNumLights = 0;
		s_minZ = 0x7F7FFFFF;
		s_maxZ = 0;
	}

	uint totalLights, dummy;
	g_pointLight.GetDimensions(totalLights, dummy);

	GroupMemoryBarrierWithGroupSync();

	float linearZ = g_normal[threadId.xy].w;

	InterlockedMin(s_minZ, asuint(linearZ));
	InterlockedMax(s_maxZ, asuint(linearZ));

	GroupMemoryBarrierWithGroupSync();

	float minTileZ = asfloat(s_minZ);
	float maxTileZ = asfloat(s_maxZ);

	float2 tileScale = float2(TEX_SIZE, TEX_SIZE) * rcp(float(2 * GROUP_WIDTH));
	float2 tileBias = tileScale - float2(groupId.xy);

	float4 c1 = float4(g_projMatrix._11 * tileScale.x, 0.0f, tileBias.x, 0.0f);
	float4 c2 = float4(0.0f, -g_projMatrix._22 * tileScale.y, tileBias.y, 0.0f);
	float4 c4 = float4(0.0f, 0.0f, 1.0f, 0.0f);

	float4 frustumPlane[6];
	frustumPlane[0] = c4 - c1;
	frustumPlane[1] = c4 + c1;
	frustumPlane[2] = c4 - c2;
	frustumPlane[3] = c4 + c2;
	frustumPlane[4] = float4(0.0f, 0.0f, 1.0f, -minTileZ);
	frustumPlane[5] = float4(0.0f, 0.0f, -1.0f, maxTileZ);

	[unroll]
	for (int i = 0; i < 4; i++) {
		frustumPlane[i] *= rcp(length(frustumPlane[i].xyz));
	}



	for (uint lightIndex = groupIndex; lightIndex < POINT_LIGHT_NUM; lightIndex += TILE_SIZE) {
		PointLight light = g_pointLight[lightIndex];

		bool inFrustum = true;
		[unroll]
		for (uint j = 0; j < 6; j++) {
			float d = dot(float4(light.viewPos.xyz, 1.0f), frustumPlane[j]);
			inFrustum = inFrustum && (d >= -light.range);
		}

		[branch]
		if (inFrustum) {
			uint listIndex;
			InterlockedAdd(s_tileNumLights, 1, listIndex);
			s_tileLightIndices[listIndex] = lightIndex;
		}
	}

	GroupMemoryBarrierWithGroupSync();

	uint numLights = s_tileNumLights;

	float2 sPos = float2(threadId.xy) / (TEX_SIZE) * 2 - float2(1, 1);
	sPos.y *= -1.0f;
	float depth = g_depth[threadId.xy];
	float3 vPos = (float3)0;
	vPos.z = -g_projMatrix._43 / (g_projMatrix._33 - depth);
	vPos.x = sPos.x * vPos.z / g_projMatrix._11;
	vPos.y = sPos.y * vPos.z / g_projMatrix._22;
	float4 viewNormal = g_normal[threadId.xy];
	float3 V = (float3)0;
	float3 L = (float3)0;
	float3 H = (float3)0;
	float3 N = viewNormal.xyz;
	float intensity = 0.0f;
	float lightLength = 0.0f;
	float reflection = 0.0f;
	float roughness = g_roughness[threadId.xy];
	float metalness = g_metalness[threadId.xy];
	float3 lit = (float3)0;
	float D = 0;
	float F = 0;
	float G = 0;
	float4 color = g_color[threadId.xy];
	lit = color.rgb;
	V = -normalize(vPos);
	
	if (numLights > 0) {
		float4 normal = g_normal[threadId.xy];
		for (uint tileLightIndex = 0; tileLightIndex < numLights; tileLightIndex++) {
			PointLight light = g_pointLight[s_tileLightIndices[tileLightIndex]];
			lightLength = distance(vPos, light.viewPos.xyz);
			lightLength = rcp(lightLength * lightLength * 2 + lightLength * 2 + 1);
			intensity = saturate(dot(-normalize(vPos - light.viewPos.xyz), normal.xyz));
			L = (-normalize(vPos - light.viewPos.xyz));
			H = normalize(L + V);
			D = GGX(roughness, N, H);
			F = Flesnel(metalness, V, H);
			G = G_CookTorrance(L, V, H, N);
			float angleLN = saturate(dot(L, N));
			reflection = (D * F * G) / (4 * saturate(dot(V, N)) * saturate(dot(L, N)) + 0.000001f);
//			reflection = pow(saturate(dot(normal.xyz, H)), 10.0f);
			lit += intensity * light.color.rgb * lightLength + light.color.rgb * reflection;
//			lit += light.color.rgb * intensity * lightLength;
//			lit += float3(reflection, reflection, reflection);
			lit = saturate(lit);
		}

//		lit = saturate(lit);

		g_dst[threadId.xy] = float4(lit, 1.0f);
	}
	else {
		g_dst[threadId.xy] = float4(lit, 1.0f);
	}
}