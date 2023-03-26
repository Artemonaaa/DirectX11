struct TInputVS {
	float2 Position : POSITION;
	float3 Color : COLOR;
};

struct TOutputVS {
	float4 Position : SV_POSITION;
	float3 Color : COLOR;
};

TOutputVS MainVS(TInputVS Input) {
	TOutputVS Output;
	Output.Position = float4(Input.Position, 0.0, 1.0);
	Output.Color = Input.Color;
	return Output;
}

struct TOutputPS {
	float4 Color : SV_TARGET0;
};

TOutputPS MainPS(TOutputVS Input) {
	TOutputPS Output;
	Output.Color = float4(Input.Color, 1.0);
	return Output;
}
