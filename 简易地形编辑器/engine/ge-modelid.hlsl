//****************************************//

#include "ge-param.hlsl"

//****************************************//


//-------------------- 数据结构 --------------------//

// Vertex Shader Input
struct VSINPUT{
	float3 CoordL   : IP_Position;
	float3 NormalL  : IP_Normal;
#ifdef INPUT_TANGENT
	float3 TangentL : IP_Tangent;
#endif
#ifdef INPUT_COLOR
	float4 Color    : IP_Color;
#endif
#ifdef INPUT_BONERIG
	uint4  Joints   : IP_BoneIndices;
	float3 Weights  : IP_BoneWeights;
#endif
#ifdef INPUT_TEXCOORD
	float2 TexCoord : IP_TexCoord;
#endif
};

// Pixel Shader Input
struct PSINPUT{
	float4 CoordP   : SV_POSITION;
	float3 CoordW   : IP_Position;
	float3 NormalW  : IP_Normal;
#ifdef INPUT_TANGENT
	float3 TangentW : IP_Tangent;
#endif
#ifdef INPUT_COLOR
	float4 Color    : IP_Color;
#endif
#ifdef INPUT_TEXCOORD
	float2 TexCoord : IP_TexCoord;
#endif
	uint ObjId      : IP_Ident;
};

// Pixel Shader Output
struct PSOUTPUT{
	float4 Color : SV_Target0;
    uint   ObjId : SV_Target1;
};

//----------------------------------------//


//-------------------- 常量数据 --------------------//

//对象属性
cbuffer cbObject : register(b2){
	float4x4 gWorldMat;
	float4x4 gTexOrNormMat;
};

//子对象属性
cbuffer cbSubobj : register(b3){
	MATBASE gMtlBase;
};

//----------------------------------------//


//-------------------- 纹理数据 --------------------//

Texture2D gColorMap  : register(t0, space0);
Texture2D gNormMap   : register(t1, space0);
Texture2D gRoughMap  : register(t2, space0);
Texture2D gAlphaMap  : register(t3, space0);
Texture2D gEmissMap  : register(t4, space0);
Texture2D gOcclusMap : register(t5, space0);

StructuredBuffer<float4x4> gBoneList : register(t1, space1);

//----------------------------------------//


//-------------------- 辅助函数 --------------------//

//解析材质
MATBASE ParseMaterial(float2 TexCoord){
	MATBASE MtlBase = gMtlBase;
	DWORD TexCtlg = gMtlBase.TexAttrs.r;
	DWORD AddrMode = gMtlBase.TexAttrs.g;

	if (TexCtlg & TEX_TYPE_Albedo){
		float4 Sample = TextureSampling2(gColorMap, TexCoord, AddrMode);
		MtlBase.Albedo = Sample.rgb;
		MtlBase.Opacity = Sample.a;
	}
	if (TexCtlg & TEX_TYPE_MetalRg){
		float4 Sample = TextureSampling2(gRoughMap, TexCoord, AddrMode);
		MtlBase.Roughness = Sample.r;
		MtlBase.Metalness = Sample.g;
	}
	if (TexCtlg & TEX_TYPE_Emission){
		float4 Sample = TextureSampling2(gEmissMap, TexCoord, AddrMode);
		MtlBase.Emission = saturate(gMtlBase.Emission + Sample.rgb);
	}

	return MtlBase;
}

//骨骼动画
void BoneAnimation(inout VSINPUT Input){
#ifdef INPUT_BONERIG
	
	float rgWeight[4];
	rgWeight[0] = Input.Weights.r;
	rgWeight[1] = Input.Weights.g;
	rgWeight[2] = Input.Weights.b;
	rgWeight[3] = 1.f - rgWeight[0] - rgWeight[1] - rgWeight[2];

	float4 OrgCoord = float4(Input.CoordL, 1.f);
	Input.CoordL = (float3)0.f;
	float4 OrgNorm = float4(Input.NormalL, 0.f);
	Input.NormalL = (float3)0.f;
#ifdef INPUT_TANGENT
	float4 OrgTan = float4(Input.TangentL, 0.f);
	Input.TangentL = (float3)0.f;
#endif

	for(uint iExe = 0; iExe < 4; ++iExe){
		if(rgWeight[iExe] == 0.f) break;
		uint BoneId = Input.Joints[iExe];
		float Weight = rgWeight[iExe];

		Input.CoordL += mul(OrgCoord, gBoneList[BoneId]).xyz * Weight;
		Input.NormalL += mul(OrgNorm, gBoneList[BoneId]).xyz * Weight;
#ifdef INPUT_TANGENT
		Input.TangentL += mul(OrgTan, gBoneList[BoneId]).xyz * Weight;
#endif
	}

#endif
}

//----------------------------------------//


//-------------------- 主函数 --------------------//

//顶点着色
PSINPUT VertexShading(VSINPUT Input){
	PSINPUT Output;
	float3x3 matWorldIT;
	bool bTexXform;

#ifdef INPUT_BONERIG
	BoneAnimation(Input);
#endif

	bTexXform = (gTexOrNormMat._m33 == 1.f);
	matWorldIT = bTexXform ? (float3x3)gWorldMat : (float3x3)gTexOrNormMat;

	Output.ObjId = asuint(gTexOrNormMat._m23);
	Output.CoordW = mul(float4(Input.CoordL, 1.f), gWorldMat).xyz;
	Output.CoordP = mul(float4(Output.CoordW, 1.f), gViewProjMat);
	Output.NormalW = mul(Input.NormalL, matWorldIT);
#ifdef INPUT_TANGENT
	Output.TangentW = mul(Input.TangentL, matWorldIT);
#endif
#ifdef INPUT_TEXCOORD
	Output.TexCoord = bTexXform ? mul(float4(Input.TexCoord, 0.f, 1.f), gTexOrNormMat).xy : Input.TexCoord;
#endif
#ifdef INPUT_COLOR
	Output.Color = Input.Color;
#endif

	return Output;
}

//像素着色
PSOUTPUT PixelShading(PSINPUT Input){
#ifdef INPUT_TEXCOORD
	MATBASE MtlBase = ParseMaterial(Input.TexCoord);
#else
	MATBASE MtlBase = gMtlBase;
#endif

#ifdef INPUT_COLOR
	MtlBase.Opacity *= Input.Color.a;
	MtlBase.Albedo *= Input.Color.rgb;
#endif

#ifdef INPUT_TANGENT
	float4 NormalT = TextureSampling2(gNormMap, Input.TexCoord, MtlBase.TexAttrs.g);
	float3 NormalW = ParseNormal(NormalT.xyz, Input.NormalW, Input.TangentW);
#else
	float3 NormalW = normalize(Input.NormalW);
#endif

	MATMIN MtlMin = { MtlBase.Albedo, MtlBase.Roughness, MtlBase.Metalness };
	float3 Color = CalcLighting(MtlMin, Input.CoordW, NormalW) + MtlBase.Emission;
	if(gFogDepth > 0.f) Color = CalcFogEffect(Input.CoordW, Color);

	PSOUTPUT Output;
	Output.Color = float4(Color, MtlBase.Opacity);
	Output.ObjId = Input.ObjId;

	return Output;
}

//----------------------------------------//