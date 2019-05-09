Texture2D<float4> srcColor : register(t0);
Texture2D<float> srcDepth : register(t1);
RWTexture2D<float4> dstColor : register(u0);
RWTexture2D<float> dstDepth : register(u1);

[numthreads(16,16,1)]
void cs_main(uint3 id : SV_DispatchThreadID) {
	dstColor[id.xy] = srcColor[id.xy];
	dstDepth[id.xy] = srcDepth[id.xy];
}