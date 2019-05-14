#ifndef _COPYCS_H_
#define _COPYCS_H_

#include "Shader.h"

class CopyCS : public ComputeShader {
public:
	bool Initialize(LPCWSTR filename = 0, LPCSTR entryPoint = 0);
};

#endif