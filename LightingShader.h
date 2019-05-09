#ifndef _LIGHTINGSHADER_H_
#define _LIGHTINGSHADER_H_

#include "Shader.h"

class LightCullShader : public ComputeShader {
public:
	LightCullShader() {}
	~LightCullShader() {}

	bool Initialize();
};


class LightingShader : public ComputeShader {
public:
	LightingShader() {}
	~LightingShader() {}

	bool Initialize();
};

#endif