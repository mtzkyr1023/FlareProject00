#include "ReflecetPass.h"
#include "Environment.h"
using namespace std;


bool ReflectPass::InitPass() {
	m_cs = make_unique<ComputeShader>();

	if (!m_cs->Initialize(L"shader/compute_shader/ss_cs.fx", "cs_main"))
		return false;

	if (!InitRelfectMap())
		return false;


	m_cubeMapSrv = Environment::Inst().GetCubeMap();

	return true;
}


bool ReflectPass::RenderPass() {

}