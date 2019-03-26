#ifndef _OBJECT_H_
#define _OBJECT_H_


#include "MatrixBuffer.h"


class Object {
private:
	std::shared_ptr<Model> m_model;
	DirectX::XMFLOAT4X4 m_matrix;

public:
	Object() {
		ModelFactory::Inst().CreateStaticModel("", 0, m_model);
		DirectX::XMStoreFloat4x4(&m_matrix, DirectX::XMMatrixIdentity());
	}
	//�C���X�^���V���O����߂�
	//���f���N���X���I�u�W�F�N�g�Ƀt�@�N�g�����玝���Ă���

	DirectX::XMFLOAT4X4* GetMatrix() { return &m_matrix; }
};


#endif