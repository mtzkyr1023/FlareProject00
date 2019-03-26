cbuffer ViewMatrix : register(b0) {
	matrix g_view;
}

cbuffer ProjMatrix : register(b1) {
	matrix g_proj;
}

StructuredBuffer<matrix> g_instanceMatrix : register(t0);
StructuredBuffer<matrix> g_boneMatrix : register(t1);

struct VS_IN {
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
};

interface Base {
	VS_OUT Main(VS_IN input, uint id);
};

class Static : Base {
	VS_OUT Main(VS_IN input, uint id) {
		VS_OUT output = (VS_OUT)0;
		
		output.pos = mul(float4(input.pos, 1.0f), g_instanceMatrix[id]);
		output.pos = mul(output.pos, g_view);
		output.pos = mul(output.pos, g_proj);
		
		output.nor = normalize(mul(input.nor, (float3x3)g_instanceMatrix[id]));
		
		output.tex = input.tex;
		
		return output;
	}
};

class Skinned : Base {
	VS_OUT Main(VS_IN input, uint id) {
		VS_OUT output = (VS_OUT)0;
		
		output.pos = mul(float4(input.pos, 1.0f), g_instanceMatrix[id]);
		output.pos = mul(output.pos, g_view);
		output.pos = mul(output.pos, g_proj);
		
		output.nor = normalize(mul(input.nor, (float3x3)g_instanceMatrix[id]));
		
		output.tex = input.tex;
		
		return output;
	}
};

Base base;

Static static_vs;
Skinned skinned_vs;

VS_OUT vs_main(VS_IN input, uint id : SV_InstanceID) {
	return base.Main(input, id);
}