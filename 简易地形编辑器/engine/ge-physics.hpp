//-------------------- Bullet3数据类型 --------------------//

typedef btVector3    BP3VEC3;
typedef btVector4    BP3VEC4;
typedef btTransform  BPXform;
typedef btQuaternion BPQuat;
typedef btSoftBody        BPSoftBody;
typedef btRigidBody       BPRigidBody;
typedef btCollisionObject BPCollider;
typedef btDynamicsWorld         BPDynaWorld;
typedef btCollisionWorld        BPCollisWorld;
typedef btSimpleDynamicsWorld   BPSimpleDynaWorld;
typedef btDiscreteDynamicsWorld BPDscrtDynaWorld;
typedef btBoxShape                         BPBoxShape;
typedef btConeShape                        BPConeShape;
typedef btEmptyShape                       BPEmptyShape;
typedef btConvexShape                      BPConvexShape;
typedef btSphereShape                      BPSphereShape;
typedef btCapsuleShape                     BPCapsuleShape;
typedef btConcaveShape                     BPConcaveShape;
typedef btCompoundShape                    BPCompoundShape;
typedef btCylinderShape                    BPCylinderShape;
typedef btCollisionShape                   BPCollisShape;
typedef btConvexHullShape                  BPConvexHullShape;
typedef btMultiSphereShape                 BPMultiSphereShape;
typedef btStaticPlaneShape                 BPStaticPlaneShape;
typedef btTriangleMeshShape                BPTriMeshShape;
typedef btConvexInternalShape              BPIntlConvexShape;
typedef btUniformScalingShape              BPUniScaleShape;
typedef btBvhTriangleMeshShape             BPBvhTriMeshShape;
typedef btPolyhedralConvexShape            BPConvexPolyShape;
typedef btConvexTriangleMeshShape          BPConvexTriMeshShape;
typedef btHeightfieldTerrainShape          BPHeightFieldShape;
typedef btScaledBvhTriangleMeshShape       BPScaledBvhTriMeshShape;
typedef btPolyhedralConvexAabbCachingShape BPAabbConvexPolyShape;
typedef btConstraintSolver                  BPCstrSolver;
typedef btVoronoiSimplexSolver              BPVoronoiSimplexSolver;
typedef btSingleConstraintRowSolver         BPSingleCstrRowSolver;
typedef btConvexPenetrationDepthSolver      BPConvexPenetraxnDepthSolver;
typedef btSequentialImpulseConstraintSolver BPSeqImpulseCstrSolver;
typedef btMotionState                            BPMotionState;
typedef btDefaultMotionState                     BPDefMotionState;
typedef btDbvtBroadphase                         BPDbvtBroadphase;
typedef btSimpleBroadphase                       BPSimpleBroadphase;
typedef btBroadphaseInterface                    BPBroadphaseIface;
typedef btCollisionDispatcher                    BPCollisDispr;
typedef btCollisionConfiguration                 BPCollisConfig;
typedef btDefaultCollisionConfiguration          BPDefCollisConfig;
typedef btRigidBody::btRigidBodyConstructionInfo   BP3RIGIDBODYINFO;
typedef btCollisionWorld::ClosestRayResultCallback BP3CLOSESTRAYCB;
typedef btCollisionWorld::AllHitsRayResultCallback BP3ALLHITSRAYCB;

//----------------------------------------//


//--------------------坐标系转换--------------------//

//左手转右手(向量)
inline BP3VEC3 LhcsToRhcs(_in VECTOR3 &Vector){
	return BP3VEC3(Vector.x, Vector.y, -Vector.z);
}

//左手转右手(四元数)
inline BPQuat LhcsToRhcs(_in VECTOR4 &Quat){
	return BPQuat(-Quat.x, -Quat.y, Quat.z, Quat.w);
}

//右手转左手(向量)
inline VECTOR3 RhcsToLhcs(_in BP3VEC3 &Vector){
	return VECTOR3(Vector.m_floats[0], Vector.m_floats[1], -Vector.m_floats[2]);
}

//右手转左手(四元数)
inline VECTOR4 RhcsToLhcs(_in BPQuat &Quat){
	return VECTOR4(-Quat.x(), -Quat.y(), Quat.z(), Quat.w());
}

//----------------------------------------//


//--------------------#物理系统--------------------//

////物理系统
//class CPhySystem: public BPDscrtDynaWorld{
//	_open struct HITINFO{
//		VECTOR3 Intxxn;
//		BPCollider *pObject;
//	};
//	_open struct GROUND{
//		SPFPN MinHeight;
//		SPFPN MaxHeight;
//		SPFPN *rgHeight;
//		BPRigidBody *pKernel;
//	};
//
//	_secr CLnkList Grounds;
//
//	_open ~CPhySystem(){
//		$m.Finalize();
//	}
//	_open CPhySystem(
//		BPCollisDispr *pCollisDispr, BPBroadphaseIface *pOverlapCache,
//		BPSeqImpulseCstrSolver *pSICSolver, BPDefCollisConfig *pCollisConfig)
//		: BPDscrtDynaWorld(pCollisDispr, pOverlapCache, pSICSolver, pCollisConfig){}
//	_open CPhySystem(_in VECTOR3 &Gravity,
//		BPCollisDispr *pCollisDispr, BPBroadphaseIface *pOverlapCache,
//		BPSeqImpulseCstrSolver *pSICSolver, BPDefCollisConfig *pCollisConfig)
//		: BPDscrtDynaWorld(pCollisDispr, pOverlapCache, pSICSolver, pCollisConfig)
//	{
//		$m.Initialize(Gravity);
//	}
//	//////
//	_open $VOID Finalize(){
//		if($m.getNumCollisionObjects() != 0){
//			//删除碰撞体
//			{
//				USINT ObjCount = $m.getNumCollisionObjects();
//				BPCollider **rgObject = &$m.getCollisionObjectArray()[0];
//
//				for(USINT iExe = 0; iExe < ObjCount; ++iExe){
//					BPRigidBody *pObject = BPRigidBody::upcast(rgObject[iExe]);
//					$m.removeCollisionObject(rgObject[iExe]);
//					delete pObject->getCollisionShape();
//					delete pObject->getMotionState();
//					delete pObject;
//				}
//			}
//
//			//删除地面信息
//			{
//				auto pNode = $m.Grounds.GetStart();
//				auto pEnd = $m.Grounds.GetEnd();
//
//				for(; pNode != pEnd; pNode = pNode->pNext){
//					GROUND *pGround = (GROUND*)pNode->Data;
//					delete[] pGround->rgHeight;
//				}
//
//				$m.Grounds.Clear(B_True);
//			}
//		}
//	}
//	_open $VOID Initialize(_in VECTOR3 &Gravity){
//		$m.Grounds.Init(sizeof(GROUND));
//		$m.setGravity(LhcsToRhcs(Gravity));
//	}
//	_open $VOID AddRigidBody(BPCollisShape *pShape, SPFPN Mass, _in VECTOR3 &Position, _in VECTOR4 &Rotation){
//		BP3VEC3 Inertia(0.f, 0.f, 0.f);
//		if(Mass != 0.f) pShape->calculateLocalInertia(Mass, Inertia);
//
//		BPXform Xform(LhcsToRhcs(Rotation), LhcsToRhcs(Position));
//		BPMotionState *pMotionState = new BPDefMotionState(Xform);
//		BP3RIGIDBODYINFO BodyInfo(Mass, pMotionState, pShape, Inertia);
//		BPRigidBody *pBody = new BPRigidBody(BodyInfo);
//
//		$m.addRigidBody(pBody);
//	}
//	_open $VOID AddHeightField(/*CGrid &Terrain, */_in VECTOR3 &Position, _in VECTOR4 &Rotation){
//		////采集高度
//
//		//SPFPN MinHeight = FLT_MAX, MaxHeight = -FLT_MAX;
//		//SPFPN Height, *rgHeight = new SPFPN[Terrain.VtxCount];
//
//		//for(USINT iExe = 0; iExe < Terrain.VtxCount; ++iExe){
//		//	Height = Terrain.prgVertex[iExe].PosY;
//		//	if(Height > MaxHeight) MaxHeight = Height;
//		//	if(Height < MinHeight) MinHeight = Height;
//		//	rgHeight[iExe] = Height;
//		//}
//
//		////调整中心
//
//		//VECTOR3 AdjPos = Position;
//		//AdjPos.y = (MaxHeight + MinHeight) * 0.5f;
//
//		////创建包围体
//
//		//BPHeightFieldShape *pShape = new BPHeightFieldShape(
//		//	Terrain.ColCount + 1, Terrain.RowCount + 1,
//		//	rgHeight, 1.f, MinHeight, MaxHeight, 1, PHY_FLOAT, B_False);
//
//		//pShape->setLocalScaling(BP3VEC3(Terrain.VtxSpanX, 1.f, Terrain.VtxSpanZ));
//
//		////创建刚体
//
//		//BPXform Xform(LhcsToRhcs(Rotation), LhcsToRhcs(AdjPos));
//		//BPMotionState *pMotionState = new BPDefMotionState(Xform);
//		//BP3RIGIDBODYINFO BodyInfo(0.f, pMotionState, pShape, BP3VEC3(0.f, 0.f, 0.f));
//		//BPRigidBody *pBody = new BPRigidBody(BodyInfo);
//		//GROUND Ground = { MinHeight, MaxHeight, rgHeight, pBody };
//
//		////添加到场景
//
//		//$m.addRigidBody(pBody);
//		//$m.Grounds.PushBack(&Ground);
//	}
//	_open IBOOL ClosestRayTest(_out HITINFO &rResult, _in VECTOR3 &Start, _in VECTOR3 &End){
//		BP3VEC3 EndR = LhcsToRhcs(End);
//		BP3VEC3 StartR = LhcsToRhcs(Start);
//		BP3CLOSESTRAYCB Callback(StartR, EndR);
//
//		$m.rayTest(StartR, EndR, Callback);
//
//		if(!Callback.hasHit()){
//			return B_False;
//		} else{
//			rResult.Intxxn = RhcsToLhcs(Callback.m_hitPointWorld);
//			rResult.pObject = (BPCollider*)Callback.m_collisionObject;
//			return B_True;
//		}
//	}
//	_open GROUND *GetGround(USINT Index){
//		return (GROUND*)$m.Grounds.Get(Index);
//	}
//};

//----------------------------------------//