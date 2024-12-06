//-------------------- DXMath命名空间 --------------------//

namespace dx = DirectX;
namespace dxsm = DirectX::SimpleMath;
namespace dxpv = DirectX::PackedVector;
namespace dxtt = DirectX::TriangleTests;

//----------------------------------------//


//-------------------- 紧缩RGB --------------------//

// Red-Green-Blue(10b Channel)
typedef DWORD RGB10;
// Alpha-Red-Green-Blue(8b Channel)
typedef DWORD ARGB8;

//----------------------------------------//


//-------------------- 紧缩向量 --------------------//

// MMX Vector
typedef dx::XMVECTOR MVECTOR;
// MMX Matrix
typedef dx::XMMATRIX MMATRIX;

// MMX Vector(for 1st-3rd parameters)
typedef dx::FXMVECTOR FMVECTOR;
// MMX Vector(for 4th parameter)
typedef dx::GXMVECTOR GMVECTOR;
// MMX Vector(for 5th-6th parameters)
typedef dx::HXMVECTOR HMVECTOR;
// MMX Vector(for 7th+ parameters)
typedef dx::CXMVECTOR CMVECTOR;

// MMX Matrix(for 1st parameter)
typedef dx::FXMMATRIX FMMATRIX;
// MMX Matrix(for 2nd+ parameters)
typedef dx::CXMMATRIX CMMATRIX;

//----------------------------------------//


//-------------------- 向量&矩阵 --------------------//

// 2D Vector
typedef dx::XMFLOAT2 VECTOR2;
// 3D Vector
typedef dx::XMFLOAT3 VECTOR3;
// 4D Vector
typedef dx::XMFLOAT4 VECTOR4;

// 3×3 Matrix
typedef dx::XMFLOAT3X3 MATRIX3;
// 4×4 Matrix
typedef dx::XMFLOAT4X4 MATRIX4;

//----------------------------------------//


//-------------------- 空间点 --------------------//

// 2D Point(INT32)
typedef dx::XMINT2 POINT2I;
// 3D Point(INT32)
typedef dx::XMINT3 POINT3I;
// 2D Point(USINT)
typedef dx::XMUINT2 POINT2U;
// 3D Point(USINT)
typedef dx::XMUINT3 POINT3U;
// 2D Point(SPFPN)
typedef dx::XMFLOAT2 POINT2;
// 3D Point(SPFPN)
typedef dx::XMFLOAT3 POINT3;

//----------------------------------------//


//-------------------- 包围体 --------------------//

// Axis Aligned Bounding Box
typedef dx::BoundingBox AABB;
// Sphere
typedef dx::BoundingSphere SPHERE;
// Frustum
typedef dx::BoundingFrustum FRUSTUM;
// Oriented Bounding Box
typedef dx::BoundingOrientedBox OBB;

//----------------------------------------//


//-------------------- 向量应用 --------------------//

// Ray
typedef dxsm::Ray RAY;
// Plane
typedef dxsm::Plane PLANE;
// 128b Color
typedef dxsm::Color COLOR128;
// Matrix Extension
typedef dxsm::Matrix MATRX4X;
// 2D Vector Extension
typedef dxsm::Vector2 VECTR2X;
// 3D Vector Extension
typedef dxsm::Vector3 VECTR3X;
// 4D Vector Extension
typedef dxsm::Vector4 VECTR4X;
// Viewport
typedef dxsm::Viewport VIEWPORT;
// Rectangle
typedef dxsm::Rectangle RECTANGLE;
// Quaternion
typedef dxsm::Quaternion QUATERNION;

//----------------------------------------//


//-------------------- 包围圈 --------------------//

// 2D Axis Aligned Bounding Box
struct AABB2{
	VECTR2X Min;
	VECTR2X Max;

	AABB2() = default;
	AABB2(_in VECTOR2 &Min, _in VECTOR2 &Max){
		$m.Min = Min;
		$m.Max = Max;
	}
};

// 2D Oriented Bounding Box
struct OBB2{
	VECTR2X Center;
	VECTR2X Extents;
	SPFPN Orient;

	OBB2() = default;
	OBB2(_in VECTOR2 &Center, _in VECTOR2 &Extents, SPFPN Orient){
		$m.Orient = Orient;
		$m.Center = Center;
		$m.Extents = Extents;
	}
};

// 2D Circle
struct CIRCLE2{
	VECTR2X Center;
	SPFPN Radius;

	CIRCLE2() = default;
	CIRCLE2(_in VECTOR2 &Center, SPFPN Radius){
		$m.Center = Center;
		$m.Radius = Radius;
	}
};

// 2D Triangle
struct TRIANGLE2{
	VECTR2X Pt0;
	VECTR2X Pt1;
	VECTR2X Pt2;

	TRIANGLE2() = default;
	TRIANGLE2(_in VECTOR2 &Point0, _in VECTOR2 &Point1, _in VECTOR2 &Point2){
		$m.Pt0 = Point0;
		$m.Pt1 = Point1;
		$m.Pt2 = Point2;
	}
};

// 2D Segment
struct SEGMENT2{
	VECTR2X Pt0;
	VECTR2X Pt1;

	SEGMENT2() = default;
	SEGMENT2(_in VECTOR2 &Point0, _in VECTOR2 &Point1){
		$m.Pt0 = Point0;
		$m.Pt1 = Point1;
	}
	//////
	SPFPN DistSq(_in VECTOR2 &Point){
		SPFPN LenX = $m.Pt1.x - $m.Pt0.x;
		SPFPN LenY = $m.Pt1.y - $m.Pt0.y;

		if((LenX == 0.f) && (LenY == 0.f))
			return VECTR2X::DistanceSquared($m.Pt0, Point);

		SPFPN DifX = Point.x - $m.Pt0.x;
		SPFPN DifY = Point.y - $m.Pt0.y;
		SPFPN Kt = (DifX*LenX + DifY*LenY) / (LenX*LenX + LenY*LenY);

		if(Kt < 0.f)
			return VECTR2X::DistanceSquared($m.Pt0, Point);
		else if(Kt > 1.f)
			return VECTR2X::DistanceSquared($m.Pt1, Point);

		VECTR2X Offset(LenX * Kt, LenY * Kt);
		VECTR2X Project = $m.Pt0 + Offset;

		return VECTR2X::DistanceSquared(Project, Point);
	}
	SPFPN Distance(_in VECTOR2 &Point){
		SPFPN LenX = $m.Pt1.x - $m.Pt0.x;
		SPFPN LenY = $m.Pt1.y - $m.Pt0.y;

		if((LenX == 0.f) && (LenY == 0.f))
			return VECTR2X::Distance($m.Pt0, Point);

		SPFPN DifX = Point.x - $m.Pt0.x;
		SPFPN DifY = Point.y - $m.Pt0.y;
		SPFPN Kt = (DifX*LenX + DifY*LenY) / (LenX*LenX + LenY*LenY);

		if(Kt < 0.f)
			return VECTR2X::Distance($m.Pt0, Point);
		else if(Kt > 1.f)
			return VECTR2X::Distance($m.Pt1, Point);

		VECTR2X Offset(LenX * Kt, LenY * Kt);
		VECTR2X Project = $m.Pt0 + Offset;

		return VECTR2X::Distance(Project, Point);
	}
};

// 2D Ray
struct RAY2{
	VECTR2X Origin;
	VECTR2X Direction;

	RAY2() = default;
	RAY2(_in VECTOR2 &Origin, _in VECTOR2 &Direction){
		$m.Origin = Origin;
		$m.Direction = Direction;
	}
	//////
	VECTOR2 Sample(SPFPN Dist){
		return $m.Origin + ($m.Direction * Dist);
	}
};

//----------------------------------------//


//-------------------- DXMath调用约定 --------------------//

#define _MM_CALL XM_CALLCONV

//----------------------------------------//


//-------------------- SIMD字段 --------------------//

#define I8m  m64_i8
#define I16m m64_i16
#define I32m m64_i32
#define I64m m64_i64
#define U8m  m64_u8
#define U16m m64_u16
#define U32m m64_u32
#define U64m m64_u64
#define F32m m64_f32

#define I8x  m128_i8
#define I16x m128_i16
#define I32x m128_i32
#define I64x m128_i64
#define U8x  m128_u8
#define U16x m128_u16
#define U32x m128_u32
#define U64x m128_u64
#define F32x m128_f32
#define F64x m128d_f64

#define I8y  m256i_i8;
#define I16y m256i_i16;
#define I32y m256i_i32;
#define I64y m256i_i64;
#define U8y  m256i_u8;
#define U16y m256i_u16;
#define U32y m256i_u32;
#define U64y m256i_u64;
#define F32y m256_f32
#define F64y m256d_f64

//----------------------------------------//


//-------------------- 向量打包解包 --------------------//

#define Pck_Mat(src) dx::XMLoadFloat4x4(src)
#define Pck_V2(src) dx::XMLoadFloat2((dx::XMFLOAT2*)(src))
#define Pck_V3(src) dx::XMLoadFloat3((dx::XMFLOAT3*)(src))
#define Pck_V4(src) dx::XMLoadFloat4((dx::XMFLOAT4*)(src))

#define Upk_Mat(dst, src) dx::XMStoreFloat4x4(dst, src)
#define Upk_V2(dst, src) dx::XMStoreFloat2((dx::XMFLOAT2*)(dst), src)
#define Upk_V3(dst, src) dx::XMStoreFloat3((dx::XMFLOAT3*)(dst), src)
#define Upk_V4(dst, src) dx::XMStoreFloat4((dx::XMFLOAT4*)(dst), src)

//----------------------------------------//


//-------------------- 矩阵运算 --------------------//

#define Mat_Identity() dx::XMMatrixIdentity()
#define Mat_Transpose(mat) dx::XMMatrixTranspose(mat)
#define Mat_Inverse(det, mat) dx::XMMatrixInverse(det, mat)
#define Mat_Multiply(mat0, mat1) dx::XMMatrixMultiply(mat0, mat1)
#define Mat_MulXpose(mat0, mat1) dx::XMMatrixMultiplyTranspose(mat0, mat1)
#define Mat_Det(mat) dx::XMVectorGetX(dx::XMMatrixDeterminant(mat))

//----------------------------------------//


//-------------------- 矩阵创建 --------------------//

#define Mat_ScaleV(vec) dx::XMMatrixScalingFromVector(vec)
#define Mat_Scale(sx, sy, sz) dx::XMMatrixScaling(sx, sy, sz)
#define Mat_TranslateV(vec) dx::XMMatrixTranslationFromVector(vec)
#define Mat_Translate(tx, ty, tz) dx::XMMatrixTranslation(tx, ty, tz)
#define Mat_RotateX(ang) dx::XMMatrixRotationX(ang)
#define Mat_RotateY(ang) dx::XMMatrixRotationY(ang)
#define Mat_RotateZ(ang) dx::XMMatrixRotationZ(ang)
#define Mat_RotateQuat(quat) dx::XMMatrixRotationQuaternion(quat)
#define Mat_RotateAxis(axis, ang) dx::XMMatrixRotationAxis(axis, ang)
#define Mat_RotateEuler(pitch, yaw, roll) dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll)

#define Mat_LookTo(pos, dir, up) dx::XMMatrixLookToLH(pos, dir, up)
#define Mat_LookAt(pos, target, up) dx::XMMatrixLookAtLH(pos, target, up)
#define Mat_Ortho(vw, vh, nz, fz) dx::XMMatrixOrthographicLH(vw, vh, nz, fz)
#define Mat_Perspect(fov, aspect, nz, fz) dx::XMMatrixPerspectiveFovLH(fov, aspect, nz, fz)

#define Mat_Reflect(plane) dx::XMMatrixReflect(plane)
#define Mat_Shadow(plane, light) dx::XMMatrixShadow(plane, light)

#define Mat_Transform(center, orient, scaling, rot, trans) dx::XMMatrixTransformation(center, orient, scaling, center, rot, trans)
#define Mat_Transform2D(center, orient, scaling, rot, trans) dx::XMMatrixTransformation2D(center, orient, scaling, center, rot, trans)
#define Mat_AffineXform(center, scaling, rot, trans) dx::XMMatrixAffineTransformation(scaling, center, rot, trans)
#define Mat_AffineXform2D(center, scaling, rot, trans) dx::XMMatrixAffineTransformation2D(scaling, center, rot, trans)

#define Mat_Decompose(scaling, rot, trans, mat) dx::XMMatrixDecompose(scaling, rot, trans, mat)

//----------------------------------------//


//-------------------- 向量运算 --------------------//

#define Vec_Negate(vec) dx::XMVectorNegate(vec)
#define Vec_Scale(vec, scaling) dx::XMVectorScale(vec, scaling)
#define Vec_Add(vec0, vec1) dx::XMVectorAdd(vec0, vec1)
#define Vec_Sub(vec0, vec1) dx::XMVectorSubtract(vec0, vec1)
#define Vec_Lerp(vec0, vec1, t) dx::XMVectorLerp(vec0, vec1, t)
#define Vec_Hermite(pos0, tan0, pos1, tan1, t) dx::XMVectorHermite(pos0, tan0, pos1, tan1, t)
#define Vec_Barycenter(pos0, pos1, pos2, f, g) dx::XMVectorBaryCentric(pos0, pos1, pos2, f, g)
#define Vec_CatmullRom(pos0, pos1, pos2, pos3, t) dx::XMVectorCatmullRom(pos0, pos1, pos2, pos3, t)

#define Vec2_Length(vec) dx::XMVectorGetX(dx::XMVector2Length(vec))
#define Vec2_LengthSq(vec) dx::XMVectorGetX(dx::XMVector2Dot(vec, vec))
#define Vec2_Normalize(vec) dx::XMVector2Normalize(vec)
#define Vec2_Orthogonal(vec) dx::XMVector2Orthogonal(vec)
#define Vec2_Dot(vec0, vec1) dx::XMVectorGetX(dx::XMVector2Dot(vec0, vec1))
#define Vec2_Angle(vec0, vec1) dx::XMVectorGetX(dx::XMVector2AngleBetweenVectors(vec0, vec1))
#define Vec2_Cross(vec0, vec1) dx::XMVectorGetX(dx::XMVector2Cross(vec0, vec1))
#define Vec2_Equal(vec0, vec1, epsilon) dx::XMVector2NearEqual(vec0, vec1, epsilon)
#define Vec2_Reflect(vec, norm) dx::XMVector2Reflect(vec, norm)
#define Vec2_ClampLength(vec, min, max) dx::XMVector2ClampLength(vec, min, max)
#define Vec2_LinePointDist(p0, p1, q) dx::XMVectorGetX(dx::XMVector2LinePointDistance(p0, p1, q))
#define Vec2_IntersectLine(p0, p1, q0, q1) dx::XMVector2IntersectLine(p0, p1, q0, q1)

#define Vec3_Length(vec) dx::XMVectorGetX(dx::XMVector3Length(vec))
#define Vec3_LengthSq(vec) dx::XMVectorGetX(dx::XMVector3Dot(vec, vec))
#define Vec3_Normalize(vec) dx::XMVector3Normalize(vec)
#define Vec3_Orthogonal(vec) dx::XMVector3Orthogonal(vec)
#define Vec3_Dot(vec0, vec1) dx::XMVectorGetX(dx::XMVector3Dot(vec0, vec1))
#define Vec3_Angle(vec0, vec1) dx::XMVectorGetX(dx::XMVector3AngleBetweenVectors(vec0, vec1))
#define Vec3_Cross(vec0, vec1) dx::XMVector3Cross(vec0, vec1)
#define Vec3_Equal(vec0, vec1, epsilon) dx::XMVector3NearEqual(vec0, vec1, epsilon)
#define Vec3_Reflect(vec, norm) dx::XMVector3Reflect(vec, norm)
#define Vec3_Components(parl, perp, vec, norm) dx::XMVector3ComponentsFromNormal(parl, perp, vec, norm)
#define Vec3_ClampLength(vec, min, max) dx::XMVector3ClampLength(vec, min, max)
#define Vec3_LinePointDist(p0, p1, q) dx::XMVectorGetX(dx::XMVector3LinePointDistance(p0, p1, q))

#define Vec4_Length(vec) dx::XMVectorGetX(dx::XMVector4Length(vec))
#define Vec4_LengthSq(vec) dx::XMVectorGetX(dx::XMVector4Dot(vec, vec))
#define Vec4_Normalize(vec) dx::XMVector4Normalize(vec)
#define Vec4_Orthogonal(vec) dx::XMVector4Orthogonal(vec)
#define Vec4_Dot(vec0, vec1) dx::XMVectorGetX(dx::XMVector4Dot(vec0, vec1))
#define Vec4_Angle(vec0, vec1) dx::XMVectorGetX(dx::XMVector4AngleBetweenVectors(vec0, vec1))
#define Vec4_Cross(vec0, vec1, vec2) dx::XMVector4Cross(vec0, vec1, vec2)
#define Vec4_Equal(vec0, vec1, epsilon) dx::XMVector4NearEqual(vec0, vec1, epsilon)
#define Vec4_Reflect(vec, norm) dx::XMVector4Reflect(vec, norm)
#define Vec4_ClampLength(vec, min, max) dx::XMVector4ClampLength(vec, min, max)

//----------------------------------------//


//-------------------- 向量变换 --------------------//

#define Vec2_Transform(vec, mat) dx::XMVector2Transform(vec, mat)
#define Vec2_XformCoord(vec, mat) dx::XMVector2TransformCoord(vec, mat)
#define Vec2_XformNormal(vec, mat) dx::XMVector2TransformNormal(vec, mat)
#define Vec2_XformStream(dst, dststride, src, srcstride, cnt, mat) dx::XMVector2TransformStream(dst, dststride, src, srcstride, cnt, mat)
#define Vec2_XformStreamCoord(dst, dststride, src, srcstride, cnt, mat) dx::XMVector2TransformCoordStream(dst, dststride, src, srcstride, cnt, mat)
#define Vec2_XformStreamNormal(dst, dststride, src, srcstride, cnt, mat) dx::XMVector2TransformNormalStream(dst, dststride, src, srcstride, cnt, mat)

#define Vec3_Transform(vec, mat) dx::XMVector3Transform(vec, mat)
#define Vec3_XformCoord(vec, mat) dx::XMVector3TransformCoord(vec, mat)
#define Vec3_XformNormal(vec, mat) dx::XMVector3TransformNormal(vec, mat)
#define Vec3_XformStream(dst, dststride, src, srcstride, cnt, mat) dx::XMVector3TransformStream(dst, dststride, src, srcstride, cnt, mat)
#define Vec3_XformStreamCoord(dst, dststride, src, srcstride, cnt, mat) dx::XMVector3TransformCoordStream(dst, dststride, src, srcstride, cnt, mat)
#define Vec3_XformStreamNormal(dst, dststride, src, srcstride, cnt, mat) dx::XMVector3TransformNormalStream(dst, dststride, src, srcstride, cnt, mat)

#define Vec3_Rotate(vec, quat) dx::XMVector3Rotate(vec, quat)
#define Vec3_InvRotate(vec, quat) dx::XMVector3InverseRotate(vec, quat)
#define Vec3_Project(vec, viewp, proj, view, world) dx::XMVector3Project(vec, viewp.TopLeftX, viewp.TopLeftY, viewp.Width, viewp.Height, viewp.MinDepth, viewp.MaxDepth, proj, view, world)
#define Vec3_Unproject(vec, viewp, proj, view, world) dx::XMVector3Unproject(vec, viewp.TopLeftX, viewp.TopLeftY, viewp.Width, viewp.Height, viewp.MinDepth, viewp.MaxDepth, proj, view, world)
#define Vec3_ProjectStream(dst, dststride, src, srcstride, cnt, viewp, proj, view, world) dx::XMVector3ProjectStream(dst, dststride, src, srcstride, cnt, viewp.TopLeftX, viewp.TopLeftY, viewp.Width, viewp.Height, viewp.MinDepth, viewp.MaxDepth, proj, view, world)
#define Vec3_UnprojectStream(dst, dststride, src, srcstride, cnt, viewp, proj, view, world) dx::XMVector3UnprojectStream(dst, dststride, src, srcstride, cnt, viewp.TopLeftX, viewp.TopLeftY, viewp.Width, viewp.Height, viewp.MinDepth, viewp.MaxDepth, proj, view, world)

//----------------------------------------//


//-------------------- 向量创建 --------------------//

#define Vec_GetX(vec) dx::XMVectorGetX(vec)
#define Vec_GetY(vec) dx::XMVectorGetY(vec)
#define Vec_GetZ(vec) dx::XMVectorGetZ(vec)
#define Vec_GetW(vec) dx::XMVectorGetW(vec)
#define Vec_Get(vec, i) dx::XMVectorGetByIndex(vec, i)

#define Vec_SetX(vec, x) dx::XMVectorSetX(vec, x)
#define Vec_SetY(vec, y) dx::XMVectorSetY(vec, y)
#define Vec_SetZ(vec, z) dx::XMVectorSetZ(vec, z)
#define Vec_SetW(vec, w) dx::XMVectorSetW(vec, w)
#define Vec_Set(x, y, z, w) dx::XMVectorSet(x, y, z, w)

#define Vec_Replicate(val) dx::XMVectorReplicate(val)
#define Vec_SplatX(vec) dx::XMVectorSplatX(vec)
#define Vec_SplatY(vec) dx::XMVectorSplatY(vec)
#define Vec_SplatZ(vec) dx::XMVectorSplatZ(vec)

//----------------------------------------//


//-------------------- 四元数运算 --------------------//

#define Quat_Identity() dx::XMQuaternionIdentity()
#define Quat_Ln(quat) dx::XMQuaternionLn(quat)
#define Quat_Exp(quat) dx::XMQuaternionExp(quat)
#define Quat_Inverse(quat) dx::XMQuaternionInverse(quat)
#define Quat_Normalize(quat) dx::XMVector4Normalize(quat)
#define Quat_Conjugate(quat) dx::XMQuaternionConjugate(quat)
#define Quat_Length(quat) dx::XMVectorGetX(dx::XMVector4Length(quat))
#define Quat_LengthSq(quat) dx::XMVectorGetX(dx::XMVector4Dot(quat, quat))
#define Quat_RecLength(quat) dx::XMVectorGetX(dx::XMVector4ReciprocalLength(quat))
#define Quat_Dot(quat0, quat1) dx::XMVectorGetX(dx::XMVector4Dot(quat0, quat1))
#define Quat_Equal(quat0, quat1) dx::XMVector4Equal(quat0, quat1)
#define Quat_Multiply(quat0, quat1) dx::XMQuaternionMultiply(quat0, quat1)
#define Quat_Slerp(quat0, quat1, t) dx::XMQuaternionSlerp(quat0, quat1, t)
#define Quat_Squad(q0, q1, q2, q3, t) dx::XMQuaternionSquad(q0, q1, q2, q3, t)
#define Quat_SquadSetup(a, b, c, q0, q1, q2, q3) dx::XMQuaternionSquadSetup(a, b, c, q0, q1, q2, q3)
#define Quat_Barycenter(q0, q1, q2, f, g) dx::XMQuaternionBaryCentric(q0, q1, q2, f, g)

//----------------------------------------//


//-------------------- 四元数创建 --------------------//

#define Quat_FromMatrix(mat) dx::XMQuaternionRotationMatrix(mat)
#define Quat_FromAxis(axis, ang) dx::XMQuaternionRotationAxis(axis, ang)
#define Quat_FromNormal(norm, ang) dx::XMQuaternionRotationNormal(norm, ang)
#define Quat_FromEuler(pitch, yaw, roll) dx::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll)
#define Quat_ToAxisAngle(axis, ang, quat) dx::XMQuaternionToAxisAngle(axis, ang, quat)

//----------------------------------------//


//-------------------- 平面运算 --------------------//

#define Plane_Normalize(plane) dx::XMPlaneNormalize(plane)
#define Plane_Transform(plane, mat) dx::XMPlaneTransform(plane, mat)
#define Plane_DotCoord(plane, coord) dx::XMVectorGetX(dx::XMPlaneDotCoord(plane, coord))
#define Plane_DotNormal(plane, norm) dx::XMVectorGetX(dx::XMVector3Dot(plane, norm))
#define Plane_IntersectLine(plane, pt0, pt1) dx::XMPlaneIntersectLine(plane, pt0, pt1)
#define Plane_IntersectPlane(ip0, ip1, plane0, plane1) dx::XMPlaneIntersectPlane(ip0, ip1, plane0, plane1)

//----------------------------------------//


//-------------------- 平面创建 --------------------//

#define Plane_FromPoints(pt0, pt1, pt2) dx::XMPlaneFromPoints(pt0, pt1, pt2)
#define Plane_FromPointNormal(pt, norm) dx::XMPlaneFromPointNormal(pt, norm)

//----------------------------------------//


//-------------------- 颜色运算 --------------------//

#define Color_Negate(clr) dx::XMColorNegative(clr)
#define Color_Contrast(clr, contrast) dx::XMColorAdjustContrast(clr, contrast)
#define Color_Saturation(clr, saturation) dx::XMColorAdjustSaturation(clr, saturation)
#define Color_Equal(clr0, clr1) dx::XMVector4Equal(clr0, clr1)
#define Color_Modulate(clr0, clr1) dx::XMVectorMultiply(clr0, clr1)
#define Color_Subtract(clr0, clr1) dx::XMVectorMax(dx::XMVectorSubtract(clr0, clr1), dx::g_XMZero)
#define Color_Add(clr0, clr1) dx::XMVectorMin(dx::XMVectorAdd(clr0, clr1), dx::g_XMOne)

//----------------------------------------//


//-------------------- 颜色压缩 --------------------//

#define Color_Decompress(src) dxpv::XMLoadColor(src)
#define Color_Compress(dst, src) dxpv::XMStoreColor(dst, src)

//----------------------------------------//


//-------------------- 平面运算 --------------------//

//平面投影点
inline MVECTOR _MM_CALL PlaneProjectPoint(FMVECTOR Plane, FMVECTOR Point){
	using namespace dx;
	MVECTOR Dot = dx::XMPlaneDotCoord(Plane, Point);
	return Point - (Dot * Plane); // Point − Dot×Plane.Normal
}

//平面反射点
inline MVECTOR _MM_CALL PlaneReflectPoint(FMVECTOR Plane, FMVECTOR Point){
	using namespace dx;
	MVECTOR Dot = dx::XMPlaneDotCoord(Plane, Point);
	return Point - (Dot * Plane * dx::g_XMTwo.v); // Point − 2×Dot×Plane.Normal
}

//平面反射向量
inline MVECTOR _MM_CALL PlaneReflectVector(FMVECTOR Plane, FMVECTOR Vector){
	using namespace dx;
	MVECTOR Dot = dx::XMVector3Dot(Plane, Vector); // Plane.Normal·Vector
	return Vector - (Dot * Plane * dx::g_XMTwo.v); // Vector − 2×Dot×Plane.Normal
}

//----------------------------------------//


//-------------------- 射线运算 --------------------//

//射线平面相交
SPFPN _MM_CALL RayIntersectPlane(_in RAY &Ray, FMVECTOR Plane, SPFPN Epsilon = 0.001f){
	MVECTOR P0 = dx::XMLoadFloat3(&Ray.position);
	MVECTOR V = dx::XMLoadFloat3(&Ray.direction);

	SPFPN A = dx::XMVectorGetX(dx::XMVector3Dot(Plane, P0)); // Plane.Normal·P0
	SPFPN B = dx::XMVectorGetX(dx::XMVector3Dot(Plane, V)); // Plane.Normal·V
	SPFPN D = dx::XMVectorGetW(Plane);

	if(Fabs(B) < Epsilon) return SPFP_INF;
	else return (Fabs(D) - A) / B;
}

//射线构建(从屏幕)
RAY _MM_CALL RayFromScreen(_in POINT2 &Cursor, _in dx::XMFLOAT2 &WndSize, FMMATRIX matProj, CMMATRIX matView){
	SPFPN ProjScaleX = dx::XMVectorGetX(matProj.r[0]);
	SPFPN ProjScaleY = dx::XMVectorGetY(matProj.r[1]);

	SPFPN TargetX = ((2.f * Cursor.x / WndSize.x) - 1.f) / ProjScaleX;
	SPFPN TargetY = ((-2.f * Cursor.y / WndSize.y) + 1.f) / ProjScaleY;

	MMATRIX matInvView = dx::XMMatrixInverse(P_Null, matView);
	MVECTOR Direction = dx::XMVectorSet(TargetX, TargetY, 1.f, 0.f);
	MVECTOR Position = dx::g_XMIdentityR3;

	Position = dx::XMVector3TransformCoord(Position, matInvView);
	Direction = dx::XMVector3TransformNormal(Direction, matInvView);
	Direction = dx::XMVector3Normalize(Direction);

	return RAY(Position, Direction);
}

//----------------------------------------//


//-------------------- 法线压缩 --------------------//

//压缩法线
RGB10 _MM_CALL NormCompression(FMVECTOR Source){
	MVECTOR Product = dx::XMVectorAdd(Source, dx::g_XMOne3);
	Product = dx::XMVectorScale(Product, 0.5f * 1023.f);
	Product = dx::XMConvertVectorFloatToUInt(Product, 0);

	dx::XMUINT3 Temp;
	dx::XMStoreFloat3((VECTOR3*)&Temp, Product);

	return (Temp.z | Temp.y << 10 | Temp.x << 20);
}

//解压法线
MVECTOR _MM_CALL NormDecompression(RGB10 Source){
	dx::XMUINT3 Temp;
	Temp.x = (Source >> 20) & 0x3FF;
	Temp.y = (Source >> 10) & 0x3FF;
	Temp.z = Source & 0x3FF;

	MVECTOR Dest = dx::XMLoadUInt3(&Temp);
	dx::XMConvertVectorUIntToFloat(Dest, 0);
	Dest = dx::XMVectorScale(Dest, 2.f / 1023.f);
	Dest = dx::XMVectorSubtract(Dest, dx::g_XMOne3);

	return Dest;
}

//----------------------------------------//


//-------------------- 矩阵创建 --------------------//

//矩阵创建(基于视口)
MMATRIX _MM_CALL MatFromViewport(_in VIEWPORT &Viewport){
	SPFPN ViewCentX = Viewport.width * 0.5f;
	SPFPN ViewCentY = Viewport.height * 0.5f;
	MVECTOR Row0 = dx::XMVectorSet(ViewCentX, 0.f, 0.f, 0.f);
	MVECTOR Row1 = dx::XMVectorSet(0.f, -ViewCentY, 0.f, 0.f);
	MVECTOR Row2 = dx::XMVectorSet(0.f, 0.f, Viewport.maxDepth - Viewport.minDepth, 0.f);
	MVECTOR Row3 = dx::XMVectorSet(Viewport.x + ViewCentX, Viewport.y + ViewCentY, Viewport.minDepth, 1.f);
	return MMATRIX(Row0, Row1, Row2, Row3);
}

//----------------------------------------//


//-------------------- 2D碰撞检测 --------------------//

//圆与圆
IBOOL Intersect(_in CIRCLE2 &Circle0, _in CIRCLE2 &Circle1){
	SPFPN Dist = VECTR2X::Distance(Circle0.Center, Circle1.Center);
	SPFPN Limit = Circle0.Radius + Circle1.Radius;
	return (Dist <= Limit);
}

//圆与AABB
IBOOL Intersect(_in CIRCLE2 &Circle, _in AABB2 &Box){
	VECTR2X Closest;
	Circle.Center.Clamp(Box.Min, Box.Max, Closest);

	SPFPN DistSq = VECTR2X::DistanceSquared(Circle.Center, Closest);
	SPFPN Limit = Circle.Radius * Circle.Radius;

	return (DistSq <= Limit);
}

//圆与OBB
IBOOL Intersect(_in CIRCLE2 &Circle, _in OBB2 &Box){
	VECTR2X MinB = Box.Center - Box.Extents;
	VECTR2X MaxB = Box.Center + Box.Extents;

	SPFPN SinB = Sin(Box.Orient);
	SPFPN CosB = Cos(Box.Orient);

	SPFPN DifX = Circle.Center.x - Box.Center.x;
	SPFPN DifY = Circle.Center.y - Box.Center.y;

	VECTR2X Cent0, Closest;

	Cent0 = VECTR2X(CosB*DifX - SinB*DifY, SinB*DifX + CosB*DifY) + Box.Center;
	Cent0.Clamp(MinB, MaxB, Closest);

	SPFPN DistSq = VECTR2X::DistanceSquared(Cent0, Closest);
	SPFPN Limit = Circle.Radius * Circle.Radius;

	return (DistSq <= Limit);
}

//圆与三角形
IBOOL Intersect(_in CIRCLE2 &Circle, _in TRIANGLE2 &Tri){
	SPFPN Limit = Circle.Radius * Circle.Radius;

	SPFPN DistSq0 = VECTR2X::DistanceSquared(Circle.Center, Tri.Pt0);
	SPFPN DistSq1 = VECTR2X::DistanceSquared(Circle.Center, Tri.Pt1);
	SPFPN DistSq2 = VECTR2X::DistanceSquared(Circle.Center, Tri.Pt2);

	if(DistSq0 <= Limit) return B_True;
	if(DistSq1 <= Limit) return B_True;
	if(DistSq2 <= Limit) return B_True;

	VECTR2X Centroid = (Tri.Pt0 + Tri.Pt1 + Tri.Pt2) / 3.f;
	SPFPN DistSq3 = VECTR2X::DistanceSquared(Circle.Center, Centroid);

	if(DistSq3 <= Limit) return B_True;

	SEGMENT2 Seg0(Tri.Pt0, Tri.Pt1);
	SEGMENT2 Seg1(Tri.Pt1, Tri.Pt2);
	SEGMENT2 Seg2(Tri.Pt2, Tri.Pt0);

	if(Seg0.DistSq(Circle.Center) <= Limit) return B_True;
	if(Seg1.DistSq(Circle.Center) <= Limit) return B_True;
	if(Seg2.DistSq(Circle.Center) <= Limit) return B_True;

	return B_False;
}

//AABB与AABB
IBOOL Intersect(_in AABB2 &Box0, _in AABB2 &Box1){
	if(Box0.Min.x > Box1.Max.x) return B_False;
	if(Box0.Max.x < Box1.Min.x) return B_False;
	if(Box0.Min.y > Box1.Max.y) return B_False;
	if(Box0.Max.y < Box1.Min.y) return B_False;
	return B_True;
}

//AABB与OBB
IBOOL Intersect(_in AABB2 &Box0, _in OBB2 &Box1){

}

//AABB与三角形
IBOOL Intersect(_in AABB2 &Box, _in TRIANGLE2 &Tri){

}

//OBB与OBB
IBOOL Intersect(_in OBB2 &Box0, _in OBB2 &Box1){

}

//OBB与三角形
IBOOL Intersect(_in OBB2 &Box, _in TRIANGLE2 &Tri){

}

//三角形与三角形
IBOOL Intersect(_in TRIANGLE2 &Tri0, _in TRIANGLE2 &Tri1){

}

//射线与AABB
IBOOL Intersect(_in RAY2 &Ray, _in AABB2 &Box, _out SPFPN *pT0 = P_Null, _out SPFPN *pT1 = P_Null){
	SPFPN MinT0, MaxT0, MinT1, MaxT1;
	VECTOR2 BoxMin = Box.Min;
	VECTOR2 BoxMax = Box.Max;

	if(Ray.Direction.x < 0.f)
		SWAP_(BoxMin.x, BoxMax.x);
	if(Ray.Direction.y < 0.f)
		SWAP_(BoxMin.y, BoxMax.y);

	if(Ray.Direction.x != 0.f){
		MinT0 = (BoxMin.x - Ray.Origin.x) / Ray.Direction.x;
		MaxT0 = (BoxMax.x - Ray.Origin.x) / Ray.Direction.x;
	} else{
		MinT0 = (Ray.Origin.x < BoxMin.x) ? FLT_MAX : -FLT_MAX;
		MaxT0 = (Ray.Origin.x > BoxMax.x) ? -FLT_MAX : FLT_MAX;
	}

	if(Ray.Direction.y != 0.f){
		MinT1 = (BoxMin.y - Ray.Origin.y) / Ray.Direction.y;
		MaxT1 = (BoxMax.y - Ray.Origin.y) / Ray.Direction.y;
	} else{
		MinT1 = (Ray.Origin.y < BoxMin.y) ? FLT_MAX : -FLT_MAX;
		MaxT1 = (Ray.Origin.y > BoxMax.y) ? -FLT_MAX : FLT_MAX;
	}

	if((MinT0 > MaxT1) || (MinT1 > MaxT0))
		return B_False;

	if(pT1)
		*pT1 = MIN_(MaxT0, MaxT1);
	if(pT0){
		*pT0 = MAX_(MinT0, MinT1);
		*pT0 = MAX_(0.f, *pT0);
	}

	return B_True;
}

//----------------------------------------//