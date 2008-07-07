//------------------------------------------------------------------------------
//  shaderserverbase.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/base/shaderserverbase.h"
#include "coregraphics/streamshaderloader.h"
#include "io/ioserver.h"

namespace Base
{
ImplementClass(Base::ShaderServerBase, 'SSRV', Core::RefCounted);
ImplementSingleton(Base::ShaderServerBase);

using namespace CoreGraphics;
using namespace IO;
using namespace Util;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
ShaderServerBase::ShaderServerBase() :
    isOpen(false),
    paramBindMode(BindBySemantic),
    curShaderFeatureBits(0)
{
    ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
ShaderServerBase::~ShaderServerBase()
{
    n_assert(!this->IsOpen());
    DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
ShaderServerBase::Open()
{
    n_assert(!this->isOpen);
    n_assert(this->shaders.IsEmpty());

    // load all shaders from disk
    Array<String> files = IoServer::Instance()->ListFiles("shd:", "*");
    IndexT i;
    for (i = 0; i < files.Size(); i++)
    {
        files[i].StripFileExtension();
        ResourceId resId("shd:" + files[i]);
        Ptr<Shader> newShader = Shader::Create();
        newShader->SetResourceId(resId);
        newShader->SetLoader(StreamShaderLoader::Create());
        newShader->SetAsyncEnabled(false);
        newShader->Load();
        if (newShader->IsLoaded())
        {
            newShader->SetLoader(0);
            this->shaders.Add(resId, newShader);
        }
        else
        {
            n_error("Failed to load shader '%s'!", files[i].AsCharPtr());
        }
    }
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ShaderServerBase::Close()
{
    n_assert(this->isOpen);

    // unload all currently loaded shaders
    IndexT i;
    for (i = 0; i < this->shaders.Size(); i++)
    {
        this->shaders.ValueAtIndex(i)->Unload();
    }
    this->shaders.Clear();

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    This creates a clone of a template shader. This is the only method
    to create a new shader object. When the shader instance is no longer
    needed, call UnregisterShaderInstance() for proper cleanup.
*/
Ptr<ShaderInstance>
ShaderServerBase::CreateShaderInstance(const ResourceId& resId)
{
    n_assert(resId.Value().IsValid());

    // first check if the shader is already loaded
    if (!this->shaders.Contains(resId))
    {
        n_error("ShaderServer: shader '%s' not found!", resId.Value().AsCharPtr());
    }

    // create a shader instance object from the shader
    /*Ptr<ShaderInstance> shaderInstance = this->shaders[resId]->CreateShaderInstance();
    return shaderInstance;*/

	// ÿ��node����һ��ʵ���ǳ���ʱ��
	if (this->shaders[resId]->GetAllShaderInstances().Size() == 0)
		this->shaders[resId]->CreateShaderInstance();

    this->shaders[resId]->GetAllShaderInstances()[0]->IncrUseCount();
	return this->shaders[resId]->GetAllShaderInstances()[0];
}

//------------------------------------------------------------------------------
/**
	real time update fx file
*/
void
ShaderServerBase::Update(/*Ptr<String> resName*/)
{
	//if (resName == NULL)
	{
		for (SizeT i = 0; i < this->shaders.Size(); i++)
		{
			this->shaders.ValueAtIndex(i)->Unload();
			//this->shaders.ValueAtIndex(i)->SetResourceId(resId);
			this->shaders.ValueAtIndex(i)->SetLoader(StreamShaderLoader::Create());
			this->shaders.ValueAtIndex(i)->SetAsyncEnabled(false);
			this->shaders.ValueAtIndex(i)->Load();
			if (this->shaders.ValueAtIndex(i)->IsLoaded())
			{
				this->shaders.ValueAtIndex(i)->SetLoader(0);
				this->shaders.ValueAtIndex(i)->Update();
			}
		}
	}
	/*else
	{
		ResourceId resId(resName);
		if (this->shaders.Contains(resId))
		{
			this->shaders[resId]->Unload();
			this->shaders[resId]->SetResourceId(resId);
			this->shaders[resId]->SetLoader(StreamShaderLoader::Create());
			this->shaders[resId]->SetAsyncEnabled(false);
			this->shaders[resId]->Load();
			if (this->shaders[resId]->IsLoaded())
			{
				this->shaders[resId]->SetLoader(0);
				this->shaders[resId]->Update();
			}
		}
	}*/
}
} // namespace Base
