#ifndef _WATERSHADER_H_
#define _WATERSHADER_H_


#include "Shader.h"

class WaterVS : public VertexShader {
public:
	bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);
};

class WaterGS : public GeometryShader {
public:
	bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);
};

class WaterPS : public PixelShader {
public:
	bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);
};

#endif