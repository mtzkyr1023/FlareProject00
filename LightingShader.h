#ifndef _LIGHTINGSHADER_H_
#define _LIGHTINGSHADER_H_

#include "Shader.h"

class LightCullShader : public ComputeShader {
public:
	LightCullShader() {}
	~LightCullShader() {}

	bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);
};


class LightingShader : public ComputeShader {
public:
	LightingShader() {}
	~LightingShader() {}

	bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);
};

#endif