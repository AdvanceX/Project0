//****************************************//

#include "ge-param.hlsl"

//****************************************//


//-------------------- 常量值 --------------------//

#define DIR_Left  (1L << 2)
#define DIR_Right (1L << 3)
#define DIR_Front (1L << 4)
#define DIR_Back  (1L << 5)

//----------------------------------------//


//-------------------- 数据结构 --------------------//

// Instance Data
struct VSInstData{
	uint2 RowCol;
	DWORD DirTag;
};

// Vertex Shader Input
struct VSInput{
	///顶点数据
	float3 CoordL   : IP_Position;
	float3 NormalL  : IP_Normal;
	float2 TexCoord : IP_TexCoord;
};

// Pixel Shader Input
struct PSInput{
	float4 CoordP   : SV_POSITION;
	float3 CoordW   : IP_Position;
	float3 NormalW  : IP_Normal;
	float2 TexCoord : IP_TexCoord;
};

//----------------------------------------//


//-------------------- 常量数据 --------------------//

//对象组属性
cbuffer cbGroup: register(b2){
	float4 gLowerLeft;
	float2 gCellSize;
	GMtlBase gMtlBase;
};

//----------------------------------------//


//-------------------- 纹理数据 --------------------//

Texture2D gColorMap  : register(t0, space0);
Texture2D gNormMap   : register(t1, space0);
Texture2D gRoughMap  : register(t2, space0);
Texture2D gAlphaMap  : register(t3, space0);
Texture2D gEmissMap  : register(t4, space0);
Texture2D gOcclusMap : register(t5, space0);

StructuredBuffer<VSInstData> gInstData: register(t1, space1);

//----------------------------------------//


//-------------------- 辅助函数 --------------------//

//解析材质
GMtlBase ParseMaterial(float2 TexCoord){
	GMtlBase MtlBase = gMtlBase;
	DWORD TexCtlg = gMtlBase.TexAttrs.r;
	DWORD AddrMode = gMtlBase.TexAttrs.g;

	if(TexCtlg & TEX_TYPE_Albedo){
		float4 Sample = TextureSampling2(gColorMap, TexCoord, AddrMode);
		MtlBase.Albedo = Sample.rgb;
		MtlBase.Opacity = Sample.a;
	}
	if(TexCtlg & TEX_TYPE_MetalRg){
		float4 Sample = TextureSampling2(gRoughMap, TexCoord, AddrMode);
		MtlBase.Roughness = Sample.r;
		MtlBase.Metalness = Sample.g;
	}
	if(TexCtlg & TEX_TYPE_Emission){
		float4 Sample = TextureSampling2(gEmissMap, TexCoord, AddrMode);
		MtlBase.Emission = saturate(gMtlBase.Emission + Sample.rgb);
	}

	return MtlBase;
}

//----------------------------------------//


//-------------------- 主函数 --------------------//

//顶点着色
PSInput VertexShading(VSInput Input, uint InstId: SV_InstanceID){
	static const float3x3 tRotBack = {
		-1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, -1.f
	};
	static const float3x3 tRotLeft = {
		-1.19209e-07f, 0.f, 1.f,
		0.f, 1.f, 0.f
		-1.f, 0.f, -1.19209e-07f
	};
	static const float3x3 tRotRight = {
		-1.19209e-07f, 0.f, -1.f,
		0.f, 1.f, 0.f,
		1.f, 0.f, -1.19209e-07f
	};

	//////

	PSInput Output;
	VSInstData InstData = gInstData[InstId];
	
	switch(InstData.DirTag){
		case DIR_Front:{
			Output.CoordW = Input.CoordL;
			Output.NormalW = Input.NormalL;
#ifdef INPUT_TANGENT
			Output.TangentW = Input.TangentL;
#endif
		} break;
		case DIR_Back:{
			Output.CoordW = mul(Input.CoordL, tRotBack);
			Output.NormalW = mul(Input.NormalL, tRotBack);
#ifdef INPUT_TANGENT
			Output.TangentW = mul(Input.TangentL, tRotBack);
#endif
		} break;
		case DIR_Left:{
			Output.CoordW = mul(Input.CoordL, tRotLeft);
			Output.NormalW = mul(Input.NormalL, tRotLeft);
#ifdef INPUT_TANGENT
			Output.TangentW = mul(Input.TangentL, tRotLeft);
#endif
		} break;
		case DIR_Right:{
			Output.CoordW = mul(Input.CoordL, tRotRight);
			Output.NormalW = mul(Input.NormalL, tRotRight);
#ifdef INPUT_TANGENT
			Output.TangentW = mul(Input.TangentL, tRotRight);
#endif
		} break;
	}

	Output.CoordW += gLowerLeft.xyz;
	Output.CoordW.xz += InstData.RowCol * gCellSize;
	Output.CoordP = mul(float4(Output.CoordW, 1.f), gViewProjMat);
	Output.TexCoord = Input.TexCoord;

	return Output;
}

//像素着色
float4 PixelShading(PSInput Input): SV_Target{
	GMtlBase MtlBase = ParseMaterial(Input.TexCoord);
	GMinMtl MtlMin = { MtlBase.Albedo, MtlBase.Roughness, MtlBase.Metalness };

#ifdef INPUT_TANGENT
	float4 NormalT = TextureSampling2(gNormMap, Input.TexCoord, MtlBase.TexAttrs.g);
	float3 NormalW = ParseNormal(NormalT.xyz, Input.NormalW, Input.TangentW);
#else
	float3 NormalW = normalize(Input.NormalW);
#endif

	float3 Color = CalcLighting(MtlMin, Input.CoordW, NormalW) + MtlBase.Emission;
	if(gFogDepth > 0.f) Color = CalcFogEffect(Input.CoordW, Color);

	return float4(Color, MtlBase.Opacity);
}

//----------------------------------------//