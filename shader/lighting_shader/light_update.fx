
#include "../shader_defines.h"
#include "../shader_structues.h"

RWStructuredBuffer<PointLight> g_pointLight : register(u0);

cbuffer Camera : register(b0) {
	matrix g_viewMatrix;
};

[numthreads(POINT_LIGHT_NUM, 1, 1)]
void cs_main(uint3 id : SV_DispatchThreadID) {
	PointLight tmp = g_pointLight[id.x];

	tmp.position.xz += float2(sin(tmp.color.a) * 8.0f, cos(tmp.color.a) * 8.0f);
	tmp.viewPos = mul(float4(tmp.position, 1.0f), g_viewMatrix);
	tmp.viewPos.w = 1.0f;

	g_pointLight[id.x] = tmp;
}