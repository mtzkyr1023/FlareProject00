#include "ObjLoader.h"

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace DirectX;


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