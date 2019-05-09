#ifndef _WATERSHADER_H_
#define _WATERSHADER_H_


#include "Shader.h"

class WaterVS : public VertexShader {
public:
	bool Initialize();
};

class WaterGS : public GeometryShader {
public:
	bool Initialize();
};

class WaterPS : public PixelShader {
public:
	bool Initialize();
};

#endif