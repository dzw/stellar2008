//------------------------------------------------------------------------------
//  d3d9streamtextureloader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/d3d9/d3d9streamtextureloader.h"
#include "interface/iointerface.h"
#include "coregraphics/d3d9/d3d9texture.h"
#include "coregraphics/d3d9/d3d9renderdevice.h"
#include "io/memorystream.h"
#include "resources/resource.h"
#include "ddslib/ddslib.h"
#include "util/string.h"
#include "resources/SharedResourceServer.h"
#include "coregraphics/StreamTextureLoader.h"
#include "coregraphics/d3d9/d3d9types.h"

namespace Direct3D9
{
ImplementClass(Direct3D9::D3D9StreamTextureLoader, 'D9TL', Resources::ResourceLoader);

using namespace CoreGraphics;
using namespace Resources;
using namespace IO;
using namespace Interface;
using namespace Messaging;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
bool
D3D9StreamTextureLoader::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
D3D9StreamTextureLoader::OnLoadRequested()
{
    n_assert(this->GetState() == Resource::Initial);
    n_assert(this->resource.isvalid());
    if (this->resource->IsAsyncEnabled())
    {
        // perform asynchronous load
        // send off an asynchronous loader job
        n_assert(!this->readStreamMsg.isvalid());
        this->readStreamMsg = ReadStream::Create();
        this->readStreamMsg->SetURI(this->resource->GetResourceId().Value());
        this->readStreamMsg->SetStream(MemoryStream::Create());
        IOInterface::Instance()->Send(this->readStreamMsg.upcast<Message>());
        
        // go into Pending state
        this->SetState(Resource::Pending);
        return true;
    }
    else
    {
        // perform synchronous load
        Ptr<Stream> stream = IoServer::Instance()->CreateStream(this->resource->GetResourceId().Value());
        if (this->SetupTextureFromStream(stream))
        {
            this->SetState(Resource::Loaded);
            return true;
        }
        // fallthrough: synchronous loading failed
        this->SetState(Resource::Failed);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9StreamTextureLoader::OnLoadCancelled()
{
    n_assert(this->GetState() == Resource::Pending);
    n_assert(this->readStreamMsg.isvalid());
    IOInterface::Instance()->Cancel(this->readStreamMsg.upcast<Message>());
    this->readStreamMsg = 0;
    ResourceLoader::OnLoadCancelled();
}

//------------------------------------------------------------------------------
/**
*/
bool
D3D9StreamTextureLoader::OnPending()
{
    n_assert(this->GetState() == Resource::Pending);
    n_assert(this->readStreamMsg.isvalid());
    bool retval = false;

    // check if asynchronous loader job has finished
    if (this->readStreamMsg->Handled())
    {
        // ok, loader job has finished
        if (this->readStreamMsg->GetResult())
        {
            // IO operation was successful
            if (this->SetupTextureFromStream(this->readStreamMsg->GetStream()))
            {
                // everything ok!
                this->SetState(Resource::Loaded);                
                retval = true;
            }
            else
            {
                // this probably wasn't a texture file...
                this->SetState(Resource::Failed);
            }
        }
        else
        {
            // error during IO operation
            this->SetState(Resource::Failed);
        }
        // we no longer need the loader job message
        this->readStreamMsg = 0;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    This method actually setups the Texture object from the data in the stream.
*/
bool
D3D9StreamTextureLoader::SetupTextureFromStream(const Ptr<Stream>& stream)
{
    n_assert(stream.isvalid());
    n_assert(stream->CanBeMapped());
    HRESULT hr;
    IDirect3DDevice9* d3d9Device = D3D9RenderDevice::Instance()->GetDirect3DDevice();
    n_assert(0 != d3d9Device);
    n_assert(this->resource->IsA(D3D9Texture::RTTI));
    const Ptr<D3D9Texture>& res = this->resource.downcast<D3D9Texture>();
    n_assert(!res->IsLoaded());

    stream->SetAccessMode(Stream::ReadAccess);
    if (stream->Open())
    {
        void* srcData = stream->Map();
        UINT srcDataSize = stream->GetSize();
		uchar* pData = (uchar*)srcData;

		
		if (pData[0] == 'B' && 
			pData[1] == 'L' && 
			pData[2] == 'P' && 
			pData[3] == '2')
		{		
			LoadBLP(pData, srcDataSize);
			stream->Unmap();
			stream->Close();
			return true;
		}
        // first need to check image info whether to determine texture type
        D3DXIMAGE_INFO imageInfo = { 0 };
        hr = D3DXGetImageInfoFromFileInMemory(srcData, srcDataSize, &imageInfo);
        if (FAILED(hr))
        {
            n_error("D3D9StreamTextureLoader: failed to obtain image info from file '%s'!", this->resource->GetResourceId().Value().AsCharPtr());
            return false;
        }

        // load texture based on texture type
        if (D3DRTYPE_TEXTURE == imageInfo.ResourceType)
        {
            // load 2D texture
            IDirect3DTexture9* d3d9Texture = 0;
            hr = D3DXCreateTextureFromFileInMemory(d3d9Device, srcData, srcDataSize, &d3d9Texture);
			// mipmap support
			/*hr = D3DXCreateTextureFromFileInMemoryEx(d3d9Device, srcData, srcDataSize, 
				D3DX_DEFAULT, D3DX_DEFAULT, imageInfo.MipLevels, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
				D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &d3d9Texture);*/
            if (FAILED(hr))
            {
                n_error("D3D9StreamTextureLoader: D3DXCreateTextureFromFileInMemory() failed for file '%s'!", this->resource->GetResourceId().Value().AsCharPtr());
                return false;
            }
            res->SetupFromD3D9Texture(d3d9Texture);
        }
        else if (D3DRTYPE_VOLUMETEXTURE == imageInfo.ResourceType)
        {
            // load 3D texture
            IDirect3DVolumeTexture9* d3d9VolumeTexture = 0;
            hr = D3DXCreateVolumeTextureFromFileInMemory(d3d9Device, srcData, srcDataSize, &d3d9VolumeTexture);
            if (FAILED(hr))
            {
                n_error("D3D9StreamTextureLoader: D3DXCreateVolumeTextureFromFileInMemory() failed for file '%s'!", this->resource->GetResourceId().Value().AsCharPtr());
                return false;
            }
            res->SetupFromD3D9VolumeTexture(d3d9VolumeTexture);
        }
        else if (D3DRTYPE_CUBETEXTURE == imageInfo.ResourceType)
        {
            // load cube texture
            IDirect3DCubeTexture9* d3d9CubeTexture = 0;
            hr = D3DXCreateCubeTextureFromFileInMemory(d3d9Device, srcData, srcDataSize, &d3d9CubeTexture);
            if (FAILED(hr))
            {
                n_error("D3D9StreamTextureLoader: D3DXCreateCubeTextureFromFileInMemory() failed for file '%s'!", this->resource->GetResourceId().Value().AsCharPtr());
                return false;
            }
            res->SetupFromD3D9CubeTexture(d3d9CubeTexture);
        }
        stream->Unmap();
        stream->Close();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
D3D9StreamTextureLoader::LoadBLP(uchar* buffer, uint size)
{
	HRESULT hr;
	IDirect3DDevice9* d3d9Device = D3D9RenderDevice::Instance()->GetDirect3DDevice();
	const Ptr<D3D9Texture>& res = this->resource.downcast<D3D9Texture>();
	D3DFORMAT d3dFormat;
	D3DLOCKED_RECT lockRect;

	char attr[4];
	int w, h, offsets[16], sizes[16];
	int pos = 8;
	bool supportCompression = true;

	Memory::Copy(buffer+pos, attr, 4); pos += 4;
	Memory::Copy(buffer+pos, &w, 4); pos += 4;
	Memory::Copy(buffer+pos, &h, 4); pos += 4; 
	Memory::Copy(buffer+pos, offsets, 4*16); pos += 4*16;
	Memory::Copy(buffer+pos, sizes, 4*16); pos += 4*16;

	bool hasmipmaps = attr[4] > 0;
	int mipmap = 0;//hasmipmaps ? 16 : 1;
	for (int i = 0; i < 16; i++)
	{
		if (offsets[i]) mipmap++;
	}

	if (attr[0] == 2)
	{
		// compressed
		unsigned char *ucbuf = NULL;
		/*if (!supportCompression)*/ ucbuf = new unsigned char[w*h*4];

		int blocksize = 8;
		int format = 0x83F1;	// GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
		d3dFormat = D3DFMT_DXT1;

		if (attr[1] == 8)
		{
			format = 0x83F2;	// GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
			blocksize = 16;
			d3dFormat = D3DFMT_DXT3;
		}

		if (attr[1] == 8 && attr[2] == 7)
		{
			format = 0x83F3;	// GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
			blocksize = 16;
			d3dFormat = D3DFMT_DXT5;
		}

		//unsigned char* buf = (uchar*) Memory::Alloc(sizes[0]);

		IDirect3DTexture9* d3d9Texture = 0;
		hr = d3d9Device->CreateTexture(w, h, mipmap, 0, d3dFormat, D3DPOOL_MANAGED, &d3d9Texture, NULL);
		if (FAILED(hr))
		{
			n_error("D3D9StreamTextureLoader: LoadBLP() failed for file '%s'!", this->resource->GetResourceId().Value().AsCharPtr());
			return false;
		}

		for (int i = 0; i < mipmap; i++)
		{
			if (w == 0) w = 1;
			if (h == 0) h = 1;
			if (offsets[i] && sizes[i])
			{
				int size = ((w+3)/4) * ((h+3)/4) * blocksize;

				// 直接手动解析DXT，可以判断使用硬件方式
				//DecompressDXTC(format, w, h, size, buffer+offsets[i], ucbuf);	

				d3d9Texture->LockRect(i, &lockRect, NULL, 0);
				//memcpy(lockRect.pBits, ucbuf, sizes[i]);
				memcpy(lockRect.pBits, buffer+offsets[i], sizes[i]);
				d3d9Texture->UnlockRect(i);
			}else break;
			w >>= 1;
			h >>= 1;
		}
		
		//Memory::Copy(buffer + offsets[0], buf, sizes[0]);

		
		//Memory::Free(buffer);
		//Memory::Free(buf);
		Memory::Free(ucbuf);
		
		//d3d9Texture->GenerateMipSubLevels();
		res->SetupFromD3D9Texture(d3d9Texture);

		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D9StreamTextureLoader::DecompressDXTC(int format, int w, int h, unsigned int size, unsigned char* src, unsigned char* dest)
{
	if (format == 0x83F1) // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
	{	
		DDSDecompressDXT1(src, w, h, dest);
		return;
	}

	// DXT3 Textures
	if (format == 0x83F2) //GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
	{
		DDSDecompressDXT3(src, w, h, dest);
		return;
	}

	// DXT5 Textures
	if (format == 0x83F3)//GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
	{
		//DXT5UnpackAlphaValues(src, w, h, dest);
		DDSDecompressDXT5(src, w, h, dest);
		return;
	}
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Texture> 
D3D9StreamTextureLoader::CreateTexture(const String& texName, SizeT width, SizeT height, SizeT level, CoreGraphics::PixelFormat::Code format, void* srcData, SizeT srcSize)
{
    Ptr<Texture> res;
    IDirect3DDevice9* d3d9Device = D3D9RenderDevice::Instance()->GetDirect3DDevice();
    IDirect3DTexture9* d3d9Texture = 0;
    D3DFORMAT d3dFormat = D3D9Types::AsD3D9PixelFormat(format);
	HRESULT hr = d3d9Device->CreateTexture(width, height, level, 0, d3dFormat, D3DPOOL_MANAGED, &d3d9Texture, NULL);
	if (FAILED(hr))
	{
		n_error("D3D9StreamTextureLoader: LoadBLP() failed for file '%s'!", texName.AsCharPtr());
		return res;
	}

    if (srcData != 0)
    {
        // no level 
        D3DLOCKED_RECT lockRect;
        d3d9Texture->LockRect(0, &lockRect, NULL, 0);
        Memory::Copy(srcData, lockRect.pBits, srcSize);
		d3d9Texture->UnlockRect(0);
    }

	 res = SharedResourceServer::Instance()->CreateSharedResource(texName, Texture::RTTI, StreamTextureLoader::RTTI).downcast<Texture>();
     //res = D3D9Texture::Create();
     res->SetHeight(height);
     res->SetWidth(width);
     res->SetNumMipLevels(level);
     res->SetPixelFormat(format);
     res->SetUsage(Texture::UsageDynamic);
     res->SetAccess(Texture::AccessWrite);
     res->SetupFromD3D9Texture(d3d9Texture);

     return res;
}

} // namespace Direct3D9