#include "pch.h"
#include "Screenshot.h"

#include <winrt/Windows.Foundation.h>
using namespace winrt;

class DesktopDuplication
{
public:
    using Callback = std::function<void(ID3D11Texture2D*, int w, int h)>;

    ~DesktopDuplication();
    bool start();
    void stop();

    bool getFrame(int timeout_ms, const Callback& calback);

private:
    com_ptr<ID3D11Device> m_device;
    com_ptr<ID3D11DeviceContext> m_context;
    com_ptr<IDXGIOutputDuplication> m_duplication;
};

DesktopDuplication::~DesktopDuplication()
{
    stop();
}

void DesktopDuplication::stop()
{
}

bool DesktopDuplication::start()
{
    // create device
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    flags |= D3D11_CREATE_DEVICE_DEBUG;

    ::D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, m_device.put(), nullptr, nullptr);
    m_device->GetImmediateContext(m_context.put());


    com_ptr<IDXGIDevice> dxgi;
    com_ptr<IDXGIAdapter> adapter;
    com_ptr<IDXGIOutput> output;
    com_ptr<IDXGIOutput1> output1;
    m_device->QueryInterface(IID_PPV_ARGS(dxgi.put()));
    dxgi->GetParent(IID_PPV_ARGS(adapter.put()));
    adapter->EnumOutputs(0, output.put());
    output->QueryInterface(IID_PPV_ARGS(output1.put()));
    output1->DuplicateOutput(m_device.get(), m_duplication.put());

    return true;
}

bool DesktopDuplication::getFrame(int timeout_ms, const Callback& calback)
{
    if (!m_duplication)
        return false;

    com_ptr<IDXGIResource> resource;
    DXGI_OUTDUPL_FRAME_INFO frame_info{};
    m_duplication->AcquireNextFrame(timeout_ms, &frame_info, resource.put());

    com_ptr<ID3D11Texture2D> surface;
    resource->QueryInterface(IID_PPV_ARGS(surface.put()));

    D3D11_TEXTURE2D_DESC desc{};
    surface->GetDesc(&desc);

    calback(surface.get(), desc.Width, desc.Height);

    m_duplication->ReleaseFrame();
    return true;
}


void TestDesktopDuplication()
{
    auto callback = [&](ID3D11Texture2D* surface, int w, int h) {
        ReadTexture(surface, w, h, [&](void* data, int stride) {
            stbi_write_png("DesktopDuplication.png", w, h, 4, data, stride);
            });
    };

    DesktopDuplication duplication;
    if (duplication.start()) {
        duplication.getFrame(500, callback);
        duplication.stop();
    }
}
