#include "FbxLoader.h"
using namespace std;
using namespace DirectX;

bool FbxLoader::LoadFbx(const char * filename) {
	FbxManager* manager = FbxManager::Create();

	FbxIOSettings* ioSettings = FbxIOSettings::Create(manager, IOSROOT);

	FbxImporter* importer = FbxImporter::Create(manager, "");
	if (!importer->Initialize(filename, -1, ioSettings))
		return false;

	FbxScene* scene = FbxScene::Create(manager, "");
	importer->Import(scene);

	FbxGeometryConverter converter(manager);
	converter.Triangulate(scene, true);
	converter.SplitMeshesPerMaterial(scene, true);

	importer->Destroy();

	vector<FbxMesh*> meshArray;
	vector<FbxSkeleton*> skeletonArray;

	int m_meshCount = scene->GetSrcObjectCount<FbxMesh>();
	for (int i = 0; i < m_meshCount; i++)
		meshArray.push_back((FbxMesh*)(scene->GetSrcObject<FbxMesh>(i)));

	int skeletonCount = scene->GetSrcObjectCount<FbxSkeleton>();
	for (int i = 0; i < skeletonCount; i++)
		skeletonArray.push_back((FbxSkeleton*)(scene->GetSrcObject<FbxSkeleton>(i)));

	m_meshArray.resize(m_meshCount);
	for (int i = 0; i < m_meshCount; i++) {
		vector<XMFLOAT3> pos;
		vector<XMFLOAT2> tex;
		int vertexCount;
		int indexCount;
		int normalCount = 0;
		int uvCount = 0;

		vertexCount = meshArray[i]->GetControlPointsCount();
		indexCount = meshArray[i]->GetPolygonCount() * 3;

		FbxVector4* srcPos = meshArray[i]->GetControlPoints();
		int* vIndex = meshArray[i]->GetPolygonVertices();

		int layerCount = meshArray[i]->GetLayerCount();
		for (int j = 0; j < layerCount; j++) {
			FbxLayer* layer = meshArray[i]->GetLayer(j);
			FbxLayerElementNormal* normalElem = layer->GetNormals();
			FbxLayerElementUV* uvElem = layer->GetUVs();
			if (normalElem)
				this->GetNormal(normalElem, vertexCount, vIndex, m_meshArray[i].nor, normalCount);
			if (uvElem)
				this->GetUV(uvElem, vertexCount, vIndex, tex, uvCount);
		}

		if (vertexCount >= normalCount) {
			m_meshArray[i].pos.resize(vertexCount);
			for (int j = 0; j < vertexCount; j++) {
				m_meshArray[i].pos[j] = XMFLOAT3((float)srcPos[j][0], (float)srcPos[j][1], (float)srcPos[j][2]);
			}
			m_meshArray[i].index.resize(indexCount);
			for (int j = 0; j < indexCount; j++) {
				m_meshArray[i].index[j] = vIndex[j];
			}

			m_meshArray[i].vertexCount = vertexCount;
			m_meshArray[i].indexCount = indexCount;
		}
		else {
			m_meshArray[i].pos.resize(normalCount);
			for (int j = 0; j < normalCount; j++) {
				m_meshArray[i].pos[j].x = (float)srcPos[vIndex[j]][0];
				m_meshArray[i].pos[j].y = (float)srcPos[vIndex[j]][1];
				m_meshArray[i].pos[j].z = (float)srcPos[vIndex[j]][2];
			}

			m_meshArray[i].index.resize(indexCount);
			for (int j = 0; j < indexCount; j++) {
				m_meshArray[i].index[j] = j;
			}

			m_meshArray[i].vertexCount = normalCount;
			m_meshArray[i].indexCount = indexCount;

			vertexCount = normalCount;
		}

		if (vertexCount > uvCount) {
			for (int j = 0; j < vertexCount; j++) {
				m_meshArray[i].tex[j] = XMFLOAT2(0.0f, 0.0f);
			}
		}
		else {
			m_meshArray[i].tex.resize(uvCount);
			for (int j = 0; j < uvCount; j++) {
				m_meshArray[i].tex[j] = tex[j];
			}
		}
	}

	return true;
}

void FbxLoader::GetNormal(FbxLayerElementNormal* normal, int vertexCount, int* index, vector<XMFLOAT3>& ret, int& normalCount) {
	FbxLayerElement::EMappingMode mappingMode = normal->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = normal->GetReferenceMode();

	int normalNum = normal->GetDirectArray().GetCount();
	int indexNum = normal->GetIndexArray().GetCount();

	if (mappingMode == FbxLayerElement::eByPolygonVertex) {
		if (refMode == FbxLayerElement::eDirect) {
			normalCount = normalNum;
			ret.resize(normalNum);
			for (int i = 0; i < normalNum; ++i) {
				ret[i].x = (float)normal->GetDirectArray().GetAt(i)[0];
				ret[i].y = (float)normal->GetDirectArray().GetAt(i)[1];
				ret[i].z = (float)normal->GetDirectArray().GetAt(i)[2];
			}
		}
	}
	else if (mappingMode == FbxLayerElement::eByControlPoint) {
		if (refMode == FbxLayerElement::eDirect) {
			normalCount = vertexCount;
			ret.resize(vertexCount);
			for (int i = 0; i < vertexCount; ++i) {
				ret[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			for (int i = 0; i < normalNum; ++i) {
				ret[index[i]].x += (float)normal->GetDirectArray().GetAt(i)[0];
				ret[index[i]].y += (float)normal->GetDirectArray().GetAt(i)[1];
				ret[index[i]].z += (float)normal->GetDirectArray().GetAt(i)[2];
			}
		}
	}
}

void FbxLoader::GetUV(FbxLayerElementUV* uv, int vertexCount, int* index, vector<XMFLOAT2>& ret, int& uvCount) {
	FbxLayerElement::EMappingMode mappingMode = uv->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = uv->GetReferenceMode();

	int UVNum = uv->GetDirectArray().GetCount();
	int indexNum = uv->GetIndexArray().GetCount();

	if (mappingMode == FbxLayerElement::eByPolygonVertex) {
		if (refMode == FbxLayerElement::eDirect) {
			uvCount = vertexCount;
			ret.resize(vertexCount);
			for (int i = 0; i < UVNum; ++i) {
				ret[index[i]].x = (float)uv->GetDirectArray().GetAt(i)[0];
				ret[index[i]].y = (float)uv->GetDirectArray().GetAt(i)[1];
			}
		}
		else if (refMode == FbxLayerElement::eIndexToDirect) {
			ret.resize(indexNum);
			uvCount = indexNum;
			for (int i = 0; i < indexNum; ++i) {
				int index = uv->GetIndexArray().GetAt(i);
				ret[i].x = (float)uv->GetDirectArray().GetAt(index)[0];
				ret[i].y = (float)uv->GetDirectArray().GetAt(index)[1];
			}
		}
	}
}