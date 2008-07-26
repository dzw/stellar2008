#ifndef DIRECT3D9_D3D9STREAMTEXTURELOADER_H
#define DIRECT3D9_D3D9STREAMTEXTURELOADER_H
//------------------------------------------------------------------------------
/**
    @class Direct3D9::D3D9StreamTextureLoader
  
    D3D9 implementation of StreamTextureLoader.

    (C) 2007 Radon Labs GmbH
*/    
#include "resources/resourceloader.h"
#include "io/stream.h"
#include "interface/iomsg/readstream.h"
#include "coregraphics/texture.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9StreamTextureLoader : public Resources::ResourceLoader
{
    DeclareClass(D3D9StreamTextureLoader);
public:
    /// return true if asynchronous loading is supported
    virtual bool CanLoadAsync() const;
    /// called by resource when a load is requested
    virtual bool OnLoadRequested();
    /// called by resource to cancel a pending load
    virtual void OnLoadCancelled();
    /// call frequently while after OnLoadRequested() to put Resource into loaded state
    virtual bool OnPending();
    /// 手动创建2d纹理
    static Ptr<CoreGraphics::Texture> CreateTexture(const Util::String& texName, SizeT width, SizeT height, SizeT level, CoreGraphics::PixelFormat::Code format, void* srcData, SizeT srcSize);
private:
    /// setup the texture from a Nebula3 stream
    bool SetupTextureFromStream(const Ptr<IO::Stream>& stream);
	/// load blp file from mpq
	bool LoadBLP(uchar* buffer, uint size);
	/// decompress dxtc
	void DecompressDXTC(int format, int w, int h, unsigned int size, unsigned char* src, unsigned char* dest);

    Ptr<Interface::ReadStream> readStreamMsg;
};

} // namespace Direct3D9
//------------------------------------------------------------------------------
#endif
