#include "lib/common.fxh"

int AlphaSrcBlend = 5;  // SrcAlpha
int AlphaDstBlend = 6;  // InvSrcAlpha

struct VsInput
{
    float3 position  : POSITION;  // the particle position in world space
    float4 color     : COLOR0;    // the particle color
	float2 uv0       : TEXCOORD0; // the particle texture coordinates
};

struct VsOutput
{
    float4 position : POSITION;
    float2 uv0      : TEXCOORD0;
    float4 diffuse  : COLOR0;
};

VsOutput vsMain(const VsInput vsIn)
{	
    VsOutput vsOut;
    // transform to projection space
    vsOut.position = mul(float4(vsIn.position, 1.0), mvp);
    vsOut.uv0      = vsIn.uv0;
    vsOut.diffuse  = vsIn.color;

    return vsOut;
}

//------------------------------------------------------------------------------
technique t0<string Mask = "Alpha|Particle";>
{
    pass p0
    {
        TextureTransformFlags[0] = 0;
        CullMode  = None;
        SrcBlend  = <AlphaSrcBlend>;
        DestBlend = <AlphaDstBlend>;        

        Sampler[0]   = <diffMapSampler>;
        VertexShader = compile vs_2_0 vsMain();
        PixelShader  = NULL;
        
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        AlphaOp[0]   = Modulate;
        AlphaArg1[0] = Texture;
        AlphaArg2[0] = Diffuse;
           
        ColorOp[1] = Disable;        
        AlphaOp[1] = Disable;
    }
}

