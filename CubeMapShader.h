#ifndef _CUBEMAPSHADER_H_
#define _CUBEMAPSHADER_H_

#include "Shader.h"

class CubeMapVS : public VertexShader {
public:
	bool Initialize();
};


class CubeMapPS : public PixelShader {
public:
	bool Initialize();
};

#endif