Texture2D<float4> colorTex : register(t0);
texture2D<float4> normaltex : register(t1);
Texture2D<float> depthTex : register(t2);
RWTexture2D<float4> dstTex : register(u0);

float3 mod(float3 a, float b) { return a-b*floor(a/b); }


[numthread(16,16,1)]
void cs_main(uint threadId : SV_DispatchThreadID) {
	
}