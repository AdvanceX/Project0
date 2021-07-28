#include "define.h"
#include "../dle/resource.h"


#define OPT_MESH     0x010000
#define OPT_BONE     0x020000
#define OPT_ANIM     0x040000
#define OPT_MATERIAL 0x080000
#define OPT_MORPH   0x100000


//程序实例
HINSTANCE gInstance;
//导出工具
CExporter gExporter;


//弹出调试框
DWORD PopDebug(CHAR* Format, ...){
	va_list Args;
	CHAR Text[256];

	va_start(Args, Format);
	vsprintf(Text, Format, Args);
	va_end(Args);

	return MessageBoxA(NULL, Text, "Debug", MB_OK);
}
//颜色压缩
COLORREF ColorShrink(Point4 &Color){
	UINT r = UINT(Color.x * 255.0f);
	UINT g = UINT(Color.y * 255.0f);
	UINT b = UINT(Color.z * 255.0f);
	UINT a = UINT(Color.w * 255.0f);

	return b + (g << 8) + (r << 16) + (a << 24);
}
//字符串复制
CHAR* StringCopy(MCHAR* Source, CHAR* Dest){
#ifdef UNICODE
	UINT Size = WideCharToMultiByte(CP_ACP, 0x0, Source, -1, NULL, 0, NULL, NULL);
	if(!Dest) Dest = new CHAR[Size];
	WideCharToMultiByte(CP_ACP, 0x0, Source, -1, Dest, Size, NULL, NULL);
#else
	if(!Dest) Dest = new CHAR[strlen(Source)];
	strcpy(Dest, Source);
#endif

	return Dest;
}
//字符串散列
UINT StringHash(MCHAR* Source){
	UINT Hash = 0;
	CHAR Temp[256];
	CHAR *String = StringCopy(Source, Temp);

	for(UINT i = 0; String[i]; ++i)
		Hash = (Hash * 131) + String[i];

	return Hash;
}

//处理场景
VOID CExporter::Process(IGameScene* pScene){
	this->ProcMaterials(pScene);
	this->ProcTopNodes(pScene);
}
//处理顶节点
VOID CExporter::ProcTopNodes(IGameScene* pScene){
	UINT MorphsCount = 0;
	IGameNode *pTopBone = NULL;
	CNodeList MeshList;

	//统计节点

	UINT NodeCount = pScene->GetTopLevelNodeCount();
	if(NodeCount == 0) return;

	for(UINT i = 0; i < NodeCount; ++i){
		IGameNode *pNode = pScene->GetTopLevelNode(i);
		if(pNode->IsTarget()) continue;

		IGameObject *pObject = pNode->GetIGameObject();
		IGameObject::ObjectTypes ObjType = pObject->GetIGameType();

		if(ObjType == IGameObject::IGAME_BONE || ObjType == IGameObject::IGAME_HELPER)
			pTopBone = pNode;
		else if(ObjType == IGameObject::IGAME_MESH)
			MeshList.push_front(pNode);
	}

	//统计变形目标

	for(auto iter = MeshList.begin(); iter != MeshList.end(); ++iter){
		IGameObject *pObject = (*iter)->GetIGameObject();
		IGameMorpher *pMorpher = this->GetMorpher(pObject);

		if(!pMorpher) continue;
		else MorphsCount++;

		UINT TargetCount = pMorpher->GetNumberOfMorphTargets();
		for(UINT i = 0; i < TargetCount; ++i){
			IGameNode *pTarget = pMorpher->GetMorphTarget(i);
			for(auto itMesh = MeshList.begin(); itMesh != MeshList.end(); ++itMesh)
				if(*itMesh == pTarget) MeshList.erase(itMesh);
		}
	}

	//获取可选项

	if(MeshList.size() != 0)
		this->ExpOption |= OPT_MESH;
	if(MorphsCount != 0)
		this->ExpOption |= OPT_MORPH;
	if(pTopBone){
		this->ExpOption |= OPT_BONE;
		this->ExpOption |= OPT_ANIM;
	}

	this->numMesh = (UINT)MeshList.size();
	this->arrMesh = new MESH[this->numMesh];

	UINT MeshIndex = 0;
	for(auto iter = MeshList.begin(); iter != MeshList.end(); ++iter){
		IGameMesh *pMesh = (IGameMesh*)(*iter)->GetIGameObject();
		pMesh->InitializeData();
		this->ParseMeshOption(pMesh, MeshIndex++);
	}

	//选择导出项

	this->ExpAction = (DWORD)DialogBox(gInstance,
		MAKEINTRESOURCE(IDD_PANEL), NULL, CExporter::PanelProc);

	if((this->ExpAction == IDCANCEL) || (this->ExpAction == IDCLOSE)){
		for(auto iter = MeshList.begin(); iter != MeshList.end(); ++iter)
			(*iter)->ReleaseIGameObject();
		return;
	}

	for(UINT i = 0; i < this->numMesh; ++i){
		MESH *pMesh = &this->arrMesh[i];
		pMesh->VtxFormat &= this->VtxFormat;
	}

	//处理节点

	if(pTopBone && (this->ExpOption & OPT_BONE)){
		this->ProcBones(pTopBone);
		this->ProcAnimations(pScene);
	}
	if((MorphsCount != 0) && (this->ExpOption & OPT_MORPH)){
		this->numMorphs = MorphsCount;
		this->arrMorphs = new MORPHS[MorphsCount];
	}
	if((MeshList.size() != 0) && (this->ExpOption & OPT_MESH))
		this->ProcMeshs(MeshList);


	//释放数据

	for(auto iter = MeshList.begin(); iter != MeshList.end(); ++iter)
		(*iter)->ReleaseIGameObject();
}
//处理材质
VOID CExporter::ProcMaterials(IGameScene* pScene){
	CMtlList MtlList;

	//统计材质

	UINT MtlCount = pScene->GetRootMaterialCount();
	if(MtlCount == 0) return;

	for(UINT i = 0; i < MtlCount; ++i){
		IGameMaterial *pMtrl = pScene->GetRootMaterial(i);
		if(!pMtrl->IsMultiType()){
			MtlList.push_front(pMtrl);
		} else{
			UINT ChildCount = pMtrl->GetSubMaterialCount();
			for(UINT j = 0; j < ChildCount; ++j)
				MtlList.push_front(pMtrl->GetSubMaterial(j));
		}
	}

	//剔除重复材质

	for(auto it0 = MtlList.begin(); it0 != MtlList.end(); ++it0){
		auto iter = it0;
		for(++iter; iter != MtlList.end();){
			auto it1 = iter++;
			const MCHAR *Name0 = (*it0)->GetMaterialName();
			const MCHAR *Name1 = (*it1)->GetMaterialName();
			if(Name0 == Name1) MtlList.erase(it1);
		}
	}

	//解析材质

	this->ExpOption |= OPT_MATERIAL;
	this->numMtrl = (UINT)MtlList.size();
	this->arrMtrl = new MATERIAL[this->numMtrl];

	auto itMtrl = MtlList.begin();
	for(UINT i = 0; i < MtlList.size(); ++i)
		this->ParseMaterial(*(itMtrl++), i);
}
//处理动画
VOID CExporter::ProcAnimations(IGameScene* pScene){
	IGameKeyTab KeyTab;
	std::set<INT> KeyTimes;

	UINT Rate = pScene->GetSceneTicks();
	UINT Duration = pScene->GetSceneEndTime() - pScene->GetSceneStartTime();

	this->AnimLength = (FLOAT)Duration / (FLOAT)Rate;
	this->arrAnim = new ANIMATION[this->numBone];

	for(UINT i = 0; i < this->numBone; ++i){
		IGameNode *pNode = (IGameNode*)this->arrBone[i].Reference;
		pNode->GetIGameObject()->InitializeData();
		IGameControl *pControl = pNode->GetIGameControl();

		//获取关键时间点

		if(pControl->IsAnimated(IGAME_POS))
			pControl->GetQuickSampledKeys(KeyTab, IGAME_POS);
		if(pControl->IsAnimated(IGAME_ROT))
			pControl->GetQuickSampledKeys(KeyTab, IGAME_ROT);
		if(pControl->IsAnimated(IGAME_SCALE))
			pControl->GetQuickSampledKeys(KeyTab, IGAME_SCALE);

		for(UINT j = 0; j < (UINT)KeyTab.Count(); ++j){
			INT Time = KeyTab[j].t;
			if((Time >= 0) && (Time <= (INT)Duration))
				KeyTimes.insert(Time);
		}

		//获取关键帧数据

		UINT KeyCount = (UINT)KeyTimes.size();
		this->arrAnim[i].numFrame = KeyCount;
		this->arrAnim[i].arrFrame = new KEYFRAME[KeyCount];

		auto itTime = KeyTimes.begin();
		for(UINT j = 0; j < KeyCount; ++j){
			UINT Time = *(itTime++);
			KEYFRAME *pFrame = &this->arrAnim[i].arrFrame[j];

			pFrame->Time = FLOAT(Time / Rate);
			pFrame->matTrans = pNode->GetLocalTM(Time);
			if(i == 0) pFrame->matTrans.SetIdentity();
		}

		//释放数据

		KeyTimes.clear();
		KeyTab.SetCount(0);
		pNode->ReleaseIGameObject();
	}
}
//处理网格
VOID CExporter::ProcMeshs(CNodeList &NodeList){
	//处理网格

	auto itMesh = NodeList.begin();
	for(UINT i = 0; i < NodeList.size(); ++i)
		this->ParseMesh(*(itMesh++), i);

	//处理变形器

	if(gExporter.ExpOption & OPT_MORPH){
		for(UINT i = 0, n = 0; i < this->numMesh; ++i){
			IGameMorpher *pMorpher = this->arrMesh[i].pMorpher;
			if(!pMorpher) continue;
			this->ParseMorphs(pMorpher, n++, i);
		}
	}
}
//处理骨骼
VOID CExporter::ProcBones(IGameNode* pTopNode){
	struct NODEEX{
		UINT nParent;
		IGameNode *pBase;
	};

	//初始化

	UINT Count = 0;
	BONE *arrBone = new BONE[UINT8_MAX];
	NODEEX Node = { UINT(-1), pTopNode };
	std::queue<NODEEX> Queue;

	//解析骨骼

	Queue.push(Node);
	while(Queue.size() != 0){
		Node = Queue.front();
		Queue.pop();

		NODEEX SubNode = { Count, NULL };
		UINT ChildCount = (UINT)Node.pBase->GetChildCount();

		for(UINT i = 0; i < ChildCount; ++i){
			SubNode.pBase = Node.pBase->GetNodeChild(i);
			Queue.push(SubNode);
		}

		BONE *pBone = &arrBone[Count++];
		pBone->Reference = Node.pBase;
		pBone->nParent = Node.nParent;
		StringCopy((MCHAR*)Node.pBase->GetName(), pBone->Name);

		if(Node.nParent != -1){
			pBone->matInitial = Node.pBase->GetLocalTM();
			pBone->matOffset = pBone->matInitial;
			pBone->matOffset *= arrBone[Node.nParent].matOffset;
		} else{
			//pBone->matInitial = Node.pBase->GetLocalTM();
			pBone->matInitial.SetIdentity();
			pBone->matOffset.SetIdentity();
		}
	}

	for(UINT i = 0; i < Count; ++i){
		BONE *pBone = &arrBone[i];
		pBone->matOffset = pBone->matOffset.Inverse();
	}

	//保存到目标

	this->numBone = Count;
	this->arrBone = arrBone;
}
//解析网格
VOID CExporter::ParseMesh(IGameNode* pNode, UINT Index){
	//获取基本信息

	MESH *Dest = &this->arrMesh[Index];
	IGameMesh *pMesh = (IGameMesh*)pNode->GetIGameObject();
	UINT FaceCount = pMesh->GetNumberOfFaces();
	UINT IdxCount = FaceCount * 3;

	//创建控制点

	std::set<UINT> MtlTable;
	CTLPOINT *arrPoint = new CTLPOINT[IdxCount];

	for(UINT n = 0, i = 0; i < FaceCount; ++i){
		UINT MtlIdent = 0;
		FaceEx *pFace = pMesh->GetFace(i);
		IGameMaterial *pMtrl = pMesh->GetMaterialFromFace(pFace);

		if(pMtrl){
			MCHAR *MtlName = (MCHAR*)pMtrl->GetMaterialName();
			MtlIdent = this->GetMaterialByName(MtlName)->Ident;
			MtlTable.insert(MtlIdent);
		}

		for(UINT j = 0; j < 3; ++j, ++n){
			arrPoint[n].nVert = pFace->vert[j];
			arrPoint[n].nColor = pFace->color[j];
			arrPoint[n].nAlpha = pFace->alpha[j];
			arrPoint[n].nNormal = pFace->norm[j];
			arrPoint[n].nTexCoord = pFace->texCoord[j];
			arrPoint[n].nTangent = pMesh->GetFaceVertexTangentBinormal(i, j);
			arrPoint[n].nMtrl = MtlIdent;
		}
	}

	//划分子网格

	SUBMESH *arrSubmesh = NULL;
	UINT MtlCount = (UINT)MtlTable.size();

	if(MtlCount > 0){
		arrSubmesh = new SUBMESH[MtlCount];
		ZeroMemory(arrSubmesh, sizeof(SUBMESH) * MtlCount);
		qsort(arrPoint, FaceCount, sizeof(CTLPOINT) * 3, CTLPOINT::Compare);

		SUBMESH *pSubmesh = &arrSubmesh[0];
		pSubmesh->IdxStart = 0;
		pSubmesh->IdxCount = 3;
		pSubmesh->MtlIdent = arrPoint[0].nMtrl;

		for(UINT i = 3; i < IdxCount; i += 3){
			if(arrPoint[i].nMtrl == pSubmesh->MtlIdent){
				pSubmesh->IdxCount += 3;
			} else{
				pSubmesh++;
				pSubmesh->IdxStart = i;
				pSubmesh->IdxCount = 3;
				pSubmesh->MtlIdent = arrPoint[i].nMtrl;
			}
		}
	}

	//创建索引和剔除重复控制点

	UINT *arrIndex = new UINT[IdxCount];
	CTLPOINT *arrPoint0 = new CTLPOINT[IdxCount];

	for(UINT i = 0; i < IdxCount; ++i)
		arrIndex[i] = -1;

	UINT VtxCount = 0;
	for(UINT i = 0; i < IdxCount; ++i){
		if(arrIndex[i] != -1) continue;

		arrIndex[i] = VtxCount;
		arrPoint0[VtxCount] = arrPoint[i];

		for(UINT j = i + 1; j < IdxCount; ++j){
			if(arrIndex[j] != -1) continue;
			if(CTLPOINT::Equal(arrPoint[i], arrPoint[j]))
				arrIndex[j] = VtxCount;
		}

		VtxCount++;
	}

	delete[] arrPoint;
	arrPoint = arrPoint0;

	//压缩索引

	BYTE *IdxBuffer;

	if(VtxCount > UINT16_MAX){
		IdxBuffer = (BYTE*)arrIndex;
	} else{
		IdxBuffer = new BYTE[sizeof(USHORT) * IdxCount];
		USHORT *pIndex = (USHORT*)IdxBuffer;
		for(UINT i = 0; i < IdxCount; ++i)
			*(pIndex++) = arrIndex[i];
		delete[] arrIndex;
	}

	//创建顶点缓存区

	VERTDESC VertDesc(Dest->VtxFormat);
	UINT cbVertex = VertDesc.ByteCount;

	BYTE *VtxBuffer = new BYTE[VtxCount * cbVertex];
	ZeroMemory(VtxBuffer, VtxCount * cbVertex);

	//获取顶点坐标

	GMatrix matLocal = pNode->GetObjectTM();
	GMatrix matWorld = pNode->GetWorldTM();
	matLocal *= matWorld.Inverse();

	Point3 Coord;
	BYTE *pVert = VtxBuffer;
	for(UINT i = 0; i < VtxCount; ++i){
		pMesh->GetVertex(arrPoint[i].nVert, Coord, TRUE);
		*(Point3*)pVert = Coord * matLocal;
		pVert += cbVertex;
	}

	//获取顶点法线

	if(Dest->VtxFormat & VERTEX_NORMAL){
		pVert = VtxBuffer + VertDesc.hNormal;
		for(UINT i = 0; i < VtxCount; ++i){
			pMesh->GetNormal(arrPoint[i].nNormal, *(Point3*)pVert, TRUE);
			pVert += cbVertex;
		}
	}

	//获取顶点切线

	if(Dest->VtxFormat & VERTEX_TANGENT){
		pVert = VtxBuffer + VertDesc.hTangent;
		for(UINT i = 0; i < VtxCount; ++i){
			pMesh->GetTangent(arrPoint[i].nTangent, *(Point3*)pVert, TRUE);
			pVert += cbVertex;
		}
	}

	//获取顶点颜色

	if(Dest->VtxFormat & VERTEX_COLOR){
		Point4 Color;
		pVert = VtxBuffer + VertDesc.hColor;
		for(UINT i = 0; i < VtxCount; ++i){
			pMesh->GetColorVertex(arrPoint[i].nColor, *(Point3*)&Color);
			Color.w = pMesh->GetAlphaVertex(arrPoint[i].nAlpha);
			*(DWORD*)pVert = ColorShrink(Color);
			pVert += cbVertex;
		}
	}

	//获取纹理坐标

	if(Dest->VtxFormat & VERTEX_TEXTURE){
		Point2 TexCoord;
		pVert = VtxBuffer + VertDesc.hTexCoord;
		for(UINT j = 0; j < VtxCount; ++j){
			pMesh->GetTexVertex(arrPoint[j].nTexCoord, TexCoord);
			((Point2*)pVert)->x = TexCoord.x;
			((Point2*)pVert)->y = TexCoord.y;
			pVert += cbVertex;
		}
	}

	//保存到目标

	Dest->arrPoint = arrPoint;
	Dest->arrSubset = arrSubmesh;
	Dest->VtxBuffer = VtxBuffer;
	Dest->IdxBuffer = IdxBuffer;
	Dest->VertDesc = VertDesc;
	Dest->IdxCount = IdxCount;
	Dest->VtxCount = VtxCount;
	Dest->MtlCount = MtlCount;
	Dest->MapCount = (Dest->VtxFormat & VERTEX_TEXTURE) ? MtlCount : 0;
	Dest->Ident = StringHash((MCHAR*)pNode->GetName());
	Dest->Name = StringCopy((MCHAR*)pNode->GetName(), NULL);

	//解析蒙皮和贴图

	if(Dest->VtxFormat & VERTEX_BONES)
		this->ParseSkin(Dest->pSkin, Index);
	if(Dest->VtxFormat & VERTEX_TEXTURE)
		this->ParseMeshMaps(Dest->arrSubset, Index);
}
//解析蒙皮
VOID CExporter::ParseSkin(IGameSkin* pSkin, UINT Index){
	MESH *pMesh = &this->arrMesh[Index];
	VERTDESC &VertDesc = pMesh->VertDesc;
	std::map<UINT, UINT8> BoneTab;

	//构建骨骼查找表

	for(UINT i = 0; i < this->numBone; ++i){
		BONE *pBone = &this->arrBone[i];
		IGameNode *pNode = (IGameNode*)pBone->Reference;
		BoneTab[pNode->GetNodeID()] = i;
	}

	//构建蒙皮顶点

	CTLPOINT *pCtrl = &pMesh->arrPoint[0];
	BYTE *pVert = pMesh->VtxBuffer + VertDesc.hWeights;

	for(UINT i = 0; i < pMesh->VtxCount; ++i){
		DWORD VertIndex = pCtrl->nVert;
		DWORD VertType = pSkin->GetVertexType(VertIndex);

		FLOAT *Weights = (FLOAT*)pVert;
		UINT8 *BoneIndexes = (UINT8*)&Weights[3];

		ZeroMemory(pVert, sizeof(FLOAT) * 4);

		if(VertType == IGameSkin::IGAME_RIGID){
			UINT BoneID = pSkin->GetBoneID(VertIndex, 0);			
			BoneIndexes[0] = BoneTab[BoneID];
			Weights[0] = 1.0f;
		} else if(VertType == IGameSkin::IGAME_RIGID_BLENDED){
			UINT BoneCount = pSkin->GetNumberOfBones(VertIndex);
			for(UINT n = 0, j = 0; (j < BoneCount) && (n < 4); ++j){
				UINT BoneID = pSkin->GetBoneID(VertIndex, j);
				FLOAT Weight = pSkin->GetWeight(VertIndex, j);
				if(Weight <= 0.0f) continue;
				if(n < 3) Weights[n] = Weight;
				BoneIndexes[n++] = BoneTab[BoneID];
			}
		}

		pCtrl++;
		pVert += VertDesc.ByteCount;
	}
}
//解析贴图
VOID CExporter::ParseMeshMaps(SUBMESH* arrSubmesh, UINT Index){
	MESH *pMesh = &this->arrMesh[Index];
	pMesh->arrMapping = new MTLMAP[pMesh->MtlCount];
	ZeroMemory(pMesh->arrMapping, sizeof(MTLMAP) * pMesh->MtlCount);

	for(UINT i = 0; i < pMesh->MtlCount; ++i){
		UINT MtlIdent = arrSubmesh[i].MtlIdent;
		MTLMAP *pMapping = &pMesh->arrMapping[i];
		IGameMaterial *pMtrl = this->GetMaterialByID(MtlIdent)->Reference;
		UINT MapCount = pMtrl->GetNumberOfTextureMaps();

		for(UINT j = 0; j < MapCount; ++j){
			IGameTextureMap *pTexture = pMtrl->GetIGameTextureMap(j);

			MCHAR *FilePath = (MCHAR*)pTexture->GetBitmapFileName();
			MCHAR *FileName = _tcsrchr(FilePath, TEXT('\\')) + 1;

			DWORD MapType = pTexture->GetStdMapSlot();
			UINT MapIdent = StringHash(FileName);

			switch(MapType){
				case ID_DI: pMapping->nDiffuse = MapIdent; break;
				case ID_BU: pMapping->nBump = MapIdent; break;
			}
		}
	}
}
//解析材质
VOID CExporter::ParseMaterial(IGameMaterial* pMtrl, UINT Index) {
	MATERIAL *Dest;
	IGameProperty *pProp;

	Dest = &this->arrMtrl[Index];
	Dest->Reference = pMtrl;
	Dest->Name = (MCHAR*)pMtrl->GetMaterialName();
	Dest->Ident = StringHash(Dest->Name);

	pProp = pMtrl->GetDiffuseData();
	pProp->GetPropertyValue(Dest->Base.Albedo);

	pProp = pMtrl->GetSpecularData();
	pProp->GetPropertyValue(Dest->Base.Fresnel);

	pProp = pMtrl->GetGlossinessData();
	pProp->GetPropertyValue(Dest->Base.Shininess);

	pProp = pMtrl->GetOpacityData();
	pProp->GetPropertyValue(Dest->Base.Opacity);
}
//解析网格选项
VOID CExporter::ParseMeshOption(IGameObject* pObject, UINT Index){
	MESH *Dest = &this->arrMesh[Index];
	IGameMesh *pMesh = (IGameMesh*)pObject;

	//获取修改器

	UINT ModCount = pObject->GetNumModifiers();
	for(UINT i = 0; i < ModCount; ++i){
		IGameModifier *pModifier = pObject->GetIGameModifier(i);
		if(pModifier->IsSkin())
			Dest->pSkin = (IGameSkin*)pModifier;
		else if(pModifier->IsMorpher())
			Dest->pMorpher = (IGameMorpher*)pModifier;
	}

	//获取顶点格式

	if(Dest->pSkin)
		Dest->VtxFormat |= VERTEX_BONES;
	if(pMesh->GetNumberOfVerts() != 0)
		Dest->VtxFormat |= VERTEX_COORD;
	if(pMesh->GetNumberOfNormals() != 0)
		Dest->VtxFormat |= VERTEX_NORMAL;
	if(pMesh->GetNumberOfTangents() != 0)
		Dest->VtxFormat |= VERTEX_TANGENT;
	if(pMesh->GetNumberOfColorVerts() != 0)
		Dest->VtxFormat |= VERTEX_COLOR;
	if(pMesh->GetNumberOfTexVerts() != 0)
		Dest->VtxFormat |= VERTEX_TEXTURE;

	this->VtxFormat |= Dest->VtxFormat;
}
//解析变形
VOID CExporter::ParseMorphs(IGameMorpher* pMorpher, UINT nMorph, UINT nMesh){
	MESH *pMesh = &this->arrMesh[nMesh];
	MORPHS *pMorphs = &this->arrMorphs[nMorph];

	UINT TargetCount = pMorpher->GetNumberOfMorphTargets() + 1;
	UINT LayerCount = (pMesh->VtxFormat & VERTEX_NORMAL) ? 2 : 1;
	UINT VecCount = pMesh->VtxCount * LayerCount;

	pMorphs->Ident = pMesh->Ident;
	pMorphs->numMesh = TargetCount;
	pMorphs->arrMesh = new Point3*[TargetCount];
	pMorphs->cbMesh = sizeof(Point3) * VecCount;

	for(UINT i = 0; i < TargetCount; ++i)
		pMorphs->arrMesh[i] = new Point3[VecCount];

	BYTE *pSrcVert = pMesh->VtxBuffer;
	Point3 *pDestVert = pMorphs->arrMesh[0];
	for(UINT i = 0; i < pMesh->VtxCount; ++i){
		*pDestVert = *(Point3*)pSrcVert;
		pDestVert += LayerCount;
		pSrcVert += pMesh->VertDesc.ByteCount;
	}

	if(LayerCount > 1){
		BYTE *pSrcNormal = pMesh->VtxBuffer + pMesh->VertDesc.hNormal;
		Point3 *pDestNormal = pMorphs->arrMesh[0] + 1;
		for(UINT i = 0; i < pMesh->VtxCount; ++i){
			*pDestNormal = *(Point3*)pSrcNormal;
			pDestNormal += LayerCount;
			pSrcNormal += pMesh->VertDesc.ByteCount;
		}
	}

	for(UINT i = 1; i < TargetCount; ++i){
		IGameNode *pNode = pMorpher->GetMorphTarget(i - 1);
		IGameMesh *pObject = (IGameMesh*)pNode->GetIGameObject();
		pObject->InitializeData();

		GMatrix matVertTrans = pNode->GetObjectTM() * pNode->GetWorldTM().Inverse();
		Matrix3 matNormTrans = matVertTrans.ExtractMatrix3();

		Point3 *pVertex = pMorphs->arrMesh[i];
		for(UINT j = 0; j < pMesh->VtxCount; ++j){
			pObject->GetVertex(pMesh->arrPoint[j].nVert, *pVertex, TRUE);
			*pVertex = *pVertex * matVertTrans;
			pVertex += LayerCount;
		}

		if(LayerCount > 1){
			Point3 *pNormal = pMorphs->arrMesh[i] + 1;
			for(UINT j = 0; j < pMesh->VtxCount; ++j){
				pObject->GetVertex(pMesh->arrPoint[j].nNormal, *pNormal, TRUE);
				*pNormal = *pNormal * matNormTrans;
				pNormal += LayerCount;
			}
		}

		pNode->ReleaseIGameObject();
	}

	pMesh->VtxFormat |= 0x80000000;
}
//写入文件
VOID CExporter::WriteToFile(TCHAR* SavePath){
	HANDLE hFile;
	UINT PrevCursor, LastCursor = 0;

	//打开文件

	if((this->ExpAction == IDCANCEL) || (this->ExpAction == IDCLOSE))
		return;

	hFile = CreateFile(SavePath, GENERIC_WRITE, 0x0,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	//写出网格

	if((this->numMesh > 0) && (this->ExpOption & OPT_MESH)){
		this->StartChunk(hFile, OPT_MESH);

		WriteFile(hFile, &this->numMesh, sizeof(UINT), NULL, NULL);
		for(UINT i = 0; i < this->numMesh; ++i){
			MESH *pMesh = &this->arrMesh[i];
			UINT StrSize = (UINT)strlen(pMesh->Name);

			WriteFile(hFile, &StrSize, sizeof(UINT), NULL, NULL);
			WriteFile(hFile, pMesh->Name, StrSize, NULL, NULL);
			WriteFile(hFile, &pMesh->IdxCount, sizeof(UINT), NULL, NULL);
			WriteFile(hFile, &pMesh->VtxCount, sizeof(UINT), NULL, NULL);
			WriteFile(hFile, &pMesh->MtlCount, sizeof(UINT), NULL, NULL);
			WriteFile(hFile, &pMesh->MapCount, sizeof(UINT), NULL, NULL);
			WriteFile(hFile, &pMesh->VtxFormat, sizeof(DWORD), NULL, NULL);

			UINT cbBuffer = pMesh->VtxCount * pMesh->VertDesc.ByteCount;
			WriteFile(hFile, pMesh->VtxBuffer, cbBuffer, NULL, NULL);

			cbBuffer = (pMesh->VtxCount > UINT16_MAX) ? sizeof(UINT) : sizeof(USHORT);
			cbBuffer *= pMesh->IdxCount;
			WriteFile(hFile, pMesh->IdxBuffer, cbBuffer, NULL, NULL);

			for(UINT i = 0; i < pMesh->MtlCount; ++i)
				WriteFile(hFile, &pMesh->arrSubset[i], sizeof(SUBMESH), NULL, NULL);

			if(pMesh->MapCount > 0){
				cbBuffer = sizeof(MTLMAP) * pMesh->MapCount;
				WriteFile(hFile, pMesh->arrMapping, cbBuffer, NULL, NULL);
			}
		}

		this->EndChunk(hFile, PrevCursor, LastCursor);
	}

	//写出材质

	if((this->numMtrl > 0) && (this->ExpOption & OPT_MATERIAL)){
		this->StartChunk(hFile, OPT_MATERIAL);

		WriteFile(hFile, &this->numMtrl, sizeof(UINT), NULL, NULL);
		for(UINT i = 0; i < this->numMtrl; ++i){
			MATERIAL *pMtrl = &this->arrMtrl[i];
			WriteFile(hFile, &pMtrl->Ident, sizeof(UINT), NULL, NULL);
			WriteFile(hFile, &pMtrl->Base, sizeof(MATERIAL::BASE), NULL, NULL);
		}
		this->EndChunk(hFile, PrevCursor, LastCursor);
	}

	//写出骨骼

	if((this->numBone > 0) && (this->ExpOption & OPT_BONE)){
		this->StartChunk(hFile, OPT_BONE);

		WriteFile(hFile, &this->numBone, sizeof(UINT), NULL, NULL);
		for(UINT i = 0; i < this->numBone; ++i){
			BONE *pBone = &this->arrBone[i];
			UINT StrSize = (UINT)strlen(pBone->Name);

			WriteFile(hFile, &StrSize, sizeof(UINT), NULL, NULL);
			WriteFile(hFile, pBone->Name, StrSize, NULL, NULL);
			WriteFile(hFile, &pBone->nParent, sizeof(UINT), NULL, NULL);
			WriteFile(hFile, &pBone->matOffset, sizeof(GMatrix), NULL, NULL);
			WriteFile(hFile, &pBone->matInitial, sizeof(GMatrix), NULL, NULL);
		}

		this->EndChunk(hFile, PrevCursor, LastCursor);
	}

	//写出动画

	if((this->numBone > 0) && (this->ExpOption & OPT_ANIM)){
		this->StartChunk(hFile, OPT_ANIM);

		WriteFile(hFile, &this->AnimLength, sizeof(FLOAT), NULL, NULL);
		WriteFile(hFile, &this->numBone, sizeof(UINT), NULL, NULL);

		for(UINT i = 0; i < this->numBone; ++i){
			ANIMATION *pAnim = &this->arrAnim[i];
			WriteFile(hFile, &pAnim->numFrame, sizeof(UINT), NULL, NULL);

			if(pAnim->numFrame > 0){
				UINT cbBuffer = sizeof(KEYFRAME) * pAnim->numFrame;
				WriteFile(hFile, pAnim->arrFrame, cbBuffer, NULL, NULL);
			}
		}

		this->EndChunk(hFile, PrevCursor, LastCursor);
	}

	//写出变形目标

	if((this->numMorphs > 0) && (this->ExpOption & OPT_MORPH)){
		this->StartChunk(hFile, OPT_MORPH);

		WriteFile(hFile, &this->numMorphs, sizeof(UINT), NULL, NULL);
		for(UINT i = 0; i < this->numMorphs; ++i){
			MORPHS *pMorphs = &this->arrMorphs[i];

			WriteFile(hFile, &pMorphs->Ident, sizeof(UINT), NULL, NULL);
			WriteFile(hFile, &pMorphs->cbMesh, sizeof(UINT), NULL, NULL);
			WriteFile(hFile, &pMorphs->numMesh, sizeof(UINT), NULL, NULL);

			for(UINT j = 0; j < pMorphs->numMesh; ++j)
				WriteFile(hFile, pMorphs->arrMesh[j], pMorphs->cbMesh, NULL, NULL);
		}

		this->EndChunk(hFile, PrevCursor, LastCursor);
	}

	//关闭文件

	CloseHandle(hFile);
}
//开始文件块
VOID CExporter::StartChunk(HANDLE hFile, DWORD Type){
	UINT InitSize = 0;
	WriteFile(hFile, &Type, sizeof(DWORD), NULL, NULL);
	WriteFile(hFile, &InitSize, sizeof(UINT), NULL, NULL);
}
//结束文件块
VOID CExporter::EndChunk(HANDLE hFile, UINT &Start, UINT &End){
	Start = End + sizeof(UINT);
	End = GetFileSize(hFile, NULL);
	UINT ChunkSize = End - Start - sizeof(UINT);

	SetFilePointer(hFile, Start, NULL, FILE_BEGIN);
	WriteFile(hFile, &ChunkSize, sizeof(UINT), NULL, NULL);
	SetFilePointer(hFile, End, NULL, FILE_BEGIN);
}

//查找材质
MATERIAL* CExporter::GetMaterialByID(UINT Ident){
	for(UINT i = 0; i < this->numMtrl; ++i){
		if(this->arrMtrl[i].Ident == Ident)
			return &this->arrMtrl[i];
	}

	return NULL;
}
//查找材质
MATERIAL* CExporter::GetMaterialByName(MCHAR* Name){
	for(UINT i = 0; i < this->numMtrl; ++i){
		if(this->arrMtrl[i].Name == Name)
			return &this->arrMtrl[i];
	}

	return NULL;
}
//获取变形器
IGameMorpher* CExporter::GetMorpher(IGameObject* pObject){
	UINT ModCount = pObject->GetNumModifiers();
	for(UINT i = 0; i < ModCount; ++i){
		IGameModifier *pModifier = pObject->GetIGameModifier(i);
		if(pModifier->IsMorpher())
			return (IGameMorpher*)pModifier;
	}

	return NULL;
}
//选项面板处理
INT_PTR CALLBACK CExporter::PanelProc(HWND hWindow, UINT MsgCode, WPARAM Param0, LPARAM Param1){
	switch(MsgCode){
		case WM_INITDIALOG:{
			UINT CtlIdent;

			//解锁顶点选项

			DWORD arrOption0[] = {
				VERTEX_NORMAL, VERTEX_TANGENT, VERTEX_COLOR, VERTEX_TEXTURE, VERTEX_BONES };
			UINT numOption0 = sizeof(arrOption0) / sizeof(arrOption0[0]);

			CtlIdent = IDC_CHECK6;
			for(UINT i = 0; i < numOption0; ++i){
				HWND hCtrl = GetDlgItem(hWindow, CtlIdent++);

				if(gExporter.VtxFormat & arrOption0[i])
					SendMessage(hCtrl, BM_SETCHECK, 1, 0);
				else EnableWindow(hCtrl, FALSE);
			}

			//解锁资源选项

			DWORD arrOption1[] = { OPT_MESH, OPT_BONE, OPT_ANIM, OPT_MATERIAL, OPT_MORPH };
			UINT numOption1 = sizeof(arrOption1) / sizeof(arrOption1[0]);

			CtlIdent = IDC_CHECK1;
			for(UINT i = 0; i < numOption1; ++i){
				HWND hCtrl = GetDlgItem(hWindow, CtlIdent++);

				if(gExporter.ExpOption & arrOption1[i])
					SendMessage(hCtrl, BM_SETCHECK, 1, 0);
				else EnableWindow(hCtrl, FALSE);
			}

			//返回

			return TRUE;
		}
		case WM_COMMAND:{
			if(LOWORD(Param0) == IDOK){
				gExporter.ExpOption = 0x0;
				gExporter.VtxFormat = 0xffffffff;
				UINT CtlIdent = 0;

				//处理顶点选项

				DWORD arrOption0[] = {
					VERTEX_NORMAL, VERTEX_TANGENT, VERTEX_COLOR, VERTEX_TEXTURE, VERTEX_BONES };
				UINT numOption0 = sizeof(arrOption0) / sizeof(arrOption0[0]);

				CtlIdent = IDC_CHECK6;
				for(UINT i = 0; i < numOption0; ++i){
					HWND hCtrl = GetDlgItem(hWindow, CtlIdent++);
					LRESULT bChecked = SendMessage(hCtrl, BM_GETCHECK, 0, 0);
					if(!bChecked) gExporter.VtxFormat &= ~arrOption0[i];
				}

				//处理资源选项

				DWORD arrOption1[] = { OPT_MESH, OPT_BONE, OPT_ANIM, OPT_MATERIAL, OPT_MORPH };
				UINT numOption1 = sizeof(arrOption1) / sizeof(arrOption1[0]);

				CtlIdent = IDC_CHECK1;
				for(UINT i = 0; i < numOption1; ++i){
					HWND hCtrl = GetDlgItem(hWindow, CtlIdent++);
					LRESULT bChecked = SendMessage(hCtrl, BM_GETCHECK, 0, 0);
					if(bChecked) gExporter.ExpOption |= arrOption1[i];
				}

				//返回

				EndDialog(hWindow, IDOK);
			}

			if(LOWORD(Param0) == IDCANCEL)
				EndDialog(hWindow, IDCANCEL);

			return TRUE;
		}
		default: return FALSE;
	}
}

//导出场景
DLL_EXPORT VOID ExportScene(HINSTANCE hInstance, TCHAR* SavePath, DWORD Option){
	//设置坐标系

	IGameConversionManager *pConverter = GetConversionManager();
	pConverter->SetCoordSystem(IGameConversionManager::IGAME_D3D);

	//初始化场景

	IGameScene *pScene = GetIGameInterface();
	pScene->InitialiseIGame(Option & 0x1);
	pScene->SetStaticFrame(0);

	//导出和释放场景

	gInstance = hInstance;
	gExporter.Process(pScene);
	gExporter.WriteToFile(SavePath);
	gExporter.Finalize();
	pScene->ReleaseIGame();
}