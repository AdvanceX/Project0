#include "ge_common.hlsl"
#ifdef INPUT_NORMAL
#include "ge_lighting.hlsl"
#endif

struct VSINPUT{
	float3 Coord: POSITION;
#ifdef INPUT_NORMAL
	float3 Normal: NORMAL;
#endif
#ifdef INPUT_TANGENT
	float3 Tangent: TANGENT;
#endif
#ifdef INPUT_COLOR
	float4 Color: COLOR;
#endif
#ifdef INPUT_TEXTURE
	float2 TexCoord: TEXCOORD;
#endif
#ifdef INPUT_BONES
	float3 Weights: WEIGHTS;
	uint4 Bones: BONEINDICES;
#endif
};
struct PSINPUT{
	float4 WVPCoord: SV_POSITION;
	float3 WCoord: POSITION;
#ifdef INPUT_NORMAL
	float3 Normal: NORMAL;
#endif
#ifdef INPUT_TANGENT
	float3 Tangent: TANGENT;
#endif
#ifdef INPUT_COLOR
	float4 Color: COLOR;
#endif
#ifdef INPUT_TEXTURE
	float2 TexCoord: TEXCOORD;
#endif
};

PSINPUT VertexShade(VSINPUT Input){
#ifdef INPUT_BONES
	float Weights[4];
	Weights[0] = Input.Weights.x;
	Weights[1] = Input.Weights.y;
	Weights[2] = Input.Weights.z;
	Weights[3] = 1.0f - Weights[0] - Weights[1] - Weights[2];

	float4 Coord = float4(Input.Coord, 1.0f);	
	Input.Coord = float3(0.0f, 0.0f, 0.0f);
#ifdef INPUT_NORMAL
	float3 Normal = Input.Normal;
	Input.Normal = float3(0.0f, 0.0f, 0.0f);
#endif

	for(uint i = 0; i < 4; ++i){
		if(Weights[i] == 0.0f) break;
		Input.Coord += Weights[i] * mul(Coord, gArrBone[Input.Bones[i]]).xyz;
#ifdef INPUT_NORMAL
		Input.Normal += Weights[i] * mul(Normal, (float3x3)gArrBone[Input.Bones[i]]);
#endif
	}
#endif

	PSINPUT Output;

	float4 WCoord = mul(float4(Input.Coord, 1.0f), gWorldTrans);
	Output.WCoord = WCoord.xyz;
	Output.WVPCoord = mul(WCoord, gViewProjTrans);

#ifdef INPUT_TEXTURE
	float4 TexCoord = float4(Input.TexCoord, 0.0f, 1.0f);
	Output.TexCoord = mul(TexCoord, gTexAnim).xy;
#endif
#ifdef INPUT_NORMAL
	Output.Normal = mul(float4(Input.Normal, 1.0f), gNormTrans).xyz;
#endif
#ifdef INPUT_TANGENT
	Output.Tangent = mul(float4(Input.Tangent, 1.0f), gNormTrans).xyz;
#endif
#ifdef INPUT_COLOR
	Output.Color = Input.Color;
#endif

	return Output;
}
float4 PixelShade(PSINPUT Input): SV_Target{
#ifdef INPUT_MATERIAL
	float4 Color = float4(gMaterial.Albedo, gMaterial.Opacity);
#else
	float4 Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
#endif

#ifdef INPUT_TEXTURE
	Color *= gTexture.Sample(gSampler, Input.TexCoord);
#endif
#ifdef INPUT_COLOR
	Color *= Input.Color;
#endif

#ifdef ALPHA_TEST
	clip(Color.a - 0.01);
#endif

#ifdef INPUT_NORMAL
	Input.Normal = normalize(Input.Normal);
#ifdef INPUT_TANGENT
	float4 BumpSample = gBumpMap.Sample(gSampler, Input.TexCoord);
	Input.Normal = TransformNormalSpace(BumpSample.xyz, normalize(Input.Normal), Input.Tangent);
#endif
	float3 ViewVector = GetViewVector(Input.WCoord);
	float3 DirectLight = ComputeLighting(Input.WCoord, Input.Normal, ViewVector);
	Color.rgb = (gAmbientLight + DirectLight) * Color.rgb;
#endif

	if(gFogRange > 0) Color.rgb =
		GetFogEffect(Input.WCoord, Color.rgb);

	return Color;
}