//------------------------------------------------------------------------------
//  sample.fx
//  sample shader for debug visualization.
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------

float4x4 mvp : ModelViewProjection;
float4 diffColor : MatDiffuse;
texture tex : DiffMap0;

struct vsOut
{
	float4 pos : POSITION;
	//float3 nor : TEXCOORD0;
	float2 tex : TEXCOORD0;
};

sampler s1 = 
sampler_state
{
	Texture = <tex>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

//------------------------------------------------------------------------------
/**
*/
vsOut
VertexShaderFunc(float4 vPos : POSITION, 
				 float3 vNormal : NORMAL,
				 float2 vTexCoord0 : TEXCOORD0)
{
	vsOut vout;
	vout.pos = mul(vPos, mvp);
	//vout.nor = vNormal;
	vout.tex = vTexCoord0;
    return vout;
}

//------------------------------------------------------------------------------
/**
*/
float4
PixelShaderFunc(vsOut In) : COLOR
{
	float4 col;
	col = tex2D(s1,In.tex);
    return col;
}

//------------------------------------------------------------------------------
/**
*/
technique t0<string Mask = "Default";>
{
    pass p0
    {
    		//FillMode					= WireFrame;
        AlphaTestEnable   = true;
        AlphaRef					= 0x000000aa;
        AlphaFunc					= greater;
        CullMode          = none;
        VertexShader = compile vs_3_0 VertexShaderFunc();
        PixelShader = compile ps_3_0 PixelShaderFunc();
    }
}
