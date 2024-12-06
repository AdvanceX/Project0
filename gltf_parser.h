//****************************************//

#define WIN32_LEAN_AND_MEAN

#pragma warning(disable:4200)
#pragma warning(disable:4996)

#include <Windows.h>
#include <DirectXMath.h>

#include "../library/BTCC/cJSON.h"
#include "../engine/ge-base.hpp"
#include "../engine/ge-util.hpp"

#ifdef _WIN64
#pragma comment(lib, "library/BTCC/x64/cJSON.lib")
#else
#pragma comment(lib, "library/BTCC/x86/cJSON.lib")
#endif

//****************************************//


//-------------------- 声明 --------------------//

using namespace std;
using namespace DirectX;

class CGltfParser;

//----------------------------------------//


//-------------------- 资源枚举 --------------------//

// Asset Type
enum ASSETTYPE{
	ASSET_TYPE_Anim = (1L << 0),     //动画
	ASSET_TYPE_Model = (1L << 1),    //模型
	ASSET_TYPE_Morph = (1L << 2),    //形变
	ASSET_TYPE_Material = (1L << 3), //材质
	ASSET_TYPE_Particle = (1L << 4), //粒子
	ASSET_TYPE_Skeleton = (1L << 5), //骨骼
	//////
	ASSET_TYPE_ColorMap = (1L << 16),  //反照率贴图
	ASSET_TYPE_NormMap = (1L << 17),   //法线贴图
	ASSET_TYPE_RoughMap = (1L << 18),  //金属性-粗糙度贴图
	ASSET_TYPE_AlphaMap = (1L << 19),  //透明度贴图
	ASSET_TYPE_EmissMap = (1L << 20),  //自发光贴图
	ASSET_TYPE_OcclusMap = (1L << 21), //环境遮蔽贴图
	ASSET_TYPE_HeightMap = (1L << 22), //高度贴图
};

// Primitive Type
enum PRIMTYPE{
	PRIM_TYPE_Points = 0L,        //点集
	PRIM_TYPE_Lines = 1L,         //线集
	PRIM_TYPE_LineLoop = 2L,      //线环
	PRIM_TYPE_LineStrip = 3L,     //线带
	PRIM_TYPE_Triangles = 4L,     //三角形集
	PRIM_TYPE_TriangleStrip = 5L, //三角形带
	PRIM_TYPE_TriangleFan = 6L,   //三角形扇
};

// Scalar Type
enum SCALARTYPE{
	SCALAR_TYPE_Int8 = 5120L,   //int8
	SCALAR_TYPE_UInt8 = 5121L,  //uint8
	SCALAR_TYPE_Int16 = 5122L,  //int16
	SCALAR_TYPE_UInt16 = 5123L, //uint16
	SCALAR_TYPE_Int32 = 5124L,  //int32
	SCALAR_TYPE_UInt32 = 5125L, //uint32
	SCALAR_TYPE_Real32 = 5126L, //real32
};

// Address Mode
enum ADDRMODE{
	ADDR_MODE_Repeat = 10497L, //重复
	ADDR_MODE_Clamp = 33071L,  //钳制
	ADDR_MODE_Mirror = 33648L, //镜像
};

// Filter Mode
enum FILTERMODE{
	FILTER_MODE_Nearest = 9728L, //最近采样
	FILTER_MODE_Linear = 9729L,  //线性采样
	FILTER_MODE_Nearest_MipmapNearest = 9984L, //最近采样,mipmap最近采样
	FILTER_MODE_Linear_MipmapNearest = 9985L,  //线性采样,mipmap最近采样
	FILTER_MODE_Nearest_MipmapLinear = 9986L,  //最近采样,mipmap线性采样
	FILTER_MODE_Linear_MipmapLinear = 9987L,   //线性采样,mipmap线性采样
};

// Index Format
enum INDEXFORMAT{
	IDX_FMT_32bit = 42L, //32位
	IDX_FMT_16bit = 57L, //16位
};

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

//----------------------------------------//


//-------------------- 资源结构 --------------------//

// 64b Color
struct COLOR64{
	UHALF R, G, B, A;

	operator COLORREF(){
		SPFPN kRatio = 255.f / 65535.f;

		USINT R = USINT($m.R * kRatio);
		USINT G = USINT($m.G * kRatio);
		USINT B = USINT($m.B * kRatio);
		USINT A = USINT($m.A * kRatio);

		return B | (G << 8) | (R << 16) | (A << 24);
	}
};

// Chunk Header
struct CHUNKHEAD{
	DWORD DataType;
	USINT DataSize;
};

// Rotation Frame
struct ROTFRAME{
	SPFPN Time;
	XMFLOAT4 Txform;
};

// Animation Frame
struct ANIMFRAME{
	SPFPN Time;
	XMFLOAT3 Txform;
};

// Morph Frame
struct MORPHFRAME{
	SPFPN Time;
	USINT iTarget;
};

// Material Base
struct MATBASE{
	XMFLOAT3 Albedo;
	SPFPN Opacity;
	XMFLOAT3 Emission;
	SPFPN Roughness;
	XMUINT3 TexAttrs;
	SPFPN Metalness;

	MATBASE(): Albedo(1.f, 1.f, 1.f), Opacity(1.f),
		Emission(0.f, 0.f, 0.f), Roughness(0.f),
		TexAttrs(0L, 0L, 0L), Metalness(0.f){}
};

// Material Maps
struct MATMAPS{
	USINT iAlbedo = 0;
	USINT iNormal = 0;
	USINT iMetalRg = 0;
	USINT iOpacity = 0;
	USINT iEmission = 0;
	USINT iOcclusion = 0;
};

// Material
struct MATERIAL{
	USINT Ident;
	MATBASE Base;
	MATMAPS Maps;
};

// Submesh
struct SUBMESH{
	USINT IdxStart;
	USINT IdxCount;
	USINT MtlIdent;
};

// Texture View
struct TEXVIEW{
	USINT TexId;
	DWORD AddrMode;
};

// Buffet View
struct BUFVIEW{
	USINT BuffId;
	USINT BufSize;
	USINT Offset;
	USINT UnitCnt;
	DWORD ArithType;
};

// Vertex View
struct VERTVIEW{
	USINT cbVertex;
	USINT hCoord;
	USINT hNormal;
	USINT hTangent;
	USINT hColor;
	USINT hPointSize;
	USINT hBoneIndices;
	USINT hBoneWeights;
	USINT hTexCoord;

	VERTVIEW() = default;
	VERTVIEW(DWORD Format){
		Var_Zero(this);

		if(Format & VFB_Coord){
			$m.hCoord = $m.cbVertex;
			$m.cbVertex += sizeof(XMFLOAT3);
		}
		if(Format & VFB_Normal){
			$m.hNormal = $m.cbVertex;
			$m.cbVertex += sizeof(XMFLOAT3);
		}
		if(Format & VFB_Tangent){
			$m.hTangent = $m.cbVertex;
			$m.cbVertex += sizeof(XMFLOAT3);
		}
		if(Format & VFB_Color){
			$m.hColor = $m.cbVertex;
			$m.cbVertex += sizeof(COLORREF);
		}
		if(Format & VFB_PSize){
			$m.hPointSize = $m.cbVertex;
			$m.cbVertex += sizeof(XMFLOAT2);
		}
		if(Format & VFB_BoneRig){
			$m.hBoneIndices = $m.cbVertex;
			$m.cbVertex += sizeof(UINT8) * 4;
			$m.hBoneWeights = $m.cbVertex;
			$m.cbVertex += sizeof(SPFPN) * 3;
		}
		if(Format & VFB_TexCoord){
			$m.hTexCoord = $m.cbVertex;
			$m.cbVertex += sizeof(XMFLOAT2);
		}

		for(USINT iTex = 1; Format & (VFB_TexCoord << iTex); ++iTex)
			$m.cbVertex += sizeof(XMFLOAT2);
	}
};

// Node
struct NODE{
	CHAR8 *psName;
	USINT iRefBone;
	USINT numChild;
	UHALF rgChild[8];
	XMFLOAT4X4 matInitial;
};

// Bone
struct BONE{
	USINT Level;
	USINT iParent;
	CHAR8 *psName;
	NODE *pRefNode;
	XMFLOAT4X4 *pOffsetMat;
	XMFLOAT4X4 *pInitialMat;

	BONE() = default;
	BONE(USINT Level, USINT iParent, NODE *_in pNode){
		$m.Level = Level;
		$m.iParent = iParent;
		$m.psName = pNode->psName;
		$m.pRefNode = pNode;
		$m.pOffsetMat = P_Null;
		$m.pInitialMat = &pNode->matInitial;
	}

	static bool IsLess(_in BONE &Bone0, _in BONE &Bone1){
		if(Bone0.Level < Bone1.Level)
			return B_True;
		if(Bone0.Level > Bone1.Level)
			return B_False;
		if(MbsCmp(Bone0.psName, Bone1.psName) < 0)
			return B_True;
		return B_False;
	}
};

//----------------------------------------//


//-------------------- 资源类 --------------------//

// Model
class CModel{
	_open USINT Ident;
	_open USINT numSubset;
	_open USINT cbIdxBuff;
	_open USINT cbVtxBuff;
	_open DWORD IdxFormat;
	_open DWORD VtxFormat;
	_open BYTET *lpsIdxBuff;
	_open BYTET *lpsVtxBuff;
	_open SUBMESH *lprgSubset;
	_open CHAR8 *psName;

	_open ~CModel(){
		SAFE_DELETEA($m.lprgSubset);
		SAFE_DELETEA($m.lpsIdxBuff);
		SAFE_DELETEA($m.lpsVtxBuff);
	}
	_open CModel(){
		Var_Zero(this);
	}
	//////
	_open $VOID Initialize(CJsonNode*, CGltfParser&);
	_secr $VOID InitVertexs(CJsonNode*, CGltfParser&);
	_secr $VOID InitSubmeshs(CJsonNode*, CGltfParser&);
	_secr $VOID InitIndexBuffer(CJsonNode*, _in BUFVIEW*);
	_secr $VOID InitVertexBuffer(CJsonNode*, _in BUFVIEW*);
};

// Morphs
class CMorphs{
	_open USINT Ident;
	_open USINT cbTarget;
	_open USINT numTarget;
	_open XMFLOAT3 **lprgTarget;

	_open ~CMorphs(){
		for(USINT iExe = 0; iExe < $m.numTarget; ++iExe)
			SAFE_DELETEA($m.lprgTarget[iExe]);
		SAFE_DELETEA($m.lprgTarget);
	}
	_open CMorphs(){
		Var_Zero(this);
	}
	//////
	_open $VOID Initialize(CJsonNode*, CGltfParser&);
};

// Animation
class CAnimation{
	_open SPFPN Duration; //秒
	_open USINT numTrans;
	_open USINT numScaling;
	_open USINT numRotation;
	_open ROTFRAME *lprgRotation;
	_open ANIMFRAME *lprgScaling;
	_open ANIMFRAME *lprgTrans;

	_open ~CAnimation(){
		SAFE_DELETEA($m.lprgTrans);
		SAFE_DELETEA($m.lprgScaling);
		SAFE_DELETEA($m.lprgRotation);
	}
	_open CAnimation(){
		Var_Zero(this);
	}
	//////
	_open $VOID Initialize(_in CHAR8*, CJsonNode*, CGltfParser&);
};

// Animation Assembly
class CAnimAssy{
	_open SPFPN Duration;
	_open USINT numElem;
	_open CAnimation *lprgElem;

	_open ~CAnimAssy(){
		SAFE_DELETEA($m.lprgElem);
	}
	_open CAnimAssy(){
		Var_Zero(this);
	}
	//////
	_open $VOID Initialize(CJsonNode*, CGltfParser&);
};

//----------------------------------------//


//-------------------- 解析器 --------------------//

// GLTF Parser
class CGltfParser{
	friend CModel;
	friend CMorphs;
	friend CAnimAssy;
	friend CAnimation;

	///计数
	_secr USINT numAnim;
	_secr USINT numBone;
	_secr USINT numMatl;
	_secr USINT numNode;
	_secr USINT numModel;
	_secr USINT numBuffer;
	_secr USINT numBufView;
	_secr USINT numTexture;
	///缓存
	_secr USINT rgBoneRig[256];
	_secr BONE *lprgBone;
	_secr NODE *lprgNode;
	_secr BYTET **lprgBuffer;
	_secr CModel *lprgModel;
	_secr BUFVIEW *lprgBufView;
	_secr TEXVIEW *lprgTexView;
	_secr MATERIAL *lprgMatl;
	_secr CAnimAssy *lprgAnim;

	_open ~CGltfParser(){
		for(USINT iBuf = 0; iBuf < $m.numBuffer; ++iBuf)
			delete[] $m.lprgBuffer[iBuf];

		SAFE_DELETEA($m.lprgAnim);
		SAFE_DELETEA($m.lprgBone);
		SAFE_DELETEA($m.lprgModel);
		SAFE_DELETEA($m.lprgMatl);
		SAFE_DELETEA($m.lprgNode);
		SAFE_DELETEA($m.lprgBuffer);
		SAFE_DELETEA($m.lprgBufView);
		SAFE_DELETEA($m.lprgTexView);
	}
	_open CGltfParser(){
		Var_Zero(this);
	}
	_open $VOID Process(_in CHAR8*);
	_secr $VOID ParseSkin(CJsonNode*);
	_secr $VOID ParseNodes(CJsonNode*);
	_secr $VOID ParseModels(CJsonNode*);
	_secr $VOID ParseBuffers(CJsonNode*);
	_secr $VOID ParseTextures(CJsonNode*);
	_secr $VOID ParseMaterials(CJsonNode*);
	_secr $VOID ParseAnimations(CJsonNode*);
	_secr $VOID ParseBufferViews(CJsonNode*);
	_secr $VOID ParseMaterial(CJsonNode*, _out MATERIAL&);
	_secr $VOID ParseSkeleton(_in NODE*);
	_secr $VOID WriteToFile(_in CHAR8*);
	_secr $VOID WriteModels(HANDLE);
	_secr $VOID WriteSkeleton(HANDLE);
	_secr $VOID WriteMaterials(HANDLE);
	_secr $VOID WriteAnimations(HANDLE);
	_secr $VOID StartChunk(HANDLE, DWORD);
	_secr $VOID EndChunk(HANDLE, _io USINT&);
	_secr BYTET *GetBuffer(USINT);
	_secr USINT GetBufferSplit(USINT);
	_secr DWORD GetAddressMode(CJsonNode*);
};

//----------------------------------------//