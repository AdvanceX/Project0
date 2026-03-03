//-------------------- 参与者设定 --------------------//

// Goods Class
struct MGGoodsClass{
	UNS32 iModel;  //模型ID
	UNS32 iFunc;   //功能ID
};

// Animal Class
struct MGAnimalClass{
	UNS32 iModel;   //模型ID
	UNS32 HpMax;    //生命上限
	UNS32 EpMax;    //能量上限
	SPFPN HpRegen;  //生命恢复率
	SPFPN EpRegen;  //能量恢复率
	UNS32 Attack;   //攻击力
	UNS32 Defense;  //防御力
	SPFPN AtkSpeed; //攻击速度
	SPFPN MovSpeed; //移动速度
	WORDT AtkType;  //攻击类型
	WORDT DefType;  //防御类型
};

// Structure Class
struct MGStructClass{
	UNS32 iModel;   //模型ID
	UNS32 HpMax;    //生命上限
	UNS32 EpMax;    //能量上限
	SPFPN HpRegen;  //生命恢复率
	SPFPN EpRegen;  //能量恢复率
	UNS32 Attack;   //攻击力
	UNS32 Defense;  //防御力
	SPFPN AtkSpeed; //攻击速度
	SPFPN MovSpeed; //移动速度
	WORDT AtkType;  //攻击类型
	WORDT DefType;  //防御类型
	UNS16 SizeX;    //X轴尺寸
	UNS16 SizeZ;    //Z轴尺寸
};

//----------------------------------------//


//-------------------- 物品&效果 --------------------//

struct SLLNode{
	UNS32 rgRef[6];
	SLLNode *pNext;
};

//----------------------------------------//


//-------------------- 参与者 --------------------//

// Goods
class CMGGoods{
	_open UNS32 iProp;
	_open VFloat3 Scaling;
	_open VFloat3 Position;
};

// Animal
class CMGAnimal{
#define ITEM_SET_SIZE 8
#define ITEM_SET_BYTES sizeof(UNS16[ITEM_SET_SIZE])

	_open UNS32 iOrgProps;
	_open MGAnimalClass CurProps;
	_open VFloat3 Scaling;
	_open VFloat3 Position;
	_open VFloat4 Rotation;
	_open SLLNode *lpGoods;
	_open SLLNode *lpEffect;

	_open ~CMGAnimal(){
		$m.Finalize();
	}
	_open CMGAnimal(){
		$m.lpGoods = P_Null;
		$m.lpEffect = P_Null;
	}
	_open CMGAnimal(UNS32 ClassId, MGAnimalClass Props, VFloat3 Position){
		$m.iOrgProps = ClassId;
		$m.CurProps = Props;
		$m.Position = Position;
	}
	_open $VOID Finalize(){
		if($m.lpGoods || $m.lpEffect){
			/*SLLNode::DeleteAll($m.lpGoods);
			SLLNode::DeleteAll($m.lpEffect);*/

			$m.lpGoods = P_Null;
			$m.lpEffect = P_Null;
		}
	}
	_open $VOID Initialize(UNS32 ClassId, _in MGAnimalClass &Class, _in VFloat3 &Position){
		$m.lpGoods = P_Null;
		$m.lpEffect = P_Null;
		$m.CurProps = Class;
		$m.iOrgProps = ClassId;
		$m.Position = Position;
		$m.Rotation = FQuat::Identity;
		$m.Scaling = FVector3::Zero;
	}

#undef ITEM_SET_SIZE
#undef ITEM_SET_BYTES
};

// Structure
class CMGStructure{
#define ITEM_SET_SIZE 8
#define ITEM_SET_BYTES sizeof(UNS16[ITEM_SET_SIZE])

	_open UNS32 iOrgProps;
	_open MGStructClass CurProps;
	_open VFloat3 Scaling;
	_open VFloat3 Position;
	_open SLLNode *lpEffect;

	_open ~CMGStructure(){
		$m.Finalize();
	}
	_open CMGStructure(){
		$m.lpEffect = P_Null;
	}
	_open CMGStructure(UNS32 ClassId, MGStructClass Props, VFloat3 Position){
		$m.iOrgProps = ClassId;
		$m.CurProps = Props;
		$m.Position = Position;
	}
	_open $VOID Finalize(){
		if($m.lpEffect){
			//SLLNode::DeleteAll($m.lpEffect);
			$m.lpEffect = P_Null;
		}
	}
	_open $VOID Initialize(UNS32 ClassId, _in MGStructClass &Class, _in VFloat3 &Position){
		$m.lpEffect = P_Null;
		$m.CurProps = Class;
		$m.iOrgProps = ClassId;
		$m.Position = Position;
		$m.Scaling = FVector3::Zero;
	}

#undef ITEM_SET_SIZE
#undef ITEM_SET_BYTES
};

//----------------------------------------//