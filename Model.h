#ifndef _MODEL_H_
#define _MODEL_H_

#define STATIC 0
#define SKINNED 1

#define MODEL_NUM 16

#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <map>

#include "Device.h"
#include "Material.h"

class Model : public DeviceUser {
protected:
	std::vector<ComPtr<ID3D11Buffer>> m_vertexBuffer;
	std::vector<ComPtr<ID3D11Buffer>> m_indexBuffer;
	std::vector<UINT> m_vertexCount;
	std::vector<UINT> m_indexCount;
	UINT m_allVertexCount;
	UINT m_allIndexCount;
	std::vector<std::string> m_materialName;
	UINT m_stride;
	UINT m_modelType;
	int m_meshCount;

protected:
	struct Vertex {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 nor;
		DirectX::XMFLOAT2 tex;
	};

public:
	Model() : m_vertexCount(0), m_indexCount(0), m_stride((UINT)sizeof(Vertex)), m_modelType(STATIC), m_meshCount(1), 
		m_allVertexCount(0), m_allIndexCount(0) {
	}
	virtual ~Model() {}

	virtual bool Initialize(const char* filename = "");

	virtual void SetBuffer(int num) {
		UINT offset = 0;

		m_context->IASetVertexBuffers(0, 1, m_vertexBuffer[num].GetAddressOf(), &m_stride, &offset);
		m_context->IASetIndexBuffer(m_indexBuffer[num].Get(), DXGI_FORMAT_R32_UINT, 0);
		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	UINT GetIndexCount(int num) {
		return m_indexCount[num];
	}

	UINT GetAllIndeCount() {
		return m_allIndexCount;
	}

	UINT GetModelType() {
		return m_modelType;
	}

	const char* GetMaterialName(int num) { return m_materialName[num].c_str(); }

	int GetMeshCount() { return m_meshCount; }
};


class SkinnedModel : public Model {
private:
	struct SkinnedVertex {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 nor;
		DirectX::XMFLOAT2 tex;
		DirectX::XMFLOAT4 boneWeight;
		DirectX::XMUINT4 boneIndex;
	};

public:
	SkinnedModel() {
		m_modelType = SKINNED;
		m_stride = (UINT)sizeof(SkinnedVertex);
	}
	~SkinnedModel() {}

	bool Initialize(const char* filename = "");

	void SetBuffer(int num);
};


class ModelFactory {
private:
	std::shared_ptr<Model> m_backgroundModel;
	std::vector<std::shared_ptr<Model>> m_generalModel;

public:
	ModelFactory() {
		m_generalModel.resize(MODEL_NUM);
	}

	bool CreateBackgroundModel(const char* filename) {
		m_backgroundModel.reset();
		m_backgroundModel = std::make_shared<Model>();
		if (!m_backgroundModel->Initialize(filename))
			return false;

		return true;
	}

	bool CreateStaticModel(const char* filename, int num) {
		m_generalModel[num].reset();
		m_generalModel[num] = std::make_shared<Model>();
		if (!m_generalModel[num]->Initialize(filename))
			return false;

		return true;
	}

	bool CreateSkinnedModel(const char* filename, int num) {
		m_generalModel[num].reset();
		m_generalModel[num] = std::make_shared<SkinnedModel>();
		if (!m_generalModel[num]->Initialize(filename))
			return false;

		return true;
	}

	bool GetBackgroundModel(std::shared_ptr<Model>& ret) {
		if (!m_backgroundModel)
			return false;

		ret = m_backgroundModel;

		return true;
	}

	bool GetModel(int num, std::shared_ptr<Model>& ret) {
		if (num >= MODEL_NUM) {
			ret = m_generalModel[0];
			return false;
		}

		if (!m_generalModel[num]) {
			m_generalModel[num] = std::make_shared<Model>();
			m_generalModel[num]->Initialize();
			ret = m_generalModel[num];
			return false;
		}

		ret = m_generalModel[num];

		return true;
	}

	std::vector<std::shared_ptr<Model>>& GetModelArray() {
		return m_generalModel;
	}


	static ModelFactory& Inst() {
		static ModelFactory inst;
		return inst;
	}
};

#endif