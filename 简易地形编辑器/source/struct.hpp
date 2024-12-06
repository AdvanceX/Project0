//-------------------- 参与者设定 --------------------//

// Goods Class
struct GOODSCLASS{
	USINT iModel;  //模型ID
	USINT iFunc;   //功能ID
};

// Animal Class
struct ANIMALCLASS{
	USINT iModel;   //模型ID
	USINT HpMax;    //生命上限
	USINT EpMax;    //能量上限
	SPFPN HpRegen;  //生命恢复率
	SPFPN EpRegen;  //能量恢复率
	USINT Attack;   //攻击力
	USINT Defense;  //防御力
	SPFPN AtkSpeed; //攻击速度
	SPFPN MovSpeed; //移动速度
	WORDT AtkType;  //攻击类型
	WORDT DefType;  //防御类型
};

// Structure Class
struct STRUCTCLASS{
	USINT iModel;   //模型ID
	USINT HpMax;    //生命上限
	USINT EpMax;    //能量上限
	SPFPN HpRegen;  //生命恢复率
	SPFPN EpRegen;  //能量恢复率
	USINT Attack;   //攻击力
	USINT Defense;  //防御力
	SPFPN AtkSpeed; //攻击速度
	SPFPN MovSpeed; //移动速度
	WORDT AtkType;  //攻击类型
	WORDT DefType;  //防御类型
	UHALF SizeX;    //X轴尺寸
	UHALF SizeZ;    //Z轴尺寸
};

//----------------------------------------//


//-------------------- 参与者 --------------------//

// Goods
class CGoods{
	_open USINT iProp;
	_open VECTOR3 Scaling;
	_open VECTOR3 Position;
};

// Animal
class CAnimal{
#define ITEM_SET_SIZE 8
#define ITEM_SET_BYTES sizeof(UHALF[ITEM_SET_SIZE])

	_open USINT iOrgProps;
	_open ANIMALCLASS CurProps;
	_open VECTOR3 Scaling;
	_open VECTOR3 Position;
	_open VECTOR4 Rotation;
	_open SLLNODE *lpGoods;
	_open SLLNODE *lpEffect;

	_open ~CAnimal(){
		$m.Finalize();
	}
	_open CAnimal(){
		$m.lpGoods = P_Null;
		$m.lpEffect = P_Null;
	}
	_open $VOID Finalize(){
		if($m.lpGoods || $m.lpEffect){
			SLLNODE::DeleteAll($m.lpGoods);
			SLLNODE::DeleteAll($m.lpEffect);

			$m.lpGoods = P_Null;
			$m.lpEffect = P_Null;
		}
	}
	_open $VOID Initialize(USINT ClassId, _in ANIMALCLASS &Class, _in VECTOR3 &Position){
		$m.lpGoods = P_Null;
		$m.lpEffect = P_Null;
		$m.CurProps = Class;
		$m.iOrgProps = ClassId;
		$m.Position = Position;
		$m.Rotation = QUATERNION::Identity;
		$m.Scaling = VECTR3X::Zero;
	}

#undef ITEM_SET_SIZE
#undef ITEM_SET_BYTES
};

// Structure
class CStructure{
#define ITEM_SET_SIZE 8
#define ITEM_SET_BYTES sizeof(UHALF[ITEM_SET_SIZE])

	_open USINT iOrgProps;
	_open STRUCTCLASS CurProps;
	_open VECTOR3 Scaling;
	_open VECTOR3 Position;
	_open SLLNODE *lpEffect;

	_open ~CStructure(){
		$m.Finalize();
	}
	_open CStructure(){
		$m.lpEffect = P_Null;
	}
	_open $VOID Finalize(){
		if($m.lpEffect){
			SLLNODE::DeleteAll($m.lpEffect);
			$m.lpEffect = P_Null;
		}
	}
	_open $VOID Initialize(USINT ClassId, _in STRUCTCLASS &Class, _in VECTOR3 &Position){
		$m.lpEffect = P_Null;
		$m.CurProps = Class;
		$m.iOrgProps = ClassId;
		$m.Position = Position;
		$m.Scaling = VECTR3X::Zero;
	}

#undef ITEM_SET_SIZE
#undef ITEM_SET_BYTES
};

//----------------------------------------//