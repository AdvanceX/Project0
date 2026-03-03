//-------------------- 索引 --------------------//

typedef UNS32 INDEX2[2];  // 2 Indexes
typedef UNS32 INDEX3[3];  // 3 Indexes
typedef UNS16 INDEX2H[2]; // 2 Indexes(SHORT)
typedef UNS16 INDEX3H[3]; // 3 Indexes(SHORT)

//----------------------------------------//


//-------------------- 紧缩RGB --------------------//

typedef UNS16 R5G6B5; // Red-Green-Blue(5-6-5b Channel)
typedef UNS16 RGB05;  // Red-Green-Blue(5b Channel)
typedef UNS16 ARGB4;  // Alpha-Red-Green-Blue(4b Channel)
typedef UNS32 RGB10;  // Red-Green-Blue(10b Channel)
typedef UNS32 ARGB8;  // Alpha-Red-Green-Blue(8b Channel)

//----------------------------------------//


//-------------------- 矩形区 --------------------//

typedef D2D_RECT_L LRect; // Rectangle(LONG)
typedef D2D_RECT_U URect; // Rectangle(UINT)
typedef D2D_RECT_F FRect; // Rectangle(FLOAT)

//----------------------------------------//


//-------------------- 方向&位置 --------------------//

// Position Tag
enum POSTAG{
	POS_Top = (1L << 0),    // 顶部
	POS_Bottom = (1L << 1), // 底部
	POS_Center = (1L << 2), // 中心
	POS_Left = (1L << 3),   // 左端
	POS_Right = (1L << 4),  // 右端
};

// Direction Tag
enum DIRTAG{
	DIR_Up = (1L << 0),    // 上方
	DIR_Down = (1L << 1),  // 下方
	DIR_Left = (1L << 2),  // 左方
	DIR_Right = (1L << 3), // 右方
	DIR_Front = (1L << 4), // 前方
	DIR_Back = (1L << 5),  // 后方
};

//----------------------------------------//


//-------------------- 素材类型 --------------------//

// Asset Type
enum ASSETTYPE{
	ASSET_TYPE_Anim = (1L << 0),     // 动画
	ASSET_TYPE_Model = (1L << 1),    // 模型
	ASSET_TYPE_Morph = (1L << 2),    // 变形
	ASSET_TYPE_Material = (1L << 3), // 材质
	ASSET_TYPE_Particle = (1L << 4), // 粒子
	ASSET_TYPE_Skeleton = (1L << 5), // 骨架
	//////
	ASSET_TYPE_Surface = (1L << 8), // 水面
	ASSET_TYPE_Terrain = (1L << 9), // 地形
	ASSET_TYPE_Spline = (1L << 10), // 样条线
};

// Texture Type
enum TEXTYPE{
	TEX_TYPE_Albedo = (1L << 16),    // 反照率贴图
	TEX_TYPE_Normal = (1L << 17),    // 法线贴图
	TEX_TYPE_MetalRg = (1L << 18),   // 金属性-粗糙度贴图
	TEX_TYPE_Opacity = (1L << 19),   // 透明度贴图
	TEX_TYPE_Emission = (1L << 20),  // 自发光贴图
	TEX_TYPE_Occlusion = (1L << 21), // 环境遮蔽贴图
	TEX_TYPE_Height = (1L << 22),    // 高度贴图
};

//----------------------------------------//


//-------------------- 顶点、粒子格式位 --------------------//

// Vertex Format Bit
enum VERTFORMATBIT{
	VFB_Coord = (1L << 0),    // 坐标
	VFB_Normal = (1L << 1),   // 法线
	VFB_Tangent = (1L << 2),  // 切线
	VFB_Color = (1L << 3),    // 颜色
	VFB_PSize = (1L << 4),    // 粒子尺寸
	VFB_BoneRig = (1L << 5),  // 骨骼绑定
	VFB_TexCoord = (1L << 6), // 纹理坐标
	VFB_RowCol = (1L << 24),  // 行列坐标
};

// Particle Format Bit
enum PARTICLEFORMATBIT{
	/// 可渲染属性
	PFB_Coord = (1L << 0),    // 坐标
	PFB_TexCoord = (1L << 1), // 纹理坐标
	PFB_Size = (1L << 2),     // 尺寸
	PFB_Color32 = (1L << 3),  // 32b颜色
	/// 不可渲染属性
	PFB_Color = (1L << 4),  // 颜色
	PFB_Veloc = (1L << 5),  // 速度
	PFB_Accel = (1L << 6),  // 加速度
	PFB_Age = (1L << 7),    // 年龄
	PFB_MaxAge = (1L << 8), // 寿命
};

//----------------------------------------//


//-------------------- 方框适应模式 --------------------//

// Box Fit Mode
enum BOXFITMODE{
	BFM_Fill,    // 拉伸
	BFM_None,    // 居中
	BFM_Cover,   // 覆盖
	BFM_Contain, // 包含
};

//----------------------------------------//


//-------------------- 灯光&材质基础 --------------------//

// Light
struct GLight{
	VFloat3 Intensity; // 发光强度
	SPFPN SpotPower;   // 聚光强度(聚光灯)
	VFloat3 Position;  // 光源位置(聚光灯/点光)
	SPFPN AttStart;    // 衰减起点(聚光灯/点光)
	VFloat3 Direction; // 光照方向(聚光灯/方向光)
	SPFPN AttEnd;      // 衰减终点(聚光灯/点光)
};

// Material Base
struct GMtlBase{
	using SDFLAG2 = DWORD[2];
	using SDFLAG3 = DWORD[3];

	VFloat3 Albedo;   // 反照率
	SPFPN Opacity;    // 不透明度
	VFloat3 Emission; // 自发光
	SPFPN Roughness;  // 粗糙度
	SDFLAG3 TexAttrs; // 纹理属性
	SPFPN Metalness;  // 金属度
};

// Material Lite
struct GMtlLite{
	VFloat4 Albedo;  // 反照率
	ARGB8 Ambient;   // 环境光
	ARGB8 Emission;  // 自发光
	SPFPN Roughness; // 粗糙度
	SPFPN Metalness; // 金属度
};

//----------------------------------------//


//-------------------- 材质 --------------------//

// Material Maps
struct GMtlMaps{
	UNS32 iAlbedo;    // 反照率贴图
	UNS32 iNormal;    // 法线贴图
	UNS32 iMetalRg;   // 金属性-粗糙度贴图
	UNS32 iOpacity;   // 透明度贴图
	UNS32 iEmission;  // 自发光贴图
	UNS32 iOcclusion; // 环境遮蔽贴图
};

// Material
struct GMaterial{
	UNS32 Ident;
	GMtlBase Base;
	GMtlMaps Maps;
};

//----------------------------------------//


//-------------------- 子网格 --------------------//

// Sub Mesh
struct GSubMesh{
	UNS32 IdxStart;
	UNS32 IdxCount;
	UNS32 MtlIdent;
};

//----------------------------------------//


//-------------------- 关键帧 --------------------//

// Rotation Frame
struct GRotFrame{
	SPFPN Time;
	VFloat4 Xform;
};

// Animation Frame
struct GAnimFrame{
	SPFPN Time;
	VFloat3 Xform;
};

// Morph Frame
struct GMorphFrame{
	SPFPN Time;
	UNS32 iTarget;
};

//----------------------------------------//


//-------------------- 骨骼 --------------------//

// Bone
struct alignas(16) MXBone{
	UNS32 Ident; // 标识
	UNS32 iParent; // 父节点
	QWORD vPadding; // 填充位
	MXMATRIX matLocal;   // 局部矩阵
	MXMATRIX matOffset;  // 偏移矩阵
	MXMATRIX matInitial; // 初始矩阵
	MXMATRIX matCombine; // 组合矩阵

	PVOID operator new[](UIPTR Count){
		return _aligned_malloc(Count * sizeof(MXBone), 16);
	}
	$VOID operator delete[](PVOID pTarget){
		_aligned_free(pTarget);
	}
};

//----------------------------------------//


//-------------------- 相机 --------------------//

// Camera
struct alignas(16) MXCamera{
	MXVECTOR Front; // 前方向
	MXVECTOR UpDir; // 上方向
	MXVECTOR Right; // 右方向
	MXVECTOR Position; // 位置
	MXMATRIX matView; // 观察矩阵
	MXMATRIX matProj; // 投影矩阵

	$VOID _MX_CALL Reset(FMXVECTOR Position){
		$m.Position = Position;
		$m.Front = dx::g_XMIdentityR2;
		$m.UpDir = dx::g_XMIdentityR1;
		$m.Right = dx::g_XMIdentityR0;
	}
	$VOID _MX_CALL Locate(FMXVECTOR Position){
		$m.Position = Position;
	}
	$VOID _MX_CALL LookTo(FMXVECTOR Front, FMXVECTOR UpDir){
		$m.Front = Front;
		$m.Right = dx::XMVector3Cross(UpDir, $m.Front);
		$m.Right = dx::XMVector3Normalize($m.Right);
		$m.UpDir = dx::XMVector3Cross($m.Front, $m.Right);
	}
	$VOID _MX_CALL LookAt(FMXVECTOR Target, FMXVECTOR UpDir){
		$m.Front = dx::XMVectorSubtract(Target, $m.Position);
		$m.Front = dx::XMVector3Normalize($m.Front);
		$m.Right = dx::XMVector3Cross(UpDir, $m.Front);
		$m.Right = dx::XMVector3Normalize($m.Right);
		$m.UpDir = dx::XMVector3Cross($m.Front, $m.Right);
	}
	$VOID Perspective(SPFPN FovAngle, SPFPN AspectRatio, SPFPN NearZ, SPFPN FarZ){
		$m.matProj = dx::XMMatrixPerspectiveFovLH(FovAngle / AspectRatio, AspectRatio, NearZ, FarZ);
	}
	$VOID Orthographic(SPFPN ViewWidth, SPFPN ViewHeight, SPFPN NearZ, SPFPN FarZ){
		$m.matProj = dx::XMMatrixOrthographicLH(ViewWidth, ViewHeight, NearZ, FarZ);
	}
	$VOID UpdateView(IBOOL bNormalize){
		if(bNormalize){
			$m.Front = dx::XMVector3Normalize($m.Front);
			$m.UpDir = dx::XMVector3Cross($m.Front, $m.Right);
			$m.UpDir = dx::XMVector3Normalize($m.UpDir);
			$m.Right = dx::XMVector3Cross($m.UpDir, $m.Front);
		}
		$m.matView = dx::XMMatrixLookToLH($m.Position, $m.Front, $m.UpDir);
	}
	/// 移动
	$VOID Fly(SPFPN Dist){
		using namespace dx;
		$m.Position += $m.UpDir * Dist;
	}
	$VOID Walk(SPFPN Dist){
		using namespace dx;
		$m.Position += $m.Front * Dist;
	}
	$VOID Strafe(SPFPN Dist){
		using namespace dx;
		$m.Position += $m.Right * Dist;
	}
	$VOID Yaw(SPFPN Angle){
		MXMATRIX matXform = dx::XMMatrixRotationAxis($m.UpDir, Angle);
		$m.Right = dx::XMVector3TransformNormal($m.Right, matXform);
		$m.Front = dx::XMVector3TransformNormal($m.Front, matXform);
	}
	$VOID Roll(SPFPN Angle){
		MXMATRIX matXform = dx::XMMatrixRotationAxis($m.Front, Angle);
		$m.Right = dx::XMVector3TransformNormal($m.Right, matXform);
		$m.UpDir = dx::XMVector3TransformNormal($m.UpDir, matXform);
	}
	$VOID Pitch(SPFPN Angle){
		MXMATRIX matXform = dx::XMMatrixRotationAxis($m.Right, Angle);
		$m.UpDir = dx::XMVector3TransformNormal($m.UpDir, matXform);
		$m.Front = dx::XMVector3TransformNormal($m.Front, matXform);
	}
	$VOID HorizontalYaw(SPFPN Angle){
		MXMATRIX matXform = dx::XMMatrixRotationAxis(dx::g_XMIdentityR1, Angle);
		$m.Front = dx::XMVector3TransformNormal($m.Front, matXform);
		$m.UpDir = dx::XMVector3TransformNormal($m.UpDir, matXform);
		$m.Right = dx::XMVector3TransformNormal($m.Right, matXform);
	}
	$VOID HorizontalWalk(SPFPN Dist){
		using namespace dx;
		MXVECTOR Direction;
		Direction = dx::XMVectorSetY($m.Front, 0.f);
		Direction = dx::XMVector3Normalize(Direction);
		$m.Position += Direction * Dist;
	}
	/// 访问
	$VOID GetPosition(_out VFloat3 &rResult) const{
		Sto_V3(&rResult, $m.Position);
	}
	$VOID GetViewProjMat(_out MFlt4x4 &rResult) const{
		Sto_M4(&rResult, dx::XMMatrixTranspose($m.matView * $m.matProj));
	}
	$VOID GetViewProjTexMat(_out MFlt4x4 &rResult) const{
		Sto_M4(&rResult, dx::XMMatrixTranspose($m.GetViewProjTexMat()));
	}
	MXMATRIX GetViewProjTexMat() const{
		MXMATRIX matViewProj = $m.matView * $m.matProj;
		MXMATRIX matTexture(
			0.5f, 0.f, 0.f, 0.f, 0.f, -0.5f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f, 0.5f, 0.5f, 0.f, 1.f);

		return matViewProj * matTexture;
	}
	MXMATRIX GetViewProjMat() const{
		return $m.matView * $m.matProj;
	}
	SPFPN GetPositionX() const{
		dx::XMVectorGetX($m.Position);
	}
	SPFPN GetPositionY() const{
		return dx::XMVectorGetY($m.Position);
	}
	SPFPN GetPositionZ() const{
		dx::XMVectorGetZ($m.Position);
	}
	//////
	PVOID operator new[](UIPTR Count){
		return _aligned_malloc(Count * sizeof(MXCamera), 16);
	}
	$VOID operator delete[](PVOID pTarget){
		_aligned_free(pTarget);
	}
};

//----------------------------------------//


//-------------------- 顶点、粒子视图 --------------------//

// Vertex View
class CVtxView{
	_secr BYTET *psVertex; // 顶点指针
	_secr UNS32 cbVertex;  // 顶点字节数
	_secr UNS08 hCoord;       // 坐标句柄
	_secr UNS08 hNormal;      // 法线句柄
	_secr UNS08 hTangent;     // 切线句柄
	_secr UNS08 hColor;       // 颜色句柄
	_secr UNS08 hPointSize;   // 粒子尺寸句柄
	_secr UNS08 hBoneIndices; // 骨骼索引句柄
	_secr UNS08 hBoneWeights; // 骨骼权重句柄
	_secr UNS08 hTexCoord;    // 纹理坐标句柄

	_open CVtxView() = default;
	_open CVtxView(DWORD Format){
		$m.Initialize(Format);
	}
	//////
	_open $VOID Initialize(DWORD Format){
		Var_Zero(this);

		if(Format & VFB_Coord){
			CAST_($m.hCoord, $m.cbVertex);
			$m.cbVertex += sizeof(VFloat3);
		}
		if(Format & VFB_Normal){
			CAST_($m.hNormal, $m.cbVertex);
			$m.cbVertex += sizeof(VFloat3);
		}
		if(Format & VFB_Tangent){
			CAST_($m.hTangent, $m.cbVertex);
			$m.cbVertex += sizeof(VFloat3);
		}
		if(Format & VFB_Color){
			CAST_($m.hColor, $m.cbVertex);
			$m.cbVertex += sizeof(PVColor);
		}
		if(Format & VFB_PSize){
			CAST_($m.hPointSize, $m.cbVertex);
			$m.cbVertex += sizeof(VFloat2);
		}
		if(Format & VFB_BoneRig){
			CAST_($m.hBoneIndices, $m.cbVertex);
			$m.cbVertex += sizeof(UNS08[4]);
			CAST_($m.hBoneWeights, $m.cbVertex);
			$m.cbVertex += sizeof(SPFPN[3]);
		}
		if(Format & VFB_TexCoord){
			CAST_($m.hTexCoord, $m.cbVertex);
			$m.cbVertex += sizeof(VFloat2);
		}

		for(UNS32 iTex = 1; Format & (VFB_TexCoord << iTex); ++iTex)
			$m.cbVertex += sizeof(VFloat2);
	}
	_open $VOID NextTarget(INT32 Offset){
		$m.psVertex += $m.cbVertex * Offset;
	}
	_open $VOID SetTarget(PVOID psVertex){
		$m.psVertex = (BYTET*)psVertex;
	}
	_open UNS32 GetStride(){
		return $m.cbVertex;
	}
	/// 属性访问
	_open UNS08 *BoneIndices(){
		return (UNS08*)($m.psVertex + $m.hBoneIndices);
	}
	_open SPFPN *BoneWeights(){
		return (SPFPN*)($m.psVertex + $m.hBoneWeights);
	}
	_open VFloat2 &PointSize(){
		return *(VFloat2*)($m.psVertex + $m.hPointSize);
	}
	_open PVColor &Color(){
		return *(PVColor*)($m.psVertex + $m.hColor);
	}
	_open FVector3 &Coord(){
		return *(FVector3*)($m.psVertex + $m.hCoord);
	}
	_open FVector3 &Normal(){
		return *(FVector3*)($m.psVertex + $m.hNormal);
	}
	_open FVector3 &Tangent(){
		return *(FVector3*)($m.psVertex + $m.hTangent);
	}
	_open FVector2 &TexCoord(UNS32 Layer = 0){
		BYTET *pProp = $m.psVertex + $m.hTexCoord;
		return *((FVector2*)pProp + Layer);
	}
};

// Particle View
class CPtcView{
	_secr UNS32 *pCount;  // 计数指针
	_secr BYTET *psPrtcl; // 粒子指针
	_secr UNS32 cbPrtcl;  // 粒子字节数
	_secr UNS08 hCoord;    // 坐标句柄
	_secr UNS08 hTexCoord; // 纹理坐标句柄
	_secr UNS08 hSize;     // 尺寸句柄
	_secr UNS08 hColor32;  // 32b颜色句柄
	_secr UNS08 hColor;    // 颜色句柄
	_secr UNS08 hVeloc;    // 速度句柄
	_secr UNS08 hAccel;    // 加速度句柄
	_secr UNS08 hAge;      // 年龄句柄
	_secr UNS08 hMaxAge;   // 寿命句柄

	_open CPtcView() = default;
	_open CPtcView(DWORD Format){
		$m.Initialize(Format);
	}
	//////
	_open $VOID Initialize(DWORD Format){
		Var_Zero(this);

		if(Format & PFB_Coord){
			CAST_($m.hCoord, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(VFloat3);
		}
		if(Format & PFB_TexCoord){
			CAST_($m.hTexCoord, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(VFloat2);
		}
		if(Format & PFB_Size){
			CAST_($m.hSize, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(VFloat2);
		}
		if(Format & PFB_Color32){
			CAST_($m.hColor32, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(PVColor);
		}
		if(Format & PFB_Color){
			CAST_($m.hColor, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(FColor4);
		}
		if(Format & PFB_Veloc){
			CAST_($m.hVeloc, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(VFloat3);
		}
		if(Format & PFB_Accel){
			CAST_($m.hAccel, $m.cbPrtcl);
			$m.cbPrtcl += sizeof(VFloat3);
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
	_open $VOID SetCount(UNS32 *_in pCount){
		$m.pCount = pCount;
	}
	_open $VOID ReduceCount(){
		*$m.pCount -= 1;
	}
	_open UNS32 GetStride(){
		return $m.cbPrtcl;
	}
	_open IBOOL IsActive(){
		SPFPN Age = *(SPFPN*)($m.psPrtcl + $m.hAge);
		SPFPN MaxAge = *(SPFPN*)($m.psPrtcl + $m.hMaxAge);
		return Age <= MaxAge;
	}
	/// 属性访问
	_open SPFPN &Age(){
		return *(SPFPN*)($m.psPrtcl + $m.hAge);
	}
	_open SPFPN &MaxAge(){
		return *(SPFPN*)($m.psPrtcl + $m.hMaxAge);
	}
	_open VFloat2 &Size(){
		return *(VFloat2*)($m.psPrtcl + $m.hSize);
	}
	_open FVector3 &Veloc(){
		return *(FVector3*)($m.psPrtcl + $m.hVeloc);
	}
	_open FVector3 &Accel(){
		return *(FVector3*)($m.psPrtcl + $m.hAccel);
	}
	_open FVector3 &Coord(){
		return *(FVector3*)($m.psPrtcl + $m.hCoord);
	}
	_open FVector2 &TexCoord(){
		return *(FVector2*)($m.psPrtcl + $m.hTexCoord);
	}
	_open PVColor &Color32(){
		return *(PVColor*)($m.psPrtcl + $m.hColor32);
	}
	_open FColor4 &Color(){
		return *(FColor4*)($m.psPrtcl + $m.hColor);
	}
	/// 子属性访问
	_open SPFPN *SizePart(UNS32 Index){
		BYTET *pProp = $m.psPrtcl + $m.hSize;
		return (SPFPN*)pProp + Index;
	}
	_open SPFPN *ColorPart(UNS32 Index){
		BYTET *pProp = $m.psPrtcl + $m.hColor;
		return (SPFPN*)pProp + Index;
	}
	_open SPFPN *CoordPart(UNS32 Index){
		BYTET *pProp = $m.psPrtcl + $m.hCoord;
		return (SPFPN*)pProp + Index;
	}
	_open SPFPN *VelocPart(UNS32 Index){
		BYTET *pProp = $m.psPrtcl + $m.hVeloc;
		return (SPFPN*)pProp + Index;
	}
	_open SPFPN *AccelPart(UNS32 Index){
		BYTET *pProp = $m.psPrtcl + $m.hAccel;
		return (SPFPN*)pProp + Index;
	}
	_open SPFPN *TexCoorPart(UNS32 Index){
		BYTET *pProp = $m.psPrtcl + $m.hTexCoord;
		return (SPFPN*)pProp + Index;
	}
};

//----------------------------------------//