struct PS_IN {
	float4 pos : SV_POSITION;
	float3 nor : NORMAL0;
	float2 tex : TEXCOORD0;
};

Texture2D<float4> diffuseTex : register(t0);

struct PS_OUT {
	float4 color : SV_Target0;
	float3 normal : SV_Target1;
};

PS_OUT model_ps_main(PS_IN input) {
	PS_OUT output = (PS_OUT)0;
	
	int width;
	int height;
	int level;
	
	diffuseTex.GetDimensions(width, height);
	
	float4 tmp = diffuseTex.Load(uint3(input.tex.x * (float)width, input.tex.y  * (float)height, 0.0f));
	
	output.color = float4(tmp.rgb, 1.0f);
	output.normal = input.nor;
	
	return output;
}