#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#define MATERIAL_NUM 16

#include <DirectXMath.h>
#include <vector>

#include "Device.h"

struct MaterialType {
	DirectX::XMFLOAT4 specular;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;
};

class Material : public DeviceUser {
private:
	MaterialType m_material;
	ComPtr<ID3D11ShaderResourceView> m_texture;

public:
	Material() {}
	~Material() {}

	void SetDiffuse(float r, float g, float b, float a) {
		m_material.diffuse.x = r;
		m_material.diffuse.y = g;
		m_material.diffuse.z = b;
		m_material.diffuse.w = a;
	}
	void SetAmbient(float r, float g, float b, float a) {
		m_material.ambient.x = r;
		m_material.ambient.y = g;
		m_material.ambient.z = b;
		m_material.ambient.w = a;
	}
	void SetSpecular(float r, float g, float b) {
		m_material.specular.x = r;
		m_material.specular.y = g;
		m_material.specular.z = b;
	}
	void SetSpecularPower(float power) {
		m_material.specular.w = power;
	}

	bool LoadTexture(const char* filename);

	MaterialType& GetMaterial() { return m_material; }

	ComPtr<ID3D11ShaderResourceView> GetTexture() { return m_texture; }
};


class MaterialPBR {
private:
	DirectX::XMFLOAT4 m_albedo;
	float m_metalness;
	float m_roughness;
	ComPtr<ID3D11ShaderResourceView> m_albedoTex;
	ComPtr<ID3D11ShaderResourceView> m_bumpTex;

public:
	MaterialPBR(DirectX::XMFLOAT4 albedo = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1), float metalness = 0, float roughness = 0) :
		m_albedo(albedo), m_metalness(metalness), m_roughness(roughness) {

	}

	DirectX::XMFLOAT4 GetAlbedo() { return m_albedo; }
	float GetMetalness() { return m_metalness; }
	float GetRoughness() { return m_roughness; }
};


class MaterialFactory {
private:
	std::vector<MaterialPBR> m_materialArray;

public:
	MaterialFactory() {
		m_materialArray.resize(MATERIAL_NUM);
	}

	std::vector<MaterialPBR>& GetMaterialArray() { return m_materialArray; }

	static MaterialFactory& Inst() {
		static MaterialFactory inst;
		return inst;
	}
};

#endif