//****************************************//

#include "ge-param.hlsl"

//****************************************//


//-------------------- 数据结构 --------------------//

// Vertex Shader Input
struct VSINPUT{
	float3 CoordL  : IP_Position;
	float3 NormalL : IP_Normal;
};

// Pixel Shader Input
struct PSINPUT{
	float4 CoordP   : SV_POSITION;
	float4 CoordW   : IP_Position;
	float3 BitanW   : IP_Bitangent;
	float3 NormalW  : IP_Normal;
	float3 TangentW : IP_Tangent;
	float2 TexCoord : IP_TexCoord;
	ARGB8  MtlRef   : IP_Material;
};

//----------------------------------------//


//-------------------- 常量数据 --------------------//

//对象属性
cbuffer cbObject : register(b2){
	uint2    gSlideIds;
	uint     gSlideCnt;
    uint     gMaskCnt;
	ARGB8    gUsedColor;
	float    gBumpScale;
	float2   gAreaSize;
	float2   gTileSizeH;
	float2   gTileSizeV;
	float4x4 gWorldMat;
};

//子对象属性
cbuffer cbSubobj : register(b3){
	MATLITE gMtlBase;
};

//----------------------------------------//


//-------------------- 纹理数据 --------------------//

Texture2DArray gMtlMasks  : register(t0, space0);
Texture2DArray gColorMaps : register(t1, space0);
Texture2DArray gRoughMaps : register(t2, space0);
Texture2DArray gNormMaps  : register(t3, space0);
Texture2DArray gBumpMaps  : register(t4, space0);
Texture2DArray gSlideList : register(t5, space0);

StructuredBuffer<RGB10>     gFaceNorms  : register(t1, space1);
StructuredBuffer<EXTRUSION> gPrimFeats  : register(t2, space1);
StructuredBuffer<float4x4>  gPrjctrList : register(t3, space1);

//----------------------------------------//


//-------------------- 辅助函数 --------------------//

//匹配对应面
uint MatchFaceId(float4 Point0, float4 Point1){
	float4 Tangent = Point0 - Point1;

	if(Tangent.x > 0.f){
		if(Tangent.z > 0.f) return 5;
		else if(Tangent.z < 0.f) return 4;
		else return 0;
	} else if(Tangent.x < 0.f){
		if(Tangent.z > 0.f) return 7;
		else if(Tangent.z < 0.f) return 6;
		else return 1;
	} else{
		if(Tangent.z > 0.f) return 3;
		else return 2;
	}

	return 0;
}

//匹配纹理坐标
float MatchTextureCoordU(float4 Vertex, uint FaceId){
	switch(FaceId){
		case 4:
		case 5:
		case 0: return Vertex.x / gTileSizeV.x;
		case 6:
		case 7:
		case 1: return (gAreaSize.x - Vertex.x) / gTileSizeV.x;
		case 2: return (gAreaSize.y - Vertex.z) / gTileSizeV.x;	
		case 3: return Vertex.z / gTileSizeV.x;
		default: return 0.f;
	}
}

//解析纹理细节
void ParseTextureDetail(float2 TexCoord, out float4 Albedo, out float3 Normal, out float2 MetalRg){
//#define REQ_COLOR_MAP
//#define REQ_ROUGH_MAP
//#define REQ_NORMAL_MAP

	static const float3 tTexScale = float3(gAreaSize / gTileSizeH, 1.f);
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
	Albedo = gMtlMasks.Sample(gCustSmplr, float3(TexCoord, 0.f));
#endif
#ifdef REQ_ROUGH_MAP
	MetalRg = (float2)0.f;
#else
	MetalRg = gMtlMasks.Sample(gCustSmplr, float3(TexCoord, 1.f)).rg;
#endif

#ifdef REQ_COLOR_MAP
	[unroll(2)]
	for(uint iSet = 0; iSet < gMaskCnt; ++iSet){
		uint MinTexId = iSet * 4;
		float4 TexWeights = gMtlMasks.Sample(gCustSmplr, float3(TexCoord, iSet + 2));

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
		MetalRg += gMtlMasks.Sample(gCustSmplr, float3(TexCoord, 1.f)).rg * TexWeight;
#endif
		Albedo += gMtlMasks.Sample(gCustSmplr, float3(TexCoord, 0.f)) * TexWeight;
	}
#endif
}

//投影映射
float3 ProjectionMapping(float4 CoordW){
	float4 CoordP;
	float3 Sample = (float3)1.f;

	for(uint iSlide = 0, iRow = 0; iRow < 2; ++iRow){
		for (uint iCol = 0; iCol < 4; ++iCol, ++iSlide){
			if(iSlide >= gSlideCnt) return Sample;

			CoordP = mul(CoordW, gPrjctrList[iSlide]);
			CoordP.xyz /= CoordP.w;

			if ((CoordP.x >= 0.f) && (CoordP.x <= 1.f) &&
				(CoordP.y >= 0.f) && (CoordP.y <= 1.f))
			{
				CoordP.z = GetChannel(gSlideIds[iRow], iCol);
				Sample *= gSlideList.SampleLevel(gCustSmplr, CoordP.xyz, 0.f).rgb;
			}
		}
	}

	return Sample;
}

//----------------------------------------//


//-------------------- 主函数 --------------------//

//顶点着色
VSINPUT VertexShading(VSINPUT Input){
	return Input;
}

//几何着色
[maxvertexcount(16)]
void GeometryShading(triangle VSINPUT Input[3], uint PrimId: SV_PrimitiveID, inout TriangleStream<PSINPUT> Output){
	static const float3 tBlockNorms[8] = {
		float3(0.f, 0.f, -1.f), //前面
		float3(0.f, 0.f, 1.f),  //后面
		float3(-1.f, 0.f, 0.f), //左面
		float3(1.f, 0.f, 0.f),  //右面
		float3(-0.707107, 0, -0.707107), //前左
		float3(0.707107, 0, -0.707107),  //前右
		float3(-0.707107, 0, 0.707107),  //后左
		float3(0.707107, 0, 0.707107)    //后右
	};
	static const float3 tBlockTans[8] = {
		float3(1.f, 0.f, 0.f),  //前面
		float3(-1.f, 0.f, 0.f), //后面
		float3(0.f, 0.f, -1.f), //左面
		float3(0.f, 0.f, 1.f),  //右面
		float3(0.707107, 0, -0.707107),  //前左
		float3(0.707107, 0, 0.707107),   //前右
		float3(-0.707107, 0, -0.707107), //后左
		float3(-0.707107, 0, 0.707107)   //后右
	};

	//////

	PSINPUT Vertex;
	EXTRUSION Feature = gPrimFeats[PrimId];

	bool bXform = (gWorldMat._m33 != 0.f);
	float3 Right = float3(1.f, 0.f, 0.f);
	float4 Offset = float4(0.f, Feature.End, 0.f, 0.f);
	float3x3 matWorld3 = (float3x3)gWorldMat;

	float4 rgRefVert[3] = {
		float4(Input[0].CoordL, 1.f),
		float4(Input[1].CoordL, 1.f),
		float4(Input[2].CoordL, 1.f)
	};

	//////

	if(!(Feature.MtlRef & 0x01000000)){
		if(gFaceNorms[PrimId] & 0xC0000000){ //平直着色
			float3 NormalL = UnzipNormal(gFaceNorms[PrimId]);
			Vertex.NormalW = bXform ? mul(NormalL, matWorld3) : NormalL;
			Vertex.TangentW = bXform ? mul(Right, matWorld3) : Right;
			Vertex.BitanW = (float3)0.f;
			Vertex.MtlRef = 0;

			for(uint iVtx = 0; iVtx < 3; ++iVtx){
				float4 CoordL = rgRefVert[iVtx] + Offset;
				Vertex.CoordW = bXform ? mul(CoordL, gWorldMat) : CoordL;
				Vertex.CoordP = mul(Vertex.CoordW, gViewProjMat);
				Vertex.TexCoord = CoordL.xz / gAreaSize;
				Output.Append(Vertex);
			}
		} else{ //平滑着色
			Vertex.TangentW = bXform ? mul(Right, matWorld3) : Right;
			Vertex.BitanW = (float3)0.f;
			Vertex.MtlRef = 0;

			for(uint iVtx = 0; iVtx < 3; ++iVtx){
				float3 NormalL = Input[iVtx].NormalL;
				float4 CoordL = rgRefVert[iVtx] + Offset;

				Vertex.CoordW = bXform ? mul(CoordL, gWorldMat) : CoordL;
				Vertex.CoordP = mul(Vertex.CoordW, gViewProjMat);
				Vertex.NormalW = bXform ? mul(NormalL, matWorld3) : NormalL;
				Vertex.TexCoord = CoordL.xz / gAreaSize;

				Output.Append(Vertex);
			}
		}
		Output.RestartStrip();
	}
	
	for(uint iExe0 = 0; iExe0 < 3; ++iExe0){
		if(Feature.End <= Feature.Starts[iExe0]) continue;

		float Stretch[2] = { Feature.End, Feature.Starts[iExe0] };
		uint FaceId = MatchFaceId(rgRefVert[iExe0], rgRefVert[(iExe0 + 1) % 3]);

		Vertex.TangentW = bXform ? mul(tBlockTans[FaceId], matWorld3) : tBlockTans[FaceId];
		Vertex.NormalW = bXform ? mul(tBlockNorms[FaceId], matWorld3) : tBlockNorms[FaceId];
		Vertex.BitanW = normalize(cross(Vertex.NormalW, Vertex.TangentW));

		for(uint iExe1 = 0; iExe1 < 2; ++iExe1){
			uint EdgeId = (iExe0 + iExe1) % 3;
			float TexCoorU = MatchTextureCoordU(rgRefVert[EdgeId], FaceId);

			for(uint iExe2 = 0; iExe2 < 2; ++iExe2){
				float4 CoordL = rgRefVert[EdgeId];
				float TexCoorV = 1.f - (Stretch[iExe2] / gTileSizeV.y);

				CoordL.y += Stretch[iExe2];
				Vertex.CoordW = bXform ? mul(CoordL, gWorldMat) : CoordL;
				Vertex.CoordP = mul(Vertex.CoordW, gViewProjMat);
				Vertex.MtlRef = Feature.MtlRef | 0x80000000;
				Vertex.TexCoord = float2(TexCoorU, TexCoorV);

				Output.Append(Vertex);
			}
		}

		Output.RestartStrip();
	}
}

//像素着色
float4 PixelShading(PSINPUT Input): SV_Target{
	if(gUsedColor != 0x0){
		if(Input.MtlRef) discard;
		return UnzipColor(gUsedColor);
	} else{
		if(Input.MtlRef){
			uint TexId = GetChannelB(Input.MtlRef);
			float3x3 matTBN = float3x3(Input.TangentW, Input.BitanW, Input.NormalW);
			float3 ViewVec = normalize(mul(matTBN, gCamCoord - Input.CoordW.xyz));
			float3 TexCoord0 = float3(Input.TexCoord, TexId);
			float3 TexCoord = ParallaxMapping3(gBumpMaps, TexCoord0, ViewVec, gBumpScale);
			float3 NormalT = gNormMaps.Sample(gCustSmplr, TexCoord).xyz;
			float3 NormalW = mul(normalize(NormalT * 2.f - 1.f), matTBN);

			float Roughness = GetChannelR(Input.MtlRef) / 255.f;
			float Metalness = GetChannelG(Input.MtlRef) / 255.f;
			float4 Albedo = gColorMaps.Sample(gCustSmplr, TexCoord);
			MATMIN MtlBase = { Albedo.rgb, Roughness, Metalness };

			float3 Color = CalcLighting(MtlBase, Input.CoordW.xyz, NormalW);
			if (gFogDepth > 0.f) Color = CalcFogEffect(Input.CoordW.xyz, Color);

			return float4(Color, Albedo.a);
		} else{
			float4 Albedo;
			float3 NormalT;
			float2 MetalRg;
			ParseTextureDetail(Input.TexCoord, Albedo, NormalT, MetalRg);

			MATMIN MtlBase = { Albedo.rgb, MetalRg.r, MetalRg.g };
			float3 NormalW = ParseNormal(NormalT, Input.NormalW, Input.TangentW);
			float3 Color = CalcLighting(MtlBase, Input.CoordW.xyz, NormalW);

			if(gSlideCnt > 0) Color *= ProjectionMapping(Input.CoordW);
			if(gFogDepth > 0.f) Color = CalcFogEffect(Input.CoordW.xyz, Color);

			return float4(Color, Albedo.a);
		}
	}
}

//----------------------------------------//