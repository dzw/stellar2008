//------------------------------------------------------------------------------
//  shape.fx
//  Shape shader for debug visualization.
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------

float4x4 mvp : ModelViewProjection;
float4 diffColor : MatDiffuse;
bool zenable : ZEnable = false;

//------------------------------------------------------------------------------
/**
*/
float4
VertexShaderFunc(float4 pos : POSITION) : POSITION
{
    return mul(pos, mvp);
}

//------------------------------------------------------------------------------
/**
*/
float4
PixelShaderFunc() : COLOR
{
    return diffColor;
}

//------------------------------------------------------------------------------
/**
*/
technique Default
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
        VertexShader = compile vs_2_0 VertexShaderFunc();
        PixelShader = compile ps_2_0 PixelShaderFunc();
    }
}

