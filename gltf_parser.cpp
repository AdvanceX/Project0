//****************************************//

#include "gltf_parser.h"

//****************************************//


//-------------------- 预留空间 --------------------//

#define BUF_SIZE 256
#define NAME_SIZE 32

//----------------------------------------//


//-------------------- ID获取 --------------------//

#define OBJ_ID(obj) StringHash(obj.psName)
#define OBJ_PID(obj) StringHash(obj->psName)

//----------------------------------------//


//-------------------- 辅助函数 --------------------//

USINT StringHash(_in CHAR8 *psInput){
	USINT Hash = 2166136261;
	const CHAR8 *pChar = psInput;

	while(*pChar){
		Hash *= 16777619;
		Hash ^= *pChar++;
	}

	return Hash;
}

$VOID ExitProgram(_in CHAR8 *psPrompt, ...){
	PVALIST pArgs;
	CHAR8 Text[BUF_SIZE];

	VA_START(pArgs, psPrompt);
	Avnprintf(Text, BUF_SIZE, psPrompt, pArgs);
	VA_END(pArgs);

	printf(Text);
	system("pause");
	exit(1);
}

//----------------------------------------//


//-------------------- CAnimation方法 --------------------//

$VOID CAnimAssy::Initialize(CJsonNode *pDomAnim, CGltfParser &Parser){
	$m.Duration = 0;
	$m.numElem = Parser.numBone;
	$m.lprgElem = new CAnimation[$m.numElem];

	CJsonNode *pDomSmpls = pDomAnim->GetChild("samplers");
	CJsonNode *pDomChans = pDomAnim->GetChild("channels");
	CJsonNode *pDomChan = pDomChans->GetChild();

	while(pDomChan){
		CJsonNode *pDomTarget = pDomChan->GetChild("target");
		CJsonNode *pDomSmpl = pDomSmpls->GetChild(pDomChan->GetMemI("sampler"));
		const CHAR8 *psType = pDomTarget->GetMemS("path");
		USINT NodeId = pDomTarget->GetMemI("node");
		USINT BoneId = Parser.lprgNode[NodeId].iRefBone;
		$m.lprgElem[BoneId].Initialize(psType, pDomSmpl, Parser);
		pDomChan = pDomChan->GetNext();
	}

	//统计动画时长

	CAnimation *pSubset = $m.lprgElem;
	CAnimation *pEndSub = pSubset + $m.numElem;

	for(; pSubset != pEndSub; ++pSubset){
		if($m.Duration < pSubset->Duration)
			$m.Duration = pSubset->Duration;
	}
}

$VOID CAnimation::Initialize(_in CHAR8 *psType, CJsonNode *pDomSmpl, CGltfParser &Parser){
	USINT BuffId0 = pDomSmpl->GetMemI("input");
	USINT BuffId1 = pDomSmpl->GetMemI("output");
	USINT FrameCnt = Parser.GetBufferSplit(BuffId0);

	SPFPN *prgTime = (SPFPN*)Parser.GetBuffer(BuffId0);
	XMFLOAT3 *prgTxform = (XMFLOAT3*)Parser.GetBuffer(BuffId1);
	XMFLOAT4 *prgRotation = (XMFLOAT4*)prgTxform;

	if(!MbsCmp(psType, "rotation")){
		$m.numRotation = FrameCnt;
		$m.lprgRotation = new ROTFRAME[FrameCnt];

		for(USINT iFrame = 0; iFrame < FrameCnt; ++iFrame){
			$m.lprgRotation[iFrame].Time = prgTime[iFrame];
			$m.lprgRotation[iFrame].Txform = prgRotation[iFrame];
		}
	} else if(!MbsCmp(psType, "scale")){
		$m.numScaling = FrameCnt;
		$m.lprgScaling = new ANIMFRAME[FrameCnt];

		for(USINT iFrame = 0; iFrame < FrameCnt; ++iFrame){
			$m.lprgScaling[iFrame].Time = prgTime[iFrame];
			$m.lprgScaling[iFrame].Txform = prgTxform[iFrame];
		}
	} else if(!MbsCmp(psType, "translation")){
		$m.numTrans = FrameCnt;
		$m.lprgTrans = new ANIMFRAME[FrameCnt];

		for(USINT iFrame = 0; iFrame < FrameCnt; ++iFrame){
			$m.lprgTrans[iFrame].Time = prgTime[iFrame];
			$m.lprgTrans[iFrame].Txform = prgTxform[iFrame];
		}
	}

	if($m.Duration < prgTime[FrameCnt - 1])
		$m.Duration = prgTime[FrameCnt - 1];
}

//----------------------------------------//


//-------------------- CModel方法 --------------------//

$VOID CModel::Initialize(CJsonNode *pDomMesh, CGltfParser &Parser){
	CJsonNode *pDomName = pDomMesh->GetChild("name");
	CJsonNode *pDomPrims = pDomMesh->GetChild("primitives");

	$m.psName = (CHAR8*)pDomName->GetValS();
	$m.numSubset = pDomPrims->GetSize();
	$m.lprgSubset = new SUBMESH[$m.numSubset];

	$m.InitIndexBuffer(pDomPrims, Parser.lprgBufView);
	$m.InitVertexBuffer(pDomPrims, Parser.lprgBufView);
	$m.InitSubmeshs(pDomPrims, Parser);
	$m.InitVertexs(pDomPrims, Parser);
}

$VOID CModel::InitVertexs(CJsonNode *pDomPrims, CGltfParser &Parser){
	BYTET *psVtxBuff = $m.lpsVtxBuff;
	VERTVIEW VtxView($m.VtxFormat);

	USINT rgOffset[] = {
		VtxView.hCoord, VtxView.hNormal, VtxView.hTangent, VtxView.hColor,
		VtxView.hBoneIndices, VtxView.hBoneWeights, VtxView.hTexCoord };
	USINT rgStride[] = {
		sizeof(XMFLOAT3), sizeof(XMFLOAT3), sizeof(XMFLOAT3), sizeof(COLORREF),
		sizeof(UINT8[4]), sizeof(SPFPN[3]), sizeof(XMFLOAT2) };
	CHAR8 *rgAttrId[] = {
		"POSITION", "NORMAL", "TANGENT", "COLOR_0",
		"JOINTS_0", "WEIGHTS_0", "TEXCOORD_0" };

	USINT AttrCnt = _ARRAYSIZE(rgAttrId);
	CJsonNode *pDomPrim = pDomPrims->GetChild();

	while(pDomPrim){
		CJsonNode *pDomAttrs = pDomPrim->GetChild("attributes");
		USINT BufIndex = pDomAttrs->GetMemI("POSITION");
		USINT VtxCount = Parser.GetBufferSplit(BufIndex);

		for(USINT iAttr = 0; iAttr < AttrCnt; ++iAttr){
			CHAR8 *psAttrId = rgAttrId[iAttr];
			CJsonNode *pDomAttr = pDomAttrs->GetChild(psAttrId);
			if(!pDomAttr) continue;

			BYTET *psSrcData = Parser.GetBuffer(pDomAttr->GetValI());
			BYTET *psDstData = psVtxBuff + rgOffset[iAttr];

			/*if(!MbsCmp(psAttrId, "POSITION") || !MbsCmp(psAttrId, "NORMAL")){
				for(USINT iVtx = 0; iVtx < VtxCount; ++iVtx){
					XMFLOAT3 *pDstVert = (XMFLOAT3*)psDstData;
					*pDstVert = *(XMFLOAT3*)psSrcData;
					pDstVert->z = -pDstVert->z;
					psSrcData += rgStride[iAttr];
					psDstData += VtxView.cbVertex;
				}
			} else */if(!MbsCmp(psAttrId, "COLOR_0")){
				for(USINT iVtx = 0; iVtx < VtxCount; ++iVtx){
					*(COLORREF*)psDstData = *(COLOR64*)psSrcData;
					psSrcData += sizeof(COLOR64);
					psDstData += VtxView.cbVertex;
				}
			} else if(!MbsCmp(psAttrId, "JOINTS_0")){
				for(USINT iVtx = 0; iVtx < VtxCount; ++iVtx){
					psDstData[0] = Parser.rgBoneRig[psSrcData[0]];
					psDstData[1] = Parser.rgBoneRig[psSrcData[1]];
					psDstData[2] = Parser.rgBoneRig[psSrcData[2]];
					psDstData[3] = Parser.rgBoneRig[psSrcData[3]];
					psSrcData += rgStride[iAttr];
					psDstData += VtxView.cbVertex;
				}
			} else if(!MbsCmp(psAttrId, "WEIGHTS_0")){
				for(USINT iVtx = 0; iVtx < VtxCount; ++iVtx){
					BStr_Move(psDstData, psSrcData, rgStride[iAttr]);
					psSrcData += rgStride[iAttr] + sizeof(SPFPN); //略过最后一个权重
					psDstData += VtxView.cbVertex;
				}
			} else{
				for(USINT iVtx = 0; iVtx < VtxCount; ++iVtx){
					BStr_Move(psDstData, psSrcData, rgStride[iAttr]);
					psSrcData += rgStride[iAttr];
					psDstData += VtxView.cbVertex;
				}
			}
		}

		psVtxBuff += VtxCount * VtxView.cbVertex;
		pDomPrim = pDomPrim->GetNext();
	}
}

$VOID CModel::InitSubmeshs(CJsonNode *pDomPrims, CGltfParser &Parser){
	USINT IdxStart = 0;
	BYTET *psIdxDest = $m.lpsIdxBuff;
	SUBMESH *pSubmesh = $m.lprgSubset;
	MATERIAL *prgMatl = Parser.lprgMatl;
	CJsonNode *pDomPrim = pDomPrims->GetChild();

	while(pDomPrim){
		CJsonNode *pDomIbv = pDomPrim->GetChild("indices");
		CJsonNode *pDomMatl = pDomPrim->GetChild("material");

		USINT IbvIndex = pDomIbv->GetValI();
		USINT MtlIndex = pDomMatl ? pDomMatl->GetValI() : UINT_MAX;

		BYTET *psIdxSrc = Parser.GetBuffer(IbvIndex);
		BUFVIEW *pIBView = Parser.lprgBufView + IbvIndex;

		BStr_Move(psIdxDest, psIdxSrc, pIBView->BufSize);

		pSubmesh->IdxStart = IdxStart;
		pSubmesh->IdxCount = pIBView->UnitCnt;
		pSubmesh->MtlIdent = pDomMatl ? prgMatl[MtlIndex].Ident : UINT_MAX;

		psIdxDest += pIBView->BufSize;
		IdxStart += pIBView->UnitCnt;
		pDomPrim = pDomPrim->GetNext();
		pSubmesh++;
	}

	//无材质时无需子网格数据

	if(($m.numSubset == 1) && ($m.lprgSubset[0].MtlIdent == UINT_MAX)){
		$m.numSubset = 0;
		SAFE_DELETEA($m.lprgSubset);
	}
}

$VOID CModel::InitIndexBuffer(CJsonNode *pDomPrims, _in BUFVIEW *prgBufView){
	USINT ViewId = 0, BufSize = 0;
	CJsonNode *pDomPrim = pDomPrims->GetChild();

	while(pDomPrim){
		ViewId = pDomPrim->GetMemI("indices");
		BufSize += prgBufView[ViewId].BufSize;
		pDomPrim = pDomPrim->GetNext();
	}

	DWORD Format = prgBufView[ViewId].ArithType;
	BOOL bBit16 = (Format == SCALAR_TYPE_UInt16);

	$m.lpsIdxBuff = new BYTET[BufSize];
	$m.cbIdxBuff = BufSize;
	$m.IdxFormat = prgBufView[ViewId].ArithType;
	$m.IdxFormat = bBit16 ? IDX_FMT_16bit : IDX_FMT_32bit;
}

$VOID CModel::InitVertexBuffer(CJsonNode *pDomPrims, _in BUFVIEW *prgBufView){
	//顶点格式

	DWORD VtxFormat = 0L;
	CJsonNode *pDomPrim = pDomPrims->GetChild();
	CJsonNode *pDomAttrs = pDomPrim->GetChild("attributes");

	if(pDomAttrs->GetChild("POSITION"))
		VtxFormat |= VFB_Coord;
	if(pDomAttrs->GetChild("NORMAL"))
		VtxFormat |= VFB_Normal;
	if(pDomAttrs->GetChild("TANGENT"))
		VtxFormat |= VFB_Tangent;
	if(pDomAttrs->GetChild("COLOR_0"))
		VtxFormat |= VFB_Color;
	if(pDomAttrs->GetChild("JOINTS_0"))
		VtxFormat |= VFB_BoneRig;
	if(pDomAttrs->GetChild("TEXCOORD_0"))
		VtxFormat |= VFB_TexCoord;

	//缓存大小

	USINT ViewId, BufSize = 0;
	USINT VtxStride = VERTVIEW(VtxFormat).cbVertex;

	while(pDomPrim){
		pDomAttrs = pDomPrim->GetChild("attributes");
		ViewId = pDomAttrs->GetMemI("POSITION");
		BufSize += prgBufView[ViewId].UnitCnt * VtxStride;
		pDomPrim = pDomPrim->GetNext();
	}

	//缓存区

	$m.cbVtxBuff = BufSize;
	$m.VtxFormat = VtxFormat;
	$m.lpsVtxBuff = new BYTET[BufSize];
}

//----------------------------------------//


//-------------------- CGltfParser方法 --------------------//

$VOID CGltfParser::Process(_in CHAR8 *psSrcPath){
	CHAR8 DstPath[MAX_PATH];
	WCHAR *psSrcPathW = Utf8ToUtf16(psSrcPath);
	CJsonNode *lpDomRoot = CJsonNode::CreateByFile(psSrcPathW);

	MbsCpy(DstPath, psSrcPath);
	MbsCpy(MbsRchr(DstPath, '.'), ".dat");

	$m.ParseBuffers(lpDomRoot);
	$m.ParseBufferViews(lpDomRoot);
	$m.ParseNodes(lpDomRoot);
	$m.ParseTextures(lpDomRoot);
	$m.ParseMaterials(lpDomRoot);
	$m.ParseSkin(lpDomRoot);
	$m.ParseAnimations(lpDomRoot);
	$m.ParseModels(lpDomRoot);
	$m.WriteToFile(DstPath);

	lpDomRoot->Release();
}

$VOID CGltfParser::ParseSkin(CJsonNode *pDomRoot){
	CJsonNode *pDomSkins = pDomRoot->GetChild("skins");
	if(!pDomSkins) return;

	CJsonNode *pDomSkin = pDomSkins->GetChild(); //只解析第一个蒙皮
	CJsonNode *pDomJoint = pDomSkin->GetChild("joints")->GetChild(); //根骨骼

	USINT iMatBuff = pDomSkin->GetMemI("inverseBindMatrices");
	XMFLOAT4X4 *rgInvMat = (XMFLOAT4X4*)$m.GetBuffer(iMatBuff);

	$m.ParseSkeleton(&$m.lprgNode[pDomJoint->GetValI()]);

	for(USINT iJoint = 0; pDomJoint; ++iJoint){
		USINT iNode = pDomJoint->GetValI();
		USINT iBone = $m.lprgNode[iNode].iRefBone;
		$m.lprgBone[iBone].pOffsetMat = &rgInvMat[iJoint];
		$m.rgBoneRig[iJoint] = iBone;
		pDomJoint = pDomJoint->GetNext();
	}
}

$VOID CGltfParser::ParseNodes(CJsonNode *pDomRoot){
	NODE *pNode;
	CJsonNode *pDomNode, *pDomNodes;
	CJsonNode *pDomVec;

	pDomNodes = pDomRoot->GetChild("nodes");
	if(!pDomNodes) return;

	$m.numNode = pDomNodes->GetSize();
	$m.lprgNode = new NODE[$m.numNode];

	pNode = $m.lprgNode;
	pDomNode = pDomNodes->GetChild();

	while(pDomNode){
		//节点名

		pNode->numChild = 0;
		pNode->psName = (CHAR8*)pDomNode->GetMemS("name");

		//子节点

		if(pDomNodes = pDomNode->GetChild("children")){
			pNode->numChild = pDomNodes->GetSize();
			pDomNodes->GetArrVal16(pNode->rgChild);
		}

		//初始矩阵

		XMVECTOR vecTxform = g_XMZero;
		XMMATRIX matTxform = XMMatrixIdentity();

		if(pDomVec = pDomNode->GetChild("translation")){
			pDomVec->GetArrValF32(vecTxform.m128_f32);
			matTxform = XMMatrixTranslationFromVector(vecTxform);
		}
		if(pDomVec = pDomNode->GetChild("rotation")){
			pDomVec->GetArrValF32(vecTxform.m128_f32);
			matTxform *= XMMatrixRotationQuaternion(vecTxform);
		}
		if(pDomVec = pDomNode->GetChild("scale")){
			pDomVec->GetArrValF32(vecTxform.m128_f32);
			matTxform *= XMMatrixScalingFromVector(vecTxform);
		}

		XMStoreFloat4x4(&pNode->matInitial, matTxform);

		//下一个

		pNode++;
		pDomNode = pDomNode->GetNext();
	}
}

$VOID CGltfParser::ParseModels(CJsonNode *pDomRoot){
	CModel *pModel;
	CJsonNode *pDomMesh, *pDomMeshes;

	pDomMeshes = pDomRoot->GetChild("meshes");
	if(!pDomMeshes) return;

	$m.numModel = pDomMeshes->GetSize();
	$m.lprgModel = new CModel[$m.numModel];

	pModel = $m.lprgModel;
	pDomMesh = pDomMeshes->GetChild();

	while(pDomMesh){
		pModel->Initialize(pDomMesh, *this);
		pDomMesh = pDomMesh->GetNext();
		pModel++;
	}
}

$VOID CGltfParser::ParseBuffers(CJsonNode *pDomRoot){
	HANDLE hrSrcFile;
	USINT BufSize;
	CHAR8 **ppBuffer, *psSrcUri;
	CJsonNode *pDomBuff, *pDomBuffs;

	pDomBuffs = pDomRoot->GetChild("buffers");
	if(!pDomBuffs) return;

	$m.numBuffer = pDomBuffs->GetSize();
	$m.lprgBuffer = new BYTET*[$m.numBuffer];

	pDomBuff = pDomBuffs->GetChild();
	ppBuffer = (CHAR8**)$m.lprgBuffer;

	while(pDomBuff){
		BufSize = pDomBuff->GetMemI("byteLength");
		psSrcUri = (CHAR8*)pDomBuff->GetMemS("uri");
		*ppBuffer = new CHAR8[BufSize];

		hrSrcFile = CreateFile2(Utf8ToUtf16(psSrcUri), GENERIC_READ, 0L, OPEN_EXISTING, P_Null);
		ReadFile(hrSrcFile, *ppBuffer, BufSize, P_Null, P_Null);
		CloseHandle(hrSrcFile);

		pDomBuff = pDomBuff->GetNext();
		ppBuffer++;
	}
}

$VOID CGltfParser::ParseTextures(CJsonNode *pDomRoot){
	CJsonNode *pDomTex, *pDomTexs;
	CJsonNode *pDomImg, *pDomImgs;
	CJsonNode *pDomSmplr, *pDomSmplrs;
	CHAR8 *psImgName, ImgPath[MAX_PATH];

	pDomTexs = pDomRoot->GetChild("textures");
	pDomImgs = pDomRoot->GetChild("images");
	pDomSmplrs = pDomRoot->GetChild("samplers");

	if(!pDomTexs || !pDomImgs) return;

	$m.numTexture = pDomTexs->GetSize();
	$m.lprgTexView = new TEXVIEW[$m.numTexture];

	for(USINT iTex = 0; iTex < $m.numTexture; ++iTex){
		pDomTex = pDomTexs->GetChild(iTex);
		pDomImg = pDomImgs->GetChild(pDomTex->GetMemI("source"));
		pDomSmplr = pDomSmplrs->GetChild(pDomTex->GetMemI("sampler"));

		MbsCpy(ImgPath, pDomImg->GetMemS("uri"));
		*MbsRchr(ImgPath, '.') = '\0';

		psImgName = MbsRchr(ImgPath, '/');
		if(!psImgName) psImgName = ImgPath;

		$m.lprgTexView[iTex].TexId = StringHash(psImgName);
		$m.lprgTexView[iTex].AddrMode = $m.GetAddressMode(pDomSmplr);
	}
}

$VOID CGltfParser::ParseMaterials(CJsonNode *pDomRoot){
	MATERIAL *pMatl;
	CJsonNode *pDomMatl, *pDomMatls;

	pDomMatls = pDomRoot->GetChild("materials");
	if(!pDomMatls) return;

	$m.numMatl = pDomMatls->GetSize();
	$m.lprgMatl = new MATERIAL[$m.numMatl];

	pMatl = $m.lprgMatl;
	pDomMatl = pDomMatls->GetChild();

	while(pDomMatl){
		$m.ParseMaterial(pDomMatl, *pMatl);
		pDomMatl = pDomMatl->GetNext();
		pMatl++;
	}
}

$VOID CGltfParser::ParseAnimations(CJsonNode *pDomRoot){
	CAnimAssy *pAnim;
	CJsonNode *pDomAnim, *pDomAnims;

	pDomAnims = pDomRoot->GetChild("animations");
	if(!pDomAnims) return;

	$m.numAnim = pDomAnims->GetSize();
	$m.lprgAnim = new CAnimAssy[$m.numAnim];

	pAnim = $m.lprgAnim;
	pDomAnim = pDomAnims->GetChild();

	while(pDomAnim){
		pAnim->Initialize(pDomAnim, *this);
		pDomAnim = pDomAnim->GetNext();
		pAnim++;
	}
}

$VOID CGltfParser::ParseBufferViews(CJsonNode *pDomRoot){
	USINT ViewId;
	BUFVIEW *pBufView;
	CJsonNode *pDomView, *pDomViews;
	CJsonNode *pDomAcsr, *pDomAcsrs;

	pDomAcsrs = pDomRoot->GetChild("accessors");
	pDomViews = pDomRoot->GetChild("bufferViews");

	if(!pDomAcsrs || !pDomViews) return;

	$m.numBufView = pDomAcsrs->GetSize();
	$m.lprgBufView = new BUFVIEW[$m.numBufView];

	pBufView = $m.lprgBufView;
	pDomAcsr = pDomAcsrs->GetChild();

	while(pDomAcsr){
		ViewId = pDomAcsr->GetMemI("bufferView");
		pDomView = pDomViews->GetChild(ViewId);

		pBufView->BuffId = pDomView->GetMemI("buffer");
		pBufView->BufSize = pDomView->GetMemI("byteLength");
		pBufView->Offset = pDomView->GetMemI("byteOffset");
		pBufView->UnitCnt = pDomAcsr->GetMemI("count");
		pBufView->ArithType = pDomAcsr->GetMemI("componentType");

		pBufView++;
		pDomAcsr = pDomAcsr->GetNext();
	}
}

$VOID CGltfParser::ParseMaterial(CJsonNode *pDomMatl, _out MATERIAL &rMatl){
	CJsonNode *pDomPbr;
	CJsonNode *pDomProp;

	MATBASE &rBase = rMatl.Base;
	MATMAPS &rMaps = rMatl.Maps;

	rMatl.Ident = StringHash(pDomMatl->GetMemS("name"));
	pDomPbr = pDomMatl->GetChild("pbrMetallicRoughness");

	if(pDomProp = pDomMatl->GetChild("emissiveFactor"))
		pDomProp->GetArrValF32((SPFPN*)&rBase.Emission);
	if(pDomProp = pDomPbr->GetChild("baseColorFactor"))
		pDomProp->GetArrValF32((SPFPN*)&rBase.Albedo);
	if(pDomProp = pDomPbr->GetChild("roughnessFactor"))
		rBase.Roughness = pDomProp->GetValF32();
	if(pDomProp = pDomPbr->GetChild("metallicFactor"))
		rBase.Metalness = pDomProp->GetValF32();

	if(pDomProp = pDomPbr->GetChild("baseColorTexture")){
		TEXVIEW *pView = $m.lprgTexView + pDomProp->GetMemI("index");
		rMaps.iAlbedo = pView->TexId;
		rBase.TexAttrs.y = pView->AddrMode;
		rBase.TexAttrs.x |= ASSET_TYPE_ColorMap;
	}
	if(pDomProp = pDomPbr->GetChild("metallicRoughnessTexture")){
		TEXVIEW *pView = $m.lprgTexView + pDomProp->GetMemI("index");
		rMaps.iMetalRg = pView->TexId;
		rBase.TexAttrs.y = pView->AddrMode;
		rBase.TexAttrs.x |= ASSET_TYPE_RoughMap;
	}
	if(pDomProp = pDomMatl->GetChild("normalTexture")){
		TEXVIEW *pView = $m.lprgTexView + pDomProp->GetMemI("index");
		rMaps.iNormal = pView->TexId;
		rBase.TexAttrs.y = pView->AddrMode;
		rBase.TexAttrs.x |= ASSET_TYPE_NormMap;
	}
	if(pDomProp = pDomMatl->GetChild("emissiveTexture")){
		TEXVIEW *pView = $m.lprgTexView + pDomProp->GetMemI("index");
		rMaps.iEmission = pView->TexId;
		rBase.TexAttrs.y = pView->AddrMode;
		rBase.TexAttrs.x |= ASSET_TYPE_EmissMap;
	}
	if(pDomProp = pDomMatl->GetChild("occlusionTexture")){
		TEXVIEW *pView = $m.lprgTexView + pDomProp->GetMemI("index");
		rMaps.iOcclusion = pView->TexId;
		rBase.TexAttrs.y = pView->AddrMode;
		rBase.TexAttrs.x |= ASSET_TYPE_OcclusMap;
	}
}

$VOID CGltfParser::ParseSkeleton(_in NODE *pRoot){
	//查找所有骨骼节点

	struct NODEEX{
		USINT Level;
		USINT iParent;
		NODE *pNode;
	};

	STLVector<BONE> BoneList;
	STLDeque<NODEEX> NodeQue;

	NodeQue.push_back({ 0, UINT_MAX, (NODE*)pRoot });
	while(!NodeQue.empty()){
		NODEEX Node = NodeQue.front();
		NODEEX SubNode = { Node.Level + 1, OBJ_PID(Node.pNode) };
		BONE Bone = { Node.Level, Node.iParent, Node.pNode };

		NodeQue.pop_front();
		BoneList.push_back(Bone);

		for(USINT iChild = 0; iChild < Node.pNode->numChild; ++iChild){
			USINT iNode = Node.pNode->rgChild[iChild];
			SubNode.pNode = $m.lprgNode + iNode;
			NodeQue.push_back(SubNode);
		}
	}

	if(BoneList.size() > 256) ExitProgram("Error: More than 256 bone nodes\n");
	std::sort(BoneList.begin(), BoneList.end(), BONE::IsLess);

	//保存骨骼

	STLMap<USINT, USINT> mapIdToSeq;
	mapIdToSeq.insert({ OBJ_ID(BoneList[0]), 0 });

	$m.numBone = (USINT)BoneList.size();
	$m.lprgBone = new BONE[$m.numBone];

	$m.lprgBone[0] = BoneList[0];
	$m.lprgBone[0].pRefNode->iRefBone = 0;

	for(USINT iBone = 1; iBone < $m.numBone; ++iBone){
		BONE *pBone = $m.lprgBone + iBone;
		*pBone = BoneList[iBone];
		pBone->iParent = mapIdToSeq[pBone->iParent];
		pBone->pRefNode->iRefBone = iBone;
		mapIdToSeq.insert({ OBJ_PID(pBone), iBone });
	}
}

$VOID CGltfParser::WriteToFile(_in CHAR8 *psPath){
	USINT Cursor = 0;
	HANDLE hrFile = CreateFileA(psPath, GENERIC_WRITE, 0b0,
		P_Null, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, P_Null);

	if($m.numModel > 0){
		$m.StartChunk(hrFile, ASSET_TYPE_Model);
		$m.WriteModels(hrFile);
		$m.EndChunk(hrFile, Cursor);
	}
	if($m.numMatl > 0){
		$m.StartChunk(hrFile, ASSET_TYPE_Material);
		$m.WriteMaterials(hrFile);
		$m.EndChunk(hrFile, Cursor);
	}
	if($m.numBone > 0){
		$m.StartChunk(hrFile, ASSET_TYPE_Skeleton);
		$m.WriteSkeleton(hrFile);
		$m.EndChunk(hrFile, Cursor);
	}
	if($m.numAnim > 0){
		$m.StartChunk(hrFile, ASSET_TYPE_Anim);
		$m.WriteAnimations(hrFile);
		$m.EndChunk(hrFile, Cursor);
	}

	CloseHandle(hrFile);
}

$VOID CGltfParser::WriteModels(HANDLE hFile){
	WriteFile(hFile, &$m.numModel, sizeof(USINT), P_Null, P_Null);

	for(USINT iMdl = 0; iMdl < $m.numModel; ++iMdl){
		CModel &rModel = $m.lprgModel[iMdl];
		USINT NameLen = (USINT)MbsLen(rModel.psName);

		WriteFile(hFile, &NameLen, sizeof(USINT), P_Null, P_Null);
		WriteFile(hFile, rModel.psName, NameLen, P_Null, P_Null);
		WriteFile(hFile, &rModel.IdxFormat, sizeof(DWORD), P_Null, P_Null);
		WriteFile(hFile, &rModel.VtxFormat, sizeof(DWORD), P_Null, P_Null);
		WriteFile(hFile, &rModel.cbIdxBuff, sizeof(USINT), P_Null, P_Null);
		WriteFile(hFile, &rModel.cbVtxBuff, sizeof(USINT), P_Null, P_Null);
		WriteFile(hFile, &rModel.numSubset, sizeof(USINT), P_Null, P_Null);
		WriteFile(hFile, rModel.lpsIdxBuff, rModel.cbIdxBuff, P_Null, P_Null);
		WriteFile(hFile, rModel.lpsVtxBuff, rModel.cbVtxBuff, P_Null, P_Null);

		if(rModel.numSubset > 0){
			USINT BufSize = rModel.numSubset * sizeof(SUBMESH);
			WriteFile(hFile, rModel.lprgSubset, BufSize, P_Null, P_Null);
		}
	}
}

$VOID CGltfParser::WriteSkeleton(HANDLE hFile){
	WriteFile(hFile, &$m.numBone, sizeof(USINT), P_Null, P_Null);

	for(USINT iBone = 0; iBone < $m.numBone; ++iBone){
		BONE &rBone = $m.lprgBone[iBone];
		USINT NameLen = (USINT)MbsLen(rBone.psName);

		WriteFile(hFile, &NameLen, sizeof(USINT), P_Null, P_Null);
		WriteFile(hFile, rBone.psName, NameLen, P_Null, P_Null);
		WriteFile(hFile, &rBone.iParent, sizeof(USINT), P_Null, P_Null);
		WriteFile(hFile, rBone.pOffsetMat, sizeof(XMFLOAT4X4), P_Null, P_Null);
		WriteFile(hFile, rBone.pInitialMat, sizeof(XMFLOAT4X4), P_Null, P_Null);
	}
}

$VOID CGltfParser::WriteMaterials(HANDLE hFile){
	USINT BufSize = $m.numMatl * sizeof(MATERIAL);
	WriteFile(hFile, &$m.numMatl, sizeof(USINT), P_Null, P_Null);
	WriteFile(hFile, $m.lprgMatl, BufSize, P_Null, P_Null);
}

$VOID CGltfParser::WriteAnimations(HANDLE hFile){
	CAnimAssy *pAssy = $m.lprgAnim;
	CAnimation *pAnim = pAssy->lprgElem;
	CAnimation *pEndAnim = pAnim + pAssy->numElem;

	WriteFile(hFile, &pAssy->Duration, sizeof(SPFPN), P_Null, P_Null);
	WriteFile(hFile, &pAssy->numElem, sizeof(USINT), P_Null, P_Null);

	for(; pAnim != pEndAnim; ++pAnim){
		USINT BufSize0 = pAnim->numTrans * sizeof(ANIMFRAME);
		USINT BufSize1 = pAnim->numScaling * sizeof(ANIMFRAME);
		USINT BufSize2 = pAnim->numRotation * sizeof(ROTFRAME);

		WriteFile(hFile, &pAnim->numTrans, sizeof(USINT), P_Null, P_Null);
		WriteFile(hFile, &pAnim->numScaling, sizeof(USINT), P_Null, P_Null);
		WriteFile(hFile, &pAnim->numRotation, sizeof(USINT), P_Null, P_Null);

		WriteFile(hFile, pAnim->lprgTrans, BufSize0, P_Null, P_Null);
		WriteFile(hFile, pAnim->lprgScaling, BufSize1, P_Null, P_Null);
		WriteFile(hFile, pAnim->lprgRotation, BufSize2, P_Null, P_Null);
	}
}

$VOID CGltfParser::StartChunk(HANDLE hFile, DWORD Type){
	CHUNKHEAD Header = { Type, 0 };
	WriteFile(hFile, &Header, sizeof(CHUNKHEAD), P_Null, P_Null);
}

$VOID CGltfParser::EndChunk(HANDLE hFile, _io USINT &rCursor){
	USINT ChunkEnd = GetFileSize(hFile, P_Null);
	USINT DataSize = ChunkEnd - rCursor - sizeof(CHUNKHEAD);

	SetFilePointer(hFile, rCursor + sizeof(DWORD), P_Null, FILE_BEGIN);
	WriteFile(hFile, &DataSize, sizeof(USINT), P_Null, P_Null);
	SetFilePointer(hFile, ChunkEnd, P_Null, FILE_BEGIN);

	rCursor = ChunkEnd; //下一个文件块
}

BYTET *CGltfParser::GetBuffer(USINT ViewId){
	BUFVIEW *pView = $m.lprgBufView + ViewId;
	BYTET *psBuffer = $m.lprgBuffer[pView->BuffId];
	return psBuffer + pView->Offset;
}

USINT CGltfParser::GetBufferSplit(USINT ViewId){
	return $m.lprgBufView[ViewId].UnitCnt;
}

DWORD CGltfParser::GetAddressMode(CJsonNode *pDomSmplr){
	enum D3DADDRMODE{
		D3DAM_Wrap = 1L,
		D3DAM_Mirror = 2L,
		D3DAM_Clamp = 3L,
		D3DAM_Border = 4L,
		D3DAM_Custom = 16L,
	};

	CJsonNode *pDomWrapU = pDomSmplr->GetChild("wrapS");
	CJsonNode *pDomWrapV = pDomSmplr->GetChild("wrapT");

	DWORD WrapModeU = pDomWrapU ? pDomWrapU->GetValI() : 0L;
	DWORD WrapModeV = pDomWrapV ? pDomWrapV->GetValI() : 0L;
	DWORD WrapMode = (WrapModeU == WrapModeV) ? WrapModeU : 0L;

	switch(WrapMode){
		case ADDR_MODE_Clamp: return D3DAM_Clamp;
		case ADDR_MODE_Repeat: return D3DAM_Wrap;
		case ADDR_MODE_Mirror: return D3DAM_Mirror;
	}

	return D3DAM_Custom;
}

//----------------------------------------//


//-------------------- 主函数 --------------------//

INT32 main(INT32 numArg, CHAR8 **prgArg){
	for(UPINT iArg = 1; iArg < numArg; ++iArg){
		CGltfParser Parser;
		Parser.Process(prgArg[iArg]);
	}
	return 0;
}

//----------------------------------------//


// printf("%d\n");
// ./tool/CGltfParser.exe "./temp/gltf/xxx.gltf"