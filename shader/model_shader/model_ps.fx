
#include "../shader_func/pbr_func.hlsli"

Texture2D tex : register(t0);
Texture2D<float> shadowTex : register(t1);
TextureCube cubeTex : register(t2);
SamplerState SampleType : register(s0);
SamplerState SampleType2 : register(s1);

cbuffer MaterialBuffer : register(b0) {
	float4 diffuse;
	float metalness;
	float roughness;
	float materialPadding1;
	float materialPadding2;
}

cbuffer DirectionalLight : register(b1) {
	float4 g_lightDir;
	float4 g_lightColor;
	float4 g_vsLightDir;
}

struct PS_IN {
	float4 pos : SV_POSITION;
	float3 nor : NORMAL0;
	float2 tex : TEXCOORD0;
	float linearZ : TEXCOORD1;
	float4 shadowPos : TEXCOORD2;
	float3 wnor : NORMAL1;
	float3 viewPos : TEXCOORD3;
	float3 wpos : TEXCOORD4;
};

struct PS_OUT {
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
	float metalness : SV_Target2;
	float roughness : SV_Target3;
};

PS_OUT ps_main(PS_IN input) {
	PS_OUT output = (PS_OUT)0;
	float4 texColor = tex.Sample(SampleType, input.tex);
	float3 V = -normalize(input.wpos - input.viewPos);
	float3 L = normalize(g_lightDir.xyz);
	float3 H = normalize(V + L);
	float3 N = input.wnor;
	float D = GGX(roughness, H, N);
	float F = Flesnel(metalness, V, H);
	float G = G_CookTorrance(L, V, H, N);
	float reflection = (D * F * G) / (4 * saturate(dot(V, N)) * saturate(dot(L, N)) + 0.0000001f);

//	float intensity = saturate(dot(input.wnor, normalize(g_lightDir.xyz))) / PI;
	float rough2 = roughness * roughness;
	float A = 1.0f - (rough2 / (rough2 + 0.57)) * 0.5f;
	float B = (rough2 / (rough2 + 0.09)) * 0.45f;
	float dotL = dot(N, L);
	float dotV = dot(N, -V);
	float3 al = L - N * dotL;
	float3 ae = -V - N * dotV;
	float gamma = max(0, dot(al, ae));
	float angleL = acos(dotL);
	float angleV = acos(dotV);
	float alpha = max(angleL, angleV);
	float beta = min(angleL, angleV);
	float intensity = max(0.0f, dotL) * (A + (B * max(0, cos(angleL - angleV) * gamma * sin(alpha) * tan(beta))));
//	intensity = intensity * .5f + 0.5f;
//	intensity *= intensity;
	
	float2 shadowUV;
	shadowUV.x = (1.0f + (input.shadowPos.x)) * 0.5f;
	shadowUV.y = (1.0f + (input.shadowPos.y)) * -0.5f;

	float depth = shadowTex.Sample(SampleType, shadowUV);


	float realZ = input.shadowPos.z;

	if (realZ > depth + 0.0005f) {
		depth = 0.5f;
	}
	else {
		depth = 1.0f;
	}
	
	realZ = 1.0f - (realZ - depth) * 0.5f;

//	output.color = texColor;

//	output.color = saturate(diffuse * intensity * depth + texColor);

//	output.color = diffuse * intensity;

//	output.color = saturate(diffuse * g_lightColor * intensity + g_lightColor * reflection * cubeTex.Sample(SampleType, normalize(reflect(-V, N))));
	output.color = diffuse * g_lightColor * intensity + g_lightColor * reflection;

//	output.color = float4(intensity, intensity, intensity, 1);

	output.normal = float4(input.nor, input.linearZ);
	
	output.metalness = metalness;
	output.roughness = roughness;
	
	return output;
}