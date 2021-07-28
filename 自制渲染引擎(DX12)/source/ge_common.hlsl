struct LIGHT{
	float3 Intensity; //!用于全部灯光
	float  SpotPower; //!用于聚光灯
	float3 Position;  //!用于点光/聚光灯
	float  AttStart;  //!用于点光/聚光灯
	float3 Direction; //!用于方向光/聚光灯
	float  AttEnd;    //!用于点光/聚光灯
};
struct MATERIAL{
	float3 Albedo;    //漫反射反照率
	float  Opacity;   //不透明度
	float3 Fresnel;   //菲涅尔反射率
	float  Shininess; //光滑度
};

cbuffer cbScene: register(b0){
	float3 gAmbientLight;
	uint   gLightCount;
	bool   gCartoonShade;
	float2 gClipEdge;
	float  gFogStart;
	float  gFogRange;
	float3 gFogColor;
}
cbuffer cbFrame: register(b1){
	float4x4 gViewProjTrans;
	float3   gCameraPos;
	float    gTimeSpan;
}
cbuffer cbObject: register(b2){
	float4x4 gWorldTrans;
	float4x4 gNormTrans;
	float4x4 gTexAnim;
}
cbuffer cbSubobj: register(b3){
	MATERIAL gMaterial;
}

SamplerState gSampler: register(s0);
Texture2D gTexture: register(t0, space0);
Texture2D gBumpMap: register(t1, space0);
StructuredBuffer<LIGHT> gArrLight: register(t0, space1);
StructuredBuffer<float4x4> gArrBone: register(t1, space1);

float3 GetViewVector(float3 ViewTarget){
	float3 ViewVector = gCameraPos - ViewTarget;
	float ViewDist = length(ViewVector);
	return ViewVector / ViewDist;
}
float3 GetFogEffect(float3 Position, float3 OrgColor){
	float ViewDist = distance(gCameraPos, Position);
	float FogDensity = saturate((ViewDist - gFogStart) / gFogRange);
	return lerp(OrgColor, gFogColor, FogDensity);
}
float3 TransformNormalSpace(float3 SrcNormal, float3 BasisNormal, float3 BasisTangent){
	float3 N = BasisNormal;
	float3 T = normalize(BasisTangent - dot(BasisTangent, N)*N);
	float3 B = cross(N, T);
	float3x3 TBNMatrix = float3x3(T, B, N);

	float3 DestNormal = 2.0f*SrcNormal - 1.0f;
	return mul(DestNormal, TBNMatrix);
}