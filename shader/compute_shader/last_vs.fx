
struct VS_OUT {
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

VS_OUT vs_main(uint vertexId : SV_VertexID) {
	VS_OUT output = (VS_OUT)0;

	const float2 vId = float2(vertexId % 2, vertexId / 2);

	output.pos = vId * 2 - float2(1, 1);
	output.pos.y *= -1;
	output.pos.zw = float2(0.5f, 1.0f);

	output.tex = vId;

	return vId;
}