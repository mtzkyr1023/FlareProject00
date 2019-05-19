
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

	if (id.x % 2 == 0) {
		tmp.position.x += tmp.color.a;
	}
	else {
		tmp.position.z += tmp.color.a;
	}
	
	if (tmp.position.x > 25.0f || tmp.position.x < -25.0f)
		tmp.color.a *= -1.0f;
	if (tmp.position.z > 25.0f || tmp.position.z < -25.0f)
		tmp.color.a *= -1.0f;

	tmp.viewPos = mul(float4(tmp.position, 1.0f), g_viewMatrix);

	g_pointLight[id.x] = tmp;
}