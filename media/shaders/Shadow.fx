TextureCube ShadowMaps[255];
SamplerState Sampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
	AddressW = Clamp;
};

int ShadowMap_Count			: SHADOWMAPCOUNT;
float4x4 World				: WORLD;
float4x4 View				: WORLDVIEW;
float4x4 Projection			: PROJECTION;
float4x4 Bones[255]			: BONEARRAY;
float4x4 CubeViews[6]			: CUBEVIEWS;

struct VS_INPUT
{
	float4 pos			: POSITION;
	float3 norm			: NORMAL;
	float2 tex			: TEXCOORD0;
	float4 bw			: TEXCOORD1;
	float4 bi			: TEXCOORD2;
};

struct GS_INPUT
{
	float4 pos			: SV_POSITION;
};

struct PS_INPUT
{
	float4 pos			: SV_POSITION;
	float4 pos2			: POSITION;
	uint RTIndex 			: SV_RenderTargetArrayIndex;
};

GS_INPUT VS( VS_INPUT In )
{
	GS_INPUT Out = (GS_INPUT)0;
   	float4x4 skinTransform = 0;
    	skinTransform += Bones[In.bi.x] * In.bw.x;
    	skinTransform += Bones[In.bi.y] * In.bw.y;
    	skinTransform += Bones[In.bi.z] * In.bw.z;
    	skinTransform += Bones[In.bi.w] * In.bw.w;
    	Out.pos = mul(In.pos, skinTransform);
	return Out;
}

[maxvertexcount(18)]
void GS( triangle GS_INPUT input[3], inout TriangleStream<PS_INPUT> CubeMapStream )
{
	for( int f = 0; f < 6; ++f )
	{
		PS_INPUT output;
		output.RTIndex = f;
		for( int v = 0; v < 3; v++ )
		{
			output.pos2 = output.pos;
			output.pos = mul( input[v].pos, mul( View, CubeViews[f] ) );
			output.pos = mul( output.pos, Projection );
			CubeMapStream.Append( output );
		}
		CubeMapStream.RestartStrip();
	}
}

float4 PS( PS_INPUT In) : SV_Target
{
return float4( distance(View[3], In.pos2), 0, 0, 1);
}

technique10 Render
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( CompileShader( gs_4_0, GS() ) );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}