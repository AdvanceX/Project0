//-------------------- 索引 --------------------//

// Binary Index
typedef USINT INDEX2[2];
// Ternary Index
typedef USINT INDEX3[3];
// Binary Index(UHALF)
typedef UHALF INDEX2H[2];
// Ternary Index(UHALF)
typedef UHALF INDEX3H[3];

//----------------------------------------//


//-------------------- 长宽&矩形区 --------------------//

// Rectangle(USINT)
typedef D2D_RECT_U RECTU;
// Rectangle(SPFPN)
typedef D2D_RECT_F RECTF;

// 2D Size(USINT)
typedef D2D_SIZE_U SIZE2U;
// 2D Size(SPFPN)
typedef D2D_SIZE_F SIZE2F;

//----------------------------------------//


//-------------------- 素材类型 --------------------//

// Asset Type
enum ASSETTYPE{
	ASSET_TYPE_Anim = (1L << 0),     //动画
	ASSET_TYPE_Model = (1L << 1),    //模型
	ASSET_TYPE_Morph = (1L << 2),    //变形
	ASSET_TYPE_Material = (1L << 3), //材质
	ASSET_TYPE_Particle = (1L << 4), //粒子
	ASSET_TYPE_Skeleton = (1L << 5), //骨架
	//////
	ASSET_TYPE_Surface = (1L << 8), //水面
	ASSET_TYPE_Terrain = (1L << 9), //地形
	ASSET_TYPE_Spline = (1L << 10), //样条线
	//////
	ASSET_TYPE_ColorMap = (1L << 16),  //反照率贴图
	ASSET_TYPE_NormMap = (1L << 17),   //法线贴图
	ASSET_TYPE_RoughMap = (1L << 18),  //金属性-粗糙度贴图
	ASSET_TYPE_AlphaMap = (1L << 19),  //透明度贴图
	ASSET_TYPE_EmissMap = (1L << 20),  //自发光贴图
	ASSET_TYPE_OcclusMap = (1L << 21), //环境遮蔽贴图
	ASSET_TYPE_HeightMap = (1L << 22), //高度贴图
};

//----------------------------------------//


//-------------------- 方向&位置 --------------------//

// Common Location
enum COMMLOC{
	LOC_Top = (1L << 0),    //顶部
	LOC_Bottom = (1L << 1), //底部
	LOC_Center = (1L << 2), //中心
	LOC_Left = (1L << 3),   //左端
	LOC_Right = (1L << 4),  //右端
};

// Common Direction
enum COMMDIR{
	DIR_Up = (1L << 0),    //上方
	DIR_Down = (1L << 1),  //下方
	DIR_Left = (1L << 2),  //左方
	DIR_Right = (1L << 3), //右方
	DIR_Front = (1L << 4), //前方
	DIR_Back = (1L << 5),  //后方
};

//----------------------------------------//


//-------------------- 顶点、粒子格式位 --------------------//

// Vertex Format Bit
enum VERTFORMATBIT{
	VFB_Coord = (1L << 0),    //坐标
	VFB_Normal = (1L << 1),   //法线
	VFB_Tangent = (1L << 2),  //切线
	VFB_Color = (1L << 3),    //颜色
	VFB_PSize = (1L << 4),    //粒子尺寸
	VFB_BoneRig = (1L << 5),  //骨骼绑定
	VFB_TexCoord = (1L << 6), //纹理坐标
};

// Particle Format Bit
enum PARTICLEFORMATBIT{
	///可渲染属性
	PFB_Coord = (1L << 0),    //坐标
	PFB_TexCoord = (1L << 1), //纹理坐标
	PFB_Size = (1L << 2),     //尺寸
	PFB_Color32 = (1L << 3),  //32b颜色
	///不可渲染属性
	PFB_Color = (1L << 4),  //颜色
	PFB_Veloc = (1L << 5),  //速度
	PFB_Accel = (1L << 6),  //加速度
	PFB_Age = (1L << 7),    //年龄
	PFB_MaxAge = (1L << 8), //寿命
};

//----------------------------------------//


//-------------------- 方框适应模式 --------------------//

// Box Fit Mode
enum BOXFITMODE{
	BFM_Fill,    //拉伸
	BFM_None,    //居中
	BFM_Cover,   //覆盖
	BFM_Contain, //包含
};

//----------------------------------------//


//-------------------- 灯光&材质基础 --------------------//

// Light
struct LIGHT{
	VECTOR3 Intensity; //发光强度
	SPFPN SpotPower;   //聚光强度(聚光灯)
	VECTOR3 Position;  //光源位置(聚光灯/点光)
	SPFPN AttStart;    //衰减起点(聚光灯/点光)
	VECTOR3 Direction; //光照方向(聚光灯/方向光)
	SPFPN AttEnd;      //衰减终点(聚光灯/点光)
};

// Material Base
struct MATBASE{
	using SLFLAG2 = DWORD[2];
	using SLFLAG3 = DWORD[3];

	VECTOR3 Albedo;   //反照率
	SPFPN Opacity;    //不透明度
	VECTOR3 Emission; //自发光
	SPFPN Roughness;  //粗糙度
	SLFLAG3 TexAttrs; //纹理属性
	SPFPN Metalness;  //金属度
};

// Material Lite
struct MATLITE{
	VECTOR4 Albedo;  //反照率
	ARGB8 Ambient;   //环境光
	ARGB8 Emission;  //自发光
	SPFPN Roughness; //粗糙度
	SPFPN Metalness; //金属度
};

//----------------------------------------//


//-------------------- 材质 --------------------//

// Material Maps
struct MATMAPS{
	USINT iAlbedo;    //反照率贴图
	USINT iNormal;    //法线贴图
	USINT iMetalRg;   //金属性-粗糙度贴图
	USINT iOpacity;   //透明度贴图
	USINT iEmission;  //自发光贴图
	USINT iOcclusion; //环境遮蔽贴图
};

// Material
struct MATERIAL{
	USINT Ident;
	MATBASE Base;
	MATMAPS Maps;
};

//----------------------------------------//


//-------------------- 子网格 --------------------//

// Submesh
struct SUBMESH{
	USINT IdxStart;
	USINT IdxCount;
	USINT MtlIdent;
};

//----------------------------------------//


//-------------------- 关键帧 --------------------//

// Rotation Frame
struct ROTFRAME{
	SPFPN Time;
	VECTOR4 Xform;
};

// Animation Frame
struct ANIMFRAME{
	SPFPN Time;
	VECTOR3 Xform;
};

// Morph Frame
struct MORPHFRAME{
	SPFPN Time;
	USINT iTarget;
};

//----------------------------------------//


//-------------------- 颜色 --------------------//

//24b Color
struct COLOR24{
	union{
		UTINY c[3];
		struct{ UTINY b, g, r; };
	};

	COLOR24() = default;
	COLOR24(ARGB8 Value){
		$m.r = R_CHANNEL(Value);
		$m.g = G_CHANNEL(Value);
		$m.b = B_CHANNEL(Value);
	}
	COLOR24(USINT R, USINT G, USINT B){
		CAST_($m.r, R);
		CAST_($m.g, G);
		CAST_($m.b, B);
	}
	explicit COLOR24(_in USINT psValue[3]){
		CAST_($m.r, psValue[0]);
		CAST_($m.g, psValue[1]);
		CAST_($m.b, psValue[2]);
	}
};

//32b Color
struct COLOR32: dxpv::XMCOLOR{
	COLOR32() = default;
	COLOR32(ARGB8 Value): XMCOLOR(Value){}
	COLOR32(USINT R, USINT G, USINT B, USINT A){
		CAST_($m.r, R);
		CAST_($m.g, G);
		CAST_($m.b, B);
		CAST_($m.a, A);
	}
	COLOR32(SPFPN R, SPFPN G, SPFPN B, SPFPN A): XMCOLOR(R, G, B, A){}
	explicit COLOR32(_in SPFPN psValue[4]): XMCOLOR(psValue){}
};

//----------------------------------------//


//-------------------- 曲线 --------------------//

// Curve
struct CURVE{
	VECTR3X Ctrl0;
	VECTR3X Ctrl1;
	VECTR3X Ctrl2;
	VECTR3X Ctrl3;

	SPFPN ClacLength(){
		SPFPN Len01 = VECTR3X::Distance($m.Ctrl1, $m.Ctrl0);
		SPFPN Len12 = VECTR3X::Distance($m.Ctrl2, $m.Ctrl1);
		SPFPN Len23 = VECTR3X::Distance($m.Ctrl3, $m.Ctrl2);
		SPFPN Len03 = VECTR3X::Distance($m.Ctrl3, $m.Ctrl0);

		return (Len01 + Len12 + Len23 + Len03) * 0.5f;
	}
	VECTR3X Sample(SPFPN Factor){
		// P(t) = P0×(1−t)³ + P1×3t(1−t)² + P2×3t²(1−t) + P3×t³

		SPFPN T1 = Factor;
		SPFPN T2 = T1 * T1;
		SPFPN T3 = T2 * T1;
		SPFPN S1 = 1.f - T1;
		SPFPN S2 = S1 * S1;
		SPFPN S3 = S2 * S1;

		VECTR3X P0 = $m.Ctrl0 * S3;
		VECTR3X P1 = $m.Ctrl1 * S2 * T1 * 3.f;
		VECTR3X P2 = $m.Ctrl2 * S1 * T2 * 3.f;
		VECTR3X P3 = $m.Ctrl3 * T3;

		return P0 + P1 + P2 + P3;
	}
	VECTR3X Derivative(SPFPN Factor){
		// P'(t) = P0×(−3)(1−t)² + P1×(3(1−t)² − 6t(1−t)) + P2×(6t(1−t) − 3t²) + P3×3t²

		SPFPN T1 = Factor;
		SPFPN T2 = T1 * T1;
		SPFPN S1 = 1.f - T1;
		SPFPN S2 = S1 * S1;

		SPFPN B0 = S2 * -3.f;
		SPFPN B1 = S2*3.f - T1*S1*6.f;
		SPFPN B2 = T1*S1*6.f - T2*3.f;
		SPFPN B3 = T2 * 3.f;

		VECTR3X V0 = $m.Ctrl0 * B0;
		VECTR3X V1 = $m.Ctrl1 * B1;
		VECTR3X V2 = $m.Ctrl2 * B2;
		VECTR3X V3 = $m.Ctrl3 * B3;

		return V0 + V1 + V2 + V3;
	}
};

//----------------------------------------//


//-------------------- 骨骼 --------------------//

// Bone
struct alignas(16) BONE{
	USINT Ident; //标识
	USINT iParent; //父节点
	QWORD vPadding; //填充位
	MMATRIX matLocal;   //局部矩阵
	MMATRIX matOffset;  //偏移矩阵
	MMATRIX matInitial; //初始矩阵
	MMATRIX matCombine; //组合矩阵

	PVOID operator new[](UPINT Count){
		return AlignedMalloc(Count * sizeof(BONE), 16);
	}
	$VOID operator delete[](PVOID pTarget){
		AlignedMfree(pTarget);
	}
};

//----------------------------------------//


//-------------------- 相机 --------------------//

// Camera
class alignas(16) CCamera{
	_secr MVECTOR Front; //前方向
	_secr MVECTOR UpDir; //上方向
	_secr MVECTOR Right; //右方向
	_secr MVECTOR Position; //位置
	_secr MMATRIX matView; //观察矩阵
	_secr MMATRIX matProj; //投影矩阵

	_open $VOID _MM_CALL Reset(FMVECTOR Position){
		$m.Position = Position;
		$m.Front = dx::g_XMIdentityR2;
		$m.UpDir = dx::g_XMIdentityR1;
		$m.Right = dx::g_XMIdentityR0;
	}
	_open $VOID _MM_CALL Locate(FMVECTOR Position){
		$m.Position = Position;
	}
	_open $VOID _MM_CALL LookTo(FMVECTOR Front, FMVECTOR UpDir){
		$m.Front = Front;
		$m.Right = Vec3_Cross(UpDir, $m.Front);
		$m.Right = Vec3_Normalize($m.Right);
		$m.UpDir = Vec3_Cross($m.Front, $m.Right);
	}
	_open $VOID _MM_CALL LookAt(FMVECTOR Target, FMVECTOR UpDir){
		$m.Front = Vec_Sub(Target, $m.Position);
		$m.Front = Vec3_Normalize($m.Front);
		$m.Right = Vec3_Cross(UpDir, $m.Front);
		$m.Right = Vec3_Normalize($m.Right);
		$m.UpDir = Vec3_Cross($m.Front, $m.Right);
	}
	_open $VOID Perspective(SPFPN FovAngle, SPFPN AspectRatio, SPFPN NearZ, SPFPN FarZ){
		$m.matProj = Mat_Perspect(FovAngle / AspectRatio, AspectRatio, NearZ, FarZ);
	}
	_open $VOID Orthographic(SPFPN ViewWidth, SPFPN ViewHeight, SPFPN NearZ, SPFPN FarZ){
		$m.matProj = Mat_Ortho(ViewWidth, ViewHeight, NearZ, FarZ);
	}
	_open $VOID UpdateView(IBOOL bNormalize){
		if(bNormalize){
			$m.Front = Vec3_Normalize($m.Front);
			$m.UpDir = Vec3_Cross($m.Front, $m.Right);
			$m.UpDir = Vec3_Normalize($m.UpDir);
			$m.Right = Vec3_Cross($m.UpDir, $m.Front);
		}
		$m.matView = Mat_LookTo($m.Position, $m.Front, $m.UpDir);
	}
	///移动
	_open $VOID Fly(SPFPN Dist){
		using namespace dx;
		$m.Position += $m.UpDir * Dist;
	}
	_open $VOID Walk(SPFPN Dist){
		using namespace dx;
		$m.Position += $m.Front * Dist;
	}
	_open $VOID Strafe(SPFPN Dist){
		using namespace dx;
		$m.Position += $m.Right * Dist;
	}
	_open $VOID Yaw(SPFPN Angle){
		MMATRIX matXform = Mat_RotateAxis($m.UpDir, Angle);
		$m.Right = Vec3_XformNormal($m.Right, matXform);
		$m.Front = Vec3_XformNormal($m.Front, matXform);
	}
	_open $VOID Roll(SPFPN Angle){
		MMATRIX matXform = Mat_RotateAxis($m.Front, Angle);
		$m.Right = Vec3_XformNormal($m.Right, matXform);
		$m.UpDir = Vec3_XformNormal($m.UpDir, matXform);
	}
	_open $VOID Pitch(SPFPN Angle){
		MMATRIX matXform = Mat_RotateAxis($m.Right, Angle);
		$m.UpDir = Vec3_XformNormal($m.UpDir, matXform);
		$m.Front = Vec3_XformNormal($m.Front, matXform);
	}
	_open $VOID HorizontalYaw(SPFPN Angle){
		MMATRIX matXform = Mat_RotateAxis(dx::g_XMIdentityR1, Angle);
		$m.Front = Vec3_XformNormal($m.Front, matXform);
		$m.UpDir = Vec3_XformNormal($m.UpDir, matXform);
		$m.Right = Vec3_XformNormal($m.Right, matXform);
	}
	_open $VOID HorizontalWalk(SPFPN Dist){
		using namespace dx;
		MVECTOR Direction;
		Direction = Vec_SetY($m.Front, 0.f);
		Direction = Vec3_Normalize(Direction);
		$m.Position += Direction * Dist;
	}
	///访问
	_open $VOID GetPosition(_out VECTOR3 &rResult){
		Upk_V3(&rResult, $m.Position);
	}
	_open $VOID GetViewMatrix(_out MATRIX4 &rResult){
		Upk_Mat(&rResult, Mat_Transpose($m.matView));
	}
	_open $VOID GetProjMatrix(_out MATRIX4 &rResult){
		Upk_Mat(&rResult, Mat_Transpose($m.matProj));
	}
	_open $VOID GetViewProjMat(_out MATRIX4 &rResult){
		Upk_Mat(&rResult, Mat_Transpose($m.matView * $m.matProj));
	}
	_open $VOID GetViewProjTexMat(_out MATRIX4 &rResult){
		Upk_Mat(&rResult, Mat_Transpose($m.GetViewProjTexMat()));
	}
	_open const MVECTOR &GetPosition(){
		return $m.Position;
	}
	_open const MMATRIX &GetViewMatrix(){
		return $m.matView;
	}
	_open const MMATRIX &GetProjMatrix(){
		return $m.matProj;
	}
	_open MMATRIX GetViewProjMat(){
		return $m.matView * $m.matProj;
	}
	_open MMATRIX GetViewProjTexMat(){
		MMATRIX matViewProj = $m.matView * $m.matProj;
		MMATRIX matTexture(
			0.5f, 0.f, 0.f, 0.f, 0.f, -0.5f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f, 0.5f, 0.5f, 0.f, 1.f);

		return matViewProj * matTexture;
	}
	_open SPFPN GetPositionX(){
		return Vec_GetX($m.Position);
	}
	_open SPFPN GetPositionY(){
		return Vec_GetY($m.Position);
	}
	_open SPFPN GetPositionZ(){
		return Vec_GetZ($m.Position);
	}
	//////
	_open PVOID operator new[](UPINT Count){
		return AlignedMalloc(Count * sizeof(CCamera), 16);
	}
	_open $VOID operator delete[](PVOID pTarget){
		AlignedMfree(pTarget);
	}
};

//----------------------------------------//


//-------------------- 顶点、粒子视图 --------------------//

// Vertex View
class CVtxView{
	_secr BYTET *psVertex; //顶点指针
	_secr USINT cbVertex;  //顶点字节数
	_secr UTINY hCoord;       //坐标句柄
	_secr UTINY hNormal;      //法线句柄
	_secr UTINY hTangent;     //切线句柄
	_secr UTINY hColor;       //颜色句柄
	_secr UTINY hPointSize;   //粒子尺寸句柄
	_secr UTINY hBoneIndices; //骨骼索引句柄
	_secr UTINY hBoneWeights; //骨骼权重句柄
	_secr UTINY hTexCoord;    //纹理坐标句柄

	_open CVtxView() = default;
	_open CVtxView(DWORD Format){
		$m.Initialize(Format);
	}
	//////
	_open $VOID Initialize(DWORD Format){
		Var_Zero(this);

		if(Format & VFB_Coord){
			CAST_($m.hCoord, $m.cbVertex);
			$m.cbVertex += sizeof(VECTOR3);
		}
		if(Format & VFB_Normal){
			CAST_($m.hNormal, $m.cbVertex);
			$m.cbVertex += sizeof(VECTOR3);
		}
		if(Format & VFB_Tangent){
			CAST_($m.hTangent, $m.cbVertex);
			$m.cbVertex += sizeof(VECTOR3);
		}
		if(Format & VFB_Color){
			CAST_($m.hColor, $m.cbVertex);
			$m.cbVertex += sizeof(COLOR32);
		}
		if(Format & VFB_PSize){
			CAST_($m.hPointSize, $m.cbVertex);
			$m.cbVertex += sizeof(SIZE2F);
		}
		if(Format & VFB_BoneRig){
			CAST_($m.hBoneIndices, $m.cbVertex);
			$m.cbVertex += sizeof(UTINY[4]);
			CAST_($m.hBoneWeights, $m.cbVertex);
			$m.cbVertex += sizeof(SPFPN[3]);
		}
		if(Format & VFB_TexCoord){
			CAST_($m.hTexCoord, $m.cbVertex);
			$m.cbVertex += sizeof(VECTOR2);
		}

		for(USINT iTex = 1; Format & (VFB_TexCoord << iTex); ++iTex)
			$m.cbVertex += sizeof(VECTOR2);
	}
	_open $VOID NextTarget(INT32 Offset){
		$m.psVertex += $m.cbVertex * Offset;
	}
	_open $VOID SetTarget(PVOID psVertex){
		$m.psVertex = (BYTET*)psVertex;
	}
	_open USINT GetDataSize(){
		return $m.cbVertex;
	}
	///属性访问
	_open UTINY *BoneIndices(){
		return (UTINY*)($m.psVertex + $m.hBoneIndices);
	}
	_open SPFPN *BoneWeights(){
		return (SPFPN*)($m.psVertex + $m.hBoneWeights);
	}
	_open SIZE2F &PointSize(){
		return *(SIZE2F*)($m.psVertex + $m.hPointSize);
	}
	_open COLOR32 &Color(){
		return *(COLOR32*)($m.psVertex + $m.hColor);
	}
	_open VECTR3X &Coord(){
		return *(VECTR3X*)($m.psVertex + $m.hCoord);
	}
	_open VECTR3X &Normal(){
		return *(VECTR3X*)($m.psVertex + $m.hNormal);
	}
	_open VECTR3X &Tangent(){
		return *(VECTR3X*)($m.psVertex + $m.hTangent);
	}
	_open VECTR2X &TexCoord(USINT Layer = 0){
		BYTET *pProp = $m.psVertex + $m.hTexCoord;
		return *((VECTR2X*)pProp + Layer);
	}
};

// Particle View
class CPtcView{
	_secr USINT *pCount;  //计数指针
	_secr BYTET *psPrtcl; //粒子指针
	_secr USINT cbPrtcl;  //粒子字节数
	_secr UTINY hCoord;    //坐标句柄
	_secr UTINY hTexCoord; //纹理坐标句柄
	_secr UTINY hSize;     //尺寸句柄
	_secr UTINY hColor32;  //32b颜色句柄
	_secr UTINY hColor;    //颜色句柄
	_secr UTINY hVeloc;    //速度句柄
	_secr UTINY hAccel;    //加速度句柄
	_secr UTINY hAge;      //年龄句柄
	_secr UTINY hMaxAge;   //寿命句柄

	_open CPtcView() = default;
	_open CPtcView(DWORD Format){
		$m.Initialize(Format);
	}
	//////
	_open $VOID Initialize(DWORD Format){
		Var_Zero(this);

		if(Format & PFB_Coord){
			CAST_($m.hCoord, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(VECTOR3);
		}
		if(Format & PFB_TexCoord){
			CAST_($m.hTexCoord, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(VECTOR2);
		}
		if(Format & PFB_Size){
			CAST_($m.hSize, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(SIZE2F);
		}
		if(Format & PFB_Color32){
			CAST_($m.hColor32, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(COLOR32);
		}
		if(Format & PFB_Color){
			CAST_($m.hColor, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(COLOR128);
		}
		if(Format & PFB_Veloc){
			CAST_($m.hVeloc, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(VECTOR3);
		}
		if(Format & PFB_Accel){
			CAST_($m.hAccel, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(VECTOR3);
		}
		if(Format & PFB_Age){
			CAST_($m.hAge, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(SPFPN);
		}
		if(Format & PFB_MaxAge){
			CAST_($m.hMaxAge, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(SPFPN);
		}
	}
	_open $VOID NextTarget(INT32 Offset){
		$m.psPrtcl += $m.cbPrtcl * Offset;
	}
	_open $VOID SetTarget(PVOID psPrtcl){
		$m.psPrtcl = (BYTET*)psPrtcl;
	}
	_open $VOID SetCount(USINT *_in pCount){
		$m.pCount = pCount;
	}
	_open $VOID ReduceCount(){
		*$m.pCount -= 1;
	}
	_open USINT GetDataSize(){
		return $m.cbPrtcl;
	}
	_open IBOOL IsActive(){
		SPFPN Age = *(SPFPN*)($m.psPrtcl + $m.hAge);
		SPFPN MaxAge = *(SPFPN*)($m.psPrtcl + $m.hMaxAge);
		return Age <= MaxAge;
	}
	///属性访问
	_open SPFPN &Age(){
		return *(SPFPN*)($m.psPrtcl + $m.hAge);
	}
	_open SPFPN &MaxAge(){
		return *(SPFPN*)($m.psPrtcl + $m.hMaxAge);
	}
	_open SIZE2F &Size(){
		return *(SIZE2F*)($m.psPrtcl + $m.hSize);
	}
	_open VECTR3X &Veloc(){
		return *(VECTR3X*)($m.psPrtcl + $m.hVeloc);
	}
	_open VECTR3X &Accel(){
		return *(VECTR3X*)($m.psPrtcl + $m.hAccel);
	}
	_open VECTR3X &Coord(){
		return *(VECTR3X*)($m.psPrtcl + $m.hCoord);
	}
	_open VECTR2X &TexCoord(){
		return *(VECTR2X*)($m.psPrtcl + $m.hTexCoord);
	}
	_open COLOR32 &Color32(){
		return *(COLOR32*)($m.psPrtcl + $m.hColor32);
	}
	_open COLOR128 &Color(){
		return *(COLOR128*)($m.psPrtcl + $m.hColor);
	}
	///子属性访问
	_open SPFPN *SizePart(USINT Index){
		BYTET *pProp = $m.psPrtcl + $m.hSize;
		return (SPFPN*)pProp + Index;
	}
	_open SPFPN *ColorPart(USINT Index){
		BYTET *pProp = $m.psPrtcl + $m.hColor;
		return (SPFPN*)pProp + Index;
	}
	_open SPFPN *CoordPart(USINT Index){
		BYTET *pProp = $m.psPrtcl + $m.hCoord;
		return (SPFPN*)pProp + Index;
	}
	_open SPFPN *VelocPart(USINT Index){
		BYTET *pProp = $m.psPrtcl + $m.hVeloc;
		return (SPFPN*)pProp + Index;
	}
	_open SPFPN *AccelPart(USINT Index){
		BYTET *pProp = $m.psPrtcl + $m.hAccel;
		return (SPFPN*)pProp + Index;
	}
	_open SPFPN *TexCoorPart(USINT Index){
		BYTET *pProp = $m.psPrtcl + $m.hTexCoord;
		return (SPFPN*)pProp + Index;
	}
};

//----------------------------------------//


//-------------------- #变形动画# --------------------//

////变形动画
//class CMorphAnim{
//	_open SPFPN Duration;
//	_open USINT numFrame;
//	_open MORPHFRAME *arrFrame;
//
//	_open ~CMorphAnim(){
//		$m.Finalize();
//	}
//	_open CMorphAnim(){
//		Var_Zero(this);
//	}
//	_open $VOID Finalize(){
//		SAFE_DELETEA($m.arrFrame);
//	}
//	_open $VOID Interpolate(SPFPN Time, USINT Targets[2], SPFPN &Lerp){
//		USINT iEndFrm = $m.numFrame - 1;
//		MORPHFRAME *Frames = $m.arrFrame;
//
//		if(Time <= Frames[0].Time){
//			Targets[0] = Targets[1] = Frames[0].iTarget;
//		} else if(Time >= Frames[iEndFrm].Time){
//			Targets[0] = Targets[1] = Frames[iEndFrm].iTarget;
//		} else{
//			MORPHFRAME *pFrm0 = &Frames[0];
//			MORPHFRAME *pFrm1 = &Frames[1];
//
//			for(USINT iExe = 1; iExe < $m.numFrame; ++iExe){
//				if((Time >= pFrm0->Time) && (Time <= pFrm1->Time)){
//					Lerp = (Time - pFrm0->Time) / (pFrm1->Time - pFrm0->Time);
//					Targets[0] = pFrm0->iTarget;
//					Targets[1] = pFrm1->iTarget;
//					break;
//				}
//				pFrm0++, pFrm1++;
//			}
//		}
//	}
//};

//----------------------------------------//


//-------------------- #柔体# --------------------//

////质点
//struct XMassPoint{
//	SPFPN Mass;  //质量
//	SPFPN MassR; //质量倒数
//	MVECTOR Force; //受力
//	MVECTOR Veloc; //速度
//	MVECTOR Origin;   //初始位置
//	MVECTOR Position; //当前位置
//};
//
////弹簧
//struct XSpring{
//	USINT P0, P1;
//	SPFPN Ks, Kd;
//	SPFPN Length;
//	XSpring *Next;
//
//	XSpring(){
//		Var_Zero(this);
//	}
//};
//
////柔体
//class CSoftBody{
//	USINT numFace;
//	USINT numPoint;
//	USINT numSpring;
//	INDEX3 *arrFace;
//	XSpring *TopSpring;
//	XMassPoint *arrPoint;
//
//	~CSoftBody(){
//		$m.Finalize();
//	}
//	CSoftBody(){
//		Var_Zero(this);
//	}
//	CSoftBody(BYTET *VtxBuff, BYTET *IdxBuff, USINT cbVertex, USINT VtxCount, USINT IdxCount, SPFPN Mass, SPFPN Ks, SPFPN Kd){
//		$m.Initialize(VtxBuff, IdxBuff, cbVertex, VtxCount, IdxCount, Mass, Ks, Kd);
//	}
//
//	$VOID Initialize(BYTET *VtxBuff, BYTET *IdxBuff, USINT cbVertex, USINT VtxCount, USINT IdxCount, SPFPN Mass, SPFPN Ks, SPFPN Kd){
//		//SPFPN MassR;
//		//BYTET *pByte;
//		//UHALF P0, P1, P2;
//		//MVECTOR Coord;
//		//VECTOR3 *pCoord;
//
//		////创建表面
//
//		//$m.numFace = IdxCount / 3;
//		//$m.arrFace = new INDEX3[$m.numFace];
//		//BStr_Move($m.arrFace, IdxBuff, sizeof(INDEX3) * $m.numFace);
//
//		////创建质点
//
//		//MassR = 1.f / Mass;
//		//pByte = VtxBuff;
//
//		//$m.numPoint = VtxCount;
//		//$m.arrPoint = new XMassPoint[$m.numPoint];
//
//		//for(USINT iExe = 0; iExe < $m.numPoint; ++iExe){
//		//	pCoord = (VECTOR3*)pByte;
//		//	Coord = Pck_V3(*pCoord);
//		//	pByte += cbVertex;
//
//		//	$m.arrPoint[iExe].MassR = MassR;
//		//	$m.arrPoint[iExe].Mass = Mass;
//		//	$m.arrPoint[iExe].Origin = Coord;
//		//	$m.arrPoint[iExe].Position = Coord;
//		//	$m.arrPoint[iExe].Veloc = dx::g_XMZero;
//		//	$m.arrPoint[iExe].Force = dx::g_XMZero;
//		//}
//
//		////创建弹簧
//
//		//for(USINT iExe = 0; iExe < $m.numFace; ++iExe){
//		//	P0 = $m.arrFace[iExe][0];
//		//	P1 = $m.arrFace[iExe][1];
//		//	P2 = $m.arrFace[iExe][2];
//
//		//	$m.AddSpring(P0, P1, Ks, Kd);
//		//	$m.AddSpring(P1, P2, Ks, Kd);
//		//	$m.AddSpring(P2, P0, Ks, Kd);
//		//}
//	}
//	$VOID AddSpring(USINT P0, USINT P1, SPFPN Ks, SPFPN Kd){
//		if(P0 == P1) return;
//
//		XSpring *pSpring;
//
//		for(pSpring = $m.TopSpring; pSpring; pSpring = pSpring->Next){
//			if(P0 == pSpring->P0 && P1 == pSpring->P1) return;
//			if(P0 == pSpring->P1 && P1 == pSpring->P0) return;
//		}
//
//		pSpring = new XSpring;
//		pSpring->Next = $m.TopSpring;
//		$m.TopSpring = pSpring;
//		$m.numSpring++;
//
//		pSpring->P0 = (UHALF)P0;
//		pSpring->P1 = (UHALF)P1;
//		pSpring->Ks = Ks;
//		pSpring->Kd = Kd;
//
//		MVECTOR &Vector0 = $m.arrPoint[P0].Position;
//		MVECTOR &Vector1 = $m.arrPoint[P1].Position;
//		pSpring->Length = Vec3_Length(Vector1 - Vector0);
//	}
//	$VOID SetMass(USINT Index, SPFPN Mass){
//		if(Index < $m.numPoint){
//			$m.arrPoint[Index].Mass = Mass;
//			$m.arrPoint[Index].MassR = (Mass == 0.f) ? 0.f : (1.f / Mass);
//		}
//	}
//	$VOID Finalize(){
//		SAFE_DELETEA($m.arrFace);
//		SAFE_DELETEA($m.arrPoint);
//	}
//	$VOID Reset(){
//		XMassPoint *pPoint = $m.arrPoint;
//		for(USINT iExe = 0; iExe < $m.numPoint; ++iExe){
//			pPoint->Position = pPoint->Origin;
//			pPoint->Veloc = { 0.f, 0.f, 0.f };
//			pPoint++;
//		}
//	}
//	$VOID ProcForces(SPFPN TimeSpan){
//		using namespace dx;
//
//		for(USINT iExe = 0; iExe < $m.numPoint; ++iExe){
//			XMassPoint *pPoint = &$m.arrPoint[iExe];
//			if(pPoint->Mass == 0.f) continue;
//
//			pPoint->Veloc += pPoint->Force * pPoint->MassR * TimeSpan;
//			pPoint->Position += pPoint->Veloc * TimeSpan;
//		}
//	}
//	$VOID Revert(SPFPN Stiffness, MMATRIX* pXform){
//		using namespace dx;
//
//		XMassPoint *pPoint;
//		MVECTOR Origin, Vector;
//
//		for(USINT iExe = 0; iExe < $m.numPoint; ++iExe){
//			pPoint = &$m.arrPoint[iExe];
//			if(pPoint->Mass == 0.f) continue;
//
//			Origin = pPoint->Origin;
//			if(pXform) Origin =
//				Vec3_XformCoord(Origin, *pXform);
//
//			Vector = Vec_Sub(Origin, pPoint->Position);
//			Vector *= Stiffness;
//
//			pPoint->Veloc += Vector;
//			pPoint->Position += Vector;
//		}
//	}
//	$VOID SetForces(SPFPN Damping, MVECTOR *pGravity, MVECTOR *pWind, MMATRIX *pXform, IBOOL bAffectAll){
//		using namespace dx;
//
//		//计算重力,阻力作用
//
//		for(USINT iExe = 0; iExe < $m.numPoint; ++iExe){
//			XMassPoint *pPoint = &$m.arrPoint[iExe];
//
//			if(pXform && (bAffectAll || (pPoint->Mass == 0.f)))
//				pPoint->Position = Vec3_XformCoord(pPoint->Origin, *pXform);
//
//			if(pPoint->Mass != 0.f){
//				if(pGravity) pPoint->Force = *pGravity * pPoint->Mass;
//				else pPoint->Force = g_XMZero;
//				pPoint->Force += pPoint->Veloc * Damping;
//			}
//		}
//
//		//计算风力作用
//
//		if(pWind){
//			for(USINT iExe = 0; iExe < $m.numFace; ++iExe){
//				XMassPoint *P0 = &$m.arrPoint[$m.arrFace[iExe][0]];
//				XMassPoint *P1 = &$m.arrPoint[$m.arrFace[iExe][1]];
//				XMassPoint *P2 = &$m.arrPoint[$m.arrFace[iExe][2]];
//
//				MVECTOR Vector0 = Vec_Sub(P1->Position, P0->Position);
//				MVECTOR Vector1 = Vec_Sub(P2->Position, P0->Position);
//				MVECTOR normal = Vec3_Cross(Vector0, Vector1);
//				normal = Vec3_Normalize(normal);
//
//				SPFPN dot = Vec3_Dot(normal, *pWind);
//				MVECTOR Force = normal * dot;
//
//				P0->Force += Force;
//				P1->Force += Force;
//				P2->Force += Force;
//			}
//		}
//
//		//计算弹力作用
//
//		for(XSpring *pSpring = $m.TopSpring; pSpring; pSpring = pSpring->Next){
//			XMassPoint *P0 = &$m.arrPoint[pSpring->P0];
//			XMassPoint *P1 = &$m.arrPoint[pSpring->P1];
//
//			MVECTOR Vector = Vec_Sub(P1->Position, P0->Position);
//			SPFPN Length = Vec3_Length(Vector);
//			Vector /= Length;
//
//			MVECTOR Veloc = Vec_Sub(P1->Veloc, P0->Veloc);
//			SPFPN speed = Vec3_Dot(Veloc, Vector);
//
//			SPFPN fs = pSpring->Ks * (Length - pSpring->Length);
//			SPFPN fd = pSpring->Kd * speed;
//
//			MVECTOR Force = (fs + fd) * Vector;
//
//			if(P0->Mass != 0.f) P0->Force += Force;
//			if(P1->Mass != 0.f) P1->Force -= Force;
//		}
//	}
//	$VOID Update(SPFPN TimeSpan, SPFPN Damping, MVECTOR* pGravity, MVECTOR *pWind, MMATRIX *pXform, IBOOL bAffectAll){
//		using namespace dx;
//
//		MMATRIX matFrame, *pFrameMat;
//		SPFPN TimeSlice, TimeRemain = TimeSpan;
//
//		if(pXform) pFrameMat = &matFrame;
//		else pFrameMat = P_Null;
//
//		while(TimeRemain > 0.f){
//			TimeSlice = MIN_(TimeRemain, 0.01f);
//			TimeRemain -= TimeSlice;
//
//			if(pFrameMat) matFrame =
//				*pXform * ((TimeSpan - TimeRemain) / TimeSpan);
//
//			$m.SetForces(Damping, pGravity, pWind, pFrameMat, bAffectAll);
//			$m.ProcForces(TimeSlice);
//		}
//	}
//};

//----------------------------------------//


//-------------------- #粒子驱动# --------------------//

////粒子驱动
//class CPtcDriver{
//	_open enum TYPE{
//		TYPE_TIME,
//		TYPE_MaxTIME,
//		TYPE_SIZE,
//		TYPE_VELOC,
//		TYPE_ACCEL,
//		TYPE_COLOR,
//		TYPE_TEXCOORD,
//		TYPE_SUBPROP,
//		TYPE_SIZE_X,
//		TYPE_SIZE_Y,
//		TYPE_VELOC_X,
//		TYPE_VELOC_Y,
//		TYPE_VELOC_Z,
//		TYPE_ACCEL_X,
//		TYPE_ACCEL_Y,
//		TYPE_ACCEL_Z,
//		TYPE_COLOR_R,
//		TYPE_COLOR_G,
//		TYPE_COLOR_B,
//		TYPE_COLOR_A,
//		TYPE_TEXCOORD_U,
//		TYPE_TEXCOORD_V,
//	};
//
//	_open IBOOL bFade; //渐变
//	_open TYPE Type; //类型
//	_open SPFPN Time; //时间
//	_open SPFPN Times[2]; //时间范围
//	_open CPtcDriver *Next; //下一个
//
//	_open virtual $VOID ParseContent(_in CHAR8* Content) = 0;
//	_open virtual $VOID SetChange(CPtcView &PtcView) = 0;
//	_open virtual $VOID SetupFading(CPtcView &PtcView, SPFPN TimeDiff) = 0;
//	_open virtual $VOID UpdateFading(CPtcView &PtcView, SPFPN TimeSlice) = 0;
//	_open virtual $VOID UpdateParticle(CPtcView &PtcView, SPFPN TimeSpan){
//		SPFPN PrevTime = PtcView.Age();
//		SPFPN LastTime = PtcView.Age() + TimeSpan;
//
//		//时间未落在本片段则给下一个驱动处理
//
//		if($m.Next && (PrevTime >= $m.Next->Time)){
//			$m.Next->UpdateParticle(PtcView, TimeSpan);
//			return;
//		}
//
//		//检查是否有渐变和分片处理
//
//		IBOOL bFadeOut = $m.Next && $m.Next->bFade;
//		IBOOL bInBatch = $m.Next && (LastTime > $m.Next->Time);
//
//		//首次进入该驱动的粒子需要初始化
//
//		if(PrevTime == $m.Time){
//			if(!$m.bFade)
//				$m.SetChange(PtcView);
//			if(bFadeOut){
//				SPFPN EndTime = ($m.Next->Time != FLT_MAX) ?
//					$m.Next->Time : PtcView.MaxAge();
//				SPFPN TimeDiff = EndTime - $m.Time;
//
//				$m.SetupFading(PtcView, TimeDiff);
//			}
//		}
//
//		//计算时间切片
//
//		SPFPN TimeSlice = bInBatch ?
//			($m.Next->Time - PrevTime) : TimeSpan;
//
//		//更新粒子位置
//
//		if($m.Type == TYPE_VELOC)
//			$m.UpdatePosition(PtcView, TimeSlice);
//
//		//如果有渐变则需要维护
//
//		if(bFadeOut)
//			$m.UpdateFading(PtcView, TimeSlice);
//
//		//如果有分片则需要继续处理
//
//		if(!bInBatch){
//			PtcView.Age() = LastTime;
//		} else{
//			PtcView.Age() = $m.Next->Time;
//			TimeSpan = LastTime - PtcView.Age();
//			$m.Next->UpdateParticle(PtcView, TimeSpan);
//		}
//	}
//	_open virtual $VOID UpdatePosition(CPtcView &PtcView, SPFPN TimeSlice){
//		MVECTOR VecStep = Pck_V3(PtcView.Veloc());
//		MVECTOR Vector = Pck_V3(PtcView.Coord()) + (VecStep * TimeSlice);
//		Upk_V3(PtcView.Coord(), Vector);
//	}
//
//	_open static INT32 Compare(_in $VOID* pElem0, _in $VOID* pElem1){
//		CPtcDriver *pDriver0 = *(CPtcDriver**)pElem0;
//		CPtcDriver *pDriver1 = *(CPtcDriver**)pElem1;
//
//		if(pDriver0->Type != pDriver1->Type)
//			return (INT32)pDriver0->Type - (INT32)pDriver1->Type;
//
//		if(pDriver0->Time < pDriver1->Time) return -1;
//		else if(pDriver0->Time == pDriver1->Time) return 0;
//		else return 1;
//	}
//	_open static CHAR8* ParseNumber(_in CHAR8* Content, SPFPN Output[2]){
//		CHAR8 Buffer[32];
//		CHAR8 *pSrcChar, *pDstChar;
//
//		pDstChar = &Buffer[0];
//		pSrcChar = (CHAR8*)Content;
//
//		if(!IsAlpha(*pSrcChar)){
//			while((*pSrcChar == '-') || (*pSrcChar == '.') || IsDigit(*pSrcChar))
//				*pDstChar++ = *pSrcChar++;
//			*pDstChar = '\0';
//
//			Output[0] = (SPFPN)MbsToF(Buffer, P_Null);
//			Output[1] = Output[0];
//
//			return pSrcChar + 1;
//		} else{
//			pSrcChar += sizeof("rand");
//			pDstChar = &Buffer[0];
//
//			while((*pSrcChar == '-') || (*pSrcChar == '.') || IsDigit(*pSrcChar))
//				*pDstChar++ = *pSrcChar++;
//			*pDstChar = '\0';
//			Output[0] = (SPFPN)MbsToF(Buffer, P_Null);
//
//			pSrcChar++;
//			pDstChar = &Buffer[0];
//
//			while((*pSrcChar == '-') || (*pSrcChar == '.') ||
//				IsDigit(*pSrcChar)) *pDstChar++ = *pSrcChar++;
//			*pDstChar = '\0';
//			Output[1] = (SPFPN)MbsToF(Buffer, P_Null);
//
//			return pSrcChar + 2;
//		}
//	}
//	_open static $VOID ParseColor(_in CHAR8* Content, COLOR128 Output[2]){
//		SPFPN R[2], G[2], B[2], A[2];
//
//		Content = ParseNumber(Content, R);
//		Content = ParseNumber(Content, G);
//		Content = ParseNumber(Content, B);
//		Content = ParseNumber(Content, A);
//
//		Output[0] = { R[0], G[0], B[0], A[0] };
//		Output[1] = { R[1], G[1], B[1], A[1] };
//	}
//	_open static $VOID ParseVector2(_in CHAR8* Content, VECTR2X Output[2]){
//		SPFPN X[2], Y[2];
//
//		Content = ParseNumber(Content, X);
//		Content = ParseNumber(Content, Y);
//
//		Output[0] = { X[0], Y[0] };
//		Output[1] = { X[1], Y[1] };
//	}
//	_open static $VOID ParseVector3(_in CHAR8* Content, VECTOR3 Output[2]){
//		SPFPN X[2], Y[2], Z[2];
//
//		Content = ParseNumber(Content, X);
//		Content = ParseNumber(Content, Y);
//		Content = ParseNumber(Content, Z);
//
//		Output[0] = { X[0], Y[0], Z[0] };
//		Output[1] = { X[1], Y[1], Z[1] };
//	}
//	_open static $VOID ParseBlendMode(_in CHAR8* Content, DWORD &Output){
//		if(StrCmpA(Content, "BLEND_ZERO") == 0)
//			Output = D3D12_BLEND_ZERO;
//		else if(StrCmpA(Content, "BLEND_ONE") == 0)
//			Output = D3D12_BLEND_ONE;
//		else if(StrCmpA(Content, "BLEND_SRCCOLOR") == 0)
//			Output = D3D12_BLEND_SRC_COLOR;
//		else if(StrCmpA(Content, "BLEND_INVSRCCOLOR") == 0)
//			Output = D3D12_BLEND_INV_SRC_COLOR;
//		else if(StrCmpA(Content, "BLEND_SRCALPHA") == 0)
//			Output = D3D12_BLEND_SRC_ALPHA;
//		else if(StrCmpA(Content, "BLEND_INVSRCALPHA") == 0)
//			Output = D3D12_BLEND_INV_SRC_ALPHA;
//		else if(StrCmpA(Content, "BLEND_DESTALPHA") == 0)
//			Output = D3D12_BLEND_DEST_ALPHA;
//		else if(StrCmpA(Content, "BLEND_INVDESTALPHA") == 0)
//			Output = D3D12_BLEND_INV_DEST_ALPHA;
//		else if(StrCmpA(Content, "BLEND_DESTCOLOR") == 0)
//			Output = D3D12_BLEND_DEST_COLOR;
//		else if(StrCmpA(Content, "BLEND_INVDESTCOLOR") == 0)
//			Output = D3D12_BLEND_INV_DEST_COLOR;
//		else if(StrCmpA(Content, "BLEND_SRCALPHASAT") == 0)
//			Output = D3D12_BLEND_SRC_ALPHA_SAT;
//		else if(StrCmpA(Content, "BLENDOP_ADD") == 0)
//			Output = D3D12_BLEND_OP_ADD;
//		else if(StrCmpA(Content, "BLENDOP_SUB") == 0)
//			Output = D3D12_BLEND_OP_SUBTRACT;
//		else if(StrCmpA(Content, "BLENDOP_REVSUB") == 0)
//			Output = D3D12_BLEND_OP_REV_SUBTRACT;
//		else if(StrCmpA(Content, "BLENDOP_MIN") == 0)
//			Output = D3D12_BLEND_OP_MIN;
//		else if(StrCmpA(Content, "BLENDOP_Max") == 0)
//			Output = D3D12_BLEND_OP_MAX;
//		else if(StrCmpA(Content, "BLENDOP_OFF") == 0)
//			Output = 0L;
//	}
//};
//
////纹理驱动
//class CPtcTexDriver: public CPtcDriver{
//	_secr VECTR2X Params[2];
//
//	_secr $VOID ParseContent(_in CHAR8* Content){
//		CPtcDriver::ParseVector2(Content, $m.Params);
//	}
//	_secr $VOID UpdateParticle(CPtcView &PtcView, SPFPN TimeSpan){
//		SPFPN PrevTime = PtcView.Age();
//		SPFPN CurTime = PtcView.Age() + TimeSpan;
//
//		if($m.Next && (CurTime >= $m.Next->Time)){
//			$m.Next->UpdateParticle(PtcView, TimeSpan);
//			return;
//		}
//
//		if(PrevTime <= $m.Time)
//			Rand_Vec2(&PtcView.TexCoord(), $m.Params);
//		PtcView.Age() = CurTime;
//	}
//	_secr $VOID UpdateFading(CPtcView &PtcView, SPFPN TimeSlice){}
//	_secr $VOID SetupFading(CPtcView &PtcView, SPFPN TimeDiff){}
//	_secr $VOID SetChange(CPtcView &PtcView){}
//};
////时间驱动
//class CPtcTimeDriver: public CPtcDriver{
//	_secr SPFPN Params[2];
//
//	_secr $VOID ParseContent(_in CHAR8* Content){
//		CPtcDriver::ParseNumber(Content, $m.Params);
//	}
//	_secr $VOID UpdateParticle(CPtcView &PtcView, SPFPN TimeSpan){
//		SPFPN PrevTime = PtcView.Age();
//		SPFPN CurTime = PtcView.Age() + TimeSpan;
//
//		if($m.Type == TYPE_MaxTIME){
//			if(PrevTime == 0.f)
//				PtcView.MaxAge() = Rand_Real($m.Params);
//			if(CurTime >= PtcView.MaxAge())
//				PtcView.ReduceTotal();
//			else PtcView.Age() = CurTime;
//		} else if($m.Type == TYPE_TIME){
//			if(CurTime >= $m.Time)
//				PtcView.Age() = Rand_Real($m.Params);
//			else PtcView.Age() = CurTime;
//		}
//	}
//	_secr $VOID UpdateFading(CPtcView &PtcView, SPFPN TimeSlice){}
//	_secr $VOID SetupFading(CPtcView &PtcView, SPFPN TimeDiff){}
//	_secr $VOID SetChange(CPtcView &PtcView){}
//};
////尺寸驱动
//class CPtcSizeDriver: public CPtcDriver{
//	_secr VECTR2X Params[2];
//
//	_secr $VOID ParseContent(_in CHAR8* Content){
//		CPtcDriver::ParseVector2(Content, $m.Params);
//	}
//	_secr $VOID UpdateFading(CPtcView &PtcView, SPFPN TimeSlice){
//		using namespace dx;
//
//		MVECTOR VecStep = Pck_V2(PtcView.SizeDif());
//		MVECTOR Vector = Pck_V2(PtcView.Size()) + (VecStep * TimeSlice);
//
//		Upk_V2(PtcView.Size(), Vector);
//	}
//	_secr $VOID SetupFading(CPtcView &PtcView, SPFPN TimeDiff){
//		using namespace dx;
//
//		VECTR2X NextValue;
//		CPtcSizeDriver *NextDriver = (CPtcSizeDriver*)$m.Next;
//		Rand_Vec2(NextValue, NextDriver->Params);
//
//		MVECTOR Vector0 = Pck_V2(PtcView.Size());
//		MVECTOR Vector1 = Pck_V2(NextValue);
//		MVECTOR VecStep = (Vector1 - Vector0) / TimeDiff;
//		Upk_V2(PtcView.SizeDif(), VecStep);
//	}
//	_secr $VOID SetChange(CPtcView &PtcView){
//		Rand_Vec2(&PtcView.Size(), $m.Params);
//	}
//};
////颜色驱动
//class CPtcColorDriver: public CPtcDriver{
//	_secr COLOR128 Params[2];
//
//	_secr $VOID ParseContent(_in CHAR8* Content){
//		CPtcDriver::ParseColor(Content, $m.Params);
//	}
//	_secr $VOID UpdateFading(CPtcView &PtcView, SPFPN TimeSlice){
//		using namespace dx;
//
//		MVECTOR VecStep = Pck_V4(PtcView.ColorDif());
//		MVECTOR Vector = Pck_V4(PtcView.Color()) + (VecStep * TimeSlice);
//
//		Upk_V4(PtcView.Color(), Vector);
//		Color_Compress(PtcView.Color32(), Pck_V4(PtcView.Color()));
//	}
//	_secr $VOID SetupFading(CPtcView &PtcView, SPFPN TimeDiff){
//		using namespace dx;
//
//		COLOR128 NextValue;
//		CPtcColorDriver *NextDriver = (CPtcColorDriver*)$m.Next;
//		Rand_Color(NextValue, NextDriver->Params);
//
//		MVECTOR Vector0 = Pck_V4(PtcView.Color());
//		MVECTOR Vector1 = Pck_V4(NextValue);
//		MVECTOR VecStep = (Vector1 - Vector0) / TimeDiff;
//		Upk_V4(PtcView.ColorDif(), VecStep);
//	}
//	_secr $VOID SetChange(CPtcView &PtcView){
//		Rand_Vec4(&PtcView.Color(), $m.Params);
//		Color_Compress(PtcView.Color32(), Pck_V4(PtcView.Color()));
//	}
//};
////速度驱动
//class CPtcVelocDriver: public CPtcDriver{
//	_secr VECTR3X Params[2];
//
//	_secr $VOID ParseContent(_in CHAR8* Content){
//		CPtcDriver::ParseVector3(Content, $m.Params);
//	}
//	_secr $VOID UpdateFading(CPtcView &PtcView, SPFPN TimeSlice){
//		MVECTOR VecStep = Pck_V3(PtcView.Accel());
//		MVECTOR Vector = Pck_V3(PtcView.Veloc()) + (VecStep * TimeSlice);
//		Upk_V3(PtcView.Veloc(), Vector);
//	}
//	_secr $VOID SetupFading(CPtcView &PtcView, SPFPN TimeDiff){
//		using namespace dx;
//
//		VECTOR3 NextValue;
//		CPtcVelocDriver *NextDriver = (CPtcVelocDriver*)$m.Next;
//		Rand_Vec3(NextValue, NextDriver->Params);
//
//		MVECTOR Vector0 = Pck_V3(PtcView.Veloc());
//		MVECTOR Vector1 = Pck_V3(NextValue);
//		MVECTOR VecStep = Vec_Sub(Vector1, Vector0) / TimeDiff;
//		Upk_V3(PtcView.Accel(), VecStep);
//	}
//	_secr $VOID SetChange(CPtcView &PtcView){
//		Rand_Vec3(PtcView.Veloc(), $m.Params);
//	}
//};
////加速度驱动
//class CPtcAccelDriver: public CPtcDriver{
//	_secr VECTR3X Params[2];
//
//	_secr $VOID ParseContent(_in CHAR8* Content){
//		CPtcDriver::ParseVector3(Content, $m.Params);
//	}
//	_secr $VOID UpdateFading(CPtcView &PtcView, SPFPN TimeSlice){
//		using namespace dx;
//
//		MVECTOR VecStep = Pck_V3(PtcView.AccelDif());
//		MVECTOR Vector = Pck_V3(PtcView.Accel()) + (VecStep * TimeSlice);
//
//		Upk_V3(PtcView.Accel(), Vector);
//	}
//	_secr $VOID SetupFading(CPtcView &PtcView, SPFPN TimeDiff){
//		using namespace dx;
//
//		VECTOR3 NextValue;
//		CPtcAccelDriver *NextDriver = (CPtcAccelDriver*)$m.Next;
//		Rand_Vec3(NextValue, NextDriver->Params);
//
//		MVECTOR Vector0 = Pck_V3(PtcView.Accel());
//		MVECTOR Vector1 = Pck_V3(NextValue);
//		MVECTOR VecStep = (Vector1 - Vector0) / TimeDiff;
//		Upk_V3(PtcView.AccelDif(), VecStep);
//	}
//	_secr $VOID SetChange(CPtcView &PtcView){
//		Rand_Vec3(PtcView.Accel(), $m.Params);
//	}
//};
////标量型驱动
//class CPtcScalarDriver: public CPtcDriver{
//	_secr SPFPN Params[2];
//
//	_secr $VOID ParseContent(_in CHAR8* Content){
//		CPtcDriver::ParseNumber(Content, $m.Params);
//	}
//	_secr $VOID UpdateParticle(CPtcView &PtcView, SPFPN TimeSpan){
//		SPFPN PrevTime = PtcView.Age();
//		SPFPN LastTime = PtcView.Age() + TimeSpan;
//
//		//时间未落在本片段则给下一个驱动处理
//
//		if($m.Next && (PrevTime >= $m.Next->Time)){
//			$m.Next->UpdateParticle(PtcView, TimeSpan);
//			return;
//		}
//
//		//检查是否有渐变和分片处理
//
//		IBOOL bFadeOut = $m.Next && $m.Next->bFade;
//		IBOOL bInBatch = $m.Next && (LastTime > $m.Next->Time);
//
//		//选取要修改的粒子属性
//
//		USINT PropIndex = 0;
//		SPFPN *pProp, *pStep;
//
//		switch($m.Type){
//			case TYPE_TEXCOORD_V: PropIndex++;
//			case TYPE_TEXCOORD_U:{
//				pProp = PtcView.TexUvMem(PropIndex);
//			} break;
//			case TYPE_SIZE_Y: PropIndex++;
//			case TYPE_SIZE_X:{
//				pProp = PtcView.SizeMem(PropIndex);
//				pStep = PtcView.SizeDifComp(PropIndex);
//			} break;
//			case TYPE_VELOC_Z: PropIndex++;
//			case TYPE_VELOC_Y: PropIndex++;
//			case TYPE_VELOC_X:{
//				pProp = PtcView.VelocMem(PropIndex);
//				pStep = PtcView.AccelMem(PropIndex);
//			} break;
//			case TYPE_ACCEL_Z: PropIndex++;
//			case TYPE_ACCEL_Y: PropIndex++;
//			case TYPE_ACCEL_X:{
//				pProp = PtcView.AccelMem(PropIndex);
//				pStep = PtcView.AccelDifComp(PropIndex);
//			} break;
//			case TYPE_COLOR_A: PropIndex++;
//			case TYPE_COLOR_B: PropIndex++;
//			case TYPE_COLOR_G: PropIndex++;
//			case TYPE_COLOR_R:{
//				pProp = PtcView.ColorMem(PropIndex);
//				pStep = PtcView.AccelDifComp(PropIndex);
//			} break;
//		}
//
//		//首次进入该驱动的粒子需要初始化
//
//		if(PrevTime == $m.Time){
//			if(!$m.bFade)
//				*pProp = Rand_Real($m.Params);
//			if(bFadeOut){
//				SPFPN EndTime = ($m.Next->Time != FLT_MAX) ?
//					$m.Next->Time : PtcView.MaxAge();
//				SPFPN TimeDiff = EndTime - $m.Time;
//
//				CPtcScalarDriver *NextDriver = (CPtcScalarDriver*)$m.Next;
//				SPFPN NextValue = Rand_Real(NextDriver->Params);
//
//				*pStep = (NextValue - *pProp) / TimeDiff;
//			}
//		}
//
//		//计算时间切片
//
//		SPFPN TimeSlice = bInBatch ?
//			($m.Next->Time - PrevTime) : TimeSpan;
//
//		//更新粒子位置
//
//		if(($m.Type >= TYPE_VELOC) && ($m.Type <= TYPE_VELOC_Z)){
//			SPFPN *pCoord = PtcView.CoordMem(PropIndex);
//			*pCoord += *pProp * TimeSlice;
//		}
//
//		//如果有渐变则需要维护
//
//		if(bFadeOut)
//			*pProp += *pStep * TimeSlice;
//
//		//如果有分片则需要继续处理
//
//		if(!bInBatch){
//			PtcView.Age() = LastTime;
//		} else{
//			PtcView.Age() = $m.Next->Time;
//			TimeSpan = LastTime - PtcView.Age();
//			$m.Next->UpdateParticle(PtcView, TimeSpan);
//		}
//	}
//	_secr $VOID SetupFading(CPtcView &PtcView, SPFPN TimeDiff){}
//	_secr $VOID UpdateFading(CPtcView &PtcView, SPFPN TimeSlice){}
//	_secr $VOID SetChange(CPtcView &PtcView){}
//};
//
////粒子驱动工厂
//class CPtcDriverFactory{
//	//创建驱动
//	_open static CPtcDriver* Create(_in CHAR8 *Action, _in CHAR8 *Time, _in CHAR8* Type, _in CHAR8* Content){
//		CPtcDriver *pDriver = P_Null;
//
//		if(StrCmpA(Type, "life") == 0){
//			pDriver = new CPtcTimeDriver;
//			pDriver->Type = CPtcDriver::TYPE_MaxTIME;
//		} else if(StrCmpA(Type, "timer") == 0){
//			pDriver = new CPtcTimeDriver;
//			pDriver->Type = CPtcDriver::TYPE_TIME;
//		} else if(StrCmpA(Type, "size") == 0){
//			pDriver = new CPtcSizeDriver;
//			pDriver->Type = CPtcDriver::TYPE_SIZE;
//		} else if(StrCmpA(Type, "size-x") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_SIZE_X;
//		} else if(StrCmpA(Type, "size-y") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_SIZE_Y;
//		} else if(StrCmpA(Type, "veloc") == 0){
//			pDriver = new CPtcVelocDriver;
//			pDriver->Type = CPtcDriver::TYPE_VELOC;
//		} else if(StrCmpA(Type, "veloc-x") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_VELOC_X;
//		} else if(StrCmpA(Type, "veloc-y") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_VELOC_Y;
//		} else if(StrCmpA(Type, "veloc-z") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_VELOC_Z;
//		} else if(StrCmpA(Type, "accel") == 0){
//			pDriver = new CPtcVelocDriver;
//			pDriver->Type = CPtcDriver::TYPE_ACCEL;
//		} else if(StrCmpA(Type, "accel-x") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_ACCEL_X;
//		} else if(StrCmpA(Type, "accel-y") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_ACCEL_Y;
//		} else if(StrCmpA(Type, "accel-z") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_ACCEL_Z;
//		} else if(StrCmpA(Type, "color") == 0){
//			pDriver = new CPtcColorDriver;
//			pDriver->Type = CPtcDriver::TYPE_COLOR;
//		} else if(StrCmpA(Type, "color-a") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_COLOR_A;
//		} else if(StrCmpA(Type, "color-r") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_COLOR_R;
//		} else if(StrCmpA(Type, "color-g") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_COLOR_G;
//		} else if(StrCmpA(Type, "color-b") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_COLOR_B;
//		} else if(StrCmpA(Type, "texcoord") == 0){
//			pDriver = new CPtcTexDriver;
//			pDriver->Type = CPtcDriver::TYPE_TEXCOORD;
//		} else if(StrCmpA(Type, "texcoord-u") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_TEXCOORD_U;
//		} else if(StrCmpA(Type, "texcoord-v") == 0){
//			pDriver = new CPtcScalarDriver;
//			pDriver->Type = CPtcDriver::TYPE_TEXCOORD_V;
//		}
//
//		if(pDriver){
//			pDriver->ParseContent(Content);
//			pDriver->bFade = B_False;
//			pDriver->Next = P_Null;
//
//			if(StrCmpA(Action, "set") == 0){
//				CPtcDriver::ParseNumber(Time, pDriver->Times);
//			} else if(StrCmpA(Action, "fade") == 0){
//				CPtcDriver::ParseNumber(Time, pDriver->Times);
//				pDriver->bFade = B_True;
//			} else if(StrCmpA(Action, "init") == 0){
//				pDriver->Times[0] = 0.f;
//				pDriver->Times[1] = 0.f;
//			} else if(StrCmpA(Action, "final") == 0){
//				pDriver->Times[0] = FLT_MAX;
//				pDriver->Times[1] = FLT_MAX;
//			}
//		}
//
//		return pDriver;
//	}
//	//创建空驱动
//	_open static CPtcDriver* CreateEmpty(CPtcDriver::TYPE Type){
//		CPtcScalarDriver *pDriver = new CPtcScalarDriver;
//		Sto_Bytes(pDriver, 0, sizeof(*pDriver));
//		pDriver->Type = Type;
//		return pDriver;
//	}
//};

//----------------------------------------//