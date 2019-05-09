#include "Model.h"
#include "ObjLoader.h"
#include "FbxLoader.h"
using namespace std;
using namespace DirectX;


bool Model::Initialize(const char* filename) {
	ObjLoader loader;

	vector<unique_ptr<Vertex[]>> vertex;
	vector<unique_ptr<int[]>> index;

	bool smoothFlag;

	if (!loader.ReadFile(filename)) {
		m_meshCount = 1;
		vertex.resize(m_meshCount);
		index.resize(m_meshCount);
		m_indexCount.push_back(3);
		m_vertexCount.push_back(3);
		vertex[0].reset(new Vertex[m_vertexCount[0]]);
		index[0].reset(new int[m_indexCount[0]]);

		vertex[0][0].pos = XMFLOAT3(0.0f, 0.5f, 0.0f);
		vertex[0][1].pos = XMFLOAT3(0.5f, -0.5f, 0.0f);
		vertex[0][2].pos = XMFLOAT3(-0.5f, -0.5f, 0.0f);

		index[0][0] = 0;
		index[0][1] = 1;
		index[0][2] = 2;
	}
	else {
		m_meshCount = loader.GetMeshCount();
		vertex.resize(m_meshCount);
		index.resize(m_meshCount);
		m_vertexCount.resize(m_meshCount);
		m_indexCount.resize(m_meshCount);
		int offsetPos = 0, offsetNor = 0, offsetTex = 0;
		for (int i = 0; i < m_meshCount; i++) {
			vector<int>& vIndex = loader.GetVertexIndex(i);
			vector<int>& nIndex = loader.GetNormalIndex(i);
			vector<int>& uvIndex = loader.GetUVIndex(i);
			vector<XMFLOAT3>& pos = loader.GetVertex(i);
			vector<XMFLOAT3>& nor = loader.GetNormal(i);
			vector<XMFLOAT2>& tex = loader.GetTexcoord(i);
			smoothFlag = loader.GetSmoothFlag(i);

			m_indexCount[i] = (UINT)vIndex.size();
			index[i].reset(new int[m_indexCount[i]]);
			if (!smoothFlag) {
				m_vertexCount[i] = (UINT)vIndex.size();
				vertex[i].reset(new Vertex[m_vertexCount[i]]);
				for (UINT j = 0; j < m_vertexCount[i]; j++) {
					vertex[i][j].pos = pos[vIndex[j] - offsetPos];
					vertex[i][j].nor = nor[nIndex[j] - offsetNor];
					vertex[i][j].tex = tex[uvIndex[j] - offsetTex];
				}
				for (UINT j = 0; j < m_indexCount[i]; j++)
					index[i][j] = (int)j;
			}
			else {
				if (pos.size() > tex.size()) {
					m_vertexCount[i] = (UINT)pos.size();
					vertex[i].reset(new Vertex[m_vertexCount[i]]);
					memset(vertex[i].get(), 0, sizeof(Vertex) * m_vertexCount[i]);
					for (UINT j = 0; j < m_vertexCount[i]; j++)
						vertex[i][j].pos = pos[j];
					for (UINT j = 0; j < (UINT)vIndex.size(); j++) {
						if (tex.size() == 1)
							vertex[i][vIndex[j] - offsetPos].tex = tex[uvIndex[j]];
						else
							vertex[i][vIndex[j] - offsetPos].tex = tex[uvIndex[j] - offsetTex];
					}
					for (UINT j = 0; j < (UINT)vIndex.size(); j++) {
						vertex[i][vIndex[j] - offsetPos].nor.x += nor[nIndex[j] - offsetNor].x;
						vertex[i][vIndex[j] - offsetPos].nor.y += nor[nIndex[j] - offsetNor].y;
						vertex[i][vIndex[j] - offsetPos].nor.z += nor[nIndex[j] - offsetNor].z;
					}
					for (UINT j = 0; j < m_indexCount[i]; j++)
						index[i][j] = vIndex[j] - offsetPos;
				}
				else {
					m_vertexCount[i] = (UINT)tex.size();
					vertex[i].reset(new Vertex[m_vertexCount[i]]);
					memset(vertex[i].get(), 0, sizeof(Vertex) * m_vertexCount[i]);
					for (UINT j = 0; j < (UINT)uvIndex.size(); j++)
						vertex[i][uvIndex[j] - offsetTex].pos = pos[vIndex[j] - offsetPos];
					for (UINT j = 0; j < (UINT)vIndex.size(); j++) {
						if (tex.size() == 1)
							vertex[i][uvIndex[j]].tex = tex[uvIndex[j]];
						else
							vertex[i][uvIndex[j] - offsetTex].tex = tex[uvIndex[j] - offsetTex];
					}
					for (UINT j = 0; j < (UINT)vIndex.size(); j++) {
						if (tex.size() == 1) {
							vertex[i][uvIndex[j]].nor.x += nor[nIndex[j] - offsetNor].x;
							vertex[i][uvIndex[j]].nor.y += nor[nIndex[j] - offsetNor].y;
							vertex[i][uvIndex[j]].nor.z += nor[nIndex[j] - offsetNor].z;
						}
						else {
							vertex[i][uvIndex[j] - offsetTex].nor.x += nor[nIndex[j] - offsetNor].x;
							vertex[i][uvIndex[j] - offsetTex].nor.y += nor[nIndex[j] - offsetNor].y;
							vertex[i][uvIndex[j] - offsetTex].nor.z += nor[nIndex[j] - offsetNor].z;
						}
					}
					for (UINT j = 0; j < m_indexCount[i]; j++) {
						if (tex.size() == 1)
							index[i][j] = uvIndex[j];
						else
							index[i][j] = uvIndex[j] - offsetTex;
					}
				}
			}

			offsetPos += (int)pos.size();
			offsetNor += (int)nor.size();
			if (tex.size() != 1)
				offsetTex += (int)tex.size();
		}
	}

	for (int i = 0; i < m_meshCount; i++) {
		m_allVertexCount += m_vertexCount[i];
		m_allIndexCount += m_indexCount[i];
	}

	HRESULT res;
	D3D11_BUFFER_DESC bufDesc{};
	D3D11_SUBRESOURCE_DATA initData{};

	m_vertexBuffer.resize(m_meshCount);
	m_indexBuffer.resize(m_meshCount);

	for (int i = 0; i < m_meshCount; i++) {
		for (UINT j = 0; j < m_vertexCount[i]; j++) {
			XMStoreFloat3(&vertex[i][j].nor, XMVector3Normalize(XMLoadFloat3(&vertex[i][j].nor)));
		}

		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.ByteWidth = sizeof(Vertex) * m_vertexCount[i];
		bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.StructureByteStride = 0;
		bufDesc.MiscFlags = 0;

		initData.pSysMem = vertex[i].get();

		res = m_device->CreateBuffer(&bufDesc, &initData, m_vertexBuffer[i].GetAddressOf());
		if (FAILED(res)) {
			MessageBox(NULL, "failed creating static model.", "Model.cpp", MB_OK);
			return false;
		}

		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.ByteWidth = sizeof(int) * m_indexCount[i];
		bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.StructureByteStride = 0;
		bufDesc.MiscFlags = 0;

		initData.pSysMem = index[i].get();


		res = m_device->CreateBuffer(&bufDesc, &initData, m_indexBuffer[i].GetAddressOf());
		if (FAILED(res)) {
			MessageBox(NULL, "failed creating static model index.", "Model.cpp", MB_OK);
			return false;
		}
	}

	return true;
}


bool SkinnedModel::Initialize(const char* filename) {
	FbxLoader loader;

	if (!loader.LoadFbx(filename))
		return false;

	vector<unique_ptr<SkinnedVertex[]>> vertex;
	vector<unique_ptr<int[]>> index;

	m_meshCount = loader.GetMeshCount();

	vertex.resize(m_meshCount);
	index.resize(m_meshCount);

	for (int i = 0; i < m_meshCount; i++) {
		int vertexCount = loader.GetVertexCount(i);
		int indexCount = loader.GetIndexCount(i);
		vertex[i].reset(new SkinnedVertex[vertexCount]);
		index[i].reset(new int[indexCount]);

		vector<XMFLOAT3>& srcPos = loader.GetVertexPosition(i);
		vector<XMFLOAT3>& srcNor = loader.GetVertexNormal(i);
		vector<XMFLOAT2>& srcTex = loader.GetVertexUV(i);
		vector<int>& srcIndex = loader.GetIndexArray(i);

		for (int j = 0; j < indexCount; j++) {
			index[i][j] = srcIndex[j];
		}

		for (int j = 0; j < vertexCount; j++) {
			vertex[i][j].pos = srcPos[j];
			vertex[i][j].nor = srcNor[srcIndex[j]];
			vertex[i][j].tex = srcTex[srcIndex[j]];
		}

		m_vertexCount.push_back((UINT)vertexCount);
		m_indexCount.push_back((UINT)indexCount);
	}

	for (int i = 0; i < m_meshCount; i++) {
		m_allVertexCount += m_vertexCount[i];
		m_allIndexCount += m_indexCount[i];
	}

	HRESULT res;
	D3D11_BUFFER_DESC bufDesc{};
	D3D11_SUBRESOURCE_DATA initData{};

	m_vertexBuffer.resize(m_meshCount);
	m_indexBuffer.resize(m_meshCount);

	for (int i = 0; i < m_meshCount; i++) {

		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.ByteWidth = sizeof(SkinnedVertex) * m_vertexCount[i];
		bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.StructureByteStride = 0;
		bufDesc.MiscFlags = 0;

		initData.pSysMem = vertex[i].get();

		res = m_device->CreateBuffer(&bufDesc, &initData, m_vertexBuffer[i].GetAddressOf());

		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.ByteWidth = sizeof(int) * m_indexCount[i];
		bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.StructureByteStride = 0;
		bufDesc.MiscFlags = 0;

		initData.pSysMem = index[i].get();


		res = m_device->CreateBuffer(&bufDesc, &initData, m_indexBuffer[i].GetAddressOf());
	}

	return true;
}

void SkinnedModel::SetBuffer(int num) {
	UINT offset = 0;

	m_context->IASetVertexBuffers(0, 1, m_vertexBuffer[num].GetAddressOf(), &m_stride, &offset);
	m_context->IASetIndexBuffer(m_indexBuffer[num].Get(), DXGI_FORMAT_R32_UINT, 0);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}