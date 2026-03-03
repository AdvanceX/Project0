//****************************************//

#include "ge-param.hlsl"

//****************************************//


//-------------------- 常量值 --------------------//

#define BSLASH_FLAG  0x80000000
#define SMOOTH_FLAG  0x40000000
#define DISCARD_FLAG 0x20000000

//----------------------------------------//


//-------------------- 数据结构 --------------------//

// Height Field Sample
struct GSHFSample{
	float CoordY;
	RGB10 Normal;
};

// Pixel Shader Input
struct PSInput{
	float4 CoordP   : SV_POSITION;
	float4 CoordW   : IP_CoordW;
	float3 NormalW  : IP_NormalW;
	float3 TangentW : IP_TangentW;
	float2 TexCoord : IP_TexCoord;
};

//----------------------------------------//


//-------------------- 常量数据 --------------------//

//对象属性
cbuffer cbObject: register(b2){
	ARGB8  gResAttr;
	ARGB8  gCurColor;
	uint2  gUsedSlides;
	uint2  gCellCount;
	float2 gAreaSize;
	float2 gCellSize;
	float2 gTileSize;
	float3 gLowerLeft;
	float  gLayerSize;
};

//----------------------------------------//


//-------------------- 纹理数据 --------------------//

Texture2DArray gMapMasks  : register(t0, space0);
Texture2DArray gColorMaps : register(t1, space0);
Texture2DArray gRoughMaps : register(t2, space0);
Texture2DArray gNormMaps  : register(t3, space0);
Texture2DArray gBumpMaps  : register(t4, space0);
Texture2DArray gSlideMaps : register(t5, space0);

StructuredBuffer<DWORD>      gPrimAttrs  : register(t1, space1);
StructuredBuffer<GSHFSample> gFieldSmpls : register(t2, space1);
StructuredBuffer<float4x4>   gSlideProjs : register(t3, space1);

//----------------------------------------//


//-------------------- 辅助函数 --------------------//

//解析纹理细节
void ParseTextureDetail(float2 TexCoord, out float4 Albedo, out float3 Normal, out float2 MetalRg){
// #define REQ_COLOR_MAP
// #define REQ_ROUGH_MAP
// #define REQ_NORMAL_MAP

	static const uint MaskCnt = GetChannelR(gResAttr);
	static const float3 tTexScale = float3(gAreaSize / gTileSize, 1.f);
	float3 TexCoord1 = float3(TexCoord.x, 1.f - TexCoord.y, 0.f) * tTexScale;

#ifdef REQ_COLOR_MAP
	float TexWeight = 0.f;
	float WeightSum = 0.f;
#endif

#ifdef REQ_NORMAL_MAP
	Normal = (float3)0.f;
#else
	Normal = float3(0.5f, 0.5f, 1.f);
#endif
#ifdef REQ_COLOR_MAP
	Albedo = (float4)0.f;
#else
	Albedo = gMapMasks.Sample(gCustSmplr, float3(TexCoord, 0.f));
#endif
#ifdef REQ_ROUGH_MAP
	MetalRg = (float2)0.f;
#else
	MetalRg = gMapMasks.Sample(gCustSmplr, float3(TexCoord, 1.f)).rg;
#endif

#ifdef REQ_COLOR_MAP
	[unroll(2)]
	for(uint iSet = 0; iSet < MaskCnt; ++iSet){
		uint MinTexId = iSet * 4;
		float4 TexWeights = gMapMasks.Sample(gCustSmplr, float3(TexCoord, iSet + 2));

		[unroll(4)]
		for(uint iTex = 0; iTex < 4; ++iTex){
			TexWeight = TexWeights[iTex];
			if(TexWeight == 0.f) continue;

			WeightSum += TexWeight;
			TexCoord1.z = MinTexId + iTex;

#ifdef REQ_ROUGH_MAP
			MetalRg += gRoughMaps.Sample(gCustSmplr, TexCoord1).rg * TexWeight;
#endif
#ifdef REQ_NORMAL_MAP
			Normal += gNormMaps.Sample(gCustSmplr, TexCoord1).xyz * TexWeight;
#endif
			Albedo += gColorMaps.Sample(gCustSmplr, TexCoord1) * TexWeight;
		}
	}

	if(WeightSum < 1.f){
		TexWeight = 1.f - WeightSum;
#ifdef REQ_NORMAL_MAP
		Normal += float3(0.5f, 0.5f, 1.f) * TexWeight;
#endif
#ifdef REQ_ROUGH_MAP
		MetalRg += gMapMasks.Sample(gCustSmplr, float3(TexCoord, 1.f)).rg * TexWeight;
#endif
		Albedo += gMapMasks.Sample(gCustSmplr, float3(TexCoord, 0.f)) * TexWeight;
	}
#endif
}

//投影映射
float3 ProjectionMapping(float4 CoordW){
	float4 CoordP;
	float3 Sample = (float3)1.f;
	uint Count = GetChannelG(gResAttr);

	for(uint iSlide = 0, iRow = 0; iRow < 2; ++iRow){
		for (uint iCol = 0; iCol < 4; ++iCol, ++iSlide){
			if(iSlide >= Count) return Sample;

			CoordP = mul(CoordW, gSlideProjs[iSlide]);
			CoordP.xyz /= CoordP.w;

			if((CoordP.x >= 0.f) && (CoordP.x <= 1.f) &&
				(CoordP.y >= 0.f) && (CoordP.y <= 1.f))
			{
				CoordP.z = GetChannel(gUsedSlides[iRow], iCol);
				Sample *= gSlideMaps.SampleLevel(gCustSmplr, CoordP.xyz, 0.f).rgb;
			}
		}
	}

	return Sample;
}

//----------------------------------------//


//-------------------- 主函数 --------------------//

//顶点着色
uint VertexShading(uint VertId: SV_VertexID): IP_CellId{
	return VertId;
}

//几何着色
[maxvertexcount(6)]
void GeometryShading(point uint CellId[1]: IP_CellId, inout TriangleStream<PSInput> Output){
	static const uint tVtxSeq[2][2][3] = {
		{{ 0, 1, 3 }, { 0, 3, 2 }},
		{{ 1, 2, 0 }, { 1, 3, 2 }}
	};

	//////

	DWORD rgTriAttr[2] = { gPrimAttrs[CellId[0] * 2], gPrimAttrs[(CellId[0] * 2) + 1] };

	bool rgSmooth[2] = { rgTriAttr[0] & SMOOTH_FLAG, rgTriAttr[1] & SMOOTH_FLAG };
	bool rgDiscard[2] = { rgTriAttr[0] & DISCARD_FLAG, rgTriAttr[1] & DISCARD_FLAG };

	if(rgDiscard[0] && rgDiscard[1]) return;

	uint RowId = CellId[0] / gCellCount.x;
	uint ColId = CellId[0] % gCellCount.x;
	uint TessId = (rgTriAttr[0] & BSLASH_FLAG) ? 1 : 0;

	uint iP0 = (gCellCount.x + 1)*RowId + ColId;
	uint iP1 = iP0 + (gCellCount.x + 1);
	uint iP2 = iP0 + 1;
	uint iP3 = iP1 + 1;

	float3 Tangent = float3(1.f, 0.f, 0.f);
	float3 LowerLeft = float3(gCellSize.x * ColId, 0.f, gCellSize.y * RowId);

	float4 Offset0 = float4(gLowerLeft, 0.f);
	float4 Offset1 = Offset0;

	Offset0.y += UintToInt2(rgTriAttr[0]).x * gLayerSize;
	Offset1.y += UintToInt2(rgTriAttr[1]).x * gLayerSize;

	//////

	float4 rgCoordL[4] = {
		float4(LowerLeft.x, gFieldSmpls[iP0].CoordY, LowerLeft.z, 1.f),
		float4(LowerLeft.x, gFieldSmpls[iP1].CoordY, LowerLeft.z + gCellSize.y, 1.f),
		float4(LowerLeft.x + gCellSize.x, gFieldSmpls[iP2].CoordY, LowerLeft.z, 1.f),
		float4(LowerLeft.x + gCellSize.x, gFieldSmpls[iP3].CoordY, LowerLeft.z + gCellSize.y, 1.f)
	};

	float4 rgCoordW[2][3] = {
		rgCoordL[tVtxSeq[TessId][0][0]] + Offset0,
		rgCoordL[tVtxSeq[TessId][0][1]] + Offset0,
		rgCoordL[tVtxSeq[TessId][0][2]] + Offset0,
		rgCoordL[tVtxSeq[TessId][1][0]] + Offset1,
		rgCoordL[tVtxSeq[TessId][1][1]] + Offset1,
		rgCoordL[tVtxSeq[TessId][1][2]] + Offset1
	};

	float4 rgCoordP[2][3] = {
		mul(rgCoordW[0][0], gViewProjMat),
		mul(rgCoordW[0][1], gViewProjMat),
		mul(rgCoordW[0][2], gViewProjMat),
		mul(rgCoordW[1][0], gViewProjMat),
		mul(rgCoordW[1][1], gViewProjMat),
		mul(rgCoordW[1][2], gViewProjMat)
	};

	float2 rgTexCoord[4] = {
		rgCoordL[0].xz / gAreaSize,
		rgCoordL[1].xz / gAreaSize,
		rgCoordL[2].xz / gAreaSize,
		rgCoordL[3].xz / gAreaSize
	};

	float3 rgNormalW[4];
	if(rgSmooth[0] || rgSmooth[1]){
		rgNormalW[0] = UnzipNormal(gFieldSmpls[iP0].Normal);
		rgNormalW[1] = UnzipNormal(gFieldSmpls[iP1].Normal);
		rgNormalW[2] = UnzipNormal(gFieldSmpls[iP2].Normal);
		rgNormalW[3] = UnzipNormal(gFieldSmpls[iP3].Normal);
	}

	//////

	PSInput Vertex;
	Vertex.TangentW = Tangent;

	for(uint iTri = 0; iTri < 2; ++iTri){
		if(rgDiscard[iTri]) continue;

		if(rgSmooth[iTri]){
			for(uint iPt = 0; iPt < 3; ++iPt){
				uint iVtx = tVtxSeq[TessId][iTri][iPt];
				Vertex.CoordW = rgCoordW[iTri][iPt];
				Vertex.CoordP = rgCoordP[iTri][iPt];
				Vertex.NormalW = rgNormalW[iVtx];
				Vertex.TexCoord = rgTexCoord[iVtx];
				Output.Append(Vertex);
			}
		} else{
			float3 Normal = CalcTriangleNormal(
				rgCoordW[iTri][0].xyz,
				rgCoordW[iTri][1].xyz,
				rgCoordW[iTri][2].xyz);

			for(uint iPt = 0; iPt < 3; ++iPt){
				uint iVtx = tVtxSeq[TessId][iTri][iPt];
				Vertex.CoordW = rgCoordW[iTri][iPt];
				Vertex.CoordP = rgCoordP[iTri][iPt];
				Vertex.NormalW = Normal;
				Vertex.TexCoord = rgTexCoord[iVtx];
				Output.Append(Vertex);
			}
		}

		Output.RestartStrip();
	}
}

//像素着色
float4 PixelShading(PSInput Input): SV_Target{
	if(gCurColor != 0x0){
		return UnzipColor(gCurColor);
	} else{
		float4 Albedo;
		float3 NormalT;
		float2 MetalRg;
		ParseTextureDetail(Input.TexCoord, Albedo, NormalT, MetalRg);

		GMinMtl MtlBase = { Albedo.rgb, MetalRg.r, MetalRg.g };
		float3 NormalW = ParseNormal(NormalT, Input.NormalW, Input.TangentW);
		float3 Color = CalcLighting(MtlBase, Input.CoordW.xyz, NormalW);

		if(GetChannelG(gResAttr) > 0)
			Color *= ProjectionMapping(Input.CoordW);
		if(gFogDepth > 0.f)
			Color = CalcFogEffect(Input.CoordW.xyz, Color);

		return float4(Color, Albedo.a);
	}
}

//----------------------------------------//