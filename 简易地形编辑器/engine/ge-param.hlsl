#ifndef PARAM_H
#define PARAM_H
#include "ge-common.hlsl"

SamplerState gCustSmplr   : register(s0);
SamplerState gWrapSmplr   : register(s1);
SamplerState gClampSmplr  : register(s2);
SamplerState gMirrorSmplr : register(s3);
SamplerState gBorderSmplr : register(s4);

StructuredBuffer<LIGHT> gLightList : register(t0, space1);

cbuffer cbScene: register(b0){
	DWORD  ShaderFlag;
	float3 gFogColor;
	uint   gLitCount;
	float3 gAmbLight;
	float  gFogStart;
    float  gFogDepth;
	float2 gScnSize;
    float4 gKdInput[2];
    float4 gKdOutput[2];
    float4 gKsInput[2];
    float4 gKsOutput[2];
};
cbuffer cbFrame: register(b1){
    float4x4 gViewProjMat;
    float3   gCamCoord;
    float    gTimeSpan;
};

float DisperseKd(float Kd){
	float rgInput[8] = (float[8])gKdInput;
	float rgOutput[8] = (float[8])gKdOutput;

	for(uint iExe = 0; iExe < 8; ++iExe){
		if(rgInput[iExe] == 0.f) return Kd;
		if(rgInput[iExe] >= Kd) return rgOutput[iExe];
	}

	return Kd;
}
float DisperseKs(float Ks){
	float rgInput[8] = (float[8])gKsInput;
	float rgOutput[8] = (float[8])gKsOutput;

	for(uint iExe = 0; iExe < 8; ++iExe){
		if(rgInput[iExe] == 0.f) return Ks;
		if(rgInput[iExe] >= Ks) return rgOutput[iExe];
	}

	return Ks;
}

float3 GetViewVector(float3 Target){
	return normalize(gCamCoord - Target);
}
float3 CalcFogEffect(float3 Position, float3 OrgColor){
	if(gFogDepth == 0.f){
		return OrgColor;
	} else{
		float ViewDist = distance(gCamCoord, Position);
		float FogPower = saturate((ViewDist - gFogStart) / gFogDepth);
		return lerp(OrgColor, gFogColor, FogPower);
	}
}
float3 CalcLighting(MATMIN MtlBase, float3 Position, float3 Normal){
	float3 ViewVec = GetViewVector(Position);
	float3 Reflection = gAmbLight * MtlBase.Albedo;

	for(uint iExe = 0; iExe < gLitCount; ++iExe){
		if(gLightList[iExe].AttEnd >= 0.f){
			if(gLightList[iExe].SpotPower != 0.f)
				Reflection += CalcSpotLighting(gLightList[iExe], MtlBase, Position, Normal, ViewVec);
			else if(gLightList[iExe].AttStart != 0.f)
				Reflection += CalcPointLighting(gLightList[iExe], MtlBase, Position, Normal, ViewVec);
			else
				Reflection += CalcDirectionalLighting(gLightList[iExe], MtlBase, Normal, ViewVec);
		}
	}

	return Reflection;
}

float2 ParallaxMapping2(Texture2D BumpMap, float2 TexCoord, float3 ViewVec, float BumpScale){
	const float MinLayer = 8.f;
	const float MaxLayer = 24.f;
	float ViewAngle = dot(float3(0.f, 0.f, 1.f), ViewVec);
	float LayerDiv = lerp(MaxLayer, MinLayer, ViewAngle);
	float CurHeight = 0.f;
	float HeightStep = 1.f / LayerDiv;
	float2 CurCoord = TexCoord;
	float2 CoordStep = float2(ViewVec.xy / LayerDiv * BumpScale);
	float CurSample = BumpMap.Sample(gCustSmplr, CurCoord).r;

	while (CurHeight < CurSample){
		CurCoord += CoordStep;
		CurHeight += HeightStep;
		CurSample = BumpMap.SampleLevel(gCustSmplr, CurCoord, 0.f).r;
	}

	float2 PrevCoord = CurCoord - CoordStep;
	float PrevHeight = CurHeight - HeightStep;
	float PrevSample = BumpMap.Sample(gCustSmplr, PrevCoord).r;
	float CurDiff = CurHeight - CurSample;
	float PrevDiff = PrevHeight - PrevSample;
	float BiasAdj = CurDiff / (CurDiff - PrevDiff);

	return lerp(CurCoord, PrevCoord, BiasAdj);
}
float3 ParallaxMapping3(Texture2DArray BumpMaps, float3 TexCoord, float3 ViewVec, float BumpScale){
	const float MinLayer = 8.f;
	const float MaxLayer = 24.f;
	float ViewAngle = dot(float3(0.f, 0.f, 1.f), ViewVec);
	float LayerDiv = lerp(MaxLayer, MinLayer, ViewAngle);
	float CurHeight = 0.f;
	float HeightStep = 1.f / LayerDiv;
	float3 CurCoord = TexCoord;
	float3 CoordStep = float3(ViewVec.xy / LayerDiv * BumpScale, 0.f);
	float CurSample = BumpMaps.Sample(gCustSmplr, CurCoord).r;

	while(CurHeight < CurSample) {
		CurCoord += CoordStep;
		CurHeight += HeightStep;
		CurSample = BumpMaps.SampleLevel(gCustSmplr, CurCoord, 0.f).r;
	}

	float3 PrevCoord = CurCoord - CoordStep;
	float PrevHeight = CurHeight - HeightStep;
	float PrevSample = BumpMaps.Sample(gCustSmplr, PrevCoord).r;
	float CurDiff = CurHeight - CurSample;
	float PrevDiff = PrevHeight - PrevSample;
	float BiasAdj = CurDiff / (CurDiff - PrevDiff);

	return lerp(CurCoord, PrevCoord, BiasAdj);
}
float4 TextureSampling2(Texture2D Texture, float2 Coord, DWORD AddrMode){
	switch(AddrMode){
		case ADDR_MODE_Wrap: return Texture.Sample(gWrapSmplr, Coord);
		case ADDR_MODE_Clamp: return Texture.Sample(gClampSmplr, Coord);
		case ADDR_MODE_Mirror: return Texture.Sample(gMirrorSmplr, Coord);
		case ADDR_MODE_Border: return Texture.Sample(gBorderSmplr, Coord);
		default: return Texture.Sample(gCustSmplr, Coord);
	}
}
float4 TextureSampling3(Texture2DArray TexArray, float3 Coord, DWORD AddrMode){
	switch(AddrMode){
		case ADDR_MODE_Wrap: return TexArray.Sample(gWrapSmplr, Coord);
		case ADDR_MODE_Clamp: return TexArray.Sample(gClampSmplr, Coord);
		case ADDR_MODE_Mirror: return TexArray.Sample(gMirrorSmplr, Coord);
		case ADDR_MODE_Border: return TexArray.Sample(gBorderSmplr, Coord);
		default: return TexArray.Sample(gCustSmplr, Coord);
	}
}

#endif