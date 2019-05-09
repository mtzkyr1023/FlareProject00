
#include "../shader_defines.h"
#include "../shader_structues.h"

#define LIGHT_UPDATE_NUM (POINT_LIGHT_NUM)

RWStructuredBuffer<PointLight> g_pointLight : register(u0);

cbuffer Camera : register(b0) {
	matrix g_viewMatrix;
}

[numthreads(128, 1, 1)]
void cs_main(uint3 id : SV_DispatchThreadID) {
	PointLight tmp = g_pointLight[id.x];

	if (id.x == 48) {
		tmp.position.x = sin(tmp.color.a) * 4.0f;
		tmp.position.z = cos(tmp.color.a) * 4.0f;
		tmp.color.a += 0.025f;
	}

	tmp.viewPos = mul(float4(tmp.position, 1.0f), g_viewMatrix);

	g_pointLight[id.x] = tmp;
}