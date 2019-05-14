#ifndef _MODELPS_H_
#define _MODELPS_H_

#include "Shader.h"


class ModelPS : public PixelShader {
public:
	bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);
};


#endif