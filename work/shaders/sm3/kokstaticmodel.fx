//------------------------------------------------------------------------------
//  kokmodel.fx
//  
//  (C) 2008 by ctuo
//------------------------------------------------------------------------------

shared float4x4 mvp : ModelViewProjection;
//shared float4x4 world : Model;
//shared float4x4 view : View;
//shared float4x4 worldView : ModelView;
//shared float3x2 texMatrix : TextureMatrix;				// 纹理动画矩阵

// material
shared float4 diffuseColor : DiffuseColor;
shared float4 ambientColor : AmbientColor;
shared float4 specularColor: SpecularColor;
shared float4 emissiveColor: EmissiveColor;
/*
shared float4 lightDiffColor[8] : LightDiffColor;
shared float4 lightAmibColor[8] : LightAmibColor;
shared float4 lightSpecColor[8] : LightSpecColor;
shared float4 lightEmisColor[8] : LightEmisColor;

texture fakeReflect 	 : FakeReflect;
texture fakeReflectLight : FakeReflectLight;
*/

texture tex 			 : DiffMap0;

int CullMode 		= 3;	// CCW
int AlphaDestBlend 	= 5;	// SrcAlpha
int AlphaSrcBlend	= 6;	// InvSrcAlpha
float4 TextureFactor	= float4(1.0f, 1.0f, 1.0f, 1.0f);

sampler s1 = 
sampler_state
{
	Texture = <tex>;
	//MinFilter = Anisotropic;
	//MaxAnisotropy = 8;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = Wrap;
	AddressV  = Wrap;
	//MipMapLodBias = -0.75;
};

/*
sampler fr = 
sampler_state
{
	Texture = <fakeReflect>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = Wrap;
	AddressV  = Wrap;
	//MipMapLodBias = -0.75;
};

sampler frl = 
sampler_state
{
	Texture = <fakeReflectLight>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = Wrap;
	AddressV  = Wrap;
	//MipMapLodBias = -0.75;
};
*/

struct vsOut
{
	float4 pos 		: POSITION;
	float2 tex		: TEXCOORD0;
};

//------------------------------------------------------------------------------
/**
  无纹理和骨骼动画
*/
vsOut
VSSample(float3 pos : POSITION,
				 float3 nor : NORMAL,
				 float2 tex : TEXCOORD0)
{
	vsOut vout;
	vout.pos = mul(float4(pos, 1.0), mvp);
	vout.tex = tex;
    return vout;
}

//------------------------------------------------------------------------------
/**
*/
float4
PSSample(vsOut vs) : COLOR
{
    float4 color = tex2D(s1, vs.tex);
	//color = color * TextureFactor;
	return color;
}

float4
PSAlpha(vsOut vs) : COLOR
{
    float4 color = tex2D(s1, vs.tex)* TextureFactor;
	return color;
}

technique t0<string Mask = "Solid|Static";>
{
    pass p0
    {
        CullMode     = CCW;
		
		// 所有地物都会开alphatest！！很浪费，文件中没有地方判断是否需要
		AlphaTestEnable = true;
		AlphaRef 		= 0x01;
		AlphaFunc		= GreaterEqual;
		
        VertexShader = compile vs_2_0 VSSample();
        PixelShader  = compile ps_2_0 PSSample();
    }
}

technique t0<string Mask = "Alpha|Static";>
{
    pass p0
    {
		CullMode     	  	= <CullMode>;
		
		DestBlend         	= <AlphaDestBlend>;
        SrcBlend          	= <AlphaSrcBlend>;
		
		AlphaTestEnable = true;
		AlphaRef 		= 0x01;
		AlphaFunc		= GreaterEqual;
		
        VertexShader = compile vs_2_0 VSSample();
        PixelShader  = compile ps_2_0 PSAlpha();
    }
}

/*
technique t0<string Mask = "Alpha|Static|Opacity";>
{
    pass p0
    {
		CullMode     	  	= <CullMode>;
		//AlphaBlendEnable  = true;			// 在b_alpha设置过
		//ZWriteEnable      = False;
		DestBlend         	= <AlphaDestBlend>;
        SrcBlend          	= <AlphaSrcBlend>;
		AlphaRef		    = 0x01;
		
		AlphaArg1[0]		= Texture;
		AlphaArg2[0]		= TFactor;
		AlphaOp[0]			= Modulate;
		ColorArg1[0]		= Texture;
		ColorArg2[0]		= TFactor;
		ColorOp[0]			= Modulate;
		TextureFactor		= <TextureFactor>;
		
		
        VertexShader = compile vs_2_0 VSSample();
        PixelShader  = compile ps_2_0 PSSample();
    }
}
*/
