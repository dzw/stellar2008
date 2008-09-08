//------------------------------------------------------------------------------
//  kokmodel.fx
//  
//  (C) 2008 by ctuo
//------------------------------------------------------------------------------

float4x4 mvp : ModelViewProjection;
float4x4 world : Model;
float4x4 view : View;
float4x4 worldView : ModelView;
float3x2 texMatrix : TextureMatrix;				// ����������
matrix<float,4,3> jPalette[72] : JointPalette;	// ������������

// material
float4 diffuseColor : DiffuseColor;
float4 ambientColor : AmbientColor;
float4 specularColor: SpecularColor;
float4 emissiveColor: EmissiveColor;

float4 lightDiffColor[8] : LightDiffColor;
float4 lightAmibColor[8] : LightAmibColor;
float4 lightSpecColor[8] : LightSpecColor;
float4 lightEmisColor[8] : LightEmisColor;

texture tex 			 : DiffMap0;
texture fakeReflect 	 : FakeReflect;
texture fakeReflectLight : FakeReflectLight;

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

sampler fr = 
sampler_state
{
	Texture = <fakeReflect>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = Wrap;
	AddressV  = Wrap;
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
};

struct vsOut
{
	float4 pos 		: POSITION;
	float2 tex		: TEXCOORD0;
};

// ���ٷ���
struct vsOutReflect
{
	float4 pos 		: POSITION;
	float2 tex		: TEXCOORD0;
	float2 texReflect : TEXCOORD1;
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
  ������͹�������
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
  ��������
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
  �й�������
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
  ��������+�ٷ���
*/
vsOutReflect
VSSkinAniReflect(float4 pos 	: POSITION,
				 float3 nor 	: NORMAL,
				 float2 tex 	: TEXCOORD0,
				 float4 weights	: BLENDWEIGHT,
				 float4 indices	: BLENDINDICES
				 )
{
	vsOutReflect vout;
	
	float4 skinPos = skinnedPosition(pos, weights, indices, jPalette);
	float3 skinNormal = skinnedNormal(nor, weights, indices, jPalette);
	
	vout.pos = mul(skinPos, mvp);
	vout.tex = tex;
	
	// ����ٷ��� ��ͼ����ӳ������
	float u = asin(skinNormal.x) / 3.1415 + 0.5;
	float v = asin(skinNormal.y) / 3.1415 + 0.5;
	vout.texReflect = float2(u, v);
	
    return vout;
}

//------------------------------------------------------------------------------
/**
  ����+�ٷ���
*/
vsOutReflect
VSWeaponReflect(float4 pos 	: POSITION,
				 float3 nor 	: NORMAL,
				 float2 tex 	: TEXCOORD0
				 )
{
	vsOutReflect vout;
	
	float3 oNor = mul(nor, world);
	
	vout.pos = mul(pos, mvp);
	vout.tex = tex;
	
	// ����ٷ�����ͼ����ӳ������
	float u = asin(oNor.x) / 3.1415 + 0.5;
	float v = asin(oNor.y) / 3.1415 + 0.5;
	vout.texReflect = float2(u, v);
	
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
  �ٷ���
*/
float4
PSReflect(vsOutReflect vs) : COLOR
{
	float4 c1 = tex2D(frl, vs.texReflect);
	float4 c2 = tex2D(fr, vs.tex);
	float4 c3 = tex2D(s1, vs.tex);
	return (c1 * c2 + c3); //* vs.color; ���Ҫ���϶�����ɫ(���)
	
	//return tex2D(s1, vs.tex);
}


//------------------------------------------------------------------------------
/**
*/
technique t0<string Mask = "Solid";>
{
    pass p0
    {
        //CullMode     = NONE;        
        VertexShader = compile vs_2_0 VSSample();
        PixelShader  = compile ps_2_0 PSSample();
    }
}

technique t0<string Mask = "Solid|Tex";>
{
    pass p0
    {
       // CullMode     = NONE;        
        VertexShader = compile vs_2_0 VSTexAni();
        PixelShader  = compile ps_2_0 PSSample();
    }
}

technique t0<string Mask = "Solid|Static";>
{
    pass p0
    {
		DepthBias	 = 0.0f;
        CullMode     = CCW;
		
        VertexShader = compile vs_2_0 VSSample();
        PixelShader  = compile ps_2_0 PSSample();
    }
}

// ��������
technique t0<string Mask = "Solid|Skin";>
{
    pass p0
    {
        CullMode     = CCW;        
        VertexShader = compile vs_2_0 VSSkinAni();
        PixelShader  = compile ps_2_0 PSSample();
    }
}

// ��������+�ٷ���
technique t0<string Mask = "Solid|Skin|Reflect";>
{
    pass p0
    {
        CullMode     = CCW;        
        VertexShader = compile vs_2_0 VSSkinAniReflect();
        PixelShader  = compile ps_2_0 PSReflect();
    }
}

// �����ٷ���
technique t0<string Mask = "Solid|Weapon|Reflect";>
{
    pass p0
    {
        CullMode     = CCW;        
        VertexShader = compile vs_2_0 VSWeaponReflect();
        PixelShader  = compile ps_2_0 PSReflect();
    }
}
