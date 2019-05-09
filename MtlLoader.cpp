
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "MtlLoader.h"


using namespace std;
using namespace DirectX;


bool MtlLoader::LoadMaterial(const char* filename, map<string, Material>& material) {
	string buf;
	stringstream ss;
	float r = 0, g = 0, b = 0;
	Material tmp;
	string matName;
	int state = 0;

	ifstream ifs(filename);
	if (ifs.fail())
		return false;

	while (getline(ifs, buf)) {
		if (buf.find("#") != string::npos)
			continue;

		if (buf.find("newmtl ") != string::npos) {
			buf.replace(0, 6, "");
			matName = buf;
			if (state == 1) {
				material[matName.c_str()] = tmp;
			}
			else {
				state = 1;
			}
		}
		else if (buf.find("Ka ") != string::npos) {
			buf.replace(0, 2, "");
			sscanf_s(buf.c_str(), "%f %f %fn", &r, &g, &b);
			tmp.SetAmbient(r, g, b, 1.0f);
		}
		else if (buf.find("Kd ") != string::npos) {
			buf.replace(0, 2, "");
			sscanf_s(buf.c_str(), "%f %f %fn", &r, &g, &b);
			tmp.SetDiffuse(r, g, b, 1.0f);
		}
		else if (buf.find("Ks ") != string::npos) {
			buf.replace(0, 2, "");
			sscanf_s(buf.c_str(), "%f %f %fn", &r, &g, &b);
			tmp.SetSpecular(r, g, b);
		}
		else if (buf.find("Ns ") != string::npos) {
			buf.replace(0, 2, "");
			sscanf_s(buf.c_str(), "%fn", &r);
			tmp.SetSpecularPower(r);
		}
	}


	material[matName.c_str()] = tmp;

	return true;
}