#ifndef _SHADOWSHADER_H_
#define _SHADOWSHADER_H_

#include "Shader.h"

class ShadowVS : public VertexShader {
public:
	bool Initialize();
};

class ShadowPS : public PixelShader {
public:
	bool Initialize();
};

#endif