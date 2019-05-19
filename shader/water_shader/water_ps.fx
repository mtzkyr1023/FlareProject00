
#define RAY_NUM 30
#define NOISE_POWER 50000

float3 mod(float3 a, float b) { return a-b*floor(a/b); }

float noise(float2 seed) {
    return frac(sin(dot(seed.xy, seed.yx)) * NOISE_POWER);
}

cbuffer VPMatrix : register(b0) {
	matrix g_vpMatrix;
}

TextureCube cubeMap : register(t0);
Texture2D g_color : register(t1);
Texture2D<float> g_depth : register(t2);
SamplerState samplerType : register(s0);

struct PS_IN {
	float4 pos : POSITION0;
	float3 nor : NORMAL0;
	float3 wnor : NORMAL1;
	float3 vpos : POSITION1;
	float3 wpos : POSITION2;
	float2 tex : TEXCOORD0;
	float linearZ : TEXCOORD1;
};

struct PS_OUT {
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
};

PS_OUT ps_main(PS_IN input) {
	PS_OUT output = (PS_OUT)0;
	float3 vReflect = reflect(input.vpos, input.wnor);
	float4 color = (float4)0;
	
	float3 step = 2.0f / RAY_NUM * vReflect;
	float thicness = 0.1f / RAY_NUM;
	float2 spos = input.pos.xy;
	spos.x = (1.0f + (spos.x)) * 0.5f;
	spos.y = (1.0f - (spos.y)) * 0.5f;
	
	for (int i = 1; i <= RAY_NUM; i++) {
		float3 rayPos = input.wpos + step * ((float)i + noise(spos));
		float4 vpPos = mul(float4(rayPos, 1.0f), g_vpMatrix);
		vpPos /= vpPos.w;
		vpPos.x = (1.0f + (vpPos.x)) * 0.5f;
		vpPos.y = (1.0f - (vpPos.y)) * 0.5f;
		float gbufDepth = g_depth.Sample(samplerType, vpPos.xy);
		if (vpPos.z - gbufDepth > 0.0f && vpPos.z - gbufDepth < thicness) {
			color += g_color.Sample(samplerType, vpPos.xy);
			break;
		}
		
		if (i == RAY_NUM) {
			color += cubeMap.Sample(samplerType, vReflect);
		}
	}
	
	color *= rcp(RAY_NUM) * 4.0f;
	
	float intensity = dot(input.wnor, normalize(float3(1.0f, 1.0f, 1.0f))) * 0.5f + 0.5f;
	intensity *= intensity;
	
	
	
//	output.color = float4(float3(1.0f, 1.0f, 1.0f) * intensity * color.rgb, 0.25f);
	output.color = float4(color.rgb + float3(0.0f, 0.0f, 0.3f) * intensity, 1.0f);
	output.normal = float4(input.nor, input.linearZ);
	
	return output;
}