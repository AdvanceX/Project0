#include <map>
#include <set>
#include <list>
#include <queue>
#include <vector>
#include <IGame.h>
#include <IGameModifier.h>
#include <MaxTypes.h>

#pragma comment(lib, "geom.lib")
#pragma comment(lib, "IGame.lib")
#pragma comment(lib, "MaxUtil.lib")


#define DELETE_PTR(ptr) if(ptr) delete ptr
#define RESET_PTR(ptr) if(ptr){ delete ptr; ptr = NULL; }
#define DELETE_ARR(arr) if(arr) delete[] arr
#define RESET_ARR(arr) if(arr){ delete[] arr; arr = NULL; }

#define DLL_IMPORT __declspec(dllimport)
#define DLL_EXPORT extern"C" _declspec(dllexport)


typedef std::list<IGameNode*> CNodeList;
typedef std::list<IGameMaterial*> CMtlList;


//顶点属性
enum VERTATTR{
	VERTEX_COORD    = 0x0001,
	VERTEX_NORMAL   = 0x0002,
	VERTEX_TANGENT  = 0x0004,
	VERTEX_SIZE     = 0x0008,
	VERTEX_BONES    = 0x0010,
	VERTEX_COLOR    = 0x0020,
	VERTEX_TEXTURE  = 0x0040,
	VERTEX_TEXTURES = 0xffffff80
};
//顶点描述
struct VERTDESC{	
	UINT ByteCount;
	UINT hCoord;
	UINT hNormal;
	UINT hTangent;
	UINT hPointSize;
	UINT hWeights;
	UINT hBoneIndexes;
	UINT hColor;
	UINT hTexCoord;

	VERTDESC(){}
	VERTDESC(DWORD Format){
		this->Initialize(Format);
	}
	UINT Initialize(DWORD Format){
		ZeroMemory(this, sizeof(*this));
		
		if(Format & VERTEX_COORD){
			this->hCoord = this->ByteCount;
			this->ByteCount += sizeof(Point3);
		}
		if(Format & VERTEX_NORMAL){
			this->hNormal = this->ByteCount;
			this->ByteCount += sizeof(Point3);
		}
		if(Format & VERTEX_TANGENT){
			this->hTangent = this->ByteCount;
			this->ByteCount += sizeof(Point3);
		}
		if(Format & VERTEX_SIZE){
			this->hPointSize = this->ByteCount;
			this->ByteCount += sizeof(Point2);
		}
		if(Format & VERTEX_BONES){
			this->hWeights = this->ByteCount;
			this->ByteCount += sizeof(FLOAT) * 3;
			this->hBoneIndexes = this->ByteCount;
			this->ByteCount += sizeof(UCHAR) * 4;
		}
		if(Format & VERTEX_COLOR){
			this->hColor = this->ByteCount;
			this->ByteCount += sizeof(COLORREF);
		}
		if(Format & VERTEX_TEXTURE){
			this->hTexCoord = this->ByteCount;
			this->ByteCount += sizeof(Point2);
		}

		DWORD fElem = VERTEX_TEXTURE << 1;
		while(Format & fElem){
			this->ByteCount += sizeof(Point2);
			fElem <<= 1;
		}

		return this->ByteCount;
	}
};
//控制点
struct CTLPOINT{
	UINT nVert;
	UINT nNormal;
	UINT nTangent;
	UINT nColor;
	UINT nAlpha;
	UINT nTexCoord;
	UINT nMtrl;

	static BOOL Equal(CTLPOINT &Point0, CTLPOINT &Point1){
		if((Point0.nVert == Point1.nVert) &&
			(Point0.nColor == Point1.nColor) &&
			(Point0.nAlpha == Point1.nAlpha) &&
			(Point0.nNormal == Point1.nNormal) &&
			(Point0.nTangent == Point1.nTangent) &&
			(Point0.nTexCoord == Point1.nTexCoord))
		{
			return TRUE;
		} else{
			return FALSE;
		}
	}
	static INT Compare(const VOID* pObject0, const VOID* pObject1){
		CTLPOINT *pPoint0 = (CTLPOINT*)pObject0;
		CTLPOINT *pPoint1 = (CTLPOINT*)pObject1;
		INT Value0 = (INT)pPoint0->nMtrl;
		INT Value1 = (INT)pPoint1->nMtrl;
		return Value0 - Value1;
	}
};
//材质
struct MATERIAL{
	struct BASE{
		Point3 Albedo;
		FLOAT Opacity;
		Point3 Fresnel;
		FLOAT Shininess;
	};

	BASE Base;
	UINT Ident;
	MCHAR *Name;
	IGameMaterial *Reference;
};
//材质贴图
struct MTLMAP{
	UINT nDiffuse;
	UINT nBump;
};
//子网格
struct SUBMESH{
	UINT IdxStart;
	UINT IdxCount;
	UINT MtlIdent;
};
//网格
struct MESH{
	CHAR *Name;
	UINT Ident;
	UINT MtlCount;
	UINT MapCount;
	UINT IdxCount;
	UINT VtxCount;
	BYTE *VtxBuffer;
	BYTE *IdxBuffer;
	MTLMAP *arrMapping;
	SUBMESH *arrSubset;
	CTLPOINT *arrPoint;
	IGameSkin *pSkin;
	IGameMorpher *pMorpher;
	VERTDESC VertDesc;
	DWORD VtxFormat;

	~MESH(){
		this->Finalize();
	}
	MESH(){
		ZeroMemory(this, sizeof(*this));
	}
	VOID Finalize(){
		if(this->Name){
			RESET_ARR(this->Name);
			RESET_ARR(this->VtxBuffer);
			RESET_ARR(this->IdxBuffer);
			RESET_ARR(this->arrMapping);
			RESET_ARR(this->arrSubset);
			RESET_ARR(this->arrPoint);
		}
	}
};
//骨骼
struct BONE{
	CHAR Name[28];
	UINT nParent;
	GMatrix matOffset;
	GMatrix matInitial;
	IGameNode *Reference;
};
//关键帧
struct KEYFRAME{
	FLOAT Time;
	GMatrix matTrans;
};
//动画
struct ANIMATION{
	UINT numFrame;
	KEYFRAME *arrFrame;

	~ANIMATION(){
		this->Finalize();
	}
	ANIMATION(){
		ZeroMemory(this, sizeof(*this));
	}
	VOID Finalize(){
		RESET_ARR(this->arrFrame);
	}
};
//变体
struct MORPHS{
	UINT Ident;
	UINT cbMesh;
	UINT numMesh;
	Point3 **arrMesh;

	~MORPHS(){
		this->Finalize();
	}
	MORPHS(){
		ZeroMemory(this, sizeof(*this));
	}
	VOID Finalize(){
		if(this->arrMesh){
			for(UINT i = 0; i < this->numMesh; ++i)
				DELETE_ARR(this->arrMesh[i]);
			RESET_ARR(this->arrMesh);
		}
	}
};
//导出器
struct CExporter{
	///场景缓存
	UINT numMesh;
	UINT numBone;
	UINT numMtrl;
	UINT numMorphs;
	MESH *arrMesh;
	BONE *arrBone;
	MORPHS *arrMorphs;
	MATERIAL *arrMtrl;
	ANIMATION *arrAnim;
	///导出选项
	DWORD ExpOption;
	DWORD ExpAction;
	DWORD VtxFormat;
	FLOAT AnimLength;

	~CExporter(){
		this->Finalize();
	}
	CExporter(){
		ZeroMemory(this, sizeof(*this));
	}
	///对象解析
	VOID Finalize(){
		RESET_ARR(this->arrMesh);
		RESET_ARR(this->arrBone);
		RESET_ARR(this->arrAnim);
		RESET_ARR(this->arrMtrl);
		RESET_ARR(this->arrMorphs);
	}
	VOID Process(IGameScene*);
	VOID ProcTopNodes(IGameScene*);
	VOID ProcMaterials(IGameScene*);
	VOID ProcAnimations(IGameScene*);
	VOID ProcMeshs(CNodeList&);
	VOID ProcBones(IGameNode*);
	VOID ParseMesh(IGameNode*, UINT);
	VOID ParseSkin(IGameSkin*, UINT);
	VOID ParseMeshMaps(SUBMESH*, UINT);
	VOID ParseMaterial(IGameMaterial*, UINT);
	VOID ParseMeshOption(IGameObject*, UINT);
	VOID ParseMorphs(IGameMorpher*, UINT, UINT);
	VOID WriteToFile(TCHAR*);
	VOID StartChunk(HANDLE, DWORD);
	VOID EndChunk(HANDLE, UINT&, UINT&);
	///对象查找
	MATERIAL* GetMaterialByID(UINT);
	MATERIAL* GetMaterialByName(MCHAR*);
	IGameMorpher* GetMorpher(IGameObject*);

	static INT_PTR CALLBACK PanelProc(HWND, UINT, WPARAM, LPARAM);
};