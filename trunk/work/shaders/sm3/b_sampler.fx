//------------------------------------------------------------------------------
//  b_sampler.fx
//
//  Batch shader for solid shapes.
//
//  2008 cTuo
//------------------------------------------------------------------------------

technique t0
{
    pass p0
    {
        ColorWriteEnable  = RED|GREEN|BLUE|ALPHA;
        ZEnable           = True;
        ZWriteEnable      = true;
        ZFunc             = LessEqual;
        AlphaBlendEnable  = false;
    }
}
