#ifndef _MODEL_H_
#define _MODEL_H_

#define STATIC 0
#define SKINNED 1

#define MODEL_NUM 16

#include <DirectXMath.h>
#include <vector>
#include <memory>

#include "Device.h"

class Model : public DeviceUser {
protected:
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;
	UINT m_vertexCount;
	UINT m_indexCount;
	UINT m_stride1;
	UINT m_modelType;

protected:
	struct DefaultVertex {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 nor;
		DirectX::XMFLOAT2 tex;
	};

public:
	Model() : m_vertexCount(0), m_indexCount(0), m_stride1(sizeof(DefaultVertex)), m_modelType(0) {
	}
	virtual ~Model() {}

	virtual bool Initialize(const char* filename);

	virtual void SetBuffer() {
		UINT offset = 0;
		m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_stride1, &offset);
		m_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	UINT GetIndexCount() {
		return m_indexCount;
	}

	UINT GetModelType() {
		return m_modelType;
	}
};


class SkinnedModel : public Model {
private:
	ComPtr<ID3D11Buffer> m_boneVertexBuffer;
	UINT m_stride2;

private:
	struct BoneVertex {
		float boneWeight[4];
		UINT boneIndex[4];
	};

public:
	SkinnedModel() : m_stride2(sizeof(BoneVertex)) {
		m_modelType = SKINNED;
	}
	~SkinnedModel() {}

	bool Initialize(const char* filename);

	void SetBuffer();
};


class ModelFactory {
private:
	std::shared_ptr<Model> m_backgroundModel;
	std::vector<std::shared_ptr<Model>> m_generalModel;

public:
	ModelFactory() {
		m_generalModel.resize(MODEL_NUM);
	}

	bool CreateBackgroundModel(const char* filename, std::shared_ptr<Model>& ret) {
		m_backgroundModel.reset();
		m_backgroundModel = std::make_shared<Model>();
		if (!m_backgroundModel->Initialize(filename))
			return false;

		return true;
	}

	bool CreateStaticModel(const char* filename, int num, std::shared_ptr<Model>& ret) {
		m_generalModel[num].reset();
		m_generalModel[num] = std::make_shared<Model>();
		if (!m_generalModel[num]->Initialize(filename))
			return false;

		return true;
	}

	bool CreateSkinnedModel(const char* filename, int num, std::shared_ptr<Model>& ret) {
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
		if (!m_generalModel[num])
			return false;

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