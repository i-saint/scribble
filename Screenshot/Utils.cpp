#include "pch.h"
#include "Screenshot.h"

#include <winrt/Windows.Foundation.h>
using namespace winrt;

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Externals/stb_image_write.h"


bool ReadTexture(ID3D11Texture2D* tex, int width, int height, const std::function<void(void*, int)>& callback)
{
    com_ptr<ID3D11Device> device;
    com_ptr<ID3D11DeviceContext> ctx;
    tex->GetDevice(device.put());
    device->GetImmediateContext(ctx.put());

    // create query
    com_ptr<ID3D11Query> query_event;
    {
        D3D11_QUERY_DESC qdesc = { D3D11_QUERY_EVENT , 0 };
        device->CreateQuery(&qdesc, query_event.put());
    }

    // create staging texture
    com_ptr<ID3D11Texture2D> staging;
    {
        D3D11_TEXTURE2D_DESC tmp;
        tex->GetDesc(&tmp);
        D3D11_TEXTURE2D_DESC desc{ (UINT)width, (UINT)height, 1, 1, tmp.Format, { 1, 0 }, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ, 0 };
        device->CreateTexture2D(&desc, nullptr, staging.put());
    }

    // dispatch copy
    {
        D3D11_BOX box{};
        box.right = width;
        box.bottom = height;
        box.back = 1;
        ctx->CopySubresourceRegion(staging.get(), 0, 0, 0, 0, tex, 0, &box);
        ctx->End(query_event.get());
        ctx->Flush();
    }

    // wait for copy to complete
    int wait_count = 0;
    while (ctx->GetData(query_event.get(), nullptr, 0, 0) == S_FALSE) {
        ++wait_count; // just for debug
    }

    // map
    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (SUCCEEDED(ctx->Map(staging.get(), 0, D3D11_MAP_READ, 0, &mapped))) {
        D3D11_TEXTURE2D_DESC desc{};
        staging->GetDesc(&desc);

        callback(mapped.pData, mapped.RowPitch);
        ctx->Unmap(staging.get(), 0);
        return true;
    }
    return false;
}

bool SaveAsPNG(const char* path, int w, int h, int src_stride, const void* data, bool flip_y)
{
    std::vector<byte> buf(w * h * 4);
    int dst_stride = w * 4;
    auto src = (const byte*)data;
    auto dst = (byte*)buf.data();
    if (flip_y) {
        for (int i = 0; i < h; ++i) {
            auto s = src + (src_stride * (h - i - 1));
            auto d = dst + (dst_stride * i);
            for (int j = 0; j < w; ++j) {
                d[0] = s[2];
                d[1] = s[1];
                d[2] = s[0];
                d[3] = s[3];
                s += 4;
                d += 4;
            }
        }
    }
    else {
        for (int i = 0; i < h; ++i) {
            auto s = src + (src_stride * i);
            auto d = dst + (dst_stride * i);
            for (int j = 0; j < w; ++j) {
                d[0] = s[2];
                d[1] = s[1];
                d[2] = s[0];
                d[3] = s[3];
                s += 4;
                d += 4;
            }
        }
    }
    return stbi_write_png(path, w, h, 4, buf.data(), dst_stride);
}
