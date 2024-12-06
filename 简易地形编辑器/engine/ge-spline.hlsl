//****************************************//

#include "ge-param.hlsl"

//****************************************//


//-------------------- 数据结构 --------------------//

// Control Point
struct CPOINT{
	float3 Coord : IP_Position;
};

// Domain Shader Output
struct DSOUTPUT{
	float4 CoordL   : IP_Position;
	float3 RightL   : IP_RightDir;
	float3 NormalL  : IP_Normal;
	float2 TexCoord : IP_TexCoord;
};

// Geometry Shader Output
struct GSOUTPUT{
	float4 CoordP   : SV_POSITION;
	float4 CoordW   : IP_Position;
	float3 NormalW  : IP_Normal;
	float2 TexCoord : IP_TexCoord;
};

// Patch Tessellation
struct PATCHTESS{
	float InsideTess[2] : SV_InsideTessFactor;
	float EdgeTess[4]   : SV_TessFactor;
	float TotalLen      : IP_TotalLen;
};

//----------------------------------------//


//-------------------- 常量数据 --------------------//

//对象属性
cbuffer cbObject : register(b2){
	float    gPitch;
	float    gWidth;
	float    gWidthB;
	float    gHeight;
	float    gRoughness;
	float    gMetalness;
	float    gTexSplit;
	float    gBumpScale;
	uint     gIsShell;
	float3   gUpDir;
	float4x4 gWorldMat;
};

//子对象属性
cbuffer cbSubobj : register(b3){
	MATBASE gMtlBase;
};

//----------------------------------------//


//-------------------- 纹理数据 --------------------//

Texture2D gColorMap : register(t0, space0);

//----------------------------------------//


//-------------------- 辅助函数 --------------------//

//贝塞尔弧长
float BezierArcLength(const InputPatch<CPOINT, 4> Patch){
	float Len01 = length(Patch[1].Coord - Patch[0].Coord);
	float Len12 = length(Patch[2].Coord - Patch[1].Coord);
	float Len23 = length(Patch[3].Coord - Patch[2].Coord);
	float Len03 = length(Patch[3].Coord - Patch[0].Coord);

	return (Len01 + Len12 + Len23 + Len03) * 0.5f;
}

//贝塞尔采样
float3 BezierSampling(const OutputPatch<CPOINT, 4> Patch, float Factor){
	// P(t) = P0×(1−t)³ + P1×3t(1−t)² + P2×3t²(1−t) + P3×t³

	float T1 = Factor;
	float T2 = T1 * T1;
	float T3 = T2 * T1;
	float S1 = 1.f - T1;
	float S2 = S1 * S1;
	float S3 = S2 * S1;

	float3 P0 = Patch[0].Coord * S3;
	float3 P1 = Patch[1].Coord * S2 * T1 * 3.f;
	float3 P2 = Patch[2].Coord * S1 * T2 * 3.f;
	float3 P3 = Patch[3].Coord * T3;

	return P0 + P1 + P2 + P3;
}

//贝塞尔求导
float3 BezierDerivation(const OutputPatch<CPOINT, 4> Patch, float Factor){
	// P'(t) = P0×(−3)(1−t)² + P1×(3(1−t)² − 6t(1−t)) + P2×(6t(1−t) − 3t²) + P3×3t²

	float T1 = Factor;
	float T2 = T1 * T1;
	float S1 = 1.f - T1;
	float S2 = S1 * S1;
	
	float B0 = S2 * -3.f;
	float B1 = S2*3.f - T1*S1*6.f;
	float B2 = T1*S1*6.f - T2*3.f;
	float B3 = T2 * 3.f;

	float3 V0 = Patch[0].Coord * B0;
	float3 V1 = Patch[1].Coord * B1;
	float3 V2 = Patch[2].Coord * B2;
	float3 V3 = Patch[3].Coord * B3;

	return V0 + V1 + V2 + V3;
}

//获取侧边
uint2 GetSideEdge(float2 Vert0, float2 Vert1, float2 Vert2){
	uint2 Edge;
	
	if(Vert0.x == Vert1.x){
		Edge[0] = 0;
		Edge[1] = 1;
	} else if(Vert1.x == Vert2.x){
		Edge[0] = 1;
		Edge[1] = 2;
		Vert0 = Vert1;
		Vert1 = Vert2;
	} else{
		Edge[0] = 0;
		Edge[1] = 2;
		Vert1 = Vert2;
	}

	if(Vert0.x == 0.f){
		if(Vert0.y < Vert1.y){
			uint Temp = Edge[0];
			Edge[0] = Edge[1];
			Edge[1] = Temp;
		}
	} else{
		if(Vert0.y > Vert1.y){
			uint Temp = Edge[0];
			Edge[0] = Edge[1];
			Edge[1] = Temp;
		}
	}

	return Edge;
}

//获取侧面法线
float3 GetSideNormal(float3 Vertical, float3 Horizontal){
	float3 Vector0 = Vertical + Horizontal;
	float3 Vector1 = Vertical - Horizontal;

	Vector1 = normalize(Vector1);
	Vector1 *= dot(Vector0, Vector1);
	Vector0 -= Vector1;

	return normalize(Vector0);
}

//----------------------------------------//


//-------------------- 主函数 --------------------//

//顶点着色
CPOINT VertexShading(CPOINT Input){
	return Input;
}

//常量外壳着色
PATCHTESS ConstHullShading(InputPatch<CPOINT, 4> Patch, uint PatchId: SV_PrimitiveID){
	PATCHTESS PatchTess;

	float Length = BezierArcLength(Patch);
	float Factor = min(64.f, Length / gPitch);

	PatchTess.TotalLen = Length;
	PatchTess.EdgeTess[0] = Factor;
	PatchTess.EdgeTess[1] = 1.f;
	PatchTess.EdgeTess[2] = Factor;
	PatchTess.EdgeTess[3] = 1.f;
	PatchTess.InsideTess[0] = 1.f;
	PatchTess.InsideTess[1] = Factor;

	return PatchTess;
}

//控制点外壳着色
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[patchconstantfunc("ConstHullShading")]
[outputcontrolpoints(4)]
[maxtessfactor(64.f)]
CPOINT HullShading(InputPatch<CPOINT, 4> Patch, uint PatchId: SV_PrimitiveID, uint PointId: SV_OutputControlPointID){
	return Patch[PointId];
}

//域着色
[domain("quad")]
DSOUTPUT DomainShading(const OutputPatch<CPOINT, 4> Patch, PATCHTESS PatchTess, float2 Location: SV_DomainLocation){
	float Extent = (0.5f - Location.x) * gWidth;
	float TexCoorV = PatchTess.TotalLen * Location.y / gPitch;
	float TexCoorU = lerp(0.f, gTexSplit, Location.x);
	float3 Tangent = BezierDerivation(Patch, Location.y);
	float3 Vertex = BezierSampling(Patch, Location.y);

	DSOUTPUT Output;
	Output.NormalL = gUpDir;
	Output.RightL = normalize(cross(gUpDir, Tangent));
	Output.CoordL = float4(Vertex + Extent*Output.RightL, 1.f);
	Output.TexCoord = float2(TexCoorU, TexCoorV);

	return Output;
}

//几何着色
[maxvertexcount(16)]
void GeometryShading(triangle DSOUTPUT Input[3], uint PrimId: SV_PrimitiveID, inout TriangleStream<GSOUTPUT> Output){
	static const GSOUTPUT tZero = { (float4)0.f, (float4)0.f, (float3)0.f, (float2)0.f };

	bool bXform = (gWorldMat._m33 != 0.f);
	uint2 SideEdge = GetSideEdge(Input[0].TexCoord, Input[1].TexCoord, Input[2].TexCoord);
	float3x3 matWorld3 = (float3x3)gWorldMat;
	GSOUTPUT rgVert[3] = { tZero, tZero, tZero };

	//顶面

	for(uint iVtx = 0; iVtx < 3; ++iVtx){
		rgVert[iVtx].CoordW = bXform ? mul(Input[iVtx].CoordL, gWorldMat) : Input[iVtx].CoordL;
		rgVert[iVtx].CoordP = mul(rgVert[iVtx].CoordW, gViewProjMat);
		rgVert[iVtx].NormalW = bXform ? mul(Input[iVtx].NormalL, matWorld3) : Input[iVtx].NormalL;
		rgVert[iVtx].TexCoord = Input[iVtx].TexCoord;
		Output.Append(rgVert[iVtx]);
	}
	Output.RestartStrip();

	//侧面

	if(gHeight != 0){
		for(uint iCol = 0; iCol < 2; ++iCol){
			GSOUTPUT Vertex = rgVert[SideEdge[iCol]];
			DSOUTPUT RefVert = Input[SideEdge[iCol]];

			float Factor = (RefVert.TexCoord.x == 0.f) ? 1.f : -1.f;
			float TexCoorU = (2 - iCol) * gTexSplit;
			float3 OffsetV = gUpDir * gHeight;
			float3 OffsetH = RefVert.RightL * Factor * gWidthB;
			float3 NormalL = GetSideNormal(OffsetV, OffsetH);
			float3 CoordL = RefVert.CoordL.xyz - OffsetV + OffsetH;

			Vertex.NormalW = bXform ? mul(NormalL, matWorld3) : NormalL;
			Vertex.TexCoord = float2(TexCoorU, 0.f);
			Output.Append(Vertex);

			Vertex.CoordW = bXform ? mul(float4(CoordL, 1.f), gWorldMat) : float4(CoordL, 1.f);
			Vertex.CoordP = mul(Vertex.CoordW, gViewProjMat);
			Vertex.TexCoord = float2(TexCoorU, 1.f);
			Output.Append(Vertex);
		}
	}
}

//像素着色
float4 PixelShading(GSOUTPUT Input): SV_Target{
	float4 Color = gColorMap.Sample(gCustSmplr, Input.TexCoord);
	MATMIN MtlBase = { Color.rgb, gRoughness, gMetalness };

	Color.rgb = CalcLighting(MtlBase, Input.CoordW.xyz, Input.NormalW);
	Color.rgb = CalcFogEffect(Input.CoordW.xyz, Color.rgb);
	
	return Color;
}

//----------------------------------------//