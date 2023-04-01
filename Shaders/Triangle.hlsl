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

struct TCommonCB {
	float4 Color;
};

cbuffer CommonCB : register(b0) {
	TCommonCB CommonCB;
}

struct TOutputPS {
	float4 Color : SV_TARGET0;
};

TOutputPS MainPS(TOutputVS Input) {
	TOutputPS Output;
	Output.Color = float4(Input.Color * CommonCB.Color, 1.0);
	return Output;
}
