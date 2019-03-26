cbuffer ViewMatrix : register(b0) {
	matrix g_viewMatrix;
}

cbuffer ProjMatrix : register(b1) {
	matrix g_projMatrix;
}

struct VS_STATIC_IN {
	float3 pos : POSITION0;
	float3 nor : NORMAL0;
	float2 tex : TEXCOORD0;
};


struct VS_SKINNED_IN {
	float3 pos : POSITION0;
	float3 nor : NORMAL0;
	float2 tex : TEXCOORD0;
};

struct VS_OUT {
	float4 pos : SV_POSITION;
	float3 nor : NORMAL0;
	float2 tex : TEXCOORD0;
};

VS_OUT static_vs_main(VS_STATIC_IN input) {
	VS_OUT output = (VS_OUT)0;
	
	output.pos = mul(float4(input.pos, 1.0f), g_viewMatrix);
	output.pos = mul(output.pos, g_projMatrix);
	
//	output.pos = float4(input.pos, 1.0f);
	
	output.nor = input.nor;
	
	output.tex = input.tex;
	
	return output;
}