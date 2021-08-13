cbuffer cbSetting: register(b0){
	float gMorphLerp;
	bool  gWithNormal;
	uint  gDestStride;
	uint  gMaxIndex;
};
StructuredBuffer<float3> gInput0: register(t0);
StructuredBuffer<float3> gInput1: register(t1);
RWStructuredBuffer<float> gOutput: register(u0);

[numthreads(32, 1, 1)]
void VertexMorph(uint3 ThreadID: SV_DispatchThreadID){
	if(ThreadID.x > gMaxIndex) return;

	uint SrcIndex = ThreadID.x;
	uint DestIndex = ThreadID.x * gDestStride;
	if(gWithNormal) SrcIndex *= 2;

	float3 Coord = gInput1[SrcIndex]*gMorphLerp +
		gInput0[SrcIndex]*(1.0f - gMorphLerp);

	gOutput[DestIndex] = Coord.x;
	gOutput[DestIndex + 1] = Coord.y;
	gOutput[DestIndex + 2] = Coord.z;

	if(gWithNormal){
		SrcIndex += 1;
		DestIndex += 3;

		float3 Normal = gInput1[SrcIndex]*gMorphLerp +
			gInput0[SrcIndex]*(1.0f - gMorphLerp);

		gOutput[DestIndex] = Normal.x;
		gOutput[DestIndex + 1] = Normal.y;
		gOutput[DestIndex + 2] = Normal.z;
	}
}