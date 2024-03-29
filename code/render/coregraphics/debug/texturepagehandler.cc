//------------------------------------------------------------------------------
//  texturepagehandler.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/debug/texturepagehandler.h"
#include "coregraphics/texture.h"
#include "http/htmlpagewriter.h"
#include "resources/sharedresourceserver.h"
#include "coregraphics/streamtexturesaver.h"
#include "io/ioserver.h"

namespace Debug
{
ImplementClass(Debug::TexturePageHandler, 'DTXH', Http::HttpRequestHandler);

using namespace IO;
using namespace CoreGraphics;
using namespace Util;
using namespace Http;
using namespace Resources;

//------------------------------------------------------------------------------
/**
*/
TexturePageHandler::TexturePageHandler()
{
    this->SetName("Textures");
    this->SetDesc("show debug information about shared texture resources");
    this->SetRootLocation("/texture");
}

//------------------------------------------------------------------------------
/**
*/
bool
TexturePageHandler::AcceptsRequest(const Ptr<HttpRequest>& request)
{
    return (HttpMethod::Get == request->GetMethod()) && (String::MatchPattern(request->GetURI().LocalPath(), "texture*"));
}

//------------------------------------------------------------------------------
/**
*/
void
TexturePageHandler::HandleRequest(const Ptr<HttpRequest>& request)
{
    n_assert(HttpMethod::Get == request->GetMethod());

    // first check if a command has been defined in the URI
    Dictionary<String,String> query = request->GetURI().ParseQuery();
    if (query.Contains("img"))
    {
        request->SetStatus(this->HandleImageRequest(query, request->GetResponseContentStream()));
        return;
    }
    else if (query.Contains("texinfo"))
    {
        request->SetStatus(this->HandleTextureInfoRequest(ResourceId(query["texinfo"]), request->GetResponseContentStream()));
        return;
    }

    // no command, send the Texture home page
    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(request->GetResponseContentStream());
    htmlWriter->SetTitle("Nebula3 Textures");
    if (htmlWriter->Open())
    {
        htmlWriter->Element(HtmlElement::Heading1, "Shared Texture Resources");
        htmlWriter->AddAttr("href", "/index.html");
        htmlWriter->Element(HtmlElement::Anchor, "Home");
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();

        // get all texture resources
        Array<Ptr<Resource>> texResources = SharedResourceServer::Instance()->GetSharedResourcesByType(Texture::RTTI);

        // create a table of all existing textures
        htmlWriter->AddAttr("border", "1");
        htmlWriter->AddAttr("rules", "cols");
        htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->AddAttr("bgcolor", "lightsteelblue");
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableHeader, "ResId");
                htmlWriter->Element(HtmlElement::TableHeader, "UseCount");
                htmlWriter->Element(HtmlElement::TableHeader, "Type");
                htmlWriter->Element(HtmlElement::TableHeader, "Width");
                htmlWriter->Element(HtmlElement::TableHeader, "Height");
                htmlWriter->Element(HtmlElement::TableHeader, "Depth");
                htmlWriter->Element(HtmlElement::TableHeader, "Mips");
                htmlWriter->Element(HtmlElement::TableHeader, "Format");
            htmlWriter->End(HtmlElement::TableRow);

            // iterate over shared resources
            IndexT i;
            for (i = 0; i < texResources.Size(); i++)
            {
                const Ptr<Texture>& tex = texResources[i].downcast<Texture>();
                htmlWriter->Begin(HtmlElement::TableRow);
                    htmlWriter->Begin(HtmlElement::TableData);
                        htmlWriter->AddAttr("href", "/texture?texinfo=" + tex->GetResourceId().Value());
                        htmlWriter->Element(HtmlElement::Anchor, tex->GetResourceId().Value());
                    htmlWriter->End(HtmlElement::TableData);
                    htmlWriter->Element(HtmlElement::TableData, String::FromInt(tex->GetUseCount()));
                    switch (tex->GetType())
                    {
                        case Texture::Texture2D:    htmlWriter->Element(HtmlElement::TableData, "2D"); break;
                        case Texture::Texture3D:    htmlWriter->Element(HtmlElement::TableData, "3D"); break;
                        case Texture::TextureCube:  htmlWriter->Element(HtmlElement::TableData, "CUBE"); break;
                        default:                    htmlWriter->Element(HtmlElement::TableData, "ERROR"); break;
                    }
                    htmlWriter->Element(HtmlElement::TableData, String::FromInt(tex->GetWidth()));
                    htmlWriter->Element(HtmlElement::TableData, String::FromInt(tex->GetHeight()));
                    htmlWriter->Element(HtmlElement::TableData, String::FromInt(tex->GetDepth()));
                    htmlWriter->Element(HtmlElement::TableData, String::FromInt(tex->GetNumMipLevels()));
                    htmlWriter->Element(HtmlElement::TableData, PixelFormat::ToString(tex->GetPixelFormat()));
                htmlWriter->End(HtmlElement::TableRow);
            }
        htmlWriter->End(HtmlElement::Table);
        htmlWriter->Close();
        request->SetStatus(HttpStatus::OK);
    }
    else
    {
        request->SetStatus(HttpStatus::InternalServerError);
    }
}

//------------------------------------------------------------------------------
/**
    Handle a "raw" texture image request.
*/
HttpStatus::Code
TexturePageHandler::HandleImageRequest(const Dictionary<String,String>& query, const Ptr<Stream>& responseStream)
{
    n_assert(query.Contains("img"));
    SharedResourceServer* resServer = SharedResourceServer::Instance();
    
    // get input args
    ResourceId resId = ResourceId(query["img"]);
    ImageFileFormat::Code format = ImageFileFormat::InvalidImageFileFormat;
    if (query.Contains("fmt"))
    {
        format = ImageFileFormat::FromString(query["fmt"]);
    }
    if (ImageFileFormat::InvalidImageFileFormat == format)
    {
        format = ImageFileFormat::PNG;
    }
    IndexT mipLevel = 0;
    if (query.Contains("mip"))
    {
        mipLevel = query["mip"].AsInt();
    }

    // check if the request resource exists and is a texture
    if (!resServer->HasSharedResource(resId))
    {
        return HttpStatus::NotFound;
    }
    const Ptr<Resource>& res = resServer->LookupSharedResource(resId);
    if (!res->IsA(Texture::RTTI))
    {
        // resource exists but is not a texture
        return HttpStatus::NotFound;
    }
    const Ptr<Texture>& tex = res.downcast<Texture>();

    // attach a StreamTextureSaver to the texture
    HttpStatus::Code httpStatus = HttpStatus::InternalServerError;
    Ptr<StreamTextureSaver> saver = StreamTextureSaver::Create();
    saver->SetStream(responseStream);
    saver->SetFormat(format);
    saver->SetMipLevel(mipLevel);
    tex->SetSaver(saver.upcast<ResourceSaver>());
    if (tex->Save())
    {
        responseStream->SetMediaType(ImageFileFormat::ToMediaType(format));
        httpStatus = HttpStatus::OK;
    }
    tex->SetSaver(0);
    return httpStatus;
}

//------------------------------------------------------------------------------
/**
    Handle a texture info request.
*/
HttpStatus::Code
TexturePageHandler::HandleTextureInfoRequest(const ResourceId& resId, const Ptr<Stream>& responseContentStream)
{
    // lookup the texture in the SharedResourceServer
    SharedResourceServer* resServer = SharedResourceServer::Instance();
    if (!resServer->HasSharedResource(resId))
    {
        return HttpStatus::NotFound;
    }
    const Ptr<Resource>& res = resServer->LookupSharedResource(resId);
    if (!res->IsA(Texture::RTTI))
    {
        // resource exists but is not a texture
        return HttpStatus::NotFound;
    }
    const Ptr<Texture>& tex = res.downcast<Texture>();

    Ptr<HtmlPageWriter> htmlWriter = HtmlPageWriter::Create();
    htmlWriter->SetStream(responseContentStream);
    htmlWriter->SetTitle("Nebula3 Texture Info");
    if (htmlWriter->Open())
    {
        htmlWriter->Element(HtmlElement::Heading1, resId.Value());
        htmlWriter->AddAttr("href", "/index.html");
        htmlWriter->Element(HtmlElement::Anchor, "Home");
        htmlWriter->LineBreak();
        htmlWriter->AddAttr("href", "/texture");
        htmlWriter->Element(HtmlElement::Anchor, "Textures Home");
        htmlWriter->LineBreak();
        htmlWriter->LineBreak();
    
        // display some info about the texture
        htmlWriter->Begin(HtmlElement::Table);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableData, "Resource Id: ");
                htmlWriter->Element(HtmlElement::TableData, resId.Value());
            htmlWriter->End(HtmlElement::TableRow);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableData, "Resolved Path: ");
                htmlWriter->Element(HtmlElement::TableData, IoServer::Instance()->ResolveAssigns(resId.Value()).AsString());
            htmlWriter->End(HtmlElement::TableRow);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableData, "Use Count: ");
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(tex->GetUseCount()));
            htmlWriter->End(HtmlElement::TableRow);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableData, "Type: ");
                switch (tex->GetType())
                {
                    case Texture::Texture2D:    htmlWriter->Element(HtmlElement::TableData, "2D"); break;
                    case Texture::Texture3D:    htmlWriter->Element(HtmlElement::TableData, "3D"); break;
                    case Texture::TextureCube:  htmlWriter->Element(HtmlElement::TableData, "CUBE"); break;
                    default:                    htmlWriter->Element(HtmlElement::TableData, "ERROR"); break;
                }
            htmlWriter->End(HtmlElement::TableRow);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableData, "Width: ");
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(tex->GetWidth()));
            htmlWriter->End(HtmlElement::TableRow);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableData, "Height: ");
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(tex->GetHeight()));
            htmlWriter->End(HtmlElement::TableRow);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableData, "Depth: ");
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(tex->GetDepth()));
            htmlWriter->End(HtmlElement::TableRow);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableData, "Mip Levels: ");
                htmlWriter->Element(HtmlElement::TableData, String::FromInt(tex->GetNumMipLevels()));
            htmlWriter->End(HtmlElement::TableRow);
            htmlWriter->Begin(HtmlElement::TableRow);
                htmlWriter->Element(HtmlElement::TableData, "Pixel Format: ");
                htmlWriter->Element(HtmlElement::TableData, PixelFormat::ToString(tex->GetPixelFormat()));
            htmlWriter->End(HtmlElement::TableRow);
        htmlWriter->End(HtmlElement::Table);
        htmlWriter->LineBreak();

        // display the texture image data
        IndexT mipLevel;
        for (mipLevel = 0; mipLevel < tex->GetNumMipLevels(); mipLevel++)
        {
            String fmt;
            fmt.Format("/texture?img=%s&mip=%d", resId.Value().AsCharPtr(), mipLevel);
            htmlWriter->AddAttr("src", fmt);
            htmlWriter->Element(HtmlElement::Image, "");
        }
        htmlWriter->Close();
        return HttpStatus::OK;
    }
    return HttpStatus::InternalServerError;
}

} // namespace CoreGraphics
