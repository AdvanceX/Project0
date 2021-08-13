#include "gs_common.hlsl"

struct CtrlPoint{
	float3 Coord: POSITION;
};
struct Vertex{
	float4 Coord: SV_POSITION;
};
struct PatchTess{
	float EdgeTess[4]: SV_TessFactor;
	float InsideTess[2]: SV_InsideTessFactor;
};

float4 BernsteinBasis(float t){
	float a0 = 1.0f - t;
	float a1 = a0 * a0;
	float a2 = a1 * a0;
	float b0 = t;
	float b1 = b0 * b0;
	float b2 = b1 * b0;

	return float4(a2, 3.0f * b1 * a2, 3.0f * b2 * a1, b2);
}
float4 BernsteinBasisDer(float t){
	float a0 = 1.0f - t;
	float a1 = a0 * a0;
	float b0 = t;
	float b1 = b0 * b0;

	return float4(-3.0f * a1, 3.0f*a1 - 6.0*b0*a0, 6.0f*b0*a0 - 3.0f*b1, 3.0f * b1);
}
float3 BezierVertex(const OutputPatch<CtrlPoint, 16> patch, float4 bu, float4 bv){
	float3 output = float3(0.0f, 0.0f, 0.0f);
	output = bv.x * (bu.x*patch[0].Coord + bu.y*patch[1].Coord + bu.z*patch[2].Coord + bu.w*patch[3].Coord);
	output += bv.y * (bu.x*patch[4].Coord + bu.y*patch[5].Coord + bu.z*patch[6].Coord + bu.w*patch[7].Coord);
	output += bv.z * (bu.x*patch[8].Coord + bu.y*patch[9].Coord + bu.z*patch[10].Coord + bu.w*patch[11].Coord);
	output += bv.w * (bu.x*patch[12].Coord + bu.y*patch[13].Coord + bu.z*patch[14].Coord + bu.w*patch[15].Coord);

	return output;
}

//顶点着色器
CtrlPoint VertexShade(CtrlPoint input){
	return input;
}
//常量外壳着色器
PatchTess ConstHS(InputPatch<CtrlPoint, 16> patch, uint patchID:SV_PrimitiveID){
	PatchTess patchTess;

	patchTess.EdgeTess[0] = 25;
	patchTess.EdgeTess[1] = 25;
	patchTess.EdgeTess[2] = 25;
	patchTess.EdgeTess[3] = 25;

	patchTess.InsideTess[0] = 25;
	patchTess.InsideTess[1] = 25;

	return patchTess;
}
//控制点外壳着色器
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(16)]
[patchconstantfunc("ConstHS")]
[maxtessfactor(64.0f)]
CtrlPoint HullShade(InputPatch<CtrlPoint, 16> patch, uint pointID: SV_OutputControlPointID, uint patchId: SV_PrimitiveID){
	return patch[i];
}
//域着色器
[domain("quad")]
Vertex DomainShade(PatchTess patchTess, float2 uv: SV_DomainLocation, const OutputPatch<CtrlPoint, 16> patch){
	Vertex output;

	float4 bu = BernsteinBasis(uv.x);
	float4 bv = BernsteinBasis(uv.y);
	float3 vertex = BezierVertex(patch, bu, bv);

	output.Coord = mul(float4(vertex, 1.0f), gWorldTrans);
	output.Coord = mul(output.Coord, gViewProjTrans);

	return output;
}
//像素着色器
float4 PixelShade(Vertex input): SV_Target{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
