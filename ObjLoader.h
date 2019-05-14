#ifndef _OBJLOADER_H_
#define _OBJLOADER_H_


#include <stdio.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <unordered_map>

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



class ObjLoaderKai {
private:

	struct SubsetInfo {
		std::vector<int> posIndex;
		std::vector<int> norIndex;
		std::vector<int> uvIndex;
		int vertexCount;
		bool smooth;
	};

private:
	std::unordered_map<std::string, int> m_subsetId;
	std::unordered_map<int, std::string> m_subsetStr;
	std::vector<std::vector<int>> m_subset;
	std::vector<DirectX::XMFLOAT4> m_diffuse;
	std::vector<DirectX::XMFLOAT4> m_ambient;
	std::vector<DirectX::XMFLOAT4> m_specular;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_srv;
	std::vector<DirectX::XMFLOAT3> m_pos;
	std::vector<DirectX::XMFLOAT3> m_nor;
	std::vector<DirectX::XMFLOAT2> m_tex;

private:
	bool ReadMaterial(const char* matFilename);

public:
	bool ReadFile(const char* filename);

	std::vector<DirectX::XMFLOAT3>& GetPos() { return m_pos; }
	std::vector<DirectX::XMFLOAT3>& GetNor() { return m_nor; }
	std::vector<DirectX::XMFLOAT2>& GetTex() { return m_tex; }

	DirectX::XMFLOAT4& GetDiffuse(int num) { return m_diffuse[num]; }
	DirectX::XMFLOAT4& GetSpecular(int num) { return m_specular[num]; }
	DirectX::XMFLOAT4& GetAmbient(int num) { return m_ambient[num]; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(int num) { return m_srv[num]; }

	std::vector<int>& GetIndex(int num) { return m_subset[num]; }

	int GetVertexCount() { return (int)m_pos.size(); }

	int GetIndexCount(int num) { return (int)m_subset[num].size(); }

	int GetSubsetCount() { return (int)m_subset.size(); }
};


#endif