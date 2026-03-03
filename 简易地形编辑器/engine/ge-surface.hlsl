//****************************************//

// D  : 方向
// A  : 振幅
// L  : 波长
// S  : 速度
// ω : 角频率,ω = 2π/L
// φ : 相常数,φ = S×2π/L
// t  : 时间

//****************************************//


//****************************************//

#include "ge-param.hlsl"

//****************************************//


//-------------------- 数据结构 --------------------//

// Lite Material
struct PSMaterial{
	float4 Albedo;
	ARGB8  Ambient;
	ARGB8  Emission;
	float  Roughness;
	float  Metalness;
};

// Pixel Shader Input
struct PSInput{
	float4 CoordP  : SV_POSITION;
	float3 CoordW  : IP_CoordW;
	float3 NormalW : IP_NormalW;
};

//----------------------------------------//


//-------------------- 常量数据 --------------------//

//对象属性
cbuffer cbObject: register(b2){
	float4 gWavDirX;
	float4 gWavDirZ;
	float4 gWavPhases;
	float4 gWavSlopes;
	float4 gWavAmplits;
	float4 gWavLengths;
	float4 gLowerLeft;
	float2 gTexScale;
	float2 gCellSize;
	ARGB8  gWireColor;
	ARGB8  gBkgdColor;
};

//子对象属性
cbuffer cbSubobj: register(b3){
	PSMaterial gMtlBase;
};

//----------------------------------------//


//-------------------- 辅助函数 --------------------//

// Fsin = sin(D·(x,z)×ω + φt)
// Fcos = cin(D·(x,z)×ω + φt)
float4 CalcWaveDisplacements(float3 Coord){
	static float4 tFrequency = 2.f * MATH_PI / gWavLengths;
	float4 Dists = (gWavDirX * Coord.xxxx) + (gWavDirZ * Coord.zzzz);
	return (Dists * tFrequency) + gWavPhases;
}

// x = x + ∑(Q×A×D.x×Fcos)
// z = z + ∑(Q×A×D.z×Fcos)
// y = ∑(A×Fsin)
float3 CalcWavePositions(float3 Coord, float4 Fsin, float4 Fcos){
	float X = Coord.x + dot(gWavSlopes * gWavAmplits * gWavDirX, Fcos);
	float Z = Coord.z + dot(gWavSlopes * gWavAmplits * gWavDirZ, Fcos);
	float Y = dot(gWavAmplits, Fsin);
	return float3(X, Y, Z);
}

// x = −∑(D.x×ω×A×Fcos)
// z = −∑(D.z×ω×A×Fcos)
// y = 1 − ∑(Q×ω×A×Fsin)
float3 CalcWaveNormals(float4 Fsin, float4 Fcos){
	static float4 tFrequency = 2.f * MATH_PI / gWavLengths;
	static float4 tFfa = tFrequency * gWavAmplits;

	float X = -dot(gWavDirX * tFfa, Fcos);
	float Z = -dot(gWavDirZ * tFfa, Fcos);
	float Y = 1.f - dot(gWavSlopes * tFfa, Fsin);

	return float3(X, Y, Z);
}

//----------------------------------------//


//-------------------- 主函数 --------------------//

//顶点着色
uint2 VertexShading(uint2 RowCol: IP_RowCol): IP_RowCol{
	return RowCol;
}

//几何着色
[maxvertexcount(6)]
void GeometryShading(point uint2 RowCol[1]: IP_RowCol, inout TriangleStream<PSInput> Output){
	static const uint tFaces[2][2][3] = {
		{{ 0, 1, 3 }, { 0, 3, 2 }},
		{{ 1, 2, 0 }, { 1, 3, 2 }}
	};

	uint ColId = RowCol[0].x;
	uint RowId = RowCol[0].y;
	uint TessId = (RowId + ColId) % 2;

	float LowerLeftX = gCellSize.x * ColId;
	float LowerLeftZ = gCellSize.y * RowId;

	//////

	float3 rgCoord[4] = {
		float3(LowerLeftX, 0.f, LowerLeftZ),
		float3(LowerLeftX, 0.f, LowerLeftZ + gCellSize.y),
		float3(LowerLeftX + gCellSize.x, 0.f, LowerLeftZ),
		float3(LowerLeftX + gCellSize.x, 0.f, LowerLeftZ + gCellSize.y)
	};
	float4 rgDisplace[4] = {
		CalcWaveDisplacements(rgCoord[0]),
		CalcWaveDisplacements(rgCoord[1]),
		CalcWaveDisplacements(rgCoord[2]),
		CalcWaveDisplacements(rgCoord[3]),
	};
	float4 rgSin[4] = {
		sin(rgDisplace[0]),
		sin(rgDisplace[1]),
		sin(rgDisplace[2]),
		sin(rgDisplace[3])
	};
	float4 rgCos[4] = {
		cos(rgDisplace[0]),
		cos(rgDisplace[1]),
		cos(rgDisplace[2]),
		cos(rgDisplace[3])
	};
	float3 rgNormalW[4] = {
		CalcWaveNormals(rgSin[0], rgCos[0]),
		CalcWaveNormals(rgSin[1], rgCos[1]),
		CalcWaveNormals(rgSin[2], rgCos[2]),
		CalcWaveNormals(rgSin[3], rgCos[3])
	};
	float3 rgCoordW[4] = {
		CalcWavePositions(rgCoord[0], rgSin[0], rgCos[0]) + gLowerLeft.xyz,
		CalcWavePositions(rgCoord[1], rgSin[1], rgCos[1]) + gLowerLeft.xyz,
		CalcWavePositions(rgCoord[2], rgSin[2], rgCos[2]) + gLowerLeft.xyz,
		CalcWavePositions(rgCoord[3], rgSin[3], rgCos[3]) + gLowerLeft.xyz,
	};
	float4 rgCoordP[4] = {
		mul(float4(rgCoordW[0], 1.f), gViewProjMat),
		mul(float4(rgCoordW[1], 1.f), gViewProjMat),
		mul(float4(rgCoordW[2], 1.f), gViewProjMat),
		mul(float4(rgCoordW[3], 1.f), gViewProjMat)
	};

	//////

	PSInput Vertex;

	for(uint iTri = 0; iTri < 2; ++iTri){
		for(uint iPt = 0; iPt < 3; ++iPt){
			uint iVtx = tFaces[TessId][iTri][iPt];
			Vertex.CoordP = rgCoordP[iVtx];
			Vertex.CoordW = rgCoordW[iVtx].xyz;
			Vertex.NormalW = rgNormalW[iVtx];
			Output.Append(Vertex);
		}
		Output.RestartStrip();
	}
}

//像素着色
float4 PixelShading(PSInput Input): SV_Target{
	if(gWireColor != 0x0){
		return UnzipColor(gWireColor);
	} else{
		GMinMtl MtlBase = { gMtlBase.Albedo.rgb, gMtlBase.Roughness, gMtlBase.Metalness };
		float3 Color = CalcLighting(MtlBase, Input.CoordW, normalize(Input.NormalW));
		if(gFogDepth > 0.f)	Color = CalcFogEffect(Input.CoordW, Color);
		return float4(Color, gMtlBase.Albedo.a);
	}
}

//----------------------------------------//