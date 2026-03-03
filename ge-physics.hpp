//--------------------坐标系转换--------------------//

// 左手转右手(向量)
inline btVector3 LhcsToRhcs(_in VFloat3 &Vector){
	return btVector3(Vector.x, Vector.y, -Vector.z);
}

// 左手转右手(四元数)
inline btQuaternion LhcsToRhcs(_in VFloat4 &Quat){
	return btQuaternion(-Quat.x, -Quat.y, Quat.z, Quat.w);
}

// 右手转左手(向量)
inline VFloat3 RhcsToLhcs(_in btVector3 &Vector){
	return VFloat3(Vector.m_floats[0], Vector.m_floats[1], -Vector.m_floats[2]);
}

// 右手转左手(四元数)
inline VFloat4 RhcsToLhcs(_in btQuaternion &Quat){
	return VFloat4(-Quat.x(), -Quat.y(), Quat.z(), Quat.w());
}

//----------------------------------------//