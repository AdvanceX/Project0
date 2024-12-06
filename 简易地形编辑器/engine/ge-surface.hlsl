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

// Vertex Shader Input
struct VSINPUT{
	float3 CoordL : IP_Position;
};

// Pixel Shader Input
struct PSINPUT{
	float4 CoordP  : SV_POSITION;
	float3 CoordW  : IP_CoordW;
	float3 CoordL  : IP_CoordL;
	float3 NormalW : IP_Normal;
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
	float4 gWorldXform;
	float  gTexScaleU;
	float  gTexScaleV;
	ARGB8  gWireColor;
	ARGB8  gBkgdColor;
};

//子对象属性
cbuffer cbSubobj: register(b3){
	MATLITE gMtlBase;
};

//----------------------------------------//


//-------------------- 辅助函数 --------------------//

// Ksin = sin(D·(x,z)×ω + φt)
// Kcos = cin(D·(x,z)×ω + φt)
float4 CalcWaveDists(float3 Coord, float4 DirX, float4 DirZ, float4 Frequency, float4 Phase){
	float4 Dists = (DirX * Coord.xxxx) + (DirZ * Coord.zzzz);
	return (Dists * Frequency) + Phase;
}

// x = x + ∑(Q×A×D.x×Kcos)
// z = z + ∑(Q×A×D.z×Kcos)
// y = ∑(A×Ksin)
float3 CalcWavePositions(float3 Coord, float4 DirX, float4 DirZ, float4 Ksin, float4 Kcos, float4 Amplitude, float4 Kq){
	float3 Result;
	Result.x = Coord.x + dot(Kq * Amplitude * DirX, Kcos);
	Result.z = Coord.z + dot(Kq * Amplitude * DirZ, Kcos);
	Result.y = dot(Amplitude, Ksin);
	return Result;
}

// x = −∑(D.x×ω×A×Kcos)
// z = −∑(D.z×ω×A×Kcos)
// y = 1 − ∑(Q×ω×A×Ksin)
float3 CalcWaveNormals(float4 DirX, float4 DirZ, float4 Ksin, float4 Kcos, float4 Kfa, float4 Kq){
	float3 Result;
	Result.x = -dot(DirX * Kfa, Kcos);
	Result.z = -dot(DirZ * Kfa, Kcos);
	Result.y = 1.f - dot(Kq * Kfa, Ksin);
	return Result;
}

//----------------------------------------//


//-------------------- 主函数 --------------------//

//顶点着色
PSINPUT VertexShading(VSINPUT Input){
	float4 Frequency = 2.f * MATH_PI / gWavLengths;
	float4 Kdis = CalcWaveDists(Input.CoordL, gWavDirX, gWavDirZ, Frequency, gWavPhases);
	float4 Ksin = sin(Kdis);
	float4 Kcos = cos(Kdis);
	float4 Kfa = Frequency * gWavAmplits;

	PSINPUT Output;
	Output.NormalW = CalcWaveNormals(gWavDirX, gWavDirZ, Ksin, Kcos, Kfa, gWavSlopes);
	Output.CoordL = CalcWavePositions(Input.CoordL, gWavDirX, gWavDirZ, Ksin, Kcos, gWavAmplits, gWavSlopes);
	Output.CoordW = (Output.CoordL * gWorldXform.w) + gWorldXform.xyz;
	Output.CoordP = mul(float4(Output.CoordW, 1.f), gViewProjMat);

	return Output;
}

//像素着色
float4 PixelShading(PSINPUT Input): SV_Target{
	if(gWireColor != 0x0){
		return UnzipColor(gWireColor);
	} else{
		MATMIN MtlBase = { gMtlBase.Albedo.rgb, gMtlBase.Roughness, gMtlBase.Metalness };
		float3 Color = CalcLighting(MtlBase, Input.CoordW, normalize(Input.NormalW));
		if(gFogDepth > 0.f)	Color = CalcFogEffect(Input.CoordW, Color);
		return float4(Color, gMtlBase.Albedo.a);
	}
}

//----------------------------------------//