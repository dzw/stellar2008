float4x4 mvp : ModelViewProjection;
texture tex : DiffMap0;

sampler s1 = 
sampler_state
{
	Texture = <tex>;
	MipFilter = LINEAR;
	MinFilter = Anisotropic;
	MagFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = Wrap;
	AddressV  = Wrap;
};


struct vsTestOut
{
	float4 pos 		: POSITION;
	float2 tex		: TEXCOORD0;
	float2 tex1		: TEXCOORD1;
	float2 tex2		: TEXCOORD2;
	float2 tex3		: TEXCOORD3;
	float2 tex4		: TEXCOORD4;
};

//------------------------------------------------------------------------------
/**
*/
vsTestOut
VertexShaderTestFunc(float3 pos 		: POSITION,
					 float3 nor 		: NORMAL,
					 float2 tex			: TEXCOORD0,
					 float2 tex1		: TEXCOORD1,
					 float2 tex2		: TEXCOORD2,
					 float2 tex3		: TEXCOORD3,
					 float2 tex4		: TEXCOORD4
					 )
{
	vsTestOut vout;
	vout.pos = mul(float4(pos, 1.0), mvp);
	//vout.pos = mul(pos, mvp);
	vout.tex = tex;
	vout.tex1 = tex1;
	vout.tex2 = tex2;
	vout.tex3 = tex3;
	vout.tex4 = tex4;

    return vout;
}

float4
PixelShaderTestFunc1(vsTestOut In) : COLOR
{
	//return float4(0, 0, 0, 0);
	vector diff = tex2D(s1, In.tex);
	return diff;
}	
float4
PixelShaderTestFunc2(vsTestOut In) : COLOR
{
	//return float4(0, 0, 0, 0);
	vector diff = tex2D(s1, In.tex1);
	return diff;
}
float4
PixelShaderTestFunc3(vsTestOut In) : COLOR
{
	//return float4(0, 0, 0, 0);
	vector diff = tex2D(s1, In.tex2);
	return diff;
}
float4
PixelShaderTestFunc4(vsTestOut In) : COLOR
{
	//return float4(0, 0, 0, 0);
	vector diff = tex2D(s1, In.tex3);
	return diff;
}


technique t0<string Mask = "Solid";>
{
    pass p0
    {
		//FillMode		  = WireFrame;
        VertexShader = compile vs_2_0 VertexShaderTestFunc();
        PixelShader = compile ps_2_0 PixelShaderTestFunc1();
    }
}

technique t0<string Mask = "Solid|KOK1";>
{
    pass p0
    {
		//FillMode		  = WireFrame;
		AlphaBlendEnable  = false;
        VertexShader = compile vs_2_0 VertexShaderTestFunc();
        PixelShader = compile ps_2_0 PixelShaderTestFunc1();
    }
}
technique t0<string Mask = "Solid|KOK2";>
{
    pass p0
    {
		//AlphaTestEnable  = true;
		//AlphaRef 		 = 0x01;
		//AlphaFunc		 = GreaterEqual;
		AlphaBlendEnable = true;
		SrcBlend         = SRCALPHA;
		DestBlend        = INVSRCALPHA;
		//AlphaArg1[0]	 = Texture;
		//AlphaArg2[0]	 = TFactor;
		//AlphaOp[0]		 = Modulate;
		
		FillMode		 = Solid;//WireFrame;
        VertexShader = compile vs_2_0 VertexShaderTestFunc();
        PixelShader = compile ps_2_0 PixelShaderTestFunc2();
    }
}
technique t0<string Mask = "Solid|KOK3";>
{
    pass p0
    {
		//AlphaTestEnable  = true;
		//AlphaRef 		 = 0x01;
		//AlphaFunc		 = GreaterEqual;
		AlphaBlendEnable = true;
		SrcBlend         = SRCALPHA;
		DestBlend        = INVSRCALPHA;
		//AlphaArg1[0]	 = Texture;
		//AlphaArg2[0]	 = TFactor;
		//AlphaOp[0]		 = Modulate;
		
		FillMode		 = Solid;//WireFrame;
        VertexShader = compile vs_2_0 VertexShaderTestFunc();
        PixelShader = compile ps_2_0 PixelShaderTestFunc3();
    }
}
technique t0<string Mask = "Solid|KOK4";>
{
    pass p0
    {
		//AlphaTestEnable  = true;
		//AlphaRef 		 = 0x01;
		//AlphaFunc		 = GreaterEqual;
		AlphaBlendEnable = true;
		SrcBlend         = SRCALPHA;
		DestBlend        = INVSRCALPHA;
		//AlphaArg1[0]	 = Texture;
		//AlphaArg2[0]	 = TFactor;
		//AlphaOp[0]		 = Modulate;
		
		FillMode		 = Solid;//WireFrame;
        VertexShader = compile vs_2_0 VertexShaderTestFunc();
        PixelShader = compile ps_2_0 PixelShaderTestFunc4();
    }
}