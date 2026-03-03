#ifndef COMMON_H
#define COMMON_H

#define MATH_PI 3.1415926f
#define FP_EPSILON 0.000001f

#define ADDR_MODE_Wrap   1L
#define ADDR_MODE_Mirror 2L
#define ADDR_MODE_Clamp  3L
#define ADDR_MODE_Border 4L

#define TEX_TYPE_Albedo    (1L << 16)
#define TEX_TYPE_Normal    (1L << 17)
#define TEX_TYPE_MetalRg   (1L << 18)
#define TEX_TYPE_Opacity   (1L << 19)
#define TEX_TYPE_Emission  (1L << 20)
#define TEX_TYPE_Occlusion (1L << 21)
#define TEX_TYPE_Height    (1L << 22)

#define DWORD uint
#define ARGB8 uint
#define RGB10 uint

struct GLight{
	float3 Intensity;
	float  SpotPower;
	float3 Position;
	float  AttStart;
	float3 Direction;
	float  AttEnd;
};
struct GMtlBase{
	float3 Albedo;
	float  Opacity;
	float3 Emission;
	float  Roughness;
	uint3  TexAttrs;
	float  Metalness;
};
struct GMinMtl{
	float3 Albedo;
	float  Roughness;
	float  Metalness;
};

DWORD GetLowByte(DWORD Value){
	return Value & 0xFF;
}
DWORD GetLowWord(DWORD Value){
	return Value & 0xFFFF;
}
DWORD GetHighByte(DWORD Value){
	return (Value >> 8) & 0xFF;
}
DWORD GetHighWord(DWORD Value){
	return (Value >> 16) & 0xFFFF;
}

uint GetChannelB(ARGB8 Value){
	return Value & 0xFF;
}
uint GetChannelG(ARGB8 Value){
	return (Value >> 8) & 0xFF;
}
uint GetChannelR(ARGB8 Value){
	return (Value >> 16) & 0xFF;
}
uint GetChannelA(ARGB8 Value){
	return (Value >> 24) & 0xFF;
}
uint GetChannel(ARGB8 Value, uint Index){
	return (Value >> (Index * 8)) & 0xFF;
}

float4 UnzipColor(ARGB8 Source){
	uint4 Temp = (uint4)Source;
	Temp >>= uint4(16, 8, 0, 24);
	Temp &= 0xFF;

	float4 Dest = float4(Temp);
	Dest /= 255.f;

	return Dest;
}
float3 UnzipNormal(RGB10 Source){
    int3 Temp = (int3)Source;
    Temp <<= uint3(22, 12, 2);
    Temp >>= 22;

    float3 Dest = float3(Temp);
    Dest /= 511.f;

    return Dest;
}

int2 UintToInt2(uint Value){
    uint Low = Value & 0xFFFF;
    if(Low & 0x8000) Low |= 0xFFFF0000;

    uint High = (Value >> 16) & 0xFFFF;
    if(High & 0x8000) High |= 0xFFFF0000;

	return int2(Low, High);
}
int4 UintToInt4(uint Value){
	int4 Result = (int4)Value;

	Result >>= uint4(0, 8, 16, 24);
	Result &= 0xFF;
	Result <<= 24;
	Result >>= 24;

    return Result;
}

float4 UintToFloat4(uint Source, float Scaling){
	uint4 Temp = (uint4)Source;
	Temp >>= uint4(24, 16, 8, 0);
	Temp &= 0xFF;

	float4 Dest = float4(Temp);
	Dest *= Scaling;

	return Dest;
}
float4 Uint2ToFloat4(uint2 Source, float Scaling){
	uint4 Temp = Source.xxyy;
	Temp.yw >>= 16;
	Temp &= 0xFFFF;

	float4 Dest = float4(Temp);
	Dest *= Scaling;

	return Dest;
}

float3 GammaCorrection(float3 Color){
	Color /= Color + 1.f;
	Color = pow(Color, 1.f / 2.2f);
	return Color;
}
float CalcAttenuation(float Dist, float Start, float End){
	return saturate((End - Dist) / (End - Start));
}
float3 ParseNormal(float3 Sample, float3 DatNorm, float3 DatTan){
	float3 AxisN = normalize(DatNorm);
	float3 AxisT = normalize(DatTan - (dot(DatTan, AxisN)*AxisN));
	float3 AxisB = normalize(cross(AxisN, AxisT));

	float3x3 matTBN = float3x3(AxisT, AxisB, AxisN);
	float3 Normal = (2.f * Sample) - 1.f;

	return normalize(mul(Normal, matTBN));
}
float3 CalcTriangleNormal(float3 Point0, float3 Point1, float3 Point2){
	float3 Vec0 = Point1 - Point0;
	float3 Vec1 = Point2 - Point0;
	float3 Normal = cross(Vec0, Vec1);
	return normalize(Normal);
}

float3 SchlickFresnel(float3 F0, float HdotV){
	return F0 + (1.f - F0)*pow(1.f - HdotV, 5.f);
}
float SchlickGgxGeometry(float NdotV, float Roughness){
	float R = Roughness + 1.f;
    float K = R*R / 8.f;
    float Denom = NdotV*(1.f - K) + K;
    return NdotV / Denom;
}
float SmithGeometry(float NdotV, float NdotL, float Roughness){
    float Ggx0 = SchlickGgxGeometry(NdotV, Roughness);
    float Ggx1 = SchlickGgxGeometry(NdotL, Roughness);
    return Ggx0 * Ggx1;
}
float GgxDistribution(float NdotH, float Roughness){
	float A = Roughness * Roughness;
	float A2 = A * A;
	float NdotH2 = NdotH * NdotH;
	float Denom = NdotH2*(A2 - 1.f) + 1.f;
	return A2 / (MATH_PI * Denom * Denom);
}
float3 CookTorrance(GMinMtl MtlBase, float3 Normal, float3 LightVec, float3 ViewVec){
	float3 HalfVec = normalize(ViewVec + LightVec);
	float3 F0 = lerp((float3)0.04f, MtlBase.Albedo, MtlBase.Metalness);

	float HdotV = max(0.f, dot(HalfVec, ViewVec));
	float NdotH = max(0.f, dot(Normal, HalfVec));
	float NdotV = max(0.f, dot(Normal, ViewVec));
	float NdotL = max(0.f, dot(Normal, LightVec));

	float D = GgxDistribution(NdotH, MtlBase.Roughness);
	float G = SmithGeometry(NdotV, NdotL, MtlBase.Roughness);
	float3 F = SchlickFresnel(F0, HdotV);

	float3 Ks = F;
	float3 Kd = (1.f - Ks) * (1.f - MtlBase.Metalness);

	float3 Cook = (F * G * D) / max(0.0001f, 4.f * NdotV * NdotL);
	float3 Lambert = MtlBase.Albedo / MATH_PI;

	float3 Is = Ks * Cook;
	float3 Id = Kd * Lambert;

	return Is + Id;
}

float3 CalcDirectionalLighting(GLight Light, GMinMtl MtlBase, float3 Normal, float3 ViewVec){
	float3 Reflection = (float3)0.f;
	float3 LightVec = -Light.Direction;
	float CosTheta = max(0.f, dot(Normal, LightVec));

	if(CosTheta > 0.f){
		Reflection = Light.Intensity * CosTheta;
		Reflection *= CookTorrance(MtlBase, Normal, LightVec, ViewVec);
	}

	return Reflection;
}
float3 CalcPointLighting(GLight Light, GMinMtl MtlBase, float3 Position, float3 Normal, float3 ViewVec){
	float3 Reflection = (float3)0.f;
	float3 LightVec = normalize(Light.Position - Position);
	float CosTheta = max(0.f, dot(Normal, LightVec));

	if(CosTheta > 0.f){
		float Dist = distance(Light.Position, Position);
		float AttFactor = CalcAttenuation(Dist, Light.AttStart, Light.AttEnd);

		if(AttFactor > 0.f){
			Reflection = Light.Intensity * CosTheta * AttFactor;
			Reflection *= CookTorrance(MtlBase, Normal, LightVec, ViewVec);
		}
	}

	return Reflection;
}
float3 CalcSpotLighting(GLight Light, GMinMtl MtlBase, float3 Position, float3 Normal, float3 ViewVec){
	float3 Reflection = (float3)0.f;
	float3 LightVec = normalize(Light.Position - Position);
	float CosTheta = max(0.f, dot(Normal, LightVec));

	if(CosTheta > 0.f){
		float Dist = distance(Light.Position, Position);
		float AttFactor = CalcAttenuation(Dist, Light.AttStart, Light.AttEnd);
		float SpotFactor = pow(max(dot(-LightVec, Light.Direction), 0.f), Light.SpotPower);

		if((AttFactor * SpotFactor) > 0.f){
			Reflection = Light.Intensity * CosTheta * SpotFactor * AttFactor;
			Reflection *= CookTorrance(MtlBase, Normal, LightVec, ViewVec);
		}
	}

	return Reflection;
}

#endif