#include "pch.h"
#include "Screenshot.h"

#include <winrt/Windows.Foundation.h>
using namespace winrt;

class DesktopDuplication
{
public:
    using Callback = std::function<void(ID3D11Texture2D*, int w, int h)>;

    DesktopDuplication();
    ~DesktopDuplication();
    bool start();
    void stop();

    bool getFrame(int timeout_ms, const Callback& calback);

private:
    com_ptr<ID3D11Device> m_device;
    com_ptr<ID3D11DeviceContext> m_context;
    com_ptr<IDXGIOutputDuplication> m_duplication;
};

DesktopDuplication::DesktopDuplication()
{
    // create device
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    flags |= D3D11_CREATE_DEVICE_DEBUG;

    ::D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, m_device.put(), nullptr, nullptr);
    m_device->GetImmediateContext(m_context.put());
}

DesktopDuplication::~DesktopDuplication()
{
    stop();
}

void DesktopDuplication::stop()
{
    m_duplication = nullptr;
}

bool DesktopDuplication::start()
{
    // create duplication
    com_ptr<IDXGIDevice> dxgi;
    com_ptr<IDXGIAdapter> adapter;
    com_ptr<IDXGIOutput> output;
    com_ptr<IDXGIOutput1> output1;
    if (FAILED(m_device->QueryInterface(IID_PPV_ARGS(dxgi.put()))))
        return false;
    if (FAILED(dxgi->GetParent(IID_PPV_ARGS(adapter.put()))))
        return false;
    if (FAILED(adapter->EnumOutputs(0, output.put())))
        return false;
    if (FAILED(output->QueryInterface(IID_PPV_ARGS(output1.put()))))
        return false;
    if (FAILED(output1->DuplicateOutput(m_device.get(), m_duplication.put())))
        return false;
    return true;
}

bool DesktopDuplication::getFrame(int timeout_ms, const Callback& calback)
{
    if (!m_duplication)
        return false;

    bool ret = false;
    com_ptr<IDXGIResource> resource;
    DXGI_OUTDUPL_FRAME_INFO frame_info{};
    if (SUCCEEDED(m_duplication->AcquireNextFrame(timeout_ms, &frame_info, resource.put()))) {
        if (frame_info.LastPresentTime.QuadPart != 0) {
            com_ptr<ID3D11Texture2D> surface;
            resource->QueryInterface(IID_PPV_ARGS(surface.put()));

            DXGI_OUTDUPL_DESC desc;
            m_duplication->GetDesc(&desc);

            calback(surface.get(), desc.ModeDesc.Width, desc.ModeDesc.Height);
            ret = true;
        }
        m_duplication->ReleaseFrame();
    }
    return ret;
}


void TestDesktopDuplication()
{
    bool arrived = false;
    auto callback = [&](ID3D11Texture2D* surface, int w, int h) {
        ReadTexture(surface, w, h, [&](void* data, int stride) {
            SaveAsPNG("DesktopDuplication.png", w, h, w * 4, data);
            });
        arrived = true;
    };

    DesktopDuplication duplication;
    if (duplication.start()) {
        while (!arrived) {
            duplication.getFrame(500, callback);
        }
        duplication.stop();
    }
}
