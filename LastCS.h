#ifndef _LASTCS_H_
#define _LASTCS_H_


#include "Shader.h"

class LastCS : public ComputeShader {
public:
	LastCS() {}
	~LastCS() {}

	bool Initialize();
};

#endif