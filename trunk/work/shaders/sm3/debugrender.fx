float4x4 mvp : ModelViewProjection;
bool zenable : ZEnable = false;

struct vsOut
{
	float4 pos : POSITION;
	float4 color : TEXCOORD0;
};

//------------------------------------------------------------------------------
/**
*/
vsOut
VertexShaderColorFunc(float4 pos : POSITION, float4 col : TEXCOORD0)
{
	vsOut vout;
	vout.pos = mul(pos, mvp);
	vout.color = col;
	return vout;
}

//------------------------------------------------------------------------------
/**
*/
float4
PixelShaderColorFunc(vsOut In) : COLOR
{
    return In.color;
}

//------------------------------------------------------------------------------
/**
*/
technique t0<string Mask = "ShapeColor";>
{
    pass p0
    {
        ColorWriteEnable  = RED|GREEN|BLUE|ALPHA;
        ZEnable           = zenable;
        ZWriteEnable      = zenable;
        StencilEnable     = false;
        FogEnable         = False;
        AlphaBlendEnable  = True;
        SrcBlend          = SrcAlpha;
        DestBlend         = InvSrcAlpha;
        AlphaTestEnable   = False;
        ScissorTestEnable = False;
        CullMode          = ccw;        
        VertexShader = compile vs_2_0 VertexShaderColorFunc();
        PixelShader = compile ps_2_0 PixelShaderColorFunc();
    }
}
