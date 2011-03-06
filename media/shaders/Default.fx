Texture2D txDiffuse;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

TextureCube txEnvironment;
SamplerState envSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
	AddressW = Clamp;
};

TextureCube ShadowMaps[255]		: SHADOWMAPS;
SamplerState Sampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
	AddressW = Clamp;
};

float Environment_map_intensity = 1.000000;

float4 Ambient = float4( 0.47f, 0.47f, 0.47f, 1.0f );
	
float4 Diffuse = float4( 0.5f, 0.5f, 0.5f, 1.0f );


int Light_Count				: LIGHTCOUNT;
float4 Lights[255]			: LIGHTARRAY;
float4 LightColors[255]			: LIGHTCOLORARRAY;
float4x4 World				: WORLD;
float4x4 View				: WORLDVIEW;
float4x4 Projection			: PROJECTION;
float4x4 Bones[255]			: BONEARRAY;




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
	float4 pos2			: POSITION;
	float2 tex			: TEXCOORD0;
	float3 norm			: NORMAL;
};

PS_INPUT VS( VS_INPUT In )
{
	PS_INPUT Out = (PS_INPUT)0;
   	float4x4 skinTransform = 0;
    	skinTransform += Bones[In.bi.x] * In.bw.x;
    	skinTransform += Bones[In.bi.y] * In.bw.y;
    	skinTransform += Bones[In.bi.z] * In.bw.z;
    	skinTransform += Bones[In.bi.w] * In.bw.w;
    	Out.pos = mul(In.pos, skinTransform);
	Out.pos2 = Out.pos;
	Out.norm = mul(In.norm, (float3x3)skinTransform);
    	Out.pos = mul(mul(Out.pos, View), Projection);
	Out.tex = In.tex;
	return Out;
}


float4 PS( PS_INPUT In) : SV_Target
{
float4 color = float4( 0, 0, 0, 0 );
float4 temp = txDiffuse.Sample( samLinear, In.tex ) + ( Environment_map_intensity * txEnvironment.Sample( envSampler, -reflect( normalize( In.pos2 - View[3] ).xyz, In.norm.xyz )));
for(int i = 0; i < Light_Count; i++)
{
float4 corrected_pos = In.pos2 + Lights[i];
color = color + clamp((((temp * LightColors[i])  * (dot(-In.norm, normalize(In.pos2 - Lights[i] )))) * 10) / distance( In.pos2, Lights[i] ), 0.0, 1.0);
}
color.w = temp.w;
return color * Diffuse;
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


