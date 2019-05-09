#ifndef _OBJLOADER_H_
#define _OBJLOADER_H_


#include <stdio.h>
#include <vector>
#include <string>
#include <DirectXMath.h>

class ObjLoader {
private:
	struct Mesh {
		std::vector<int> vIndices;
		std::vector<int> nIndices;
		std::vector<int> uvIndices;
		std::vector<DirectX::XMFLOAT3> vertices;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT2> texcoords;
		bool smoothFlag;

		Mesh() {
			smoothFlag = true;
		}

		void Clear() {
			vIndices.clear();
			nIndices.clear();
			uvIndices.clear();
			vertices.clear();
			normals.clear();
			texcoords.clear();
		}
	};

private:
	std::vector<Mesh> m_mesh;
	std::vector<std::vector<int>> m_indexArray;
	std::vector<std::vector<int>> m_uvIndexArray;
	std::vector<std::vector<int>> m_nIndexArray;
	std::vector<std::string> m_materialNameArray;
	std::string m_materialFileName;

public:
	bool ReadFile(const char* filename);

	std::vector<int>& GetVertexIndex(int num) { return m_mesh[num].vIndices; }
	std::vector<int>& GetNormalIndex(int num) { return m_mesh[num].nIndices; }
	std::vector<int>& GetUVIndex(int num) { return m_mesh[num].uvIndices; }
	std::vector<DirectX::XMFLOAT3>& GetVertex(int num) { return m_mesh[num].vertices; }
	std::vector<DirectX::XMFLOAT3>& GetNormal(int num) { return m_mesh[num].normals; }
	std::vector<DirectX::XMFLOAT2>& GetTexcoord(int num) { return m_mesh[num].texcoords; }

	bool GetSmoothFlag(int num) { return m_mesh[num].smoothFlag; }

	int GetMeshCount() { return (int)m_mesh.size(); }


	std::vector<int>& GetIndexArray(int num) { return m_indexArray[num]; }
	std::vector<int>& GetUVIndexArray(int num) { return m_uvIndexArray[num]; }
	std::vector<int>& GetNIndexArray(int num) { return m_nIndexArray[num]; }
	std::vector<std::string>& GetaterialNameArray() { return m_materialNameArray; }
	std::string GetMaterialFileName() { return m_materialFileName; }
	int GetIndexArrayCount() { return (int)m_indexArray.size(); }
};

#endif