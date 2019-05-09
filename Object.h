#ifndef _OBJECT_H_
#define _OBJECT_H_


#define MO_FAILED_DRAW 0xffffffff


#include <list>

#include "MatrixBuffer.h"
#include "Model.h"


class ModelObject;


class ModelObjectManager {
private:
	std::list<ModelObject*> m_objList;
	std::list<ModelObject*> m_transparentObjList;

public:
	void Regist(ModelObject* ptr) {
		m_objList.push_back(ptr);
	}

	void UnRegist(ModelObject* ptr) {
		m_objList.remove(ptr);
	}

	void RegistTrans(ModelObject* ptr) {
		m_transparentObjList.push_back(ptr);
	}

	void UnRegistTrans(ModelObject* ptr) {
		m_transparentObjList.remove(ptr);
	}

	std::list<ModelObject*>& GetObjectArray() { return m_objList; }

	static ModelObjectManager& Inst() {
		static ModelObjectManager inst;
		return inst;
	}
};

class ModelObject {
public:
	enum Status{
		NORMAL = 0,
		EXPLOSION = 1,
	};

private:
	static ModelObjectManager* m_manager;

private:
	int m_modelId;
	bool m_transFlag;
	DirectX::XMFLOAT4X4 m_matrix;
	std::shared_ptr<Model> m_model;
	ModelObject::Status m_status;

public:
	ModelObject(int modelId = 0, bool trans = false) : m_status(ModelObject::NORMAL) {
		DirectX::XMStoreFloat4x4(&m_matrix, DirectX::XMMatrixIdentity());

		m_modelId = modelId;
		m_transFlag = trans;
		ModelFactory::Inst().GetModel(0, m_model);

		if (m_manager) {
			if (!trans)
				m_manager->Regist(this);
			else
				m_manager->RegistTrans(this);
		}
	}

	virtual ~ModelObject() {
		if (m_manager) {
			if (!m_transFlag)
				m_manager->UnRegist(this);
			else
				m_manager->UnRegistTrans(this);
		}
	}

	DirectX::XMFLOAT4X4* GetMatrix() { return &m_matrix; }

	int GetModelId() { return m_modelId; }

	void SetModelStatus(ModelObject::Status state) { m_status = state; }
	ModelObject::Status GetStatus() { return m_status; }

	static void SetManager(ModelObjectManager* manager) { m_manager = manager; }
};

#endif