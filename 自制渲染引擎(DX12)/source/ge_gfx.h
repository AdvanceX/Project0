//--------------------资源类型--------------------//

#define ASSET_MESH     0x010000
#define ASSET_BONE     0x020000
#define ASSET_ANIM     0x040000
#define ASSET_MATERIAL 0x080000
#define ASSET_MORPHS   0x100000
#define ASSET_PARTICLE 0x200000

//----------------------------------------//


//--------------------索引--------------------//

//二元索引
typedef USRT INDEX2[2];
//二元索引
typedef UINT INDEX2X[2];

//三元索引
typedef USRT INDEX3[3];
//三元索引
typedef UINT INDEX3X[3];

//----------------------------------------//


//--------------------顶点/索引格式--------------------//

//顶点格式
typedef DWORD VERTFORMAT;

//索引格式
typedef DXGIFormat INDEXFORMAT;

//----------------------------------------//


//--------------------灯光/材质--------------------//

//灯光
struct LIGHT{
	VECTOR3 Intensity; //!用于全部灯光
	SPFP SpotPower;    //!用于聚光灯
	VECTOR3 Position;  //!用于点光/聚光灯
	SPFP AttStart;     //!用于点光/聚光灯
	VECTOR3 Direction; //!用于方向光/聚光灯	
	SPFP AttEnd;       //!用于点光/聚光灯
};
//材质
struct MATERIAL{
	VECTOR3 Albedo;  //漫反射反照率
	SPFP Opacity;    //不透明度
	VECTOR3 Fresnel; //菲涅尔反射率
	SPFP Shininess;  //光滑度
};

//----------------------------------------//


//--------------------着色器缓存--------------------//

//场景缓存结构
struct CBSCENE{
	VECTOR3 AmbientLight;
	UINT LightCount;
	BOOL bCartoonShade;
	VECTOR2 ClipEdge;
	SPFP FogStart;
	SPFP FogRange;
	VECTOR3 FogColor;
};
//帧缓存结构
struct CBFRAME{
	MATRIX matViewProj;
	VECTOR3 CameraPos;
	SPFP TimeSpan;
};
//对象缓存结构
struct CBOBJECT{
	MATRIX matWorld;    //!世界矩阵,用于顶点变换
	MATRIX matInvWorld; //!逆转的世界矩阵,用于法线变换
	MATRIX matTexAnim;  //!纹理动画矩阵,用于纹理坐标变换
};

//----------------------------------------//


//--------------------相机--------------------//

//相机
class CCamera{
	open XVECTOR Front; //前方向
	open XVECTOR Right; //右方向
	open XVECTOR vecUp; //上方向
	open XVECTOR Position; //位置
	open XMATRIX matView; //观察矩阵
	open XMATRIX matProj; //投影矩阵

	//构造//
	open CCamera(){
		MemZero(this, sizeof(*this));
	}
	//构造(位置,视场角,投影宽高比,近裁剪面,远裁剪面)//
	open CCamera(VECTOR3* pPosition, SPFP FOVAngle, SPFP AspectRatio, SPFP NearZ, SPFP FarZ){
		self.Initialize(*pPosition, FOVAngle, AspectRatio, NearZ, FarZ);
	}
	//预置(位置,视场角,投影宽高比,近裁剪面,远裁剪面)
	open VOID Initialize(VECTOR3 &Position, SPFP FOVAngle, SPFP AspectRatio, SPFP NearZ, SPFP FarZ){
		self.Position = PackV3(Position);

		self.Front = dx::g_XMIdentityR2;
		self.vecUp = dx::g_XMIdentityR1;
		self.Right = dx::g_XMIdentityR0;

		self.matView = MatIdentity();
		self.matProj = MatPerspect(FOVAngle / AspectRatio, AspectRatio, NearZ, FarZ);
	}
	//观测(位置,目标,上方向)
	open VOID LookAt(VECTOR3 &Position, VECTOR3 &Target, VECTOR3 &vecUp){
		self.Position = PackV3(Position);
		self.Front = VecSub(PackV3(Target), self.Position);
		self.Front = Vec3Normalize(self.Front);
		self.Right = Vec3Cross(PackV3(vecUp), self.Front);
		self.Right = Vec3Normalize(self.Right);
		self.vecUp = Vec3Cross(self.Front, self.Right);
		self.matView = MatLookTo(self.Position, self.Front, self.vecUp);
	}
	//更新
	open VOID Update(){
		self.Front = Vec3Normalize(self.Front);
		self.vecUp = Vec3Cross(self.Front, self.Right);
		self.vecUp = Vec3Normalize(self.vecUp);
		self.Right = Vec3Cross(self.vecUp, self.Front);
		self.Right = Vec3Normalize(self.Right);
		self.matView = MatLookTo(self.Position, self.Front, self.vecUp);
	}
	//升降(位移)
	open VOID Fly(SPFP Dist){
		using namespace dx;
		self.Position += self.vecUp * Dist;
	}
	//进退(位移)
	open VOID Walk(SPFP Dist){
		using namespace dx;
		self.Position += self.Front * Dist;
	}
	//扫视(位移)
	open VOID Strafe(SPFP Dist){
		using namespace dx;
		self.Position += self.Right * Dist;
	}
	//偏航(角度)
	open VOID Yaw(SPFP Angle){
		XMATRIX matTrans = MatRotateAxis(self.vecUp, Angle);
		self.Right = Vec3TransformCoord(self.Right, matTrans);
		self.Front = Vec3TransformCoord(self.Front, matTrans);
	}
	//翻滚(角度)
	open VOID Roll(SPFP Angle){
		XMATRIX matTrans = MatRotateAxis(self.Front, Angle);
		self.Right = Vec3TransformCoord(self.Right, matTrans);
		self.vecUp = Vec3TransformCoord(self.vecUp, matTrans);
	}
	//俯仰(角度)
	open VOID Pitch(SPFP Angle){
		XMATRIX matTrans = MatRotateAxis(self.Right, Angle);
		self.vecUp = Vec3TransformCoord(self.vecUp, matTrans);
		self.Front = Vec3TransformCoord(self.Front, matTrans);
	}
	//水平偏航(角度)
	open VOID HorizontalYaw(SPFP Angle){
		XMATRIX matTrans = MatRotateAxis(dx::g_XMIdentityR1, Angle);
		self.Front = Vec3TransformCoord(self.Front, matTrans);
		self.vecUp = Vec3TransformCoord(self.vecUp, matTrans);
		self.Right = Vec3TransformCoord(self.Right, matTrans);
	}
};

//----------------------------------------//


//--------------------顶点视图--------------------//

//顶点属性
enum VERTATTR{
	VERT_COORD = 0x0001,
	VERT_NORMAL = 0x0002,
	VERT_TANGENT = 0x0004,
	VERT_SIZE = 0x0008,
	VERT_BONES = 0x0010,
	VERT_COLOR = 0x0020,
	VERT_TEXCOORD = 0x0040,
};

//顶点视图
class CVertView{
	hide BYTE *Address;
	open UINT ByteCount;
	hide UCHR hCoord;
	hide UCHR hNormal;
	hide UCHR hTangent;
	hide UCHR hPointSize;
	hide UCHR hWeights;
	hide UCHR hBoneIndexes;
	hide UCHR hColor;
	hide UCHR hTexCoord;

	open CVertView(){}
	open CVertView(DWRD Format){
		self.Initialize(Format);
	}
	open VOID Initialize(DWRD Format){
		MemZero(this, sizeof(*this));

		if(Format & VERT_COORD){
			FORCE_CVT(self.ByteCount, self.hCoord);
			self.ByteCount += sizeof(VECTOR3);
		}
		if(Format & VERT_NORMAL){
			FORCE_CVT(self.ByteCount, self.hNormal);
			self.ByteCount += sizeof(VECTOR3);
		}
		if(Format & VERT_TANGENT){
			FORCE_CVT(self.ByteCount, self.hTangent);
			self.ByteCount += sizeof(VECTOR3);
		}
		if(Format & VERT_SIZE){
			FORCE_CVT(self.ByteCount, self.hPointSize);
			self.ByteCount += sizeof(VECTOR2);
		}
		if(Format & VERT_BONES){
			FORCE_CVT(self.ByteCount, self.hWeights);
			self.ByteCount += sizeof(SPFP) * 3;
			FORCE_CVT(self.ByteCount, self.hBoneIndexes);
			self.ByteCount += sizeof(UCHR) * 4;
		}
		if(Format & VERT_COLOR){
			FORCE_CVT(self.ByteCount, self.hColor);
			self.ByteCount += sizeof(ARGB);
		}
		if(Format & VERT_TEXCOORD){
			FORCE_CVT(self.ByteCount, self.hTexCoord);
			self.ByteCount += sizeof(VECTOR2);
		}

		DWRD fElem = VERT_TEXCOORD << 1;
		while(Format & fElem){
			self.ByteCount += sizeof(VECTOR2);
			fElem <<= 1;
		}
	}
	open VOID SetAddress(VOID* Address){
		self.Address = (BYTE*)Address;
	}
	open VOID ShiftPointer(INT4 Offset){
		self.Address += self.ByteCount * Offset;
	}
	open SPFP* Weights(){
		BYTE *pElement = self.Address + self.hWeights;
		return (SPFP*)pElement;
	}
	open UCHR* BoneIndexes(){
		BYTE *pElement = self.Address + self.hBoneIndexes;
		return (UCHR*)pElement;
	}
	open ARGB &Color(){
		BYTE *pElement = self.Address + self.hColor;
		return *((ARGB*)pElement);
	}
	open VECTOR3 &Coord(){
		BYTE *pElement = self.Address + self.hCoord;
		return *((VECTOR3*)pElement);
	}
	open VECTOR3 &Normal(){
		BYTE *pElement = self.Address + self.hNormal;
		return *((VECTOR3*)pElement);
	}
	open VECTOR3 &Tangent(){
		BYTE *pElement = self.Address + self.hTangent;
		return *((VECTOR3*)pElement);
	}
	open VECTOR2 &PointSize(){
		BYTE *pElement = self.Address + self.hPointSize;
		return *((VECTOR2*)pElement);
	}
	open VECTOR2 &TexCoord(){
		BYTE *pElement = self.Address + self.hTexCoord;
		return *((VECTOR2*)pElement);
	}
};

//----------------------------------------//


//--------------------子网格--------------------//

//网格贴图
struct MESHMAP{
	UINT nDiffuse;
	UINT nBump;
};

//子网格
struct SUBMESH{
	UINT IdxStart;
	UINT IdxCount;
	UINT MtlIdent;
};

//----------------------------------------//


//--------------------骨骼--------------------//

//骨骼
struct BONE{
	CHAR Name[28];
	UINT nParent;
	XMATRIX matTrans;
	XMATRIX matOffset;
	XMATRIX matInitial;
	XMATRIX matCombine;

	VOID* operator new[](UPTR Count){
		return AMAlloc(sizeof(BONE) * Count, 16);
	}
	VOID operator delete[](VOID* Pointer){
		AMFree(Pointer);
	}
};

//----------------------------------------//


//--------------------动画--------------------//

//关键帧
struct KEYFRAME{
	SPFP Time;
	MATRIX matTrans;
};

//子动画
class CSubAnim{
	open UINT numFrame;
	open KEYFRAME *arrFrame;

	open ~CSubAnim(){
		self.Finalize();
	}
	open CSubAnim(){
		MemZero(this, sizeof(*this));
	}
	open VOID Finalize(){
		RESET_ARR(self.arrFrame);
	}
	open VOID Interpolate(SPFP Time, XMATRIX &matOut){
		UINT LastFrame = self.numFrame - 1;
		KEYFRAME *Frames = self.arrFrame;

		if(Time <= Frames[0].Time){
			matOut = PackMat(Frames[0].matTrans);
		} else if(Time >= Frames[LastFrame].Time){
			matOut = PackMat(Frames[LastFrame].matTrans);
		} else{
			KEYFRAME *F0 = &Frames[0];
			KEYFRAME *F1 = &Frames[1];

			for(UINT i = 1; i <= LastFrame; ++i){
				if((Time >= F0->Time) && (Time <= F1->Time)){
					SPFP Lerp = (Time - F0->Time) / (F1->Time - F0->Time);
					XMATRIX Mat0 = PackMat(F0->matTrans);
					XMATRIX Mat1 = PackMat(F1->matTrans);
					matOut = Mat0 + (Mat1 - Mat0)*Lerp;
					return;
				}
				F0++, F1++;
			}
		}
	}
};

//动画
class CAnimation{
	open SPFP Duration;
	open UINT numSubset;
	open CSubAnim *arrSubset;

	open ~CAnimation(){
		self.Finalize();
	}
	open CAnimation(){
		MemZero(this, sizeof(*this));
	}
	open VOID Finalize(){
		RESET_ARR(self.arrSubset);
	}
	open VOID Initialize(TCHR* FilePath){
		//定位数据

		HANDLE hFile = CreateFile(FilePath, GENERIC_READ, 0x0,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		hFile = FindFileChunk(hFile, ASSET_ANIM);
		if(hFile == INVALID_HANDLE_VALUE) goto done;

		//读取数据

		ReadFile(hFile, &self.Duration, sizeof(SPFP), NULL, NULL);
		ReadFile(hFile, &self.numSubset, sizeof(SPFP), NULL, NULL);

		self.arrSubset = new CSubAnim[self.numSubset];

		for(UINT i = 0; i < self.numSubset; ++i){
			CSubAnim *pAnim = &self.arrSubset[i];
			ReadFile(hFile, &pAnim->numFrame, sizeof(UINT), NULL, NULL);
			if(pAnim->numFrame == 0){
				pAnim->arrFrame = NULL;
			} else{
				pAnim->arrFrame = new KEYFRAME[pAnim->numFrame];
				UINT BuffSize = sizeof(KEYFRAME) * pAnim->numFrame;
				ReadFile(hFile, pAnim->arrFrame, BuffSize, NULL, NULL);
			}
		}
done:
		CloseHandle(hFile);
	}
	open VOID UpdateBones(SPFP Time, BONE* arrBone, BOOL bLoop = TRUE){
		if(bLoop && (Time > self.Duration))
			Time = Mod(Time, self.Duration);

		for(UINT i = 0; i < self.numSubset; ++i){
			if(self.arrSubset[i].numFrame > 0)
				self.arrSubset[i].Interpolate(Time, arrBone[i].matTrans);
		}
	}
};

//----------------------------------------//


//--------------------变形动画--------------------//

//变形关键帧
struct MORPHFRAME{
	SPFP Time;
	UINT Target;
};

//变形动画
class CMorphAnim{
	open SPFP Duration;
	open UINT numFrame;
	open MORPHFRAME *arrFrame;

	open ~CMorphAnim(){
		self.Finalize();
	}
	open CMorphAnim(){
		MemZero(this, sizeof(*this));
	}
	open VOID Finalize(){
		RESET_ARR(self.arrFrame);
	}
	open VOID Interpolate(SPFP Time, UINT Targets[2], SPFP &Lerp){
		if(Time <= self.arrFrame[0].Time){
			Targets[0] = Targets[1] = self.arrFrame[0].Target;
		} else if(Time >= self.arrFrame[self.numFrame - 1].Time){
			Targets[0] = Targets[1] = self.arrFrame[self.numFrame - 1].Target;
		} else{
			MORPHFRAME *F0 = &self.arrFrame[0];
			MORPHFRAME *F1 = &self.arrFrame[1];

			for(UINT i = 1; i < self.numFrame; ++i){
				if((Time >= F0->Time) && (Time <= F1->Time)){
					Lerp = (Time - F0->Time) / (F1->Time - F0->Time);
					Targets[0] = F0->Target;
					Targets[1] = F1->Target;
					break;
				}
				F0++; F1++;
			}
		}
	}
};

//----------------------------------------//


//--------------------柔体--------------------//

//质点
struct MASSPOINT{
	SPFP RM;   //质量倒数
	SPFP Mass; //质量
	XVECTOR Force; //受力
	XVECTOR Veloc; //速度
	XVECTOR Origin;   //初始位置
	XVECTOR Position; //当前位置
};

//弹簧
struct SPRING{
	UINT P0, P1;
	SPFP KS, KD;
	SPFP Length;
	SPRING *Next;

	SPRING(){
		MemZero(this, sizeof(this));
	}
};

//柔体
struct CSoftBody{
	UINT numFace;
	UINT numPoint;
	UINT numSpring;
	INDEX3 *arrFace;
	SPRING *TopSpring;
	MASSPOINT *arrPoint;

	~CSoftBody(){
		self.Finalize();
	}
	CSoftBody(){
		MemZero(this, sizeof(*this));
	}
	CSoftBody(BYTE *VertBytes, BYTE *IdxBytes, UINT cbVertex, UINT VertCount, UINT IdxCount, SPFP Mass, SPFP KS, SPFP KD){
		self.Initialize(VertBytes, IdxBytes, cbVertex, VertCount, IdxCount, Mass, KS, KD);
	}

	VOID Initialize(BYTE *VertBytes, BYTE *IdxBytes, UINT cbVertex, UINT VertCount, UINT IdxCount, SPFP Mass, SPFP KS, SPFP KD){
		SPFP RM;
		BYTE *pByte;
		USRT P0, P1, P2;
		XVECTOR Coord;
		VECTOR3 *pCoord;

		//创建表面

		self.numFace = IdxCount / 3;
		self.arrFace = new INDEX3[self.numFace];
		MemCopy(self.arrFace, IdxBytes, sizeof(INDEX3) * self.numFace);

		//创建质点

		RM = 1.0f / Mass;
		pByte = VertBytes;

		self.numPoint = VertCount;
		self.arrPoint = new MASSPOINT[self.numPoint];

		for(UINT i = 0; i < self.numPoint; ++i){
			pCoord = (VECTOR3*)pByte;
			Coord = PackV3(*pCoord);
			pByte += cbVertex;

			self.arrPoint[i].RM = RM;
			self.arrPoint[i].Mass = Mass;
			self.arrPoint[i].Origin = Coord;
			self.arrPoint[i].Position = Coord;
			self.arrPoint[i].Veloc = dx::g_XMZero;
			self.arrPoint[i].Force = dx::g_XMZero;
		}

		//创建弹簧

		for(UINT i = 0; i < self.numFace; ++i){
			P0 = self.arrFace[i][0];
			P1 = self.arrFace[i][1];
			P2 = self.arrFace[i][2];

			self.AddSpring(P0, P1, KS, KD);
			self.AddSpring(P1, P2, KS, KD);
			self.AddSpring(P2, P0, KS, KD);
		}
	}
	VOID AddSpring(UINT P0, UINT P1, SPFP KS, SPFP KD){
		using namespace dx;

		if(P0 == P1) return;

		SPRING *pSpring;

		for(pSpring = self.TopSpring; pSpring; pSpring = pSpring->Next){
			if(P0 == pSpring->P0 && P1 == pSpring->P1) return;
			if(P0 == pSpring->P1 && P1 == pSpring->P0) return;
		}

		pSpring = new SPRING;
		pSpring->Next = self.TopSpring;
		self.TopSpring = pSpring;
		self.numSpring++;

		pSpring->P0 = (USRT)P0;
		pSpring->P1 = (USRT)P1;
		pSpring->KS = KS;
		pSpring->KD = KD;

		XVECTOR &Vector0 = self.arrPoint[P0].Position;
		XVECTOR &Vector1 = self.arrPoint[P1].Position;
		pSpring->Length = Vec3Length(Vector1 - Vector0);
	}
	VOID SetMass(UINT Index, SPFP Mass){
		if(Index < self.numPoint){
			self.arrPoint[Index].Mass = Mass;
			self.arrPoint[Index].RM = (Mass == 0.0f) ? 0.0f : (1.0f / Mass);
		}
	}
	VOID Finalize(){
		RESET_ARR(self.arrFace);
		RESET_ARR(self.arrPoint);
	}
	VOID Reset(){
		MASSPOINT *pPoint = self.arrPoint;
		for(UINT i = 0; i < self.numPoint; ++i){
			pPoint->Position = pPoint->Origin;
			pPoint->Veloc = { 0.0f, 0.0f, 0.0f };
			pPoint++;
		}
	}
	VOID ProcForces(SPFP TimeSpan){
		using namespace dx;

		for(UINT i = 0; i < self.numPoint; ++i){
			MASSPOINT *pPoint = &self.arrPoint[i];
			if(pPoint->Mass == 0.0f) continue;

			pPoint->Veloc += pPoint->Force * pPoint->RM * TimeSpan;
			pPoint->Position += pPoint->Veloc * TimeSpan;
		}
	}
	VOID Revert(SPFP Stiffness, XMATRIX* pTransform){
		using namespace dx;

		MASSPOINT *pPoint;
		XVECTOR Origin, Vector;

		for(UINT i = 0; i < self.numPoint; ++i){
			pPoint = &self.arrPoint[i];
			if(pPoint->Mass == 0.0f) continue;

			Origin = pPoint->Origin;
			if(pTransform) Origin =
				Vec3TransformCoord(Origin, *pTransform);

			Vector = Origin - pPoint->Position;
			Vector *= Stiffness;

			pPoint->Veloc += Vector;
			pPoint->Position += Vector;
		}
	}
	VOID SetForces(SPFP Damping, XVECTOR *pGravity, XVECTOR *pWind, XMATRIX *pTransform, BOOL bAffectAll){
		using namespace dx;

		//计算重力,阻力作用

		for(UINT i = 0; i < self.numPoint; ++i){
			MASSPOINT *pPoint = &self.arrPoint[i];

			if(pTransform && (bAffectAll || (pPoint->Mass == 0.0f)))
				pPoint->Position = Vec3TransformCoord(pPoint->Origin, *pTransform);

			if(pPoint->Mass != 0.0f){
				if(pGravity) pPoint->Force = *pGravity * pPoint->Mass;
				else pPoint->Force = g_XMZero;
				pPoint->Force += pPoint->Veloc * Damping;
			}
		}

		//计算风力作用

		if(pWind){
			for(UINT i = 0; i < self.numFace; ++i){
				MASSPOINT *P0 = &self.arrPoint[self.arrFace[i][0]];
				MASSPOINT *P1 = &self.arrPoint[self.arrFace[i][1]];
				MASSPOINT *P2 = &self.arrPoint[self.arrFace[i][2]];

				XVECTOR Vector0 = P1->Position - P0->Position;
				XVECTOR Vector1 = P2->Position - P0->Position;
				XVECTOR normal = Vec3Cross(Vector0, Vector1);
				normal = Vec3Normalize(normal);

				SPFP dot = Vec3Dot(normal, *pWind);
				XVECTOR Force = normal * dot;

				P0->Force += Force;
				P1->Force += Force;
				P2->Force += Force;
			}
		}

		//计算弹力作用

		for(SPRING *pSpring = self.TopSpring; pSpring; pSpring = pSpring->Next){
			MASSPOINT *P0 = &self.arrPoint[pSpring->P0];
			MASSPOINT *P1 = &self.arrPoint[pSpring->P1];

			XVECTOR Vector = P1->Position - P0->Position;
			SPFP Length = Vec3Length(Vector);
			Vector /= Length;

			XVECTOR Veloc = P1->Veloc - P0->Veloc;
			SPFP speed = Vec3Dot(Veloc, Vector);

			SPFP fs = pSpring->KS * (Length - pSpring->Length);
			SPFP fd = pSpring->KD * speed;

			XVECTOR Force = (fs + fd) * Vector;

			if(P0->Mass != 0.0f) P0->Force += Force;
			if(P1->Mass != 0.0f) P1->Force -= Force;
		}
	}
	VOID Update(SPFP TimeSpan, SPFP Damping, XVECTOR* pGravity, XVECTOR *pWind, XMATRIX *pTransform, BOOL bAffectAll){
		using namespace dx;

		XMATRIX matFrame, *pFrameMat;
		SPFP TimeSlice, TimeRemain = TimeSpan;

		if(pTransform) pFrameMat = &matFrame;
		else pFrameMat = NULL;

		while(TimeRemain > 0.0f){
			TimeSlice = MIN(TimeRemain, 0.01f);
			TimeRemain -= TimeSlice;

			if(pFrameMat) matFrame =
				*pTransform * ((TimeSpan - TimeRemain) / TimeSpan);

			self.SetForces(Damping, pGravity, pWind, pFrameMat, bAffectAll);
			self.ProcForces(TimeSlice);
		}
	}
};

//----------------------------------------//