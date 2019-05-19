cbuffer ViewMatrix : register(b0) {
	matrix g_view;
}

cbuffer ProjMatrix : register(b1) {
	matrix g_proj;
}

cbuffer WorldMatrix : register(b2) {
	matrix g_world;
}

cbuffer ShadowMatrix : register(b3) {
	matrix g_shadowView;
	matrix g_shadowProj;
}

StructuredBuffer<matrix> g_boneMatrix : register(t0);

struct VS_STATIC_IN {
	float3 pos : POSITION0;
	float3 nor : NORMAL0;
	float2 tex : TEXCOORD0;
};

struct VS_SKINNED_IN {
	float3 pos : POSITION0;
	float3 nor : NORMAL0;
	float2 tex : TEXCOORD0;
	float4 boneWeight : BONE_WEIGHT0;
	uint4 boneIndex : BONE_INDEX0;
};

struct VS_OUT {
	float4 pos : SV_POSITION;
	float3 nor : NORMAL0;
	float2 tex : TEXCOORD0;
	float linearZ : TEXCOORD1;
	float4 shadowPos : TEXCOORD2;
	float3 wnor : NORMAL1;
	float3 viewPos : TEXCOORD3;
	float3 wpos : TEXCOORD4;
};

VS_OUT vs_static_main(VS_STATIC_IN input) {
	VS_OUT output = (VS_OUT)0;
	float4 shadowPos;

	float3 viewPos;
	viewPos.x = -dot(g_view[0], g_view[3]);
	viewPos.y = -dot(g_view[1], g_view[3]);
	viewPos.z = -dot(g_view[2], g_view[3]);
	
	output.pos = mul(float4(input.pos, 1.0f), g_world);
	output.viewPos = viewPos;
	output.wpos = output.pos.xyz;
	output.pos = mul(output.pos, g_view);
	output.linearZ = output.pos.z;
	output.pos = mul(output.pos, g_proj);

	output.nor = mul(input.nor, (float3x3)g_world);
	output.wnor = normalize(output.nor);
	output.nor = normalize(mul(output.nor, (float3x3)g_view));

	shadowPos = mul(float4(input.pos, 1.0f), g_world);
	shadowPos = mul(shadowPos, g_shadowView);
	shadowPos = mul(shadowPos, g_shadowProj);

	output.shadowPos = shadowPos;
	output.shadowPos /= shadowPos.w;

	output.tex = input.tex;

	return output;
}


VS_OUT vs_skinned_main(VS_SKINNED_IN input) {
	VS_OUT output = (VS_OUT)0;

	float3 viewPos;
	viewPos.x = -dot(g_view[0], g_view[3]);
	viewPos.y = -dot(g_view[1], g_view[3]);
	viewPos.z = -dot(g_view[2], g_view[3]);
	
	output.viewPos = viewPos;

	output.pos = mul(float4(input.pos, 1.0f), g_world);
	output.wpos = output.pos;
	output.pos = mul(output.pos, g_view);
	output.linearZ = output.pos.z;
	output.pos = mul(output.pos, g_proj);

	output.nor = mul(input.nor, (float3x3)g_world);
	output.wnor = normalize(output.nor);
	output.nor = normalize(mul(output.nor, (float3x3)g_view));

	output.tex = input.tex;

	return output;
}