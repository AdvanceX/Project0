//#include "ge-param.hlsl"

//struct TParticle {
//	float3 Coord: POSITION;
//#ifdef INPUT_PSIZE
//	float2 Size: PSIZE;
//#endif
//#ifdef INPUT_COLOR
//	float4 Color: COLOR;
//#endif
//#ifdef INPUT_TEXCOORD
//	float2 TexCoord: TEXCOORD;
//#endif
//};
//struct TVertex {
//	uint PrimID: SV_PrimitiveID;
//	float4 CoordP: SV_POSITION;
//	float3 CoordW: POSITION;
//	float3 Normal: NORMAL;
//	float2 TexCoord: TEXCOORD;
//#ifdef INPUT_COLOR
//	float4 Color: COLOR;
//#endif
//};

////顶点着色器
//TParticle VertexShading(TParticle Input) {
//	return Input;
//}
////几何着色器
//[maxvertexcount(4)]
//void GeometryShading(point TParticle Input[1], uint PrimID: SV_PrimitiveID, inout TriangleStream<TVertex> OutStream) {
//	float3 Front, Up, Right;
//	Up = float3(0.f, 1.f, 0.f);
//	Front = gCamCoord - Input[0].Coord;
//	Front = normalize(Front);
//	Right = cross(Up, Front);

//	#ifdef INPUT_PSIZE
//	float HalfWidth = Input[0].Size.x * 0.5f;
//	float HalfHeight = Input[0].Size.y * 0.5f;
//	#else
//	float HalfWidth = gWorldMat[1][0] * 0.5f;
//	float HalfHeight = gWorldMat[1][1] * 0.5f;
//	#endif

//	float4 Coords[4];
//	Coords[0] = float4(Input[0].Coord + HalfWidth * Right - HalfHeight * Up, 1.f);
//	Coords[1] = float4(Input[0].Coord + HalfWidth * Right + HalfHeight * Up, 1.f);
//	Coords[2] = float4(Input[0].Coord - HalfWidth * Right - HalfHeight * Up, 1.f);
//	Coords[3] = float4(Input[0].Coord - HalfWidth * Right + HalfHeight * Up, 1.f);

//	#ifdef INPUT_TEXCOORD
//	float2 TexCoords[4] = {
//		Input[0].TexCoord, Input[0].TexCoord,
//		Input[0].TexCoord, Input[0].TexCoord
//	};
//	#else
//	float2 TexCoords[4] = {
//		float2(0.f, 1.f), float2(0.f, 0.f),
//		float2(1.f, 1.f), float2(1.f, 0.f)
//	};
//	#endif

//	TVertex Vertex;
//	[unroll]
//	for(uint iExe = 0; iExe < 4; ++iExe) {
//		Vertex.CoordP = mul(Coords[iExe], gViewProjMat);
//		Vertex.CoordW = Coords[iExe].xyz;
//		Vertex.TexCoord = TexCoords[iExe];
//		Vertex.Normal = Front;
//		Vertex.PrimID = PrimID;
//		#ifdef INPUT_COLOR
//		Vertex.Color = Input[0].Color;
//		#endif
//		OutStream.Append(Vertex);
//	}
//}
////像素着色器
//float4 PixelShading(TVertex Input): SV_Target {
//	float4 Color = float4(gMtlBase.Albedo, gMtlBase.Opacity);
//	Color *= gColorMap.Sample(gCustSmplr, Input.TexCoord);

//	#ifdef INPUT_COLOR
//	Color *= Input.Color;
//	#endif
//	#ifdef ALPHA_TEST
//	clip(Color.a - 0.01);
//	#endif

//	#ifdef INPUT_NORMAL
//	Input.Normal = normalize(Input.Normal)
//	float3 vecView = GetViewVector(Input.CoordW);
//	float3 DirectLight = ComputeLighting(Input.CoordW, Input.Normal, vecView);
//	Color.rgb = (gAmbLight + DirectLight) * Color.rgb;
//	#endif

//	if(gFogDepth > 0)
//		Color.rgb =	CalcFogEffect(Input.CoordW, Color.rgb);

//	return Color;
//}