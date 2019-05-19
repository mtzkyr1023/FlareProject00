RWTexture2D<float4> dst : register(u0);
Texture2D<float4> color : register(t0);
Texture2D<float4> reflect : register(t1);

[numthreads(16,16,1)]
void cs_main(uint3 id :  SV_DispatchThreadID) {
	dst[id.xy] = color[id.xy] * 1.0f + reflect[id.xy] * 0.0f;
}