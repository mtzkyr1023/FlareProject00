#ifndef _MTLLOADER_H_
#define _MTLLOADER_H_

#include <map>
#include <string>

#include "Material.h"

class MtlLoader {
public:
	bool LoadMaterial(const char* filename, std::map<std::string, Material>& material);
};

#endif