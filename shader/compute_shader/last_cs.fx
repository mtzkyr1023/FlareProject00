RWTexture2D<float4> dst : register(u0);
Texture2D<float4> color : register(t0);
Texture2D<float3> normal : register(t1);

[numthreads(40,20,1)]
void cs_main(uint3 id :  SV_DispatchThreadID) {
	dst[id.xy] = color[id.xy];
}