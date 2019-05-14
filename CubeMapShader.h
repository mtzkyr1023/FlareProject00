#ifndef _CUBEMAPSHADER_H_
#define _CUBEMAPSHADER_H_

#include "Shader.h"

class CubeMapVS : public VertexShader {
public:
	bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);
};


class CubeMapPS : public PixelShader {
public:
	bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);
};

#endif