#ifndef _FBXLOADER_H_
#define _FBXLOADER_H_

#include <fbxsdk.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>

#include "Material.h"

class FbxLoader {
private:
	struct Mesh {
		std::vector<DirectX::XMFLOAT3> pos;
		std::vector<DirectX::XMFLOAT3> nor;
		std::vector<DirectX::XMFLOAT2> tex;
		std::vector<DirectX::XMFLOAT4> boneWeight;
		std::vector<DirectX::XMUINT4> boneIndex;
		std::vector<int> index;
		int vertexCount;
		int indexCount;
	};

private:
	std::vector<Mesh> m_meshArray;

private:
	void GetNormal(FbxLayerElementNormal* normal, int vertexCount, int* index, std::vector<DirectX::XMFLOAT3>& ret, int& normalCount);
	void GetUV(FbxLayerElementUV* uv, int vertexCount, int* index, std::vector<DirectX::XMFLOAT2>& ret, int& uvCount);

public:
	bool LoadFbx(const char* filename);

	int GetMeshCount() { return (int)m_meshArray.size(); }

	int GetVertexCount(int num) { return m_meshArray[num].vertexCount; }
	int GetIndexCount(int num) { return m_meshArray[num].indexCount; }
	std::vector<DirectX::XMFLOAT3>& GetVertexPosition(int num) { return m_meshArray[num].pos; }
	std::vector<DirectX::XMFLOAT3>& GetVertexNormal(int num) { return m_meshArray[num].nor; }
	std::vector<DirectX::XMFLOAT2>& GetVertexUV(int num) { return m_meshArray[num].tex; }
	std::vector<int>& GetIndexArray(int num) { return m_meshArray[num].index; }
};

#endif