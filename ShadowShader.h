#ifndef _SHADOWSHADER_H_
#define _SHADOWSHADER_H_

#include "Shader.h"

class ShadowVS : public VertexShader {
public:
	bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);
};

class ShadowPS : public PixelShader {
public:
	bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);
};

#endif