//-------------------- MMX调用约定 --------------------//

#define _MX_CALL XM_CALLCONV

//----------------------------------------//


//-------------------- MMX向量加载 --------------------//

#define VtoF_(vec) dx::XMVectorGetX(vec)

#define Ld_V2(src) dx::XMLoadFloat2((VFloat2*)(src))
#define Ld_V3(src) dx::XMLoadFloat3((VFloat3*)(src))
#define Ld_V4(src) dx::XMLoadFloat4((VFloat4*)(src))
#define Ld_M4(src) dx::XMLoadFloat4x4((MFlt4x4*)(src))

#define Sto_V2(dst, src) dx::XMStoreFloat2((VFloat2*)(dst), src)
#define Sto_V3(dst, src) dx::XMStoreFloat3((VFloat3*)(dst), src)
#define Sto_V4(dst, src) dx::XMStoreFloat4((VFloat4*)(dst), src)
#define Sto_M4(dst, src) dx::XMStoreFloat4x4((MFlt4x4*)(dst), src)

//----------------------------------------//


//-------------------- DXMath命名空间 --------------------//

namespace dx = DirectX;
namespace dxsm = DirectX::SimpleMath;
namespace dxpv = DirectX::PackedVector;
namespace dxtt = DirectX::TriangleTests;

//----------------------------------------//


//-------------------- MMX向量 --------------------//

typedef dx::XMVECTOR MXVECTOR; // MMX Vector
typedef dx::XMMATRIX MXMATRIX; // MMX Matrix

typedef dx::FXMVECTOR FMXVECTOR; // MMX Vector(for 1st-3rd parameters)
typedef dx::GXMVECTOR GMXVECTOR; // MMX Vector(for 4th parameter)
typedef dx::HXMVECTOR HMXVECTOR; // MMX Vector(for 5th-6th parameters)
typedef dx::CXMVECTOR CMXVECTOR; // MMX Vector(for 7th+ parameters)

typedef dx::FXMMATRIX FMXMATRIX; // MMX Matrix(for 1st parameter)
typedef dx::CXMMATRIX CMXMATRIX; // MMX Matrix(for 2nd+ parameters)

//----------------------------------------//


//-------------------- 基类元组 --------------------//

typedef dx::XMINT2 VInt2;     // 2 INT(Vector)
typedef dx::XMINT3 VInt3;     // 3 INT(Vector)
typedef dx::XMINT4 VInt4;     // 4 INT(Vector)

typedef dx::XMUINT2 VUint2;   // 2 UINT(Vector)
typedef dx::XMUINT3 VUint3;   // 3 UINT(Vector)
typedef dx::XMUINT4 VUint4;   // 4 UINT(Vector)

typedef dx::XMFLOAT2 VFloat2; // 2 FLOAT(Vector)
typedef dx::XMFLOAT3 VFloat3; // 3 FLOAT(Vector)
typedef dx::XMFLOAT4 VFloat4; // 4 FLOAT(Vector)

typedef dx::XMFLOAT3X3 MFlt3x3; // 3×3 FLOAT(Matrix)
typedef dx::XMFLOAT3X4 MFlt3x4; // 3×4 FLOAT(Matrix)
typedef dx::XMFLOAT4X3 MFlt4x3; // 4×3 FLOAT(Matrix)
typedef dx::XMFLOAT4X4 MFlt4x4; // 4×4 FLOAT(Matrix)

//----------------------------------------//


//-------------------- 空间几何 --------------------//

typedef dx::BoundingBox FBox;         // Box(FLOAT)
typedef dx::BoundingSphere FSphere;   // Sphere(FLOAT)
typedef dx::BoundingFrustum FFrustum; // Frustum(FLOAT)
typedef dx::BoundingOrientedBox FObb; // OBB(FLOAT)

//----------------------------------------//


//-------------------- 向量应用 --------------------//

typedef dxsm::Ray FRay3; // 3D Ray(FLOAT)
typedef dxsm::Plane FPlane; // Plane(FLOAT)
typedef dxsm::Color FColor4; // RGBA Color(FLOAT)
typedef dxsm::Matrix FMatrix4; // 4×4 Matrix(FLOAT)
typedef dxsm::Vector2 FVector2; // 2D Vector(FLOAT)
typedef dxsm::Vector3 FVector3; // 3D Vector(FLOAT)
typedef dxsm::Vector4 FVector4; // 4D Vector(FLOAT)
typedef dxsm::Quaternion FQuat; // Quaternion(FLOAT)
typedef dxsm::Rectangle FRectXY; // XYWH Rectangle(FLOAT)
typedef dxsm::Viewport FViewport; // Viewport(FLOAT)

//----------------------------------------//


//-------------------- 紧缩向量 --------------------//

typedef dxpv::XMBYTE2   PVByte2;   // 2 BYTE(Packed Vector)
typedef dxpv::XMBYTEN2  PVByte2N;  // 2 BYTE(Normalized Vector)
typedef dxpv::XMUBYTE2  PVUbyte2;  // 2 UBYTE(Packed Vector)
typedef dxpv::XMUBYTEN2 PVUbyte2N; // 2 UBYTE(Normalized Vector)

typedef dxpv::XMBYTE4   PVByte4;   // 4 BYTE(Packed Vector)
typedef dxpv::XMBYTEN4  PVByte4N;  // 4 BYTE(Normalized Vector)
typedef dxpv::XMUBYTE4  PVUbyte4;  // 4 UBYTE(Packed Vector)
typedef dxpv::XMUBYTEN4 PVUbyte4N; // 4 UBYTE(Normalized Vector)

typedef dxpv::XMSHORT2   PVShort2;   // 2 SHORT(Packed Vector)
typedef dxpv::XMSHORTN2  PVShort2N;  // 2 SHORT(Normalized Vector)
typedef dxpv::XMUSHORT2  PVUshort2;  // 2 USHORT(Packed Vector)
typedef dxpv::XMUSHORTN2 PVUshort2N; // 2 USHORT(Normalized Vector)

typedef dxpv::XMSHORT4   PVShort4;   // 4 SHORT(Packed Vector)
typedef dxpv::XMSHORTN4  PVShort4N;  // 4 SHORT(Normalized Vector)
typedef dxpv::XMUSHORT4  PVUshort4;  // 4 USHORT(Packed Vector)
typedef dxpv::XMUSHORTN4 PVUshort4N; // 4 USHORT(Normalized Vector)

typedef dxpv::XMHALF2 PVHalf2; // 2 HALF(Packed Vector)
typedef dxpv::XMHALF4 PVHalf4; // 4 HALF(Packed Vector)

typedef dxpv::XMFLOAT3PK PVFloat3;   // Packed Vector for 3 FLOAT
typedef dxpv::XMFLOAT3SE PVFloatSE3; // Packed Vector for 3 Shared Exponent FLOAT

typedef dxpv::XMU565     PV565u;  // U565(Packed Vector)
typedef dxpv::XMU555     PV555u;  // U555(Packed Vector)
typedef dxpv::XMUNIBBLE4 PV4444u; // U4444(Packed Vector)

typedef dxpv::XMDEC4   PVDec3;   // 3 Decimal(Packed Vector)
typedef dxpv::XMDECN4  PVDec3N;  // 3 Decimal(Normalized Vector)
typedef dxpv::XMXDEC4  PVXdec3;  // 3 Decimal(Packed Vector)
typedef dxpv::XMXDECN4 PVXdec3N; // 3 Decimal(Normalized Vector)
typedef dxpv::XMUDEC4  PVUdec3;  // 3 Decimal(Packed Vector)
typedef dxpv::XMUDECN4 PVUdec3N; // 3 Decimal(Normalized Vector)

typedef dxpv::XMCOLOR PVColor; // Packed Vector for Color

//----------------------------------------//


//-------------------- 平面几何 --------------------//

// 2D Box(FLOAT)
struct FBox2{
	FVector2 Min;
	FVector2 Max;

	FBox2() = default;
	FBox2(_in VFloat2 &Min, _in VFloat2 &Max): Min(Min), Max(Max){}
	//////
	$VOID Expand(_in VFloat2 &Point){
		$m.Min = FVector2::Min($m.Min, Point);
		$m.Max = FVector2::Max($m.Max, Point);
	}
	$VOID Translate(_in VFloat2 &Offset){
		$m.Min += Offset;
		$m.Max += Offset;
	}
	$VOID GetVertexs(_out VFloat2 prgResult[4]) const{
		prgResult[0] = $m.Min;
		prgResult[1] = VFloat2($m.Max.x, $m.Min.y);
		prgResult[2] = $m.Max;
		prgResult[3] = VFloat2($m.Min.x, $m.Max.y);
	};
	IBOOL ContainsPoint(_in VFloat2 &Point) const{
		return ((Point.x >= $m.Min.x) &&
			(Point.x <= $m.Max.x) &&
			(Point.y >= $m.Min.y) &&
			(Point.y <= $m.Max.y));
	}
	SPFPN GetPerimeter() const{
		VFloat2 Size = $m.GetSize();
		return (Size.x + Size.y) * 2.f;
	}
	SPFPN GetArea() const{
		VFloat2 Size = $m.GetSize();
		return Size.x * Size.y;
	}
	VFloat2 GetSize() const{
		return $m.Max - $m.Min;
	}
	VFloat2 GetCenter() const{
		return ($m.Min + $m.Max) / 2.f;
	}
	VFloat2 ClampPoint(_in VFloat2 &Point) const{
		SPFPN X = CLAMP_(Point.x, $m.Min.x, $m.Max.x);
		SPFPN Y = CLAMP_(Point.y, $m.Min.y, $m.Max.y);
		return FVector2(X, Y);
	}

	static FBox2 Union(_in FBox2 &Box0, _in FBox2 &Box1){
		FVector2 Min = FVector2::Min(Box0.Min, Box1.Min);
		FVector2 Max = FVector2::Max(Box0.Max, Box1.Max);
		return FBox2(Min, Max);
	}
	static FBox2 Intersection(_in FBox2 &Box0, _in FBox2 &Box1){
		FVector2 Min = FVector2::Max(Box0.Min, Box1.Min);
		FVector2 Max = FVector2::Min(Box0.Max, Box1.Max);
		return FBox2(Min, Max);
	}
};

// 2D OBB(FLOAT)
struct FObb2{
	FVector2 AxisX;
	FVector2 AxisY;
	FVector2 Center;
	FVector2 Extents;

	FObb2() = default;
	FObb2(_in VFloat2 &Center, _in VFloat2 &Extents, SPFPN Rotation):
		Center(Center), Extents(Extents)
	{
		$m.SetRotation(Rotation);
	}
	FObb2(_in VFloat2 &Center, _in VFloat2 &Extents, _in VFloat2 prgAxes[2]):
		Center(Center), Extents(Extents), AxisX(prgAxes[0]), AxisY(prgAxes[1]){}
	//////
	$VOID Rotate(SPFPN Radian){
		SPFPN CosR = cos(Radian);
		SPFPN SinR = sin(Radian);

		auto fpRotAxis = [=](_in FVector2 &Axis)->FVector2{
			return FVector2(
				Axis.x*CosR - Axis.y*SinR,
				Axis.x*SinR + Axis.y*CosR);
		};

		fpRotAxis($m.AxisX).Normalize($m.AxisX);
		fpRotAxis($m.AxisY).Normalize($m.AxisY);
	}
	$VOID SetRotation(SPFPN Radian){
		SPFPN CosR = cos(Radian);
		SPFPN SinR = sin(Radian);
		$m.AxisX = FVector2(CosR, SinR);
		$m.AxisY = FVector2(-SinR, CosR);
	}
	$VOID Translate(_in VFloat2 &Offset){
		$m.Center += Offset;
	}
	$VOID GetVertexs(_out VFloat2 prgResult[4]) const{
		FVector2 DeltaX = $m.AxisX * $m.Extents.x;
		FVector2 DeltaY = $m.AxisY * $m.Extents.y;

		prgResult[0] = $m.Center - DeltaX - DeltaY;
		prgResult[1] = $m.Center + DeltaX - DeltaY;
		prgResult[2] = $m.Center + DeltaX + DeltaY;
		prgResult[3] = $m.Center - DeltaX + DeltaY;
	};
	VFloat2 ProjectOnAxis(_in VFloat2 &Axis) const{
		SPFPN Center = $m.Center.Dot(Axis);
		SPFPN Radius = $m.GetProjectedRadius(Axis);
		return VFloat2(Center - Radius, Center + Radius); // [min,max]
	}
	IBOOL ContainsPoint(_in VFloat2 &Point) const{
		FVector2 Delta = Point - $m.Center;
		SPFPN ProjX = Delta.Dot($m.AxisX);
		SPFPN ProjY = Delta.Dot($m.AxisY);

		if(fabs(ProjX) > $m.Extents.x)
			return B_False;
		if(fabs(ProjY) > $m.Extents.y)
			return B_False;
		return B_True;
	}
	SPFPN GetProjectedRadius(_in VFloat2 &Axis) const{
		SPFPN ProjX = $m.AxisX.Dot(Axis) * $m.Extents.x;
		SPFPN ProjY = $m.AxisY.Dot(Axis) * $m.Extents.y;
		return fabs(ProjX) + fabs(ProjY);
	}
	SPFPN GetPerimeter() const{
		return ($m.Extents.x + Extents.y) * 4.f;
	}
	SPFPN GetArea() const{
		return $m.Extents.x * Extents.y * 4.f;
	}
	FBox2 GetAabb() const{
		VFloat2 Min, Max;
		VFloat2 rgVert[4];

		$m.GetVertexs(rgVert);

		Min = rgVert[0];
		Max = rgVert[0];

		for(UNS32 iVtx = 1; iVtx < 4; ++iVtx){
			Min = FVector2::Min(Min, rgVert[iVtx]);
			Max = FVector2::Max(Max, rgVert[iVtx]);
		}

		return FBox2(Min, Max);
	}

	static IBOOL IsSeparatingAxis(_in FObb2 &Box0, _in FObb2 &Box1, _in VFloat2 &Axis){
		FVector2 Dist = Box1.Center - Box0.Center;
		SPFPN Proj = fabs(Dist.Dot(Axis));

		SPFPN Rad0 = Box0.GetProjectedRadius(Axis);
		SPFPN Rad1 = Box1.GetProjectedRadius(Axis);

		return (Proj > (Rad0 + Rad1));
	}
};

// 2D Circle(FLOAT)
struct FCircle2{
	SPFPN Radius;
	FVector2 Center;
	
	FCircle2() = default;
	FCircle2(_in VFloat2 &Center, SPFPN Radius): Center(Center), Radius(Radius){}
	//////
	$VOID Translate(_in VFloat2 &Offset){
		$m.Center += Offset;
	}
	IBOOL ContainsPoint(_in VFloat2 &Point) const{
		SPFPN DistSq = FVector2::DistanceSquared($m.Center, Point);
		SPFPN RadSq = $m.Radius * $m.Radius;
		return DistSq <= RadSq;
	}
	SPFPN GetPerimeter() const{
		return $m.Radius * MATH_PI * 2.f;
	}
	SPFPN GetArea() const{
		return $m.Radius * $m.Radius * MATH_PI;
	}
};

// 2D Triangle(FLOAT)
struct FTriangle2{
	union{
		struct{
			FVector2 Pt0;
			FVector2 Pt1;
			FVector2 Pt2;
		};
		FVector2 Pt[3];
	};

	FTriangle2(){};
	FTriangle2(_in VFloat2 &Point0, _in VFloat2 &Point1, _in VFloat2 &Point2):
		Pt0(Point0), Pt1(Point1), Pt2(Point2){}
	//////
	SPFPN GetArea() const{
		/// 使用行列式公式 |AB × AC| / 2
		SPFPN A = $m.Pt0.x * ($m.Pt1.y - $m.Pt2.y);
		SPFPN B = $m.Pt1.x * ($m.Pt2.y - $m.Pt0.y);
		SPFPN C = $m.Pt2.x * ($m.Pt0.y - $m.Pt1.y);
		return fabs(A + B + C) / 2.f;
	}
	SPFPN GetSideA() const{
		return FVector2::Distance($m.Pt1, $m.Pt2);
	}
	SPFPN GetSideB() const{
		return FVector2::Distance($m.Pt2, $m.Pt0);
	}
	SPFPN GetSideC() const{
		return FVector2::Distance($m.Pt0, $m.Pt1);
	}
	SPFPN GetPerimeter() const{
		return $m.GetSideA() + $m.GetSideB() + $m.GetSideC();
	}
	VFloat2 GetCentroid() const{
		return ($m.Pt0 + $m.Pt1 + $m.Pt2) / 3.f;
	}
	IBOOL ContainsPoint(_in VFloat2 &Point, SPFPN Tol = 1e-5f) const{
		/// 使用重心坐标法
		SPFPN Area10 = FTriangle2(Point, $m.Pt1, $m.Pt2).GetArea();
		SPFPN Area11 = FTriangle2($m.Pt0, Point, $m.Pt2).GetArea();
		SPFPN Area12 = FTriangle2($m.Pt0, $m.Pt1, Point).GetArea();
		SPFPN Area1 = Area10 + Area11 + Area12;
		SPFPN Area0 = $m.GetArea();
		return NEAR_EQUAL(Area0, Area1, Tol);
	}
	$VOID Translate(_in VFloat2 &Offset){
		$m.Pt0 += Offset;
		$m.Pt1 += Offset;
		$m.Pt2 += Offset;
	}
};

// 2D Segment(FLOAT)
struct FSegment2{
	union{
		struct{
			FVector2 Pt0;
			FVector2 Pt1;
		};
		FVector2 Pt[2];
	};

	FSegment2(){};
	FSegment2(_in VFloat2 &Point0, _in VFloat2 &Point1): Pt0(Point0), Pt1(Point1){}
	//////
	SPFPN Length() const{
		return ($m.Pt1 - $m.Pt0).Length();
	}
	SPFPN LengthSq() const{
		return ($m.Pt1 - $m.Pt0).LengthSquared();
	}
	SPFPN DistanceToPoint(_in VFloat2 &Point) const{
		FVector2 Closest = $m.ProjectPoint(Point);
		return (Point - Closest).Length();
	}
	IBOOL ContainsPoint(_in VFloat2 &Point, SPFPN Tol = 1e-5f) const{
		SPFPN Len10 = (Point - $m.Pt0).LengthSquared();
		SPFPN Len11 = (Point - $m.Pt1).LengthSquared();
		SPFPN Len1 = Len10 + Len11;
		SPFPN Len0 = $m.LengthSq();
		return NEAR_EQUAL(Len0, Len1, Tol);
	}
	VFloat2 ProjectPoint(_in VFloat2 &Point) const{
		FVector2 Vec0 = $m.Pt1 - $m.Pt0;
		FVector2 Vec1 = Point - $m.Pt0;
		SPFPN Ratio = Vec1.Dot(Vec0) / Vec0.Dot(Vec0);
		Ratio = CLAMP_(Ratio, 0.f, 1.f);
		return $m.Pt0 + (Vec0 * Ratio);
	}
	VFloat2 Direction() const{
		return $m.Pt1 - $m.Pt0;
	}

	static VFloat2 Intersection(_in FSegment2 &Segment0, _in FSegment2 &Segment1){
		FVector2 Vec0 = Segment0.Pt1 - Segment0.Pt0;
		FVector2 Vec1 = Segment1.Pt1 - Segment1.Pt0;
		FVector2 Vec2 = Segment1.Pt0 - Segment0.Pt0;
		SPFPN Ratio = Vec2.Cross(Vec1).x / Vec0.Cross(Vec1).x;
		return Segment0.Pt0 + (Vec0 * Ratio);
	}
};

// 2D Ray(FLOAT)
struct FRay2{
	FVector2 Origin;
	FVector2 Direction;

	FRay2() = default;
	FRay2(_in VFloat2 &Origin, _in VFloat2 &Direction): Origin(Origin), Direction(Direction){}
	//////
	VFloat2 PointAt(SPFPN Dist) const{
		return $m.Origin + ($m.Direction * Dist);
	}
};

//----------------------------------------//


//-------------------- 曲线 --------------------//

// 2D Curve(FLOAT)
struct FCurve2{
	FVector2 Ctrl0;
	FVector2 Ctrl1;
	FVector2 Ctrl2;
	FVector2 Ctrl3;

	FCurve2() = default;
	FCurve2(_in VFloat2 &Ctrl0, _in VFloat2 &Ctrl1, _in VFloat2 &Ctrl2, _in VFloat2 &Ctrl3):
		Ctrl0(Ctrl0), Ctrl1(Ctrl1), Ctrl2(Ctrl2), Ctrl3(Ctrl3){}
	//////
	SPFPN EstLength() const{
		SPFPN Len01 = FVector2::Distance($m.Ctrl1, $m.Ctrl0);
		SPFPN Len12 = FVector2::Distance($m.Ctrl2, $m.Ctrl1);
		SPFPN Len23 = FVector2::Distance($m.Ctrl3, $m.Ctrl2);
		SPFPN Len03 = FVector2::Distance($m.Ctrl3, $m.Ctrl0);

		return (Len01 + Len12 + Len23 + Len03) * 0.5f;
	}
	VFloat2 Evaluate(SPFPN Factor) const{
		// P(t) = P0×(1−t)³ + P1×3t(1−t)² + P2×3t²(1−t) + P3×t³

		SPFPN T1 = Factor;
		SPFPN T2 = T1 * T1;
		SPFPN T3 = T2 * T1;
		SPFPN S1 = 1.f - T1;
		SPFPN S2 = S1 * S1;
		SPFPN S3 = S2 * S1;

		FVector2 Pt0 = $m.Ctrl0 * S3;
		FVector2 Pt1 = $m.Ctrl1 * S2 * T1 * 3.f;
		FVector2 Pt2 = $m.Ctrl2 * S1 * T2 * 3.f;
		FVector2 Pt3 = $m.Ctrl3 * T3;

		return Pt0 + Pt1 + Pt2 + Pt3;
	}
	VFloat2 Derivative(SPFPN Factor) const{
		// P'(t) = P0×(−3)(1−t)² + P1×(3(1−t)² − 6t(1−t)) + P2×(6t(1−t) − 3t²) + P3×3t²

		SPFPN T1 = Factor;
		SPFPN T2 = T1 * T1;
		SPFPN S1 = 1.f - T1;
		SPFPN S2 = S1 * S1;

		SPFPN B0 = S2 * -3.f;
		SPFPN B1 = S2*3.f - T1*S1*6.f;
		SPFPN B2 = T1*S1*6.f - T2*3.f;
		SPFPN B3 = T2 * 3.f;

		FVector2 Vec0 = $m.Ctrl0 * B0;
		FVector2 Vec1 = $m.Ctrl1 * B1;
		FVector2 Vec2 = $m.Ctrl2 * B2;
		FVector2 Vec3 = $m.Ctrl3 * B3;

		return Vec0 + Vec1 + Vec2 + Vec3;
	}
};

// 3D Curve(FLOAT)
struct FCurve3{
	FVector3 Ctrl0;
	FVector3 Ctrl1;
	FVector3 Ctrl2;
	FVector3 Ctrl3;

	FCurve3() = default;
	FCurve3(_in VFloat3 &Ctrl0, _in VFloat3 &Ctrl1, _in VFloat3 &Ctrl2, _in VFloat3 &Ctrl3):
		Ctrl0(Ctrl0), Ctrl1(Ctrl1), Ctrl2(Ctrl2), Ctrl3(Ctrl3){}
	//////
	SPFPN EstLength() const{
		SPFPN Len01 = FVector3::Distance($m.Ctrl1, $m.Ctrl0);
		SPFPN Len12 = FVector3::Distance($m.Ctrl2, $m.Ctrl1);
		SPFPN Len23 = FVector3::Distance($m.Ctrl3, $m.Ctrl2);
		SPFPN Len03 = FVector3::Distance($m.Ctrl3, $m.Ctrl0);

		return (Len01 + Len12 + Len23 + Len03) * 0.5f;
	}
	VFloat3 Evaluate(SPFPN Factor) const{
		// P(t) = P0×(1−t)³ + P1×3t(1−t)² + P2×3t²(1−t) + P3×t³

		SPFPN T1 = Factor;
		SPFPN T2 = T1 * T1;
		SPFPN T3 = T2 * T1;
		SPFPN S1 = 1.f - T1;
		SPFPN S2 = S1 * S1;
		SPFPN S3 = S2 * S1;

		FVector3 Pt0 = $m.Ctrl0 * S3;
		FVector3 Pt1 = $m.Ctrl1 * S2 * T1 * 3.f;
		FVector3 Pt2 = $m.Ctrl2 * S1 * T2 * 3.f;
		FVector3 Pt3 = $m.Ctrl3 * T3;

		return Pt0 + Pt1 + Pt2 + Pt3;
	}
	VFloat3 Derivative(SPFPN Factor) const{
		// P'(t) = P0×(−3)(1−t)² + P1×(3(1−t)² − 6t(1−t)) + P2×(6t(1−t) − 3t²) + P3×3t²

		SPFPN T1 = Factor;
		SPFPN T2 = T1 * T1;
		SPFPN S1 = 1.f - T1;
		SPFPN S2 = S1 * S1;

		SPFPN B0 = S2 * -3.f;
		SPFPN B1 = S2*3.f - T1*S1*6.f;
		SPFPN B2 = T1*S1*6.f - T2*3.f;
		SPFPN B3 = T2 * 3.f;

		FVector3 Vec0 = $m.Ctrl0 * B0;
		FVector3 Vec1 = $m.Ctrl1 * B1;
		FVector3 Vec2 = $m.Ctrl2 * B2;
		FVector3 Vec3 = $m.Ctrl3 * B3;

		return Vec0 + Vec1 + Vec2 + Vec3;
	}
};

//----------------------------------------//


//-------------------- 平面运算 --------------------//

namespace DirectX{
	// 平面投影点
	inline MXVECTOR _MX_CALL XMPlaneProjectPoint(FMXVECTOR Plane, FMXVECTOR Point){
		MXVECTOR Dot = dx::XMPlaneDotCoord(Plane, Point);
		return Point - (Dot * Plane); // Point − Dot×Plane.Normal
	}

	// 平面反射点
	inline MXVECTOR _MX_CALL XMPlaneReflectPoint(FMXVECTOR Plane, FMXVECTOR Point){
		MXVECTOR Dot = dx::XMPlaneDotCoord(Plane, Point);
		return Point - (Dot * Plane * dx::g_XMTwo.v); // Point − 2×Dot×Plane.Normal
	}

	// 平面反射向量
	inline MXVECTOR _MX_CALL XMPlaneReflectVector(FMXVECTOR Plane, FMXVECTOR Vector){
		MXVECTOR Dot = dx::XMVector3Dot(Plane, Vector); // Plane.Normal·Vector
		return Vector - (Dot * Plane * dx::g_XMTwo.v); // Vector − 2×Dot×Plane.Normal
	}
}

//----------------------------------------//


//-------------------- 基于屏幕创建 --------------------//

namespace DirectX{
	// 矩阵创建基于视口
	MXMATRIX _MX_CALL XMMatrixFromViewport(_in FViewport &Viewport){
		SPFPN CenterX = Viewport.width * 0.5f;
		SPFPN CenterY = Viewport.height * 0.5f;
		MXVECTOR Row0 = dx::XMVectorSet(CenterX, 0.f, 0.f, 0.f);
		MXVECTOR Row1 = dx::XMVectorSet(0.f, -CenterY, 0.f, 0.f);
		MXVECTOR Row2 = dx::XMVectorSet(0.f, 0.f, Viewport.maxDepth - Viewport.minDepth, 0.f);
		MXVECTOR Row3 = dx::XMVectorSet(Viewport.x + CenterX, Viewport.y + CenterY, Viewport.minDepth, 1.f);
		return MXMATRIX(Row0, Row1, Row2, Row3);
	}

	// 矩阵创建基于视口
	$VOID _MX_CALL XMMatrixFromViewport(_out FMatrix4 &rResult, _in FViewport &Viewport, IBOOL bIdentity = B_False){
		SPFPN CenterX = Viewport.width * 0.5f;
		SPFPN CenterY = Viewport.height * 0.5f;

		if(!bIdentity) rResult = FMatrix4::Identity;

		rResult.m[0][0] = CenterX;
		rResult.m[1][1] = -CenterY;
		rResult.m[2][2] = Viewport.maxDepth - Viewport.minDepth;
		rResult.m[3][0] = Viewport.x + CenterX;
		rResult.m[3][1] = Viewport.y + CenterY;
		rResult.m[3][2] = Viewport.minDepth;
		rResult.m[3][3] = 1.f;
	}

	// 射线创建基于屏幕
	$VOID _MX_CALL XMRayFromScreen(_out XMVECTOR &rPosition, _out XMVECTOR &rDirection,  _in VFloat2 &Cursor, _in VFloat2 &WndSize, FMXMATRIX matProj, CMXMATRIX matView){
		SPFPN ProjScaleX = dx::XMVectorGetX(matProj.r[0]);
		SPFPN ProjScaleY = dx::XMVectorGetY(matProj.r[1]);

		SPFPN TargetX = ((2.f * Cursor.x / WndSize.x) - 1.f) / ProjScaleX;
		SPFPN TargetY = ((-2.f * Cursor.y / WndSize.y) + 1.f) / ProjScaleY;

		MXMATRIX matInvView = dx::XMMatrixInverse(P_Null, matView);
		MXVECTOR Direction = dx::XMVectorSet(TargetX, TargetY, 1.f, 0.f);
		MXVECTOR Position = dx::g_XMIdentityR3;

		rPosition = dx::XMVector3TransformCoord(Position, matInvView);
		rDirection = dx::XMVector3TransformNormal(Direction, matInvView);
		rDirection = dx::XMVector3Normalize(rDirection);
	}
}

//----------------------------------------//


//-------------------- 多边形计算 --------------------//

// 多边形转AABB
FBox2 PolygonToAabb(_in VFloat2 *prgVert, UNS32 Count){
	FVector2 *pVert = (FVector2*)prgVert;
	FVector2 *pEndV = pVert + Count;

	FVector2 Min = *pVert++;
	FVector2 Max = Min;

	for(; pVert != pEndV; ++pVert){
		Min = FVector2::Min(Min, *pVert);
		Max = FVector2::Max(Max, *pVert);
	}

	return FBox2(Min, Max);
}

// 多边形质心
VFloat2 PolygonCentroid(_in VFloat2 *prgVert, UNS32 Count){
	FVector2 *pVert = (FVector2*)prgVert;
	FVector2 *pEndV = pVert + Count;
	FVector2 Center = *pVert++;

	for(; pVert != pEndV; ++pVert)
		Center += *pVert;

	return Center / (SPFPN)Count;
}

// 多边形投影
VFloat2 PolygonProject(_in VFloat2 *prgVert, UNS32 Count, _in VFloat2 &Axis){
	FVector2 *pVert = (FVector2*)prgVert;
	FVector2 *pEndV = pVert + Count;

	SPFPN Min = pVert->Dot(Axis);
	SPFPN Max = Min;

	for(++pVert; pVert != pEndV; ++pVert){
		SPFPN Dot = pVert->Dot(Axis);
		if(Dot < Min) Min = Dot;
		if(Dot > Max) Max = Dot;
	}

	return VFloat2(Min, Max);
}

// 多边形包含点
IBOOL PolygonContainsPoint(_in VFloat2 *prgVert, UNS32 Count, _in VFloat2 &Point){
	FVector2 Edge, Vep;
	FVector2 *pFirstV = (FVector2*)prgVert;
	FVector2 *pLastV = pFirstV + Count - 1;
	FVector2 *pVert = pFirstV;

	/// 检查前 n-1 条边

	for(; pVert != pLastV; ++pVert){
		Edge = pVert[1] - pVert[0];
		Vep = Point - pVert[0];

		if(Edge.Cross(Vep).x < 0.f)
			return B_False;
	}

	/// 检查最后一条边

	Edge = *pLastV - *pFirstV;
	Vep = Point - *pFirstV;

	if(Edge.Cross(Vep).x < 0.f)
		return B_False;

	//////

	return B_True;
}

// 多边形分离轴测试
IBOOL PolygonSatTest(_in VFloat2 *prgVert0, UNS32 Count0, _in VFloat2 *prgVert1, UNS32 Count1, _in VFloat2 Axis){
	VFloat2 Proj0 = PolygonProject(prgVert0, Count0, Axis);
	VFloat2 Proj1 = PolygonProject(prgVert1, Count1, Axis);

	if(Proj0.y < Proj1.x) return B_True;
	if(Proj1.y < Proj0.x) return B_True;
	return B_False;
}

// 多边形法线
$VOID PolygonNormals(_in VFloat2 *prgVert, UNS32 Count, _out VFloat2 *prgResult){
	FVector2 *pNorm = (FVector2*)prgResult;
	FVector2 *pVert = (FVector2*)prgVert;
	FVector2 *pLastV = pVert + Count - 1;

	for(; pVert != pLastV; ++pVert){
		FVector2 Edge = pVert[1] - pVert[0];
		FVector2(-Edge.y, Edge.x).Normalize(*pNorm++);
	}

	FVector2 Edge = *pLastV - prgVert[0];
	FVector2(-Edge.y, Edge.x).Normalize(*pNorm);
}

// 多边形旋转
$VOID PolygonRotate(_in VFloat2 *prgVert, UNS32 Count, SPFPN Radian){
	SPFPN CosR = std::cos(Radian);
	SPFPN SinR = std::sin(Radian);

	FVector2 *pVert = (FVector2*)prgVert;
	FVector2 *pEndV = pVert + Count;
	FVector2 Center = PolygonCentroid(prgVert, Count);

	for(; pVert != pEndV; ++pVert){
		FVector2 Vec = *pVert - Center;
		pVert->x = (Vec.x * CosR) - (Vec.y * SinR);
		pVert->y = (Vec.x * SinR) + (Vec.y * CosR);
		*pVert += Center;
	}
}

//----------------------------------------//


//-------------------- 2D碰撞检测 --------------------//

//凸多边形与凸多边形
IBOOL Intersects(_in VFloat2 *prgVert0, UNS32 Count0, _in VFloat2 *prgVert1, UNS32 Count1){
	auto pfnVecPerp = [](_in FVector2 &Vector)->FVector2{
		FVector2 Result(-Vector.y, Vector.x);
		Result.Normalize();
		return Result;
	};

	FVector2 *pPt00 = (FVector2*)prgVert0;
	FVector2 *pPt01 = pPt00 + 1;
	FVector2 *pEnd0 = pPt00 + Count0;

	for(; pPt00 != pEnd0; ++pPt00, ++pPt01){
		if(pPt01 == pEnd0) pPt01 = (FVector2*)prgVert0;

		FVector2 Edge = *pPt01 - *pPt00;
		FVector2 Axis = pfnVecPerp(Edge);

		VFloat2 Proj0 = PolygonProject(prgVert0, Count0, Axis);
		VFloat2 Proj1 = PolygonProject(prgVert1, Count1, Axis);

		if((Proj0.y < Proj1.x) || (Proj1.y < Proj0.x))
			return B_False;
	}

	FVector2 *pPt10 = (FVector2*)prgVert1;
	FVector2 *pPt11 = pPt10 + 1;
	FVector2 *pEnd1 = pPt10 + Count1;

	for(; pPt10 != pEnd1; ++pPt10, ++pPt11){
		if(pPt11 == pEnd1) pPt11 = (FVector2*)prgVert1;

		FVector2 Edge = *pPt11 - *pPt10;
		FVector2 Axis = pfnVecPerp(Edge);

		VFloat2 Proj0 = PolygonProject(prgVert0, Count0, Axis);
		VFloat2 Proj1 = PolygonProject(prgVert1, Count1, Axis);

		if((Proj0.y < Proj1.x) || (Proj1.y < Proj0.x))
			return B_False;
	}

	return B_True;
}

//凸多边形与圆
IBOOL Intersects(_in VFloat2 *prgVert, UNS32 Count, _in FCircle2 &Circle){
	if(PolygonContainsPoint(prgVert, Count, Circle.Center))
		return B_True;

	SPFPN RadSq = Circle.Radius * Circle.Radius;

	for(UNS32 iPt0 = 0; iPt0 < Count; ++iPt0){
		UNS32 iPt1 = (iPt0 + 1) % Count;
		FSegment2 Edge(prgVert[iPt0], prgVert[iPt1]);
		FVector2 Closest = Edge.ProjectPoint(Circle.Center);
		SPFPN DistSq = FVector2::DistanceSquared(Closest, Circle.Center);
		if(DistSq < RadSq) return B_True;
	}

	return B_False;
}

// 圆与圆
IBOOL Intersects(_in FCircle2 &Circle0, _in FCircle2 &Circle1){
	SPFPN Dist = FVector2::Distance(Circle0.Center, Circle1.Center);
	SPFPN Limit = Circle0.Radius + Circle1.Radius;
	return (Dist <= Limit);
}

// 圆与AABB
IBOOL Intersects(_in FCircle2 &Circle, _in FBox2 &Box){
	FVector2 Closest;
	Circle.Center.Clamp(Box.Min, Box.Max, Closest);

	SPFPN DistSq = FVector2::DistanceSquared(Circle.Center, Closest);
	SPFPN Limit = Circle.Radius * Circle.Radius;

	return (DistSq <= Limit);
}

// 圆与OBB
IBOOL Intersects(_in FCircle2 &Circle, _in FObb2 &Box){
	FVector2 Vcb = Circle.Center - Box.Center;
	FVector2 Pbox = { Vcb.Dot(Box.AxisX), Vcb.Dot(Box.AxisY) };
	FVector2 Closest;
	Pbox.Clamp(-Box.Extents, Box.Extents, Closest);

	SPFPN DistSq = FVector2::DistanceSquared(Pbox, Closest);
	SPFPN Limit = Circle.Radius * Circle.Radius;

	return (DistSq <= Limit);
}

// 圆与三角形
IBOOL Intersects(_in FCircle2 &Circle, _in FTriangle2 &Tri){
	FSegment2 Edge;
	FVector2 Closest;
	SPFPN RadSq, DistSq;

	RadSq = Circle.Radius * Circle.Radius;

	Edge = { Tri.Pt0, Tri.Pt1 };
	Closest = Edge.ProjectPoint(Circle.Center);
	DistSq = FVector2::DistanceSquared(Circle.Center, Closest);
	if(DistSq <= RadSq) return B_True;

	Edge = { Tri.Pt1, Tri.Pt2 };
	Closest = Edge.ProjectPoint(Circle.Center);
	DistSq = FVector2::DistanceSquared(Circle.Center, Closest);
	if(DistSq <= RadSq) return B_True;

	Edge = { Tri.Pt2, Tri.Pt0 };
	Closest = Edge.ProjectPoint(Circle.Center);
	DistSq = FVector2::DistanceSquared(Circle.Center, Closest);
	if(DistSq <= RadSq) return B_True;

	if(Tri.ContainsPoint(Circle.Center))
		return B_True;

	return B_False;
}

// AABB与AABB
IBOOL Intersects(_in FBox2 &Box0, _in FBox2 &Box1){
	if(Box0.Min.x > Box1.Max.x) return B_False;
	if(Box0.Max.x < Box1.Min.x) return B_False;
	if(Box0.Min.y > Box1.Max.y) return B_False;
	if(Box0.Max.y < Box1.Min.y) return B_False;
	return B_True;
}

// AABB与OBB
IBOOL Intersects(_in FBox2 &Box0, _in FObb2 &Box1){
	VFloat2 AxisX = { 1.f, 0.f };
	VFloat2 AxisY = { 0.f, 1.f };

	VFloat2 rgVert0[4], rgVert1[4];

	Box0.GetVertexs(rgVert0);
	Box1.GetVertexs(rgVert1);

	if(PolygonSatTest(rgVert0, 4, rgVert1, 4, AxisX))
		return B_False;
	if(PolygonSatTest(rgVert0, 4, rgVert1, 4, AxisY))
		return B_False;
	if(PolygonSatTest(rgVert0, 4, rgVert1, 4, Box1.AxisX))
		return B_False;
	if(PolygonSatTest(rgVert0, 4, rgVert1, 4, Box1.AxisY))
		return B_False;

	return B_True;
}

// AABB与三角形
IBOOL Intersects(_in FBox2 &Box, _in FTriangle2 &Tri){
	/// 检查AABB的轴

	VFloat2 AxisX = { 1.f, 0.f };
	VFloat2 AxisY = { 0.f, 1.f };

	VFloat2 rgBoxVtx[4];
	Box.GetVertexs(rgBoxVtx);

	if(PolygonSatTest(rgBoxVtx, 4, Tri.Pt, 3, AxisX))
		return B_False;
	if(PolygonSatTest(rgBoxVtx, 4, Tri.Pt, 3, AxisY))
		return B_False;

	/// 检查三角形的轴

	for(UNS32 iPt0 = 0; iPt0 < 3; ++iPt0){
		UNS32 iPt1 = (iPt0 + 1) % 3;

		FVector2 Edge = Tri.Pt[iPt1] - Tri.Pt[iPt0];
		FVector2 Axis = { -Edge.y, Edge.x };
		Axis.Normalize();

		if(PolygonSatTest(rgBoxVtx, 4, Tri.Pt, 3, Axis))
			return B_False;
	}

	/// 不存在分离轴

	return B_True;
}

// OBB与OBB
IBOOL Intersects(_in FObb2 &Box0, _in FObb2 &Box1){
	if(FObb2::IsSeparatingAxis(Box0, Box1, Box0.AxisX))
		return B_False;
	if(FObb2::IsSeparatingAxis(Box0, Box1, Box0.AxisY))
		return B_False;
	if(FObb2::IsSeparatingAxis(Box0, Box1, Box1.AxisX))
		return B_False;
	if(FObb2::IsSeparatingAxis(Box0, Box1, Box1.AxisY))
		return B_False;
	return B_True;
}

// OBB与三角形
IBOOL Intersects(_in FObb2 &Box, _in FTriangle2 &Tri){
	/// 检查OOB的轴

	VFloat2 rgBoxVtx[4];
	Box.GetVertexs(rgBoxVtx);

	if(PolygonSatTest(rgBoxVtx, 4, Tri.Pt, 3, Box.AxisX))
		return B_False;
	if(PolygonSatTest(rgBoxVtx, 4, Tri.Pt, 3, Box.AxisY))
		return B_False;

	/// 检查三角形的轴

	for(UNS32 iPt0 = 0; iPt0 < 3; ++iPt0){
		UNS32 iPt1 = (iPt0 + 1) % 3;

		FVector2 Edge = Tri.Pt[iPt1] - Tri.Pt[iPt0];
		FVector2 Axis = { -Edge.y, Edge.x };
		Axis.Normalize();

		if(PolygonSatTest(rgBoxVtx, 4, Tri.Pt, 3, Axis))
			return B_False;
	}

	/// 不存在分离轴

	return B_True;
}

// 三角形与三角形
IBOOL Intersects(_in FTriangle2 &Tri0, _in FTriangle2 &Tri1){
	return Intersects(Tri0.Pt, 3, Tri1.Pt, 3);
}

// 射线与圆
IBOOL Intersects(_in FRay2 &Ray, _in FCircle2 &Circle, _out SPFPN *pDist = P_Null){
	FVector2 Vco = Circle.Center - Ray.Origin;
	SPFPN Tca = Vco.Dot(Ray.Direction);
	if(Tca < 0.f) return B_False; // 圆在射线后方

	SPFPN DistSq = Vco.LengthSquared() - (Tca * Tca);
	SPFPN RadSq = Circle.Radius * Circle.Radius;
	if(DistSq > RadSq) return B_False; // 最近点距离大于半径

	if(pDist){
		SPFPN Thc = sqrt(RadSq - DistSq);
		*pDist = Tca - Thc; // 最近的交点距离
	}

	return B_True;
}

// 射线与AABB
IBOOL Intersects(_in FRay2 &Ray, _in FBox2 &Box, _out SPFPN *pDist0 = P_Null, _out SPFPN *pDist1 = P_Null){
	SPFPN MinT0, MaxT0;
	SPFPN MinT1, MaxT1;

	VFloat2 BoxMin = Box.Min;
	VFloat2 BoxMax = Box.Max;

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

	if(pDist1)
		*pDist1 = MIN_(MaxT0, MaxT1);
	if(pDist0){
		*pDist0 = MAX_(MinT0, MinT1);
		*pDist0 = MAX_(0.f, *pDist0);
	}

	return B_True;
}

//----------------------------------------//