#include "ObjLoader.h"

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <unordered_map>

#include "Device.h"
#include "stb_lib/stb_image.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;


bool ObjLoader::ReadFile(const char* filename) {
	string buf;
	stringstream ss;
	XMFLOAT3 tmpVec;
	XMFLOAT2 tmpUV;
	int vIndex, uvIndex, nIndex;
	Mesh tmp;
	vector<int> indexTmp;
	vector<int> uvIndexTmp;
	vector<int> nIndexTmp;
	string matNameTmp;
	int state = 0;
	int indexState = 0;

	m_materialFileName = "res/model/";

	ifstream ifs(filename);
	if (ifs.fail())
		return false;

	while (getline(ifs, buf)) {
		if (buf.find("#") != string::npos)
			continue;
		if (buf.find("g ") != string::npos || buf.find("o ") != string::npos) {
			if (state == 0) {
				state = 1;
				indexState = 1;
			}
			else {
				state = 0;
				if (tmp.texcoords.size() == 0)
					tmp.texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
				m_mesh.push_back(tmp);
				tmp.Clear();
				m_indexArray.push_back(indexTmp);
				m_nIndexArray.push_back(nIndexTmp);
				m_uvIndexArray.push_back(uvIndexTmp);
				indexTmp.clear();
				nIndexTmp.clear();
				uvIndexTmp.clear();
			}
		}
		else if (buf.find("v ") != string::npos) {
			buf.replace(0, 1, "");
			sscanf_s(buf.c_str(), "%f %f %fn", &tmpVec.x, &tmpVec.y, &tmpVec.z);
			tmp.vertices.push_back(tmpVec);
		}
		else if (buf.find("vt ") != string::npos) {
			buf.replace(0, 2, "");
			sscanf_s(buf.c_str(), "%f %fn", &tmpUV.x, &tmpUV.y);
			tmp.texcoords.push_back(tmpUV);
		}
		else if (buf.find("vn ") != string::npos) {
			buf.replace(0, 2, "");
			sscanf_s(buf.c_str(), "%f %f %fn", &tmpVec.x, &tmpVec.y, &tmpVec.z);
			tmp.normals.push_back(tmpVec);
		}
		else if (buf.find("f ") != string::npos) {
			buf.replace(0, 1, " ");
			int num = (int)count(buf.begin(), buf.end(), '/') / 2;
			while (buf.find("//") != string::npos) {
				auto ite = buf.find("//");
				buf.insert(buf.begin() + ite + 1, '1');
			}
			replace(buf.begin(), buf.end(), '/', ' ');
			ss = stringstream(buf);
			int vi, uvi, ni, i2;
			for (int i = 0; i < num; i++) {
				ss >> vIndex >> uvIndex >> nIndex;
				if (i == 0) {
					vi = vIndex - 1;
					uvi = uvIndex - 1;
					ni = nIndex - 1;
				}
				if (i < 3) {
					tmp.vIndices.push_back(vIndex - 1);
					tmp.nIndices.push_back(nIndex - 1);
					tmp.uvIndices.push_back(uvIndex - 1);
					indexTmp.push_back(vIndex - 1);
					nIndexTmp.push_back(nIndex - 1);
					uvIndexTmp.push_back(uvIndex - 1);
				}
				else {
					i2 = tmp.vIndices[tmp.vIndices.size() - 1];
					tmp.vIndices.push_back(vi);
					tmp.vIndices.push_back(i2);
					tmp.vIndices.push_back(vIndex - 1);
					indexTmp.push_back(vi);
					indexTmp.push_back(i2);
					indexTmp.push_back(vIndex - 1);
					i2 = tmp.nIndices[tmp.nIndices.size() - 1];
					tmp.nIndices.push_back(ni);
					tmp.nIndices.push_back(i2);
					tmp.nIndices.push_back(nIndex - 1);
					nIndexTmp.push_back(ni);
					nIndexTmp.push_back(i2);
					nIndexTmp.push_back(nIndex - 1);
					i2 = tmp.uvIndices[tmp.uvIndices.size() - 1];
					tmp.uvIndices.push_back(uvi);
					tmp.uvIndices.push_back(i2);
					tmp.uvIndices.push_back(uvIndex - 1);
					uvIndexTmp.push_back(uvi);
					uvIndexTmp.push_back(i2);
					uvIndexTmp.push_back(uvIndex - 1);
				}
			}
		}
		else if (buf.find("s ") != string::npos){
			buf.replace(0, 1, "");
			if (buf.find("0") != string::npos || buf.find("off") != string::npos)
				tmp.smoothFlag = false;
			else
				tmp.smoothFlag = true;
		}
		else if (buf.find("mtllib") != string::npos) {
			buf.replace(0, 6, "");
			m_materialFileName += buf;
		}
		else if (buf.find("usemtl") != string::npos) {
			buf.replace(0, 6, "");
			matNameTmp = buf;
			m_indexArray.push_back(indexTmp);
			m_nIndexArray.push_back(nIndexTmp);
			m_uvIndexArray.push_back(uvIndexTmp);
			indexTmp.clear();
			nIndexTmp.clear();
			uvIndexTmp.clear();
			m_materialNameArray.push_back(matNameTmp);
		}
	}


	if (tmp.texcoords.size() == 0)
		tmp.texcoords.push_back(XMFLOAT2(1.0f, 1.0f));
	m_mesh.push_back(tmp);

	return true;
}


bool ObjLoaderKai::ReadFile(const char* filename) {
	string buf;
	string materialFileName;
	string materialName;
	stringstream ss;
	vector<XMFLOAT3> tmpPos;
	vector<XMFLOAT3> tmpNor;
	vector<XMFLOAT2> tmpUV;
	int vIndex, uvIndex, nIndex;
	vector<int> indexTmp;
	vector<int> uvIndexTmp;
	vector<int> nIndexTmp;
	unordered_map<string, SubsetInfo> subsetInfo;
	bool smoothFlag = false;
	int state = 0;
	int indexCount = 0;
	int subsetNum = 0;

	materialFileName = "res/model/";

	ifstream ifs(filename);
	if (ifs.fail())
		return false;


	while (getline(ifs, buf)) {
		if (buf.find("#") != string::npos)
			continue;

		if (buf.find("g ") != string::npos || buf.find("o ") != string::npos) {
		}
		else if (buf.find("v ") != string::npos) {
			buf.replace(0, 1, "");
			float x, y, z;
			sscanf_s(buf.c_str(), "%f %f %fn", &x, &y, &z);
			tmpPos.push_back(XMFLOAT3(x, y, z));
		}
		else if (buf.find("vt ") != string::npos) {
			buf.replace(0, 2, "");
			float x, y;
			sscanf_s(buf.c_str(), "%f %fn", &x, &y);
			tmpUV.push_back(XMFLOAT2(x, y));
		}
		else if (buf.find("vn ") != string::npos) {
			buf.replace(0, 2, "");
			float x, y, z;
			sscanf_s(buf.c_str(), "%f %f %fn", &x, &y, &z);
			tmpNor.push_back(XMFLOAT3(x, y, z));
		}
		else if (buf.find("f ") != string::npos) {
			buf.replace(0, 1, " ");
			int num = (int)count(buf.begin(), buf.end(), '/') / 2;
			while (buf.find("//") != string::npos) {
				auto ite = buf.find("//");
				buf.insert(buf.begin() + ite + 1, '1');
			}
			replace(buf.begin(), buf.end(), '/', ' ');
			ss = stringstream(buf);
			int vi, uvi, ni, i2;
			for (int i = 0; i < num; i++) {
				ss >> vIndex >> uvIndex >> nIndex;
				if (i == 0) {
					vi = vIndex - 1;
					uvi = uvIndex - 1;
					ni = nIndex - 1;
				}
				if (i < 3) {
					indexTmp.push_back(vIndex - 1);
					nIndexTmp.push_back(nIndex - 1);
					uvIndexTmp.push_back(uvIndex - 1);
				}
				else {
					i2 = indexTmp[indexTmp.size() - 1];
					indexTmp.push_back(vi);
					indexTmp.push_back(i2);
					indexTmp.push_back(vIndex - 1);
					i2 = nIndexTmp[nIndexTmp.size() - 1];
					nIndexTmp.push_back(ni);
					nIndexTmp.push_back(i2);
					nIndexTmp.push_back(nIndex - 1);
					i2 = uvIndexTmp[uvIndexTmp.size() - 1];
					uvIndexTmp.push_back(uvi);
					uvIndexTmp.push_back(i2);
					uvIndexTmp.push_back(uvIndex - 1);
				}
			}
		}
		else if (buf.find("s ") != string::npos) {
			buf.replace(0, 1, "");
			if (buf.find("0") != string::npos || buf.find("off") != string::npos)
				smoothFlag = false;
			else
				smoothFlag = true;
		}
		else if (buf.find("mtllib") != string::npos) {
			buf.replace(0, 7, "");
			materialFileName += buf;
		}
		else if (buf.find("usemtl") != string::npos) {
			buf.replace(0, 7, "");
			if (state == 1) {
				if (subsetInfo.find(materialName) == subsetInfo.end()) {
					subsetInfo[materialName].posIndex = indexTmp;
					subsetInfo[materialName].norIndex = nIndexTmp;
					subsetInfo[materialName].uvIndex = uvIndexTmp;
					subsetInfo[materialName].smooth = smoothFlag;
					indexCount += (int)indexTmp.size();

					m_subsetId[materialName] = subsetNum++;
					m_subsetStr[subsetNum - 1] = materialName;
				}
				else {
					subsetInfo[materialName].posIndex.reserve(subsetInfo[materialName].posIndex.size() + indexTmp.size());
					subsetInfo[materialName].uvIndex.reserve(subsetInfo[materialName].norIndex.size() + nIndexTmp.size());
					subsetInfo[materialName].norIndex.reserve(subsetInfo[materialName].uvIndex.size() + uvIndexTmp.size());
					copy(indexTmp.begin(), indexTmp.end(), back_inserter(subsetInfo[materialName].posIndex));
					copy(nIndexTmp.begin(), nIndexTmp.end(), back_inserter(subsetInfo[materialName].norIndex));
					copy(uvIndexTmp.begin(), uvIndexTmp.end(), back_inserter(subsetInfo[materialName].uvIndex));
					indexCount += (int)indexTmp.size();
				}

				indexTmp.clear();
				nIndexTmp.clear();
				uvIndexTmp.clear();

				state = 0;
			}
			else {
				state = 1;
			}

			materialName = buf;
		}
	}

	if (subsetInfo.find(materialName) == subsetInfo.end()) {
		subsetInfo[materialName].posIndex = indexTmp;
		subsetInfo[materialName].norIndex = nIndexTmp;
		subsetInfo[materialName].uvIndex = uvIndexTmp;
		subsetInfo[materialName].smooth = smoothFlag;
		indexCount += (int)indexTmp.size();

		m_subsetId[materialName] = subsetNum++;
		m_subsetStr[subsetNum - 1] = materialName;
	}
	else {
		subsetInfo[materialName].posIndex.reserve(subsetInfo[materialName].posIndex.size() + indexTmp.size());
		subsetInfo[materialName].uvIndex.reserve(subsetInfo[materialName].norIndex.size() + nIndexTmp.size());
		subsetInfo[materialName].norIndex.reserve(subsetInfo[materialName].uvIndex.size() + uvIndexTmp.size());
		copy(indexTmp.begin(), indexTmp.end(), back_inserter(subsetInfo[materialName].posIndex));
		copy(nIndexTmp.begin(), nIndexTmp.end(), back_inserter(subsetInfo[materialName].norIndex));
		copy(uvIndexTmp.begin(), uvIndexTmp.end(), back_inserter(subsetInfo[materialName].uvIndex));
		indexCount += (int)indexTmp.size();
	}

	if (tmpUV.size() == 0)
		tmpUV.push_back(XMFLOAT2(0.0f, 0.0f));
	if (tmpNor.size() == 0)
		tmpNor.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));

	indexTmp.clear();
	nIndexTmp.clear();
	uvIndexTmp.clear();

	int i = 0, vCount = 0;
	m_pos.resize(indexCount);
	m_nor.resize(indexCount);
	m_tex.resize(indexCount);
	m_subset.resize(subsetInfo.size());


	for (const auto& ite : subsetInfo) {

		m_subset[m_subsetId[ite.first]].resize(ite.second.posIndex.size());
		for (int j = 0; j < (int)ite.second.posIndex.size(); j++) {
			m_pos[vCount] = tmpPos[ite.second.posIndex[j]];
			m_nor[vCount] = tmpNor[ite.second.norIndex[j]];
			m_tex[vCount] = tmpUV[ite.second.uvIndex[j]];

			m_subset[m_subsetId[ite.first.c_str()]][j] = vCount;
			vCount++;
		}
		i++;
	}

	ifs.close();

	if (!this->ReadMaterial(materialFileName.c_str()))
		return false;

	return true;
}

bool ObjLoaderKai::ReadMaterial(const char* matFilename) {
	ifstream ifs;
	string buf;
	string texname = "res/model/";
	string materialName;
	stringstream ss;
	XMFLOAT4 tmpDiffuse(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4 tmpAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4 tmpSpecular(0.0f, 0.0f, 0.0f, 0.0f);
	ComPtr<ID3D11ShaderResourceView> tmpsrv;
	int state = 0;
	int id = 0;
	unordered_map<string, int> mat;

	ifs.open(matFilename);

	if (ifs.fail())
		return false;

	m_diffuse.resize(m_subsetId.size());
	m_ambient.resize(m_subsetId.size());
	m_specular.resize(m_subsetId.size());
	m_srv.resize(m_subsetId.size());

	while (getline(ifs, buf)) {
		if (buf.find("#") != string::npos)
			continue;

		if (buf.find("newmtl ") != string::npos) {
			buf.replace(0, 7, "");
			if (state == 1) {
				if (m_subsetId.find(buf.c_str()) != m_subsetId.end()) {
					m_diffuse[m_subsetId[materialName]] = tmpDiffuse;
					m_specular[m_subsetId[materialName]] = tmpSpecular;
					m_ambient[m_subsetId[materialName]] = tmpAmbient;
					m_srv[m_subsetId[materialName]] = tmpsrv;
					tmpsrv.Reset();
				}
				mat[buf];
			}
			else {
				state = 1;
			}

			materialName = buf;
		}
		else if (buf.find("Ka ") != string::npos && buf.find("map_") == string::npos) {
			buf.replace(0, 3, "");
			sscanf_s(buf.c_str(), "%f %f %fn", &tmpAmbient.x, &tmpAmbient.y, &tmpAmbient.z);
		}
		else if (buf.find("Kd ") != string::npos && buf.find("map_") == string::npos) {
			buf.replace(0, 3, "");
			sscanf_s(buf.c_str(), "%f %f %fn", &tmpDiffuse.x, &tmpDiffuse.y, &tmpDiffuse.z);
		}
		else if (buf.find("Ks ") != string::npos && buf.find("map_") == string::npos) {
			buf.replace(0, 3, "");
			sscanf_s(buf.c_str(), "%f %f %fn", &tmpSpecular.x, &tmpSpecular.y, &tmpSpecular.z);
		}
		else if (buf.find("Kn ") != string::npos && buf.find("map_") == string::npos) {
			buf.replace(0, 3, "");
			sscanf_s(buf.c_str(), "%fn", &tmpSpecular.w);
		}
		else if (buf.find("map_Kd ") != string::npos || buf.find("map_Ks ") != string::npos || buf.find("map_Ka ") != string::npos) {
			BYTE* pixels;
			int width;
			int height;
			int bpp;

			buf.replace(0, 7, "");
			texname += buf.c_str();
			pixels = stbi_load(texname.c_str(), &width, &height, &bpp, 4);

			HRESULT res;
			D3D11_TEXTURE2D_DESC texDesc{};
			D3D11_SUBRESOURCE_DATA initData{};
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			ComPtr<ID3D11Texture2D> tex;

			texDesc.Width = width;
			texDesc.Height = height;
			texDesc.ArraySize = 1;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texDesc.CPUAccessFlags = 0;
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.MipLevels = 1;
			texDesc.MiscFlags = 0;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;

			initData.pSysMem = pixels;
			initData.SysMemPitch = width * 4;
			initData.SysMemSlicePitch = 0;

			srvDesc.Format = texDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;

			res = DeviceUser::GetDevice()->CreateTexture2D(&texDesc, &initData, tex.GetAddressOf());

			res = DeviceUser::GetDevice()->CreateShaderResourceView(tex.Get(), &srvDesc, tmpsrv.ReleaseAndGetAddressOf());

		}
	}

	m_diffuse[m_subsetId[materialName]] = tmpDiffuse;
	m_specular[m_subsetId[materialName]] = tmpSpecular;
	m_ambient[m_subsetId[materialName]] = tmpAmbient;
	m_srv[m_subsetId[materialName]] = tmpsrv;

	return true;
}