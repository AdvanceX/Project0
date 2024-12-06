//-------------------- 几何函数 --------------------//

inline SPFPN PointsDistSq(_in POINT3 &Point0, _in POINT3 &Point1){
	SPFPN DistX = Point0.x - Point1.x;
	SPFPN DistZ = Point0.z - Point1.z;
	return DistX*DistX + DistZ*DistZ;
}
inline SPFPN PointsDistSq(_in POINT2 &Point0, _in POINT2 &Point1){
	SPFPN DistX = Point0.x - Point1.x;
	SPFPN DistY = Point0.y - Point1.y;
	return DistX*DistX + DistY*DistY;
}

inline IBOOL PointInRect(_in POINT3 &Point, _in RECTF &Rect){
	if(Point.x < Rect.left) return B_False;
	if(Point.x > Rect.right) return B_False;
	if(Point.z < Rect.bottom) return B_False;
	if(Point.z > Rect.top) return B_False;
	return B_True;
}
inline IBOOL PointInRect(_in POINT2 &Point, _in RECTF &Rect){
	if(Point.x < Rect.left) return B_False;
	if(Point.x > Rect.right) return B_False;
	if(Point.y > Rect.bottom) return B_False;
	if(Point.y < Rect.top) return B_False;
	return B_True;
}

inline IBOOL PointInCircle(_in POINT3 &Point, _in POINT3 &Center, SPFPN RadSq){
	SPFPN DistSq = PointsDistSq(Point, Center);
	return (DistSq <= RadSq);
}
inline IBOOL PointInCircle(_in POINT2 &Point, _in POINT2 &Center, SPFPN RadSq){
	SPFPN DistSq = PointsDistSq(Point, Center);
	return (DistSq <= RadSq);
}

inline RECTF ToRectF(_in RECTU &Rect){
	return RECTF{
		(SPFPN)Rect.left, (SPFPN)Rect.top,
		(SPFPN)Rect.right, (SPFPN)Rect.bottom };
}
inline RECTU ToRectU(_in RECTF &Rect){
	return RECTU{
		(USINT)Rect.left, (USINT)Rect.top,
		(USINT)Rect.right, (USINT)Rect.bottom };
}

inline POINT3 ToPoint(USINT X, USINT Z){
	return POINT3((SPFPN)X, 0.f, (SPFPN)Z);
}
inline POINT3 ToPoint(_in POINT2 &Point){
	return POINT3(Point.x, 0.f, Point.y);
}

//----------------------------------------//