//------------------------------------------------------------------------------
//  kokmodel.fx
//  
//  (C) 2008 by ctuo
//------------------------------------------------------------------------------

float4x4 mvp : ModelViewProjection;
float4x4 world : Model;
float4x4 view : View;
float4x4 worldView : ModelView;
float3x2 texMatrix : TextureMatrix;				// 纹理动画矩阵
matrix<float,4,3> jPalette[72] : JointPalette;	// 骨骼动画矩阵

// material
float4 diffuseColor : DiffuseColor;
float4 ambientColor : AmbientColor;
float4 specularColor: SpecularColor;
float4 emissiveColor: EmissiveColor;

float4 lightDiffColor[8] : LightDiffColor;
float4 lightAmibColor[8] : LightAmibColor;
float4 lightSpecColor[8] : LightSpecColor;
float4 lightEmisColor[8] : LightEmisColor;

texture tex : DiffMap0;

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
};

struct vsOut
{
	float4 pos 		: POSITION;
	float2 tex		: TEXCOORD0;
};

//------------------------------------------------------------------------------
/**
    skinnedPosition()
    
    Compute a skinned position.
    
    @param  inPos           input vertex position
    @param  weights         4 weights
    @param  indices         4 joint indices into the joint palette
    @param  jointPalette    joint palette as vector4 array
    @return                 the skinned position
*/
float4
skinnedPosition(const float4 inPos, const float4 weights, const float4 indices, const matrix<float,4,3> jointPalette[72])
{    
    float3 pos = (0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; i++)
	{
		if (weights[i] > 0.0)
		{
			pos += (mul(inPos, jointPalette[indices[i]])) * weights[i];
		}
	}
	return float4(pos, 1.0f);
		
	/*float3 pos[4];
    pos[0] = (mul(inPos, jointPalette[indices[0]])) * weights[0];
    pos[1] = (mul(inPos, jointPalette[indices[1]])) * weights[1];
    pos[2] = (mul(inPos, jointPalette[indices[2]])) * weights[2];
    pos[3] = (mul(inPos, jointPalette[indices[3]])) * weights[3];
    return float4(pos[0] + pos[1] + pos[2] + pos[3], 1.0f);*/
}

//------------------------------------------------------------------------------
/**
    skinnedNormal()
    
    Compute a skinned normal vector (without renormalization).
    
    @param  inNormal        input normal vector
    @param  weights         4 weights
    @param  indices         4 joint indices into the joint palette
    @param  jointPalette    joint palette as vector4 array
    @return                 the skinned normal
*/
float3
skinnedNormal(const float3 inNormal, const float4 weights, const float4 indices, const matrix<float,4,3> jointPalette[72])
{
    float3 normal[4];
    normal[0] = mul(inNormal, (matrix<float,3,3>)jointPalette[indices[0]]) * weights[0];
    normal[1] = mul(inNormal, (matrix<float,3,3>)jointPalette[indices[1]]) * weights[1];
    normal[2] = mul(inNormal, (matrix<float,3,3>)jointPalette[indices[2]]) * weights[2];
    normal[3] = mul(inNormal, (matrix<float,3,3>)jointPalette[indices[3]]) * weights[3];
    return float3(normal[0] + normal[1] + normal[2] + normal[3]);
}

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
  有纹理动画
*/
vsOut
VSTexAni(float3 pos : POSITION,
				 float3 nor : NORMAL,
				 float3 tex : TEXCOORD0)
{
	vsOut vout;
	vout.pos = mul(float4(pos, 1.0), mvp);
	tex.z = 1.0f;
	vout.tex = mul(tex, texMatrix);
    return vout;
}

//------------------------------------------------------------------------------
/**
  有骨骼动画
*/
vsOut
VSSkinAni(float4 pos 	: POSITION,
		 float3 nor 	: NORMAL,
		 float2 tex 	: TEXCOORD0,
		 float4 weights	: BLENDWEIGHT,
		 float4 indices	: BLENDINDICES
		 )
{
	vsOut vout;
	
	float4 skinPos = skinnedPosition(pos, weights, indices, jPalette);
	vout.pos = mul(skinPos, mvp);
	vout.tex = tex;
    return vout;
}

//------------------------------------------------------------------------------
/**
*/
float4
PSSample(vsOut vs) : COLOR
{
    return tex2D(s1, vs.tex);
}

//------------------------------------------------------------------------------
/**
*/
technique t0<string Mask = "Solid";>
{
    pass p0
    {
        CullMode     = NONE;        
        VertexShader = compile vs_2_0 VSSample();
        PixelShader  = compile ps_2_0 PSSample();
    }
}

technique t0<string Mask = "Solid|Tex";>
{
    pass p0
    {
        CullMode     = NONE;        
        VertexShader = compile vs_2_0 VSTexAni();
        PixelShader  = compile ps_2_0 PSSample();
    }
}

technique t0<string Mask = "Solid|Skin";>
{
    pass p0
    {
        CullMode     = CCW;        
        VertexShader = compile vs_2_0 VSSkinAni();
        PixelShader  = compile ps_2_0 PSSample();
    }
}
