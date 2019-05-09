
cbuffer ViewMatrix : register(b1) {
	matrix g_view;
}

cbuffer ProjMatrix : register(b2) {
	matrix g_proj;
}

cbuffer Timer : register(b3) {
	float g_timer : packoffset(c0);
};

struct GS_IN {
	float4 pos : POSITION0;
	float3 nor : NORMAL0;
	float2 tex : TECOORD0;
}

struct GS_OUT {
	float4 pos : SV_POSITION;
	float3 nor : NORMAL0;
	float3 wnor : NORMAL1;
	float2 tex : TEXCOORD0;
	float linearZ : TEXCOORD1;
};


[maxvertexcount(3)]
void ga_main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> output,
			uint id : SV_PrimitiveID) {
	GS_OUT output = (GS_OUT)0;
	float3 center = (input[0].xyz + input[1].xyz + input[2].xyz) / 3;
	float4x4 matrixt[3];
	
	matrix[0] = float4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(g_timer), -sin(g_timer), 0.0f,
		0.0f, sin(g_timer), cos(g_timer), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
		
	matrix[1] = float4x4(
		cos(g_timer), 0.0f, sin(g_timer), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sin(g_timer), 0.0f, cos(g_timer), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f)
		
	matrix[2] = float4x4(
		cos(g_timer), -sin(g_timer), 0.0f, 0.0f,
		sin(g_timer), cos(g_timer), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f)
	
	[unroll]
	for (int i = 0; i < 3; i++) {
		output.pos. = input[i].pos - float4(center, 0);
		
		output.pos = mul(output.pos, matrix[id]);
		output.pos += float4(center, 0);
		
		output.pos = mul(output.pos, g_view);
		output.linerZ = output.pos.z;
		output.pos = mul(output.pos, g_proj);
		
		output.wnor = input[i].nor;
		output.nor = normalize(output.wnor, (float3x3)g_view);
		
		output.tex = input.tex;
		
		output.Append(output);
	}
	
	output.RestartStrip();
}