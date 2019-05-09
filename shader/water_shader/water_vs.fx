cbuffer WorldMatrix : register(b0) {
	matrix g_world;
}

struct VS_IN {
	float3 pos : POSITION0;
	float2 tex : TEXCOORD0;
};

struct VS_OUT {
	float4 pos : POSITION0;
	float2 tex : TEXCOORD0;
};

VS_OUT vs_main(float3 pos : POSITION0, float3 nor : NORMAL0, float2 tex : TEXCOORD0) {
	VS_OUT output = (VS_OUT)0;
	
	output.pos = mul(float4(pos, 1.0f), g_world);
	
	output.tex = tex;
	
	return output;
}