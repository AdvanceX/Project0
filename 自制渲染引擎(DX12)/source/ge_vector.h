//--------------------矩阵/向量/四元数--------------------//

typedef dx::XMVECTOR XVECTOR;
typedef dx::XMMATRIX XMATRIX;

typedef dx::XMFLOAT2 VECTOR2;
typedef dx::XMFLOAT3 VECTOR3;
typedef dx::XMFLOAT4 VECTOR4;

typedef dx::XMFLOAT4X4 MATRIX;
typedef dx::XMFLOAT4 QUATERNION;

//----------------------------------------//


//--------------------色彩--------------------//

//24b色彩
struct COLOR24{
	union{
		UCHR c[3];
		struct{ UCHR b, g, r; };
	};

	COLOR24(ARGB Color){
		self.r = (Color >> 16) & 0xff;
		self.g = (Color >> 8) & 0xff;
		self.b = Color & 0xff;
	}
	COLOR24(UINT R, UINT G, UINT B){
		self.r = (UCHR)R;
		self.g = (UCHR)G;
		self.b = (UCHR)B;
	}
	explicit COLOR24(UINT* Channels){
		self.r = (UCHR)Channels[0];
		self.g = (UCHR)Channels[1];
		self.b = (UCHR)Channels[2];
	}
};
//32b色彩
struct COLOR32: dx_pv::XMCOLOR{
	COLOR32(){}
	COLOR32(ARGB Color):XMCOLOR(Color){}
	COLOR32(SPFP* Channels):XMCOLOR(Channels){}
	COLOR32(SPFP R, SPFP G, SPFP B, SPFP A):XMCOLOR(R, G, B, A){}
	COLOR32(UINT R, UINT G, UINT B, UINT A){
		self.r = (UCHR)R;
		self.g = (UCHR)G;
		self.b = (UCHR)B;
		self.a = (UCHR)A;
	}
};
//128b色彩
struct COLOR128{
	union{
		SPFP c[4];
		struct{ SPFP r, g, b, a; };
	};

	COLOR128(){}
	COLOR128(SPFP R, SPFP G, SPFP B, SPFP A){
		self.r = R;
		self.g = G;
		self.b = B;
		self.a = A;
	}
	explicit COLOR128(SPFP* Channels){
		self.r = Channels[0];
		self.g = Channels[1];
		self.b = Channels[2];
		self.a = Channels[3];
	}
};

//----------------------------------------//


//--------------------单位矩阵--------------------//

const MATRIX gIdentMat(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

//----------------------------------------//


//--------------------向量打包解包--------------------//

#define PackMat(src) dx::XMLoadFloat4x4(&(src))
#define PackV2(src) dx::XMLoadFloat2(&(src))
#define PackV3(src) dx::XMLoadFloat3(&(src))
#define PackV4(src) dx::XMLoadFloat4(&(src))
#define PackF2(src) dx::XMLoadFloat2((VECTOR2*)&(src))
#define PackF3(src) dx::XMLoadFloat3((VECTOR3*)&(src))
#define PackF4(src) dx::XMLoadFloat4((VECTOR4*)&(src))

#define UnpackMat(dest, src) dx::XMStoreFloat4x4(&(dest), src)
#define UnpackV2(dest, src) dx::XMStoreFloat2(&(dest), src)
#define UnpackV3(dest, src) dx::XMStoreFloat3(&(dest), src)
#define UnpackV4(dest, src) dx::XMStoreFloat4(&(dest), src)
#define UnpackF2(dest, src) dx::XMStoreFloat2((VECTOR2*)&(dest), src)
#define UnpackF3(dest, src) dx::XMStoreFloat3((VECTOR3*)&(dest), src)
#define UnpackF4(dest, src) dx::XMStoreFloat4((VECTOR4*)&(dest), src)

//----------------------------------------//


//--------------------矩阵运算--------------------//

#define MatMultiply(mat0, mat1) dx::XMMatrixMultiply(mat0, mat1)
#define MatTranspose(mat) dx::XMMatrixTranspose(mat)
#define MatInverse(mat, det) dx::XMMatrixInverse(det, mat)
#define MatDet(mat) dx::XMMatrixDeterminant(mat).m128_f32[0]
#define MatIdentity() dx::XMMatrixIdentity()

//----------------------------------------//


//--------------------矩阵构建--------------------//

#define MatLookAt(pos, target, up) dx::XMMatrixLookAtLH(pos, target, up)
#define MatLookTo(pos, orient, up) dx::XMMatrixLookToLH(pos, orient, up)
#define MatPerspect(fov, aspect, nz, fz) dx::XMMatrixPerspectiveFovLH(fov, aspect, nz, fz)
#define MatOrtho(width, height, nz, fz) dx::XMMatrixOrthographicLH(width, height, nz, fz)

#define MatScale(sx, sy, sz) dx::XMMatrixScaling(sx, sy, sz)
#define MatTranslate(x, y, z) dx::XMMatrixTranslation(x, y, z)
#define MatRotateX(angle) dx::XMMatrixRotationX(angle)
#define MatRotateY(angle) dx::XMMatrixRotationY(angle)
#define MatRotateZ(angle) dx::XMMatrixRotationZ(angle)
#define MatRotateQuat(quat) dx::XMMatrixRotationQuaternion(quat)
#define MatRotateAxis(axis, angle) dx::XMMatrixRotationAxis(axis, angle)
#define MatRotateHeading(pitch, yaw, roll) dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll)

#define MatReflect(plane) dx::XMMatrixReflect(plane)
#define MatShadow(plane, light) dx::XMMatrixShadow(plane, light)

#define MatTransform(center, orient, scaling, rotation, translation) dx::XMMatrixTransformation(center, orient, scaling, center, rotation, translation)
#define MatTransform2D(center, orient, scaling, rotation, translation) dx::XMMatrixTransformation2D(center, orient, scaling, center, rotation, translation)

//----------------------------------------//


//--------------------向量运算--------------------//

#define VecNegate(vec) dx::XMVectorNegate(vec)
#define VecScale(vec, scaling) dx::XMVectorScale(vec, scaling)
#define VecAdd(vec0, vec1) dx::XMVectorAdd(vec0, vec1)
#define VecSub(vec0, vec1) dx::XMVectorSubtract(vec0, vec1)

#define Vec2Length(vec) dx::XMVector2Length(vec).m128_f32[0]
#define Vec2LengthSq(vec) dx::XMVector2Dot(vec, vec).m128_f32[0]
#define Vec2Normalize(vec) dx::XMVector2Normalize(vec)
#define Vec2Dot(vec0, vec1) dx::XMVector2Dot(vec0, vec1).m128_f32[0]
#define Vec2Cross(vec0, vec1) dx::XMVector2Cross(vec0, vec1)
#define Vec2Angle(vec0, vec1) dx::XMVector2AngleBetweenVectors(vec0, vec1).m128_f32[0]
#define Vec2Equal(vec0, vec1, epsilon) dx::XMVector2NearEqual(vec0, vec1, epsilon)

#define Vec3Length(vec) dx::XMVector3Length(vec).m128_f32[0]
#define Vec3LengthSq(vec) dx::XMVector3Dot(vec, vec).m128_f32[0]
#define Vec3Normalize(vec) dx::XMVector3Normalize(vec)
#define Vec3Dot(vec0, vec1) dx::XMVector3Dot(vec0, vec1).m128_f32[0]
#define Vec3Cross(vec0, vec1) dx::XMVector3Cross(vec0, vec1)
#define Vec3Angle(vec0, vec1) dx::XMVector3AngleBetweenVectors(vec0, vec1).m128_f32[0]
#define Vec3Equal(vec0, vec1, epsilon) dx::XMVector3NearEqual(vec0, vec1, epsilon)

#define Vec4Length(vec) dx::XMVector4Length(vec).m128_f32[0]
#define Vec4LengthSq(vec) dx::XMVector4Dot(vec, vec).m128_f32[0]
#define Vec4Normalize(vec) dx::XMVector4Normalize(vec)
#define Vec4Dot(vec0, vec1) dx::XMVector4Dot(vec0, vec1).m128_f32[0]
#define Vec4Angle(vec0, vec1) dx::XMVector4AngleBetweenVectors(vec0, vec1).m128_f32[0]
#define Vec4Equal(vec0, vec1, epsilon) dx::XMVector4NearEqual(vec0, vec1, epsilon)
#define Vec4Cross(vec0, vec1, vec2) dx::XMVector4Cross(vec0, vec1, vec2)

//----------------------------------------//


//--------------------向量变换--------------------//

#define Vec2Transform(vec, mat) dx::XMVector2Transform(vec, mat)
#define Vec2TransformCoord(vec, mat) dx::XMVector2TransformCoord(vec, mat)
#define Vec2TransformNormal(vec, mat) dx::XMVector2TransformNormal(vec, mat)

#define Vec3Transform(vec, mat) dx::XMVector3Transform(vec, mat)
#define Vec3TransformCoord(vec, mat) dx::XMVector3TransformCoord(vec, mat)
#define Vec3TransformNormal(vec, mat) dx::XMVector3TransformNormal(vec, mat)

//----------------------------------------//


//--------------------向量构建--------------------//

#define VecSet(x,y,z,w) dx::XMVectorSet(x,y,z,w)
#define VecReplicate(value) dx::XMVectorReplicate(value)
#define VecSplatX(vec) dx::XMVectorSplatX(vec)
#define VecSplatY(vec) dx::XMVectorSplatY(vec)
#define VecSplatZ(vec) dx::XMVectorSplatZ(vec)

//----------------------------------------//


//--------------------四元数运算--------------------//

#define QuatInverse(quat) dx::XMQuaternionInverse(quat)
#define QuatConjugate(quat) dx::XMQuaternionConjugate(quat)
#define QuatNormalize(quat) dx::XMQuaternionNormalize(quat)
#define QuatMultiply(quat0, quat1) dx::XMQuaternionMultiply(quat0, quat1)
#define QuatSlerp(quat0, quat1, percent) dx::XMQuaternionSlerp(quat0, quat1, percent)
#define QuatDot(quat0, quat1) Vec4Dot(quat0, quat1)
#define QuatLength(quat) Vec4Length(quat)
#define QuatIdentity() dx::XMQuaternionIdentity()

//----------------------------------------//


//--------------------四元数构建--------------------//

#define QuatRotateMat(mat) dx::XMQuaternionRotationMatrix(mat)
#define QuatRotateAxis(axis, angle) dx::XMQuaternionRotationAxis(axis, angle)
#define QuatRotateNormal(normal, angle) dx::XMQuaternionRotationNormal(normal, angle)
#define QuatToAxisAngle(quat, axis, angle) dx::XMQuaternionToAxisAngle(&axis, &angle, quat)

//----------------------------------------//


//--------------------色彩运算--------------------//

#define ColorAdd(color0, color1) _mm_min_ps(_mm_add_ps(color0, color1), dx::g_XMOne)
#define ColorSub(color0, color1) _mm_max_ps(_mm_sub_ps(color0, color1), dx::g_XMZero)
#define ColorModulate(color0, color1) dx::XMColorModulate(color0, color1)
#define ColorContrast(color, contrast) dx::XMColorAdjustContrast(color, contrast)
#define ColorSaturation(color, saturation) dx::XMColorAdjustSaturation(color, saturation)
#define ColorNegate(color) dx::XMColorNegative(color)

//----------------------------------------//


//--------------------色彩转换--------------------//

#define ColorConvert128(src) dx_pv::XMLoadColor(&(src))
#define ColorConvert32(dest, src) dx_pv::XMStoreColor(&(dest), src)

//----------------------------------------//