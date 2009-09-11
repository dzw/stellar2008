//------------------------------------------------------------------------------
//  p_color.fx
//
//  Pass-shader for the color pass.
//
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------

technique t0
{
    pass p0
    {
        ColorWriteEnable  = RED|GREEN|BLUE|ALPHA;
        ZEnable           = True;
        ZWriteEnable      = True;
        ZFunc             = LessEqual;
        StencilEnable     = False;
        FogEnable         = False;
        AlphaBlendEnable  = False;
        AlphaTestEnable   = False;
        ScissorTestEnable = False;
        CullMode          = CCW;
        FillMode		  = solid;
		DepthBias		  = 0.0f;
		//fillmode		  = wireframe;
    }
}
       