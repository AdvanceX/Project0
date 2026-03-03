//-------------------- 几何函数 --------------------//

inline IBOOL PointInRect(_in VFloat2 &Point, _in FRect &Rect){
	if(Point.x < Rect.left) return B_False;
	if(Point.x > Rect.right) return B_False;
	if(Point.y > Rect.bottom) return B_False;
	if(Point.y < Rect.top) return B_False;
	return B_True;
}
inline IBOOL PointInCircle(_in FVector2 &Point, _in FCircle2 &Circle, SPFPN *pDistSq = P_Null){
	SPFPN DistSq = FVector2::DistanceSquared(Point, Circle.Center);
	if(pDistSq) *pDistSq = DistSq;
	return (DistSq <= Circle.Radius*Circle.Radius);
}
inline IBOOL PointInRhombus(_in VFloat2 &Point, _in FRect &Rect, SPFPN HalfSize){
	FVector2 Pa(Point);
	FVector2 Pb0(Rect.left, Rect.top);
	FVector2 Pb1(Rect.right, Rect.bottom);
	FVector2 Pb2(Rect.right, Rect.top);
	FVector2 Pb3(Rect.left, Rect.bottom);

	SPFPN Limit = sqrt(HalfSize * HalfSize * 2.f) * 0.5f;
	SPFPN Dist0 = VtoF_(dx::XMVector2LinePointDistance(Pb0, Pb1, Pa));
	SPFPN Dist1 = VtoF_(dx::XMVector2LinePointDistance(Pb2, Pb3, Pa));

	if(Dist0 > Limit) return B_False;
	if(Dist1 > Limit) return B_False;
	return B_True;
}

inline FRect ToRectF(_in URect &Rect){
	return FRect{
		(SPFPN)Rect.left, (SPFPN)Rect.top,
		(SPFPN)Rect.right, (SPFPN)Rect.bottom };
}
inline VFloat2 ToPoint(UNS32 X, UNS32 Z){
	return VFloat2((SPFPN)X, (SPFPN)Z);
}

//----------------------------------------//