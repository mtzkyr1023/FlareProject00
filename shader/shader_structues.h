#ifndef _SHADER_STRUCTURES_H_
#define _SHADER_STRUCTURES_H_

struct PointLight {
	float3 position;
	float4 viewPos;
	float range;
	float4 color;
};

#endif