#include "ge_common.hlsl"
#ifdef INPUT_NORMAL
#include "ge_lighting.hlsl"
#endif

struct PARTICLE{
	float3 Coord: POSITION;
#ifdef INPUT_SIZE
	float2 Size: SIZE;
#endif
#ifdef INPUT_COLOR
	float4 Color: COLOR;
#endif
#ifdef INPUT_TEXCOORD
	float2 TexCoord: TEXCOORD;
#endif
};
struct VERTEX{
	uint PrimID: SV_PrimitiveID;
	float4 WVPCoord: SV_POSITION;
	float3 WCoord: POSITION;
	float3 Normal: NORMAL;
	float2 TexCoord: TEXCOORD;
#ifdef INPUT_COLOR
	float4 Color: COLOR;
#endif
};

//顶点着色器
PARTICLE VertexShade(PARTICLE Input){
	return Input;
}
//几何着色器
[maxvertexcount(4)]
void GeometryShade(point PARTICLE Input[1], uint PrimID: SV_PrimitiveID, inout TriangleStream<VERTEX> OutStream){
	float3 Front, Up, Right;
	Up = float3(0.0f, 1.0f, 0.0f);
	Front = gCameraPos - Input[0].Coord;
	Front = normalize(Front);
	Right = cross(Up, Front);

#ifdef INPUT_SIZE
	float HalfWidth = Input[0].Size.x * 0.5f;
	float HalfHeight = Input[0].Size.y * 0.5f;
#else
	float HalfWidth = gWorldTrans[1][0] * 0.5f;
	float HalfHeight = gWorldTrans[1][1] * 0.5f;
#endif

	float4 Coords[4];
	Coords[0] = float4(Input[0].Coord + HalfWidth*Right - HalfHeight*Up, 1.0f);
	Coords[1] = float4(Input[0].Coord + HalfWidth*Right + HalfHeight*Up, 1.0f);
	Coords[2] = float4(Input[0].Coord - HalfWidth*Right - HalfHeight*Up, 1.0f);
	Coords[3] = float4(Input[0].Coord - HalfWidth*Right + HalfHeight*Up, 1.0f);

#ifdef INPUT_TEXCOORD
	float2 TexCoords[4] = {
		Input[0].TexCoord, Input[0].TexCoord,
		Input[0].TexCoord, Input[0].TexCoord
	};
#else
	float2 TexCoords[4] = {
		float2(0.0f, 1.0f), float2(0.0f, 0.0f),
		float2(1.0f, 1.0f), float2(1.0f, 0.0f)
	};
#endif

	VERTEX Vertex;
	[unroll]
	for(uint i = 0; i < 4; ++i){
		Vertex.WVPCoord = mul(Coords[i], gViewProjTrans);
		Vertex.WCoord = Coords[i].xyz;
		Vertex.TexCoord = TexCoords[i];
		Vertex.Normal = Front;
		Vertex.PrimID = PrimID;
#ifdef INPUT_COLOR
		Vertex.Color = Input[0].Color;
#endif
		OutStream.Append(Vertex);
	}
}
//像素着色器
float4 PixelShade(VERTEX Input): SV_Target{
	float4 Color = float4(gMaterial.Albedo, gMaterial.Opacity);
	Color *= gTexture.Sample(gSampler, Input.TexCoord);

#ifdef INPUT_COLOR
	Color *= Input.Color;
#endif
#ifdef ALPHA_TEST
	clip(Color.a - 0.01);
#endif

#ifdef INPUT_NORMAL
	Input.Normal = normalize(Input.Normal)
	float3 ViewVector = GetViewVector(Input.WCoord);
	float3 DirectLight = ComputeLighting(Input.WCoord, Input.Normal, ViewVector);
	Color.rgb = (gAmbientLight + DirectLight) * Color.rgb;
#endif

	if(gFogRange > 0) Color.rgb =
		GetFogEffect(Input.WCoord, Color.rgb);

	return Color;
}