
#define VERTEX_STRIDE 32
#define OFFSET_POS 0
#define OFFSET_NOR 12
#define OFFSET_TEX 24
#define OFFSET_INDEX 4

ByteAddressBuffer srcVBuffer : register(t0);
ByteAddressBuffer srcIBuffer : register(t1);

RWTexture1D dstVBuffer : register(u0);

groupshared uint sIndex[3];
groupshared uint3 sPos[3];
groupshared uint3 sNor[3];
groupshared uint2 sTex[3];

[numthreads(3,1,1)]
void cs_main(uint3 threadId : SV_DispatchThreadID,
		uint3 groupThreadId : SV_GroupThreadID) {
	
	uint index[3];
	float3 pos[3];
	float3 nor[3];
	float2 tex[3];
}