//cbuffer cbFrame: register(b0){
//	float4x4 gViewProjMat;
//	float3   gCamCoord;
//	float    gTimeSpan;
//};
//TextureCube gColorMap: register(t0);
//SamplerState gSampler: register(s0);

//struct VSINPUT{
//	float3 Coord: POSITION;
//};
//struct PSInput{
//	float4 ProjCoord: SV_POSITION;
//    float3 LocalCoord: POSITION;
//};
 
//PSInput VertexShade(VSINPUT Input){
//	PSInput Output;

//	float4 WorldCoord = float4(Input.Coord + gCamCoord, 1.f);
//	Output.ProjCoord = mul(WorldCoord, gViewProjMat).xyww;
//	Output.LocalCoord = Input.Coord;

//	return Output;
//}
//float4 PixelShade(PSInput Input): SV_Target{
//	return gColorMap.Sample(gSampler, Input.LocalCoord);
//}