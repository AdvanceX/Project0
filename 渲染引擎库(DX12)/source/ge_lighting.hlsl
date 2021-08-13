#include "ge_param.hlsl"


//离散点积(点积)[离散点积]
float HashCos(float CosIncidence){
	if(CosIncidence <= DP_LVL_0) return DP_VAL_0;
	else if(CosIncidence <= DP_LVL_1) return DP_VAL_1;
	else if(CosIncidence <= DP_LVL_2) return DP_VAL_2;
	else if(CosIncidence <= DP_LVL_3) return DP_VAL_3;
	else if(CosIncidence <= DP_LVL_4) return DP_VAL_4;
	else return DP_VAL_5;
}
//离散粗糙度(粗糙度)[离散粗糙度]
float HashRoughness(float Roughness){
	if(Roughness <= RF_LVL_0) return RF_VAL_0;
	else if(Roughness <= RF_LVL_1) return RF_VAL_1;
	else if(Roughness <= RF_LVL_2) return RF_VAL_2;
	else if(Roughness <= RF_LVL_3) return RF_VAL_3;
	else if(Roughness <= RF_LVL_4) return RF_VAL_4;
	else return RF_VAL_5;
}

//计算衰减因子(距离,衰减起点,衰减终点)[衰减因子]
float ComputeAttenuation(float Dist, float AttStart, float AttEnd){
	return saturate((AttEnd - Dist) / (AttEnd - AttStart));
}
//计算石里克近似值(垂直反射率,表面法线,入射光)[石里克值]
float3 ComputeSchlickFresnel(float3 R0, float3 FaceNormal, float3 LightVector){
	float CosIncidence = saturate(dot(FaceNormal, LightVector));
	float F0 = 1.0f - CosIncidence;
	float3 Reflectance = R0 + (1.0f - R0)*pow(F0, 5);

	return Reflectance;
}
//计算布林-冯反射(光强度,光向量,表面法线,观察向量)[反射光]
float3 ComputeBlinnPhong(float3 Intensity, float3 LightVector, float3 FaceNormal, float3 ViewVector){
	float M = gMaterial.Shininess * 256.0f;
	float3 HalfVector = normalize(ViewVector + LightVector);

	float Roughness = (M + 8.0f)*pow(max(dot(HalfVector, FaceNormal), 0.0f), M) / 8.0f;
	float3 Reflectance = ComputeSchlickFresnel(gMaterial.Fresnel, HalfVector, LightVector);

	float3 SpecAlbedo = Reflectance * Roughness;
	SpecAlbedo = SpecAlbedo / (SpecAlbedo + 1.0f);

	return (gMaterial.Albedo + SpecAlbedo) * Intensity;
}
//计算卡通反射(光强度,光向量,表面法线,观察向量)[反射光]
float3 ComputeCartoonReflect(float3 Intensity, float3 LightVector, float3 FaceNormal, float3 ViewVector){
	float M = gMaterial.Shininess * 256.0f;
	float3 HalfVector = normalize(ViewVector + LightVector);

	float Roughness = (M + 8.0f)*pow(max(dot(HalfVector, FaceNormal), 0.0f), M) / 8.0f;
	float3 Reflectance = ComputeSchlickFresnel(gMaterial.Fresnel, HalfVector, LightVector);

	float3 SpecAlbedo = Reflectance * HashRoughness(Roughness);
	SpecAlbedo = SpecAlbedo / (SpecAlbedo + 1.0f);

	return (gMaterial.Albedo + SpecAlbedo) * Intensity;
}

//计算平滑方向光(灯光,表面法线,观察向量)[反射光]
float3 ComputeSmoothDirectionalLight(LIGHT Light, float3 FaceNormal, float3 ViewVector){
	float3 LightVector = -Light.Direction;
	float CosIncidence = max(dot(LightVector, FaceNormal), 0.0f);
	float3 Intensity = Light.Intensity * CosIncidence;

	return ComputeBlinnPhong(Intensity, LightVector, FaceNormal, ViewVector);
}
//计算平滑点光(灯光,入射点,表面法线,观察向量)[反射光]
float3 ComputeSmoothPointLight(LIGHT Light, float3 Point, float3 FaceNormal, float3 ViewVector){
	float3 LightVector = Light.Position - Point;
	float Dist = length(LightVector);

	if(Dist > Light.AttEnd){
		return float3(0.0f, 0.0f, 0.0f);
	} else{
		LightVector /= Dist;

		float CosIncidence = max(dot(LightVector, FaceNormal), 0.0f);
		float Atten = ComputeAttenuation(Dist, Light.AttStart, Light.AttEnd);
		float3 Intensity = Light.Intensity * CosIncidence * Atten;

		return ComputeBlinnPhong(Intensity, LightVector, FaceNormal, ViewVector);
	}
}
//计算平滑聚光灯(灯光,入射点,表面法线,观察向量)[反射光]
float3 ComputeSmoothSpotLight(LIGHT Light, float3 Point, float3 FaceNormal, float3 ViewVector){
	float3 LightVector = Light.Position - Point;
	float Dist = length(LightVector);

	if(Dist > Light.AttEnd){
		return float3(0.0f, 0.0f, 0.0f);
	} else{
		LightVector /= Dist;

		float CosIncidence = max(dot(LightVector, FaceNormal), 0.0f);
		float Atten = ComputeAttenuation(Dist, Light.AttStart, Light.AttEnd);
		float SpotFactor = pow(max(dot(-LightVector, Light.Direction), 0.0f), Light.SpotPower);
		float3 Intensity = Light.Intensity * CosIncidence * Atten * SpotFactor;

		return ComputeBlinnPhong(Intensity, LightVector, FaceNormal, ViewVector);
	}
}

//计算卡通方向光(灯光,表面法线,观察向量)[反射光]
float3 ComputeCartoonDirectionalLight(LIGHT Light, float3 FaceNormal, float3 ViewVector){
	float3 LightVector = -Light.Direction;
	float CosIncidence = max(dot(LightVector, FaceNormal), 0.0f);
	float3 Intensity = Light.Intensity * HashCos(CosIncidence);

	return ComputeCartoonReflect(Intensity, LightVector, FaceNormal, ViewVector);
}
//计算卡通点光(灯光,入射点,表面法线,观察向量)[反射光]
float3 ComputeCartoonPointLight(LIGHT Light, float3 Point, float3 FaceNormal, float3 ViewVector){
	float3 LightVector = Light.Position - Point;
	float Dist = length(LightVector);

	if(Dist > Light.AttEnd){
		return float3(0.0f, 0.0f, 0.0f);
	} else{
		LightVector /= Dist;

		float CosIncidence = max(dot(LightVector, FaceNormal), 0.0f);
		float Atten = ComputeAttenuation(Dist, Light.AttStart, Light.AttEnd);
		float3 Intensity = Light.Intensity * HashCos(CosIncidence) * Atten;

		return ComputeCartoonReflect(Intensity, LightVector, FaceNormal, ViewVector);
	}
}
//计算卡通聚光灯(灯光,入射点,表面法线,观察向量)[反射光]
float3 ComputeCartoonSpotLight(LIGHT Light, float3 Point, float3 FaceNormal, float3 ViewVector){
	float3 LightVector = Light.Position - Point;
	float Dist = length(LightVector);

	if(Dist > Light.AttEnd){
		return float3(0.0f, 0.0f, 0.0f);
	} else{
		LightVector /= Dist;

		float CosIncidence = max(dot(LightVector, FaceNormal), 0.0f);
		float Atten = ComputeAttenuation(Dist, Light.AttStart, Light.AttEnd);
		float SpotFactor = pow(max(dot(-LightVector, Light.Direction), 0.0f), Light.SpotPower);
		float3 Intensity = Light.Intensity * HashCos(CosIncidence) * Atten * SpotFactor;

		return ComputeCartoonReflect(Intensity, LightVector, FaceNormal, ViewVector);
	}
}

//计算光照(入射点,表面法线,观察向量)[反射光]
float3 ComputeLighting(float3 Point, float3 FaceNormal, float3 ViewVector){
	float3 Lighting = (float3)0.0f;

	if(!gCartoonShade){
		for(uint i = 0; i < gLightCount; ++i){
			if(gArrLight[i].AttEnd < 0.0f) continue;

			if(gArrLight[i].SpotPower != 0.0f)
				Lighting += ComputeSmoothSpotLight(gArrLight[i], Point, FaceNormal, ViewVector);
			else if(gArrLight[i].AttStart != 0.0f)
				Lighting += ComputeSmoothPointLight(gArrLight[i], Point, FaceNormal, ViewVector);
			else
				Lighting += ComputeSmoothDirectionalLight(gArrLight[i], FaceNormal, ViewVector);
		}
	} else{
		for(uint i = 0; i < gLightCount; ++i){
			if(gArrLight[i].AttEnd < 0.0f) continue;

			if(gArrLight[i].SpotPower != 0.0f)
				Lighting += ComputeCartoonSpotLight(gArrLight[i], Point, FaceNormal, ViewVector);
			else if(gArrLight[i].AttStart != 0.0f)
				Lighting += ComputeCartoonPointLight(gArrLight[i], Point, FaceNormal, ViewVector);
			else
				Lighting += ComputeCartoonDirectionalLight(gArrLight[i], FaceNormal, ViewVector);
		}
	}

	return Lighting;
}