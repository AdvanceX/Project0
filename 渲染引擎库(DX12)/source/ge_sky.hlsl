cbuffer cbFrame: register(b0){
	float4x4 gViewProjTrans;
	float3   gCameraPos;
	float    gTimeSpan;
};
TextureCube gTexture: register(t0);
SamplerState gSampler: register(s0);

struct VSINPUT{
	float3 Coord: POSITION;
};
struct PSINPUT{
	float4 WVPCoord: SV_POSITION;
    float3 Coord: POSITION;
};
 
PSINPUT VertexShade(VSINPUT Input){
	PSINPUT Output;

	float4 WCoord = float4(Input.Coord + gCameraPos, 1.0f);
	Output.WVPCoord = mul(WCoord, gViewProjTrans).xyww;
	Output.Coord = Input.Coord;

	return Output;
}
float4 PixelShade(PSINPUT Input): SV_Target{
	return gTexture.Sample(gSampler, Input.Coord);
}