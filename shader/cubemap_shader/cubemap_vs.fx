cbuffer VPMatrix : register(b0) {
	matrix g_vpMatrix;
}

struct VS_OUT {
	float4 pos : SV_POSITION;
	float3 tex : TEXCOORD0;
};

VS_OUT vs_main(float3 pos : POSITION0) {
	VS_OUT output = (VS_OUT)0;
	
	float3 tmpPos = pos * 20000.0f;
	output.pos = mul(float4(tmpPos, 1.0f), g_vpMatrix).xyww;
	output.pos.z -= 1.0f;
	
	output.tex = pos;
	
	return output;
}