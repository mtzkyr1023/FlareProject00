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

	for (int i = 0; i < skeletonCount; i++)
		m_boneIndexArray[skeletonArray[i]->GetNode()->GetName()];

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
		vertexCount = indexCount;

		FbxVector4* srcPos = meshArray[i]->GetControlPoints();
		int* vIndex = meshArray[i]->GetPolygonVertices();

/*		int layerCount = meshArray[i]->GetLayerCount();
		for (int j = 0; j < layerCount; j++) {
			FbxLayer* layer = meshArray[i]->GetLayer(j);
			FbxLayerElementNormal* normalElem = layer->GetNormals();
			FbxLayerElementUV* uvElem = layer->GetUVs();
			if (normalElem)
				this->GetNormal(normalElem, vertexCount, vIndex, m_meshArray[i].nor, normalCount);
			if (uvElem)
				this->GetUV(uvElem, vertexCount, vIndex, tex, uvCount);
		}*/
		FbxArray<FbxVector4> norArray;
		meshArray[i]->GetPolygonVertexNormals(norArray);

		FbxStringList uvsetNames;
		meshArray[i]->GetUVSetNames(uvsetNames);
		FbxArray<FbxVector2> uvArray;
		meshArray[i]->GetPolygonVertexUVs(uvsetNames.GetStringAt(0), uvArray);

//		if (vertexCount >= normalCount) {
			m_meshArray[i].pos.resize(indexCount);
			m_meshArray[i].nor.resize(indexCount);
			m_meshArray[i].tex.resize(indexCount);
			for (int j = 0; j < vertexCount; j++) {
				m_meshArray[i].pos[j] = XMFLOAT3((float)srcPos[vIndex[j]][0], (float)srcPos[vIndex[j]][1], (float)srcPos[vIndex[j]][2]);
				m_meshArray[i].nor[j] = XMFLOAT3((float)norArray[j][0], (float)norArray[j][1], (float)norArray[j][2]);
				m_meshArray[i].tex[j] = XMFLOAT2((float)uvArray[j][0], (float)uvArray[j][1]);
			}
			m_meshArray[i].index.resize(indexCount);
			for (int j = 0; j < indexCount; j++) {
				m_meshArray[i].index[j] = j;
			}

			m_meshArray[i].vertexCount = vertexCount;
			m_meshArray[i].indexCount = indexCount;
//		}
/*		else {
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

			vertexCount = normalCount;*/
//		}

/*		if (vertexCount > uvCount) {
			for (int j = 0; j < vertexCount; j++) {
				m_meshArray[i].tex[j] = XMFLOAT2(0.0f, 0.0f);
			}
		}
		else {*/
	//		m_meshArray[i].tex.resize(uvCount);
	//		for (int j = 0; j < uvCount; j++) {
	//			m_meshArray[i].tex[j] = tex[j];
	//		}
//		}

		this->GetBoneInfo(meshArray[i], m_meshArray[i].vertexCount, m_meshArray[i].indexCount, m_meshArray[i].index.data(),
			m_meshArray[i].boneWeight, m_meshArray[i].boneIndex);
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
				XMStoreFloat3(&ret[index[i]], XMVector3Normalize(XMLoadFloat3(&ret[index[i]])));
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

void FbxLoader::GetBoneInfo(FbxMesh* mesh, int vertexCount, int indexCount, int* index, std::vector<DirectX::XMFLOAT4>& retWeight,
	std::vector<DirectX::XMUINT4>& retIndex) {
	vector<deque<pair<DWORD, float>>> tmpBone(vertexCount);
	int skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
	retWeight.resize(vertexCount);
	retIndex.resize(vertexCount);
	if (skinCount == 0) {
		for (int i = 0; i < vertexCount; i++) {
			retIndex[i] = XMUINT4(0, 0, 0, 0);
			retWeight[i] = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		}
	}
	else {
		FbxSkin* skin = (FbxSkin*)mesh->GetDeformer(0, FbxDeformer::eSkin);

		int clusterNum = skin->GetClusterCount();
		for (int k = 0; k < clusterNum; k++) {
			FbxCluster* cluster = skin->GetCluster(k);
			int pointNum = cluster->GetControlPointIndicesCount();
			int* pointAry = cluster->GetControlPointIndices();
			double* weightAry = cluster->GetControlPointWeights();

			for (int l = 0; l < pointNum; l++) {
				int vId = pointAry[l];
				tmpBone[vId].push_back(pair<DWORD, float>(m_boneIndexArray[cluster->GetLink()->GetName()], (float)weightAry[l]));
			}
		}

		for (int j = 0; j < (int)tmpBone.size(); j++) {

			deque<pair<DWORD, float>>& tmp = tmpBone[j];
			sort(tmp.begin(), tmp.end(), [](const std::pair<DWORD, float>& weightA, const std::pair<DWORD, float>& weightB) { return weightA.second > weightB.second; });
			while (tmp.size() > 4) {
				tmp.pop_back();
			}
			while (tmp.size() < 4) {
				tmp.push_back(pair<DWORD, float>(0, 0.0f));
			}
			sort(tmp.begin(), tmp.end(), [](const std::pair<DWORD, float>& weightA, const std::pair<DWORD, float>& weightB) { return weightA.second < weightB.second; });

			float total = 0.0f;
			for (int k = 0; k < 4; k++) {
				total += tmpBone[j][k].second;
			}
			for (int k = 0; k < 4; k++) {
				tmpBone[j][k].second /= total;
			}
		}

		for (int j = 0; j < (int)tmpBone.size(); j++) {
			UINT tmpIndex[4];
			float tmpWeight[4];
			for (int k = 0; k < 4; k++) {
				tmpIndex[k] = tmpBone[j][k].first;
				tmpWeight[k] = tmpBone[j][k].second;
			}
			retIndex[j].x = tmpIndex[0];
			retIndex[j].y = tmpIndex[1];
			retIndex[j].z = tmpIndex[2];
			retIndex[j].w = tmpIndex[3];
			retWeight[j].x = tmpWeight[0];
			retWeight[j].y = tmpWeight[1];
			retWeight[j].z = tmpWeight[2];
			retWeight[j].w = tmpWeight[3];
		}
	}
}
