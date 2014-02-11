#include "rpsPCH.h"
#include "rpsInternal.h"
#include "rpsD3D9.h"

///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3D9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3D9::RegisterSoftwareDevice( void* pInitializeFunction )
{
    HRESULT r = m_inst->RegisterSoftwareDevice(pInitializeFunction);
    return r;
}

UINT __stdcall rpsDirect3D9::GetAdapterCount()
{
    UINT r = m_inst->GetAdapterCount();
    return r;
}

HRESULT __stdcall rpsDirect3D9::GetAdapterIdentifier( UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier )
{
    HRESULT r = m_inst->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
    return r;
}

UINT __stdcall rpsDirect3D9::GetAdapterModeCount( UINT Adapter, D3DFORMAT Format )
{
    HRESULT r = m_inst->GetAdapterModeCount(Adapter, Format);
    return r;
}

HRESULT __stdcall rpsDirect3D9::EnumAdapterModes( UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode )
{
    HRESULT r = m_inst->EnumAdapterModes(Adapter, Format, Mode, pMode);
    return r;
}

HRESULT __stdcall rpsDirect3D9::GetAdapterDisplayMode( UINT Adapter, D3DDISPLAYMODE* pMode )
{
    HRESULT r = m_inst->GetAdapterDisplayMode(Adapter, pMode);
    return r;
}

HRESULT __stdcall rpsDirect3D9::CheckDeviceType( UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed )
{
    HRESULT r = m_inst->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
    return r;
}

HRESULT __stdcall rpsDirect3D9::CheckDeviceFormat( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat )
{
    HRESULT r = m_inst->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
    return r;
}

HRESULT __stdcall rpsDirect3D9::CheckDeviceMultiSampleType( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels )
{
    HRESULT r = m_inst->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
    return r;
}

HRESULT __stdcall rpsDirect3D9::CheckDepthStencilMatch( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat )
{
    HRESULT r = m_inst->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
    return r;
}

HRESULT __stdcall rpsDirect3D9::CheckDeviceFormatConversion( UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat )
{
    HRESULT r = m_inst->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
    return r;
}

HRESULT __stdcall rpsDirect3D9::GetDeviceCaps( UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps )
{
    HRESULT r = m_inst->GetDeviceCaps(Adapter, DeviceType, pCaps);
    return r;
}

HMONITOR __stdcall rpsDirect3D9::GetAdapterMonitor( UINT Adapter )
{
    HMONITOR r = m_inst->GetAdapterMonitor(Adapter);
    return r;
}

HRESULT __stdcall rpsDirect3D9::CreateDevice( UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface )
{
    HRESULT r = m_inst->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
    if(r==D3D_OK) {
        *ppReturnedDeviceInterface = new rpsDirect3DDevice9(*ppReturnedDeviceInterface);
    }
    return r;
}

rpsDirect3D9::rpsDirect3D9(IDirect3D9 *inst)
    : super(inst)
{
}

rpsDirect3D9::~rpsDirect3D9()
{
}



///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3DDevice9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3DDevice9::TestCooperativeLevel()
{
    HRESULT r = m_inst->TestCooperativeLevel();
    return r;
}

UINT __stdcall rpsDirect3DDevice9::GetAvailableTextureMem()
{
    UINT r = m_inst->GetAvailableTextureMem();
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::EvictManagedResources()
{
    HRESULT r = m_inst->EvictManagedResources();
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetDirect3D( IDirect3D9** ppD3D9 )
{
    HRESULT r = m_inst->GetDirect3D(ppD3D9);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetDeviceCaps( D3DCAPS9* pCaps )
{
    HRESULT r = m_inst->GetDeviceCaps(pCaps);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetDisplayMode( UINT iSwapChain, D3DDISPLAYMODE* pMode )
{
    HRESULT r = m_inst->GetDisplayMode(iSwapChain, pMode);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetCreationParameters( D3DDEVICE_CREATION_PARAMETERS *pParameters )
{
    HRESULT r = m_inst->GetCreationParameters(pParameters);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetCursorProperties( UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap )
{
    HRESULT r = m_inst->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
    return r;
}

void __stdcall rpsDirect3DDevice9::SetCursorPosition( int X, int Y,DWORD Flags )
{
    m_inst->SetCursorPosition(X, Y, Flags);
}

BOOL __stdcall rpsDirect3DDevice9::ShowCursor( BOOL bShow )
{
    BOOL r = m_inst->ShowCursor(bShow);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::CreateAdditionalSwapChain( D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain )
{
    HRESULT r = m_inst->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetSwapChain( UINT iSwapChain,IDirect3DSwapChain9** pSwapChain )
{
    HRESULT r = m_inst->GetSwapChain(iSwapChain, pSwapChain);
    return r;
}

UINT __stdcall rpsDirect3DDevice9::GetNumberOfSwapChains()
{
    UINT r = m_inst->GetNumberOfSwapChains();
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::Reset( D3DPRESENT_PARAMETERS* pPresentationParameters )
{
    HRESULT r = m_inst->Reset(pPresentationParameters);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::Present( CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion )
{
    HRESULT r = m_inst->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetBackBuffer( UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer )
{
    HRESULT r = m_inst->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetRasterStatus( UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus )
{
    HRESULT r = m_inst->GetRasterStatus(iSwapChain, pRasterStatus);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetDialogBoxMode( BOOL bEnableDialogs )
{
    HRESULT r = m_inst->SetDialogBoxMode(bEnableDialogs);
    return r;
}

void __stdcall rpsDirect3DDevice9::SetGammaRamp( UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp )
{
    m_inst->SetGammaRamp(iSwapChain, Flags, pRamp);
}

void __stdcall rpsDirect3DDevice9::GetGammaRamp( UINT iSwapChain,D3DGAMMARAMP* pRamp )
{
    m_inst->GetGammaRamp(iSwapChain, pRamp);
}

HRESULT __stdcall rpsDirect3DDevice9::CreateTexture( UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle )
{
    HRESULT r = m_inst->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
    if(r==D3D_OK) {
        *ppTexture = new rpsDirect3DTexture9(*ppTexture);
    }
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::CreateVolumeTexture( UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle )
{
    HRESULT r = m_inst->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
    if(r==D3D_OK) {
        *ppVolumeTexture = new rpsDirect3DVolumeTexture9(*ppVolumeTexture);
    }
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::CreateCubeTexture( UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle )
{
    HRESULT r = m_inst->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
    if(r==D3D_OK) {
        *ppCubeTexture = new rpsDirect3DCubeTexture9(*ppCubeTexture);
    }
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::CreateVertexBuffer( UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle )
{
    HRESULT r = m_inst->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
    if(r==D3D_OK) {
        *ppVertexBuffer = new rpsDirect3DVertexBuffer9(*ppVertexBuffer);
    }
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::CreateIndexBuffer( UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle )
{
    HRESULT r = m_inst->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
    if(r==D3D_OK) {
        *ppIndexBuffer = new rpsDirect3DIndexBuffer9(*ppIndexBuffer);
    }
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::CreateRenderTarget( UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle )
{
    HRESULT r = m_inst->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
    if(r==D3D_OK) {
        *ppSurface = new rpsDirect3DSurface9(*ppSurface);
    }
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::CreateDepthStencilSurface( UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle )
{
    HRESULT r = m_inst->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
    return r;
    if(r==D3D_OK) {
        *ppSurface = new rpsDirect3DSurface9(*ppSurface);
    }
}

HRESULT __stdcall rpsDirect3DDevice9::UpdateSurface( IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint )
{
    HRESULT r = m_inst->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::UpdateTexture( IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture )
{
    HRESULT r = m_inst->UpdateTexture(pSourceTexture, pDestinationTexture);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetRenderTargetData( IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface )
{
    HRESULT r = m_inst->GetRenderTargetData(pRenderTarget, pDestSurface);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetFrontBufferData( UINT iSwapChain,IDirect3DSurface9* pDestSurface )
{
    HRESULT r = m_inst->GetFrontBufferData(iSwapChain, pDestSurface);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::StretchRect( IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter )
{
    HRESULT r = m_inst->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::ColorFill( IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color )
{
    HRESULT r = m_inst->ColorFill(pSurface, pRect, color);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::CreateOffscreenPlainSurface( UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle )
{
    HRESULT r = m_inst->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
    if(r==D3D_OK) {
        *ppSurface = new rpsDirect3DSurface9(*ppSurface);
    }
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetRenderTarget( DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget )
{
    HRESULT r = m_inst->SetRenderTarget(RenderTargetIndex, pRenderTarget);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetRenderTarget( DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget )
{
    HRESULT r = m_inst->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetDepthStencilSurface( IDirect3DSurface9* pNewZStencil )
{
    HRESULT r = m_inst->SetDepthStencilSurface(pNewZStencil);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetDepthStencilSurface( IDirect3DSurface9** ppZStencilSurface )
{
    HRESULT r = m_inst->GetDepthStencilSurface(ppZStencilSurface);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::BeginScene()
{
    HRESULT r = m_inst->BeginScene();
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::EndScene()
{
    HRESULT r = m_inst->EndScene();
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::Clear( DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil )
{
    HRESULT r = m_inst->Clear(Count, pRects, Flags, Color, Z, Stencil);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetTransform( D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix )
{
    HRESULT r = m_inst->SetTransform(State, pMatrix);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetTransform( D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix )
{
    HRESULT r = m_inst->GetTransform(State, pMatrix);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::MultiplyTransform( D3DTRANSFORMSTATETYPE Type,CONST D3DMATRIX* pMatrix )
{
    HRESULT r = m_inst->MultiplyTransform(Type, pMatrix);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetViewport( CONST D3DVIEWPORT9* pViewport )
{
    HRESULT r = m_inst->SetViewport(pViewport);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetViewport( D3DVIEWPORT9* pViewport )
{
    HRESULT r = m_inst->GetViewport(pViewport);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetMaterial( CONST D3DMATERIAL9* pMaterial )
{
    HRESULT r = m_inst->SetMaterial(pMaterial);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetMaterial( D3DMATERIAL9* pMaterial )
{
    HRESULT r = m_inst->GetMaterial(pMaterial);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetLight( DWORD Index,CONST D3DLIGHT9* pLight )
{
    HRESULT r = m_inst->SetLight(Index, pLight);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetLight( DWORD Index,D3DLIGHT9* pLight )
{
    HRESULT r = m_inst->GetLight(Index, pLight);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::LightEnable( DWORD Index,BOOL Enable )
{
    HRESULT r = m_inst->LightEnable(Index, Enable);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetLightEnable( DWORD Index,BOOL* pEnable )
{
    HRESULT r = m_inst->GetLightEnable(Index, pEnable);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetClipPlane( DWORD Index,CONST float* pPlane )
{
    HRESULT r = m_inst->SetClipPlane(Index, pPlane);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetClipPlane( DWORD Index,float* pPlane )
{
    HRESULT r = m_inst->GetClipPlane(Index, pPlane);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetRenderState( D3DRENDERSTATETYPE State,DWORD Value )
{
    HRESULT r = m_inst->SetRenderState(State, Value);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetRenderState( D3DRENDERSTATETYPE State,DWORD* pValue )
{
    HRESULT r = m_inst->GetRenderState(State, pValue);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::CreateStateBlock( D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB )
{
    HRESULT r = m_inst->CreateStateBlock(Type, ppSB);
    if(r==D3D_OK) {
        *ppSB = new rpsDirect3DStateBlock9(*ppSB);
    }
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::BeginStateBlock()
{
    HRESULT r = m_inst->BeginStateBlock();
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::EndStateBlock( IDirect3DStateBlock9** ppSB )
{
    HRESULT r = m_inst->EndStateBlock(ppSB);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetClipStatus( CONST D3DCLIPSTATUS9* pClipStatus )
{
    HRESULT r = m_inst->SetClipStatus(pClipStatus);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetClipStatus( D3DCLIPSTATUS9* pClipStatus )
{
    HRESULT r = m_inst->GetClipStatus(pClipStatus);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetTexture( DWORD Stage,IDirect3DBaseTexture9** ppTexture )
{
    HRESULT r = m_inst->GetTexture(Stage, ppTexture);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetTexture( DWORD Stage,IDirect3DBaseTexture9* pTexture )
{
    HRESULT r = m_inst->SetTexture(Stage, pTexture);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetTextureStageState( DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue )
{
    HRESULT r = m_inst->GetTextureStageState(Stage, Type, pValue);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetTextureStageState( DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value )
{
    HRESULT r = m_inst->SetTextureStageState(Stage, Type, Value);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetSamplerState( DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue )
{
    HRESULT r = m_inst->GetSamplerState(Sampler, Type, pValue);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetSamplerState( DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value )
{
    HRESULT r = m_inst->SetSamplerState(Sampler, Type, Value);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::ValidateDevice( DWORD* pNumPasses )
{
    HRESULT r = m_inst->ValidateDevice(pNumPasses);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetPaletteEntries( UINT PaletteNumber,CONST PALETTEENTRY* pEntries )
{
    HRESULT r = m_inst->SetPaletteEntries(PaletteNumber, pEntries);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetPaletteEntries( UINT PaletteNumber,PALETTEENTRY* pEntries )
{
    HRESULT r = m_inst->GetPaletteEntries(PaletteNumber, pEntries);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetCurrentTexturePalette( UINT PaletteNumber )
{
    HRESULT r = m_inst->SetCurrentTexturePalette(PaletteNumber);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetCurrentTexturePalette( UINT *PaletteNumber )
{
    HRESULT r = m_inst->GetCurrentTexturePalette(PaletteNumber);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetScissorRect( CONST RECT* pRect )
{
    HRESULT r = m_inst->SetScissorRect(pRect);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetScissorRect( RECT* pRect )
{
    HRESULT r = m_inst->GetScissorRect(pRect);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetSoftwareVertexProcessing( BOOL bSoftware )
{
    HRESULT r = m_inst->SetSoftwareVertexProcessing(bSoftware);
    return r;
}

BOOL __stdcall rpsDirect3DDevice9::GetSoftwareVertexProcessing()
{
    HRESULT r = m_inst->GetSoftwareVertexProcessing();
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetNPatchMode( float nSegments )
{
    HRESULT r = m_inst->SetNPatchMode(nSegments);
    return r;
}

float __stdcall rpsDirect3DDevice9::GetNPatchMode()
{
    float r = m_inst->GetNPatchMode();
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::DrawPrimitive( D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount )
{
    HRESULT r = m_inst->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::DrawIndexedPrimitive( D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount )
{
    HRESULT r = m_inst->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::DrawPrimitiveUP( D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride )
{
    HRESULT r = m_inst->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::DrawIndexedPrimitiveUP( D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride )
{
    HRESULT r = m_inst->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::ProcessVertices( UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags )
{
    HRESULT r = m_inst->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::CreateVertexDeclaration( CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl )
{
    HRESULT r = m_inst->CreateVertexDeclaration(pVertexElements, ppDecl);
    if(r==D3D_OK) {
        *ppDecl = new rpsDirect3DVertexDeclaration9(*ppDecl);
    }
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetVertexDeclaration( IDirect3DVertexDeclaration9* pDecl )
{
    HRESULT r = m_inst->SetVertexDeclaration(pDecl);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetVertexDeclaration( IDirect3DVertexDeclaration9** ppDecl )
{
    HRESULT r = m_inst->GetVertexDeclaration(ppDecl);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetFVF( DWORD FVF )
{
    HRESULT r = m_inst->SetFVF(FVF);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetFVF( DWORD* pFVF )
{
    HRESULT r = m_inst->GetFVF(pFVF);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::CreateVertexShader( CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader )
{
    HRESULT r = m_inst->CreateVertexShader(pFunction, ppShader);
    if(r==D3D_OK) {
        *ppShader = new rpsDirect3DVertexShader9(*ppShader);
    }
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetVertexShader( IDirect3DVertexShader9* pShader )
{
    HRESULT r = m_inst->SetVertexShader(pShader);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetVertexShader( IDirect3DVertexShader9** ppShader )
{
    HRESULT r = m_inst->GetVertexShader(ppShader);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetVertexShaderConstantF( UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount )
{
    HRESULT r = m_inst->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetVertexShaderConstantF( UINT StartRegister,float* pConstantData,UINT Vector4fCount )
{
    HRESULT r = m_inst->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetVertexShaderConstantI( UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount )
{
    HRESULT r = m_inst->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetVertexShaderConstantI( UINT StartRegister,int* pConstantData,UINT Vector4iCount )
{
    HRESULT r = m_inst->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetVertexShaderConstantB( UINT StartRegister,CONST BOOL* pConstantData,UINT BoolCount )
{
    HRESULT r = m_inst->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetVertexShaderConstantB( UINT StartRegister,BOOL* pConstantData,UINT BoolCount )
{
    HRESULT r = m_inst->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetStreamSource( UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride )
{
    HRESULT r = m_inst->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetStreamSource( UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride )
{
    HRESULT r = m_inst->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetStreamSourceFreq( UINT StreamNumber,UINT Setting )
{
    HRESULT r = m_inst->SetStreamSourceFreq(StreamNumber, Setting);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetStreamSourceFreq( UINT StreamNumber,UINT* pSetting )
{
    HRESULT r = m_inst->GetStreamSourceFreq(StreamNumber, pSetting);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetIndices( IDirect3DIndexBuffer9* pIndexData )
{
    HRESULT r = m_inst->SetIndices(pIndexData);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetIndices( IDirect3DIndexBuffer9** ppIndexData )
{
    HRESULT r = m_inst->GetIndices(ppIndexData);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::CreatePixelShader( CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader )
{
    HRESULT r = m_inst->CreatePixelShader(pFunction, ppShader);
    if(r==D3D_OK) {
        *ppShader = new rpsDirect3DPixelShader9(*ppShader);
    }
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetPixelShader( IDirect3DPixelShader9* pShader )
{
    HRESULT r = m_inst->SetPixelShader(pShader);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetPixelShader( IDirect3DPixelShader9** ppShader )
{
    HRESULT r = m_inst->GetPixelShader(ppShader);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetPixelShaderConstantF( UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount )
{
    HRESULT r = m_inst->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetPixelShaderConstantF( UINT StartRegister,float* pConstantData,UINT Vector4fCount )
{
    HRESULT r = m_inst->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetPixelShaderConstantI( UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount )
{
    HRESULT r = m_inst->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetPixelShaderConstantI( UINT StartRegister,int* pConstantData,UINT Vector4iCount )
{
    HRESULT r = m_inst->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::SetPixelShaderConstantB( UINT StartRegister,CONST BOOL* pConstantData,UINT BoolCount )
{
    HRESULT r = m_inst->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::GetPixelShaderConstantB( UINT StartRegister,BOOL* pConstantData,UINT BoolCount )
{
    HRESULT r = m_inst->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::DrawRectPatch( UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo )
{
    HRESULT r = m_inst->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::DrawTriPatch( UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo )
{
    HRESULT r = m_inst->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::DeletePatch( UINT Handle )
{
    HRESULT r = m_inst->DeletePatch(Handle);
    return r;
}

HRESULT __stdcall rpsDirect3DDevice9::CreateQuery( D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery )
{
    HRESULT r = m_inst->CreateQuery(Type, ppQuery);
    if(r==D3D_OK) {
        *ppQuery = new rpsDirect3DQuery9(*ppQuery);
    }
    return r;
}

rpsDirect3DDevice9::rpsDirect3DDevice9( IDirect3DDevice9 *inst )
    : super(inst)
{
}

rpsDirect3DDevice9::~rpsDirect3DDevice9()
{
}


///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3DSwapChain9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3DSwapChain9::Present( CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags )
{
    HRESULT r = m_inst->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
    return r;
}

HRESULT __stdcall rpsDirect3DSwapChain9::GetFrontBufferData( IDirect3DSurface9* pDestSurface )
{
    HRESULT r = m_inst->GetFrontBufferData(pDestSurface);
    return r;
}

HRESULT __stdcall rpsDirect3DSwapChain9::GetBackBuffer( UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer )
{
    HRESULT r = m_inst->GetBackBuffer(iBackBuffer, Type, ppBackBuffer);
    return r;
}

HRESULT __stdcall rpsDirect3DSwapChain9::GetRasterStatus( D3DRASTER_STATUS* pRasterStatus )
{
    HRESULT r = m_inst->GetRasterStatus(pRasterStatus);
    return r;
}

HRESULT __stdcall rpsDirect3DSwapChain9::GetDisplayMode( D3DDISPLAYMODE* pMode )
{
    HRESULT r = m_inst->GetDisplayMode(pMode);
    return r;
}

HRESULT __stdcall rpsDirect3DSwapChain9::GetDevice( IDirect3DDevice9** ppDevice )
{
    HRESULT r = m_inst->GetDevice(ppDevice);
    return r;
}

HRESULT __stdcall rpsDirect3DSwapChain9::GetPresentParameters( D3DPRESENT_PARAMETERS* pPresentationParameters )
{
    HRESULT r = m_inst->GetPresentParameters(pPresentationParameters);
    return r;
}

rpsDirect3DSwapChain9::rpsDirect3DSwapChain9( IDirect3DSwapChain9 *inst )
    : super(inst)
{
}

rpsDirect3DSwapChain9::~rpsDirect3DSwapChain9()
{
}


///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3DTexture9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3DTexture9::GetLevelDesc( UINT Level,D3DSURFACE_DESC *pDesc )
{
    HRESULT r = m_inst->GetLevelDesc(Level, pDesc);
    return r;
}

HRESULT __stdcall rpsDirect3DTexture9::GetSurfaceLevel( UINT Level,IDirect3DSurface9** ppSurfaceLevel )
{
    HRESULT r = m_inst->GetSurfaceLevel(Level, ppSurfaceLevel);
    return r;
}

HRESULT __stdcall rpsDirect3DTexture9::LockRect( UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags )
{
    HRESULT r = m_inst->LockRect(Level, pLockedRect, pRect, Flags);
    return r;
}

HRESULT __stdcall rpsDirect3DTexture9::UnlockRect( UINT Level )
{
    HRESULT r = m_inst->UnlockRect(Level);
    return r;
}

HRESULT __stdcall rpsDirect3DTexture9::AddDirtyRect( CONST RECT* pDirtyRect )
{
    HRESULT r = m_inst->AddDirtyRect(pDirtyRect);
    return r;
}

rpsDirect3DTexture9::rpsDirect3DTexture9( IDirect3DTexture9 *inst )
    : super(inst)
{
}

rpsDirect3DTexture9::~rpsDirect3DTexture9()
{
}


///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3DVolumeTexture9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3DVolumeTexture9::GetLevelDesc( UINT Level,D3DVOLUME_DESC *pDesc )
{
    HRESULT r = m_inst->GetLevelDesc(Level, pDesc);
    return r;
}

HRESULT __stdcall rpsDirect3DVolumeTexture9::GetVolumeLevel( UINT Level,IDirect3DVolume9** ppVolumeLevel )
{
    HRESULT r = m_inst->GetVolumeLevel(Level, ppVolumeLevel);
    return r;
}

HRESULT __stdcall rpsDirect3DVolumeTexture9::LockBox( UINT Level,D3DLOCKED_BOX* pLockedVolume,CONST D3DBOX* pBox,DWORD Flags )
{
    HRESULT r = m_inst->LockBox(Level, pLockedVolume, pBox, Flags);
    return r;
}

HRESULT __stdcall rpsDirect3DVolumeTexture9::UnlockBox( UINT Level )
{
    HRESULT r = m_inst->UnlockBox(Level);
    return r;
}

HRESULT __stdcall rpsDirect3DVolumeTexture9::AddDirtyBox( CONST D3DBOX* pDirtyBox )
{
    HRESULT r = m_inst->AddDirtyBox(pDirtyBox);
    return r;
}

rpsDirect3DVolumeTexture9::rpsDirect3DVolumeTexture9( IDirect3DVolumeTexture9 *inst )
    : super(inst)
{
}

rpsDirect3DVolumeTexture9::~rpsDirect3DVolumeTexture9()
{
}



///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3DCubeTexture9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3DCubeTexture9::GetLevelDesc( UINT Level,D3DSURFACE_DESC *pDesc )
{
    HRESULT r = m_inst->GetLevelDesc(Level, pDesc);
    return r;
}

HRESULT __stdcall rpsDirect3DCubeTexture9::GetCubeMapSurface( D3DCUBEMAP_FACES FaceType,UINT Level,IDirect3DSurface9** ppCubeMapSurface )
{
    HRESULT r = m_inst->GetCubeMapSurface(FaceType, Level, ppCubeMapSurface);
    return r;
}

HRESULT __stdcall rpsDirect3DCubeTexture9::LockRect( D3DCUBEMAP_FACES FaceType,UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags )
{
    HRESULT r = m_inst->LockRect(FaceType, Level, pLockedRect, pRect, Flags);
    return r;
}

HRESULT __stdcall rpsDirect3DCubeTexture9::UnlockRect( D3DCUBEMAP_FACES FaceType,UINT Level )
{
    HRESULT r = m_inst->UnlockRect(FaceType, Level);
    return r;
}

HRESULT __stdcall rpsDirect3DCubeTexture9::AddDirtyRect( D3DCUBEMAP_FACES FaceType,CONST RECT* pDirtyRect )
{
    HRESULT r = m_inst->AddDirtyRect(FaceType, pDirtyRect);
    return r;
}

rpsDirect3DCubeTexture9::rpsDirect3DCubeTexture9( IDirect3DCubeTexture9 *inst )
    : super(inst)
{
}

rpsDirect3DCubeTexture9::~rpsDirect3DCubeTexture9()
{
}


///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3DVertexBuffer9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3DVertexBuffer9::Lock( UINT OffsetToLock,UINT SizeToLock,void** ppbData,DWORD Flags )
{
    HRESULT r = m_inst->Lock(OffsetToLock, SizeToLock, ppbData, Flags);
    return r;
}

HRESULT __stdcall rpsDirect3DVertexBuffer9::Unlock()
{
    HRESULT r = m_inst->Unlock();
    return r;
}

HRESULT __stdcall rpsDirect3DVertexBuffer9::GetDesc( D3DVERTEXBUFFER_DESC *pDesc )
{
    HRESULT r = m_inst->GetDesc(pDesc);
    return r;
}

rpsDirect3DVertexBuffer9::rpsDirect3DVertexBuffer9( IDirect3DVertexBuffer9 *inst )
    : super(inst)
{
}

rpsDirect3DVertexBuffer9::~rpsDirect3DVertexBuffer9()
{
}



///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3DIndexBuffer9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3DIndexBuffer9::Lock( UINT OffsetToLock,UINT SizeToLock,void** ppbData,DWORD Flags )
{
    HRESULT r = m_inst->Lock(OffsetToLock, SizeToLock, ppbData, Flags);
    return r;
}

HRESULT __stdcall rpsDirect3DIndexBuffer9::Unlock()
{
    HRESULT r = m_inst->Unlock();
    return r;
}

HRESULT __stdcall rpsDirect3DIndexBuffer9::GetDesc( D3DINDEXBUFFER_DESC *pDesc )
{
    HRESULT r = m_inst->GetDesc(pDesc);
    return r;
}

rpsDirect3DIndexBuffer9::rpsDirect3DIndexBuffer9( IDirect3DIndexBuffer9 *inst )
    : super(inst)
{
}

rpsDirect3DIndexBuffer9::~rpsDirect3DIndexBuffer9()
{
}



///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3DSurface9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3DSurface9::GetContainer( REFIID riid,void** ppContainer )
{
    HRESULT r = m_inst->GetContainer(riid, ppContainer);
    return r;
}

HRESULT __stdcall rpsDirect3DSurface9::GetDesc( D3DSURFACE_DESC *pDesc )
{
    HRESULT r = m_inst->GetDesc(pDesc);
    return r;
}

HRESULT __stdcall rpsDirect3DSurface9::LockRect( D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags )
{
    HRESULT r = m_inst->LockRect(pLockedRect, pRect, Flags);
    return r;
}

HRESULT __stdcall rpsDirect3DSurface9::UnlockRect()
{
    HRESULT r = m_inst->UnlockRect();
    return r;
}

HRESULT __stdcall rpsDirect3DSurface9::GetDC( HDC *phdc )
{
    HRESULT r = m_inst->GetDC(phdc);
    return r;
}

HRESULT __stdcall rpsDirect3DSurface9::ReleaseDC( HDC hdc )
{
    HRESULT r = m_inst->ReleaseDC(hdc);
    return r;
}

rpsDirect3DSurface9::rpsDirect3DSurface9( IDirect3DSurface9 *inst )
    : super(inst)
{
}

rpsDirect3DSurface9::~rpsDirect3DSurface9()
{
}



///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3DStateBlock9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3DStateBlock9::GetDevice( IDirect3DDevice9** ppDevice )
{
    HRESULT r = m_inst->GetDevice(ppDevice);
    return r;
}

HRESULT __stdcall rpsDirect3DStateBlock9::Capture()
{
    HRESULT r = m_inst->Capture();
    return r;
}

HRESULT __stdcall rpsDirect3DStateBlock9::Apply()
{
    HRESULT r = m_inst->Apply();
    return r;
}

rpsDirect3DStateBlock9::rpsDirect3DStateBlock9( IDirect3DStateBlock9 *inst )
    : super(inst)
{
}

rpsDirect3DStateBlock9::~rpsDirect3DStateBlock9()
{
}



///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3DVertexDeclaration9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3DVertexDeclaration9::GetDevice( IDirect3DDevice9** ppDevice )
{
    HRESULT r = m_inst->GetDevice(ppDevice);
    return r;
}

HRESULT __stdcall rpsDirect3DVertexDeclaration9::GetDeclaration( D3DVERTEXELEMENT9* pElement,UINT* pNumElements )
{
    HRESULT r = m_inst->GetDeclaration(pElement, pNumElements);
    return r;
}

rpsDirect3DVertexDeclaration9::rpsDirect3DVertexDeclaration9( IDirect3DVertexDeclaration9 *inst )
    : super(inst)
{
}

rpsDirect3DVertexDeclaration9::~rpsDirect3DVertexDeclaration9()
{
}



///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3DVertexShader9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3DVertexShader9::GetDevice( IDirect3DDevice9** ppDevice )
{
    HRESULT r = m_inst->GetDevice(ppDevice);
    return r;
}

HRESULT __stdcall rpsDirect3DVertexShader9::GetFunction( void* pData,UINT* pSizeOfData )
{
    HRESULT r = m_inst->GetFunction(pData, pSizeOfData);
    return r;
}

rpsDirect3DVertexShader9::rpsDirect3DVertexShader9( IDirect3DVertexShader9 *inst )
    : super(inst)
{
}

rpsDirect3DVertexShader9::~rpsDirect3DVertexShader9()
{
}



///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3DPixelShader9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3DPixelShader9::GetDevice( IDirect3DDevice9** ppDevice )
{
    HRESULT r = m_inst->GetDevice(ppDevice);
    return r;
}

HRESULT __stdcall rpsDirect3DPixelShader9::GetFunction( void* pData,UINT* pSizeOfData )
{
    HRESULT r = m_inst->GetFunction(pData, pSizeOfData);
    return r;
}

rpsDirect3DPixelShader9::rpsDirect3DPixelShader9( IDirect3DPixelShader9 *inst )
    : super(inst)
{
}

rpsDirect3DPixelShader9::~rpsDirect3DPixelShader9()
{
}



///////////////////////////////////////////////////////////////////////////////
// 
// rpsDirect3DQuery9
// 
///////////////////////////////////////////////////////////////////////////////

HRESULT __stdcall rpsDirect3DQuery9::GetDevice( IDirect3DDevice9** ppDevice )
{
    HRESULT r = m_inst->GetDevice(ppDevice);
    return r;
}

D3DQUERYTYPE __stdcall rpsDirect3DQuery9::GetType()
{
    D3DQUERYTYPE r = m_inst->GetType();
    return r;
}

DWORD __stdcall rpsDirect3DQuery9::GetDataSize()
{
    DWORD r = m_inst->GetDataSize();
    return r;
}

HRESULT __stdcall rpsDirect3DQuery9::Issue( DWORD dwIssueFlags )
{
    HRESULT r = m_inst->Issue(dwIssueFlags);
    return r;
}

HRESULT __stdcall rpsDirect3DQuery9::GetData( void* pData,DWORD dwSize,DWORD dwGetDataFlags )
{
    HRESULT r = m_inst->GetData(pData, dwSize, dwGetDataFlags);
    return r;
}

rpsDirect3DQuery9::rpsDirect3DQuery9( IDirect3DQuery9 *inst )
    : super(inst)
{
}

rpsDirect3DQuery9::~rpsDirect3DQuery9()
{
}
