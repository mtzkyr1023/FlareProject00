Texture2D<float> heightMap : register(t0);
SamplerState heightSampler : register(s0);

cbuffer ViewMatrix : register(b0) {
	matrix g_view;
}

cbuffer ProjMatrix : register(b1) {
	matrix g_proj;
}

struct GS_IN {
	float4 pos : POSITION0;
	float2 tex : TEXCOORD0;
};

struct GS_OUT {
	float4 pos : SV_POSITION;
	float3 nor : NORMAL0;
	float3 wnor : NORMAL1;
	float3 vpos : POSITION1;
	float3 wpos : POSITION2;
	float2 tex : TEXCOORD0;
	float linearZ : TEXCOORD1;
};

[maxvertexcount(3)]
void gs_main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> output) {
	GS_OUT tmp[3];
	
	float3 ab, bc, nor;
	int i;
	[unroll]
	for (i = 0; i < 3; i++) {
		tmp[i].pos = input[i].pos;
//		tmp[i].pos.y += heightMap.SampleLevel(heightSampler, input[i].tex, 0) * 0.08f;
		tmp[i].pos.y += 0.08f;
	}
	
	ab = tmp[1].pos.xyz - tmp[0].pos.xyz;
	bc = tmp[2].pos.xyz - tmp[1].pos.xyz;
	nor = normalize(cross(ab, bc));
	
	float3 viewPos;
	viewPos.x = -dot(g_view[0], g_view[3]);
	viewPos.y = -dot(g_view[1], g_view[3]);
	viewPos.z = -dot(g_view[2], g_view[3]);
	
	[unroll]
	for (i = 0; i < 3; i++) {
		tmp[i].wpos = tmp[i].pos;
		tmp[i].vpos = tmp[i].pos - viewPos;
		tmp[i].pos = mul(tmp[i].pos, g_view);
		tmp[i].linearZ = tmp[i].pos.z;
		tmp[i].pos = mul(tmp[i].pos, g_proj);
		
		tmp[i].wnor = nor;
		tmp[i].nor = normalize(mul(nor, (float3x3)g_view));
		
		tmp[i].tex = input[i].tex;
		
		output.Append(tmp[i]);
	}
	
	output.RestartStrip();
}