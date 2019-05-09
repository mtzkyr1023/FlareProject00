#ifndef _LASTSHADER_H_
#define _LASTSHADER_H_

#include "Shader.h"

class LastVS : public VertexShader {
public:
//	bool Initialize();
};

class LastPS : public PixelShader {
private:
	ComPtr<ID3D11SamplerState> m_samplerState;

public:
//	bool Initialize();
};

#endif