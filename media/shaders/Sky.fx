
TextureCube txEnvironment;
SamplerState envSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
	AddressW = Clamp;
};

float4x4 World				: WORLD;
float4x4 View				: WORLDVIEW;
float4x4 Projection			: PROJECTION;

struct VS_INPUT
{
	float4 pos			: POSITION;
	float3 norm			: NORMAL;
	float2 tex			: TEXCOORD0;
	float4 bw			: TEXCOORD1;
	float4 bi			: TEXCOORD2;
};

struct PS_INPUT
{
	float4 pos			: SV_POSITION;
	float2 tex			: TEXCOORD0;
	float3 norm			: NORMAL;
};

PS_INPUT VS( VS_INPUT In )
{
	PS_INPUT Out = (PS_INPUT)0;
    	Out.pos = mul( float4( mul( In.pos.xyz, (float3x3)View), 0 ), Projection);
	Out.norm = In.pos.xyz;
	Out.tex = In.tex;
	return Out;
}


float4 PS( PS_INPUT In) : SV_Target
{
return txEnvironment.Sample( envSampler, In.norm );
}

technique10 Render
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}


