//------------------------------------------------------------------------------
//  terrain1.fx
//  
//  (C) 2008 by ctuo
//------------------------------------------------------------------------------

float4x4 mvp : ModelViewProjection;
float4x4 world : Model;
float4x4 view : View;
float4x4 worldView : ModelView;

texture tex : DiffMap0;
texture tex1 : DiffMap1;
texture tex2 : DiffMap2;
texture tex3 : DiffMap3;
texture texBlend : TexBlend0;

float4 diff_color = float4( 1.0f, 1.0f, 1.0f, 1.0f );
float4 spec_color = float4( 1.0f, 1.0f, 1.0f, 1.0f );
float4 shadow_col = float4( 0.1f, 0.1f, 0.1f, 0.1f );
float4 vecLightDir= float4(1.0f, -1.0f, 1.0f, 1.0f );			// �����ⷽ��

float4 I_a = { 0.1f, 0.1f, 0.1f, 1.0f };						// ������
float4 I_d = { 1.0f, 1.0f, 1.0f, 1.0f };						// �����
float4 I_s = { 1.0f, 1.0f, 1.0f, 1.0f };						// �����

float4 k_a : MATERIALAMBIENT = { 1.0f, 1.0f, 1.0f, 1.0f };		// ���������
float4 k_d : MATERIALAMBIENT = { 1.0f, 1.0f, 1.0f, 1.0f };		// ��������
float4 k_s : MATERIALAMBIENT = { 1.0f, 1.0f, 1.0f, 1.0f };		// ��������

int n : MATERIALPOWER = 64;

struct vsOut
{
	float4 pos 		: POSITION;
	float2 tex		: TEXCOORD0;
	float2 texBlend : TEXCOORD1;
	float4 diffuse	: COLOR0;
	float4 specular	: COLOR1;
};

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

sampler s2 = 
sampler_state
{
	Texture = <tex1>;
	MipFilter = LINEAR;
	MinFilter = Anisotropic;
	MagFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = Wrap;
	AddressV  = Wrap;
};

sampler s3 = 
sampler_state
{
	Texture = <tex2>;
	MipFilter = LINEAR;
	MinFilter = Anisotropic;
	MagFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = Wrap;
	AddressV  = Wrap;
};

sampler s4 = 
sampler_state
{
	Texture = <tex3>;
	MipFilter = LINEAR;
	MinFilter = Anisotropic;
	MagFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = Wrap;
	AddressV  = Wrap;
};

sampler samplerTileBlend = 
sampler_state
{
	Texture = <texBlend>;
	MipFilter = LINEAR;
	MinFilter = Anisotropic;
	MagFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = Wrap;
	AddressV  = Wrap;
};

//------------------------------------------------------------------------------
/**
*/
vsOut
VertexShaderFunc(float3 vPos 	 : POSITION, 
				 float3 vNormal  : NORMAL,
				 float2 tex 	 : TEXCOORD0,
				 float2 texBlend : TEXCOORD1)
{
	vsOut vout;
	vout.pos = mul(float4(vPos, 1.0), mvp);
	vout.tex = tex;
	vout.texBlend = texBlend;
	
	float3 L = -vecLightDir;
	float4x4 worldView = mul(world, view);
	float3 P = mul(float4(vPos, 1.0), (float4x3)worldView);
	float3 N = normalize(mul(vNormal, (float3x3)worldView));
	float3 R = normalize(2 * dot(N, L) * N - L);
	float3 V = -normalize(P);
	vout.diffuse = I_a * k_a + I_d * k_d * max(0, dot(N, L));
	vout.specular = I_s * k_s * pow(max(0, dot(R, V)), n/4);
	
    return vout;
}

//------------------------------------------------------------------------------
/**
*/
float4
PixelShaderFunc1(vsOut In) : COLOR
{
	//return float4(0, 0, 0, 0);

	vector diffuse;
	vector col;
	float4 blend;
	float4 specular;
	
	blend = tex2D(samplerTileBlend, In.texBlend);
	col = tex2D(s1, In.tex);
	
	// specular
	//specular = spec_color * col.w;
	//col = col * diff_color + specular;
	//col = col * In.diffuse;
	
	// shadow
	diffuse = lerp(col, shadow_col, blend.a);
	//diffuse = col;
	
	// alpha
	//diffuse.a = shadow_col.a;
	
	// ������
	//diffuse = diffuse + diff_color;
	
    return diffuse;
}

//------------------------------------------------------------------------------
/**
*/
float4
PixelShaderFunc2(vsOut In) : COLOR
{
	//return float4(1, 0, 0, 0);

	vector diffuse;
	vector col;
	vector layer;
	float4 blend;
	float4 specular;
	
	blend = tex2D(samplerTileBlend, In.texBlend);
	col = tex2D(s1, In.tex);
	
	// layer1
	layer = tex2D(s2, In.tex);
	col = lerp(col, layer, blend.r);
	
	// specular
	//specular = spec_color * col.w;
	//col = col * diff_color + specular;
	
	// shadow
	//diffuse = col;
	diffuse = lerp(col, shadow_col, blend.a);
	
	// alpha
	//diffuse.a = shadow_col.a;
	
	// ������
	//diffuse = diffuse + diff_color;
	
    return diffuse;
}

//------------------------------------------------------------------------------
/**
*/
float4
PixelShaderFunc3(vsOut In) : COLOR
{
	//return float4(0, 1, 0, 0);

	vector diffuse;
	vector col;
	vector layer;
	float4 blend;
	float4 specular;
	
	blend = tex2D(samplerTileBlend, In.texBlend);
	col = tex2D(s1, In.tex);
	
	// layer1
	layer = tex2D(s2, In.tex);
	col = lerp(col, layer, blend.r);
	
	// layer2
	layer = tex2D(s3, In.tex);
	col = lerp(col, layer, blend.g);
	
	// specular
	//specular = spec_color * col.w;
	//col = col * diff_color + specular;
	
	// shadow
	//diffuse = col;
	diffuse = lerp(col, shadow_col, blend.a);
	
	// alpha
	//diffuse.a = shadow_col.a;
	
	// ������
	//diffuse = diffuse + diff_color;
	
    return diffuse;
}

//------------------------------------------------------------------------------
/**
*/
float4
PixelShaderFunc4(vsOut In) : COLOR
{
	//return float4(0, 0, 1, 0);

	vector diffuse;
	vector col;
	vector layer;
	float4 blend;
	float4 specular;
	
	blend = tex2D(samplerTileBlend, In.texBlend);
	col = tex2D(s1, In.tex);
	
	// layer1
	layer = tex2D(s2, In.tex);
	col = lerp(col, layer, blend.r);
	
	// layer2
	layer = tex2D(s3, In.tex);
	col = lerp(col, layer, blend.g);
	
	// layer3
	layer = tex2D(s4, In.tex);
	col = lerp(col, layer, blend.b);
	
	// specular
	//specular = spec_color * col.w;
	//col = col * diff_color + specular;
	//col = col * In.diffuse;
	
	// shadow
	diffuse = lerp(col, shadow_col, blend.a);
	
	// alpha
	//diffuse.a = shadow_col.a;
	
	// ������
	//diffuse = diffuse + diff_color;
	//diffuse = float4(blend.b, blend.b, blend.b, 1.0);
	//if(blend.r > 0)
	//	diffuse = float4(1, 0, 0, 1);
	//if(blend.g > 0)
	//	diffuse = float4(0, 1, 0, 1);
	//if(blend.b > 0)
	//	diffuse = float4(0, 0, 1, 1);
	//diffuse = col;
    return diffuse;
}

//------------------------------------------------------------------------------
/**
*/
technique t0<string Mask = "Solid|Terrain1";>
{
    pass p0
    {
				//FillMode		  = WireFrame;
        AlphaTestEnable   = false;
        //CullMode          = NONE;
        VertexShader = compile vs_2_0 VertexShaderFunc();
        PixelShader = compile ps_2_0 PixelShaderFunc1();
    }
}
technique t0<string Mask = "Solid|Terrain2";>
{
    pass p0
    {
				//FillMode		  = WireFrame;
        AlphaTestEnable   = false;
        //CullMode          = NONE;
        VertexShader = compile vs_2_0 VertexShaderFunc();
        PixelShader = compile ps_2_0 PixelShaderFunc2();
    }
}
technique t0<string Mask = "Solid|Terrain3";>
{
    pass p0
    {
				//FillMode		  = WireFrame;
        AlphaTestEnable   = false;
        //CullMode          = NONE;
        VertexShader = compile vs_2_0 VertexShaderFunc();
        PixelShader = compile ps_2_0 PixelShaderFunc3();
    }
}
technique t0<string Mask = "Solid|Terrain4";>
{
    pass p0
    {
				//FillMode		  = WireFrame;
        AlphaTestEnable   = false;
        //CullMode          = NONE;
        VertexShader = compile vs_2_0 VertexShaderFunc();
        PixelShader = compile ps_2_0 PixelShaderFunc4();
    }
}