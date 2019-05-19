
cbuffer ViewMatrix : register(b0) {
	matrix g_view;
}

cbuffer ProjMatrix : register(b1) {
	matrix g_proj;
}

cbuffer Timer : register(b2) {
	float g_timer : packoffset(c0);
};

struct GS_IN {
	float4 pos : POSITION0;
	float3 nor : NORMAL0;
	float2 tex : TECOORD0;
};

struct GS_OUT {
	float4 pos : SV_POSITION;
	float3 nor : NORMAL0;
	float3 wnor : NORMAL1;
	float2 tex : TEXCOORD0;
	float linearZ : TEXCOORD1;
};


static const float4x4 mat[3] = {
	float4x4(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, cos(g_timer * 0.05f), -sin(g_timer * 0.05f), 0.0f,
	0.0f, sin(g_timer * 0.05f), cos(g_timer * 0.05f), 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f),
	float4x4(
	cos(g_timer * 0.05f), 0.0f, sin(g_timer * 0.05f), 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	-sin(g_timer * 0.05f), 0.0f, cos(g_timer * 0.05f), 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f),
	float4x4(
	cos(g_timer * 0.05f), -sin(g_timer * 0.05f), 0.0f, 0.0f,
	sin(g_timer * 0.05f), cos(g_timer * 0.05f), 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f)
};

[maxvertexcount(3)]
void gs_main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> output,
			uint id : SV_PrimitiveID) {
	GS_OUT tmp = (GS_OUT)0;
	float3 center = (input[0].pos.xyz + input[1].pos.xyz + input[2].pos.xyz) / 3;
	float timer = g_timer * 0.05f;
	
	float3 ab = input[1].pos - input[0].pos;
	float3 bc = input[2].pos - input[1].pos;
	float3 nor = normalize(cross(ab, bc));
	
	[unroll]
	for (int i = 0; i < 3; i++) {
		tmp.pos = input[i].pos - float4(center, 0);
		
		tmp.pos = mul(tmp.pos, mat[(id) % 3]);
		tmp.pos += float4(center, 0);
		tmp.pos.xyz += nor * g_timer * 0.01f;
		
		tmp.pos = mul(tmp.pos, g_view);
		tmp.linearZ = tmp.pos.z;
		tmp.pos = mul(tmp.pos, g_proj);
		
		tmp.wnor = input[i].nor;
		tmp.nor = normalize(mul(tmp.wnor, (float3x3)g_view));
		
		tmp.tex = input[i].tex;
		
		output.Append(tmp);
	}
	
	output.RestartStrip();
}