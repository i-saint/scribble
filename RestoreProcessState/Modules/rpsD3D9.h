
template<class T>
class rpsTUnknown : public T
{
public:
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj)
    {
        return m_inst->QueryInterface(riid, ppvObj);
    }
    ULONG   __stdcall AddRef()
    {
        return m_inst->AddRef();
    }
    ULONG   __stdcall Release()
    {
        ULONG r = m_inst->Release();
        if(r==0) { delete this; }
        return r;
    }

    rpsTUnknown(T *inst) : m_inst(inst) {}
    ~rpsTUnknown() {}
    T *m_inst;
};

template<class T>
class rpsTDirect3DResource9 : public rpsTUnknown<T>
{
public:
    HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice)
    {
        return m_inst->GetDevice(ppDevice);
    }
    HRESULT __stdcall SetPrivateData(REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
    {
        return m_inst->SetPrivateData(refguid, pData, SizeOfData, Flags);
    }
    HRESULT __stdcall GetPrivateData(REFGUID refguid,void* pData,DWORD* pSizeOfData)
    {
        return m_inst->GetPrivateData(refguid, pData, pSizeOfData);
    }
    HRESULT __stdcall FreePrivateData(REFGUID refguid)
    {
        return m_inst->FreePrivateData(refguid);
    }
    DWORD   __stdcall SetPriority(DWORD PriorityNew)
    {
        return m_inst->SetPriority(PriorityNew);
    }
    DWORD   __stdcall GetPriority()
    {
        return m_inst->GetPriority();
    }
    void    __stdcall PreLoad()
    {
        m_inst->PreLoad();
    }
    D3DRESOURCETYPE __stdcall GetType()
    {
        return m_inst->GetType();
    }

    rpsTDirect3DResource9(T *inst) : rpsTUnknown<T>(inst) {}
    ~rpsTDirect3DResource9() {}
};

template<class T>
class rpsTDirect3DBaseTexture9 : public rpsTDirect3DResource9<T>
{
public:
    DWORD   __stdcall SetLOD(DWORD LODNew)
    {
        return m_inst->SetLOD(LODNew);
    }
    DWORD   __stdcall GetLOD()
    {
        return m_inst->GetLOD();
    }
    DWORD   __stdcall GetLevelCount()
    {
        return m_inst->GetLevelCount();
    }
    HRESULT __stdcall SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
    {
        return m_inst->SetAutoGenFilterType(FilterType);
    }
    D3DTEXTUREFILTERTYPE __stdcall GetAutoGenFilterType()
    {
        return m_inst->GetAutoGenFilterType();
    }
    void    __stdcall GenerateMipSubLevels()
    {
        return m_inst->GenerateMipSubLevels();
    }

    rpsTDirect3DBaseTexture9(T *inst) : rpsTDirect3DResource9<T>(inst) {}
    ~rpsTDirect3DBaseTexture9() {}
};




class rpsDirect3D9 : public rpsTUnknown<IDirect3D9>
{
typedef rpsTUnknown<IDirect3D9> super;
public:
    HRESULT  __stdcall RegisterSoftwareDevice(void* pInitializeFunction);
    UINT     __stdcall GetAdapterCount();
    HRESULT  __stdcall GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier);
    UINT     __stdcall GetAdapterModeCount(UINT Adapter, D3DFORMAT Format);
    HRESULT  __stdcall EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode);
    HRESULT  __stdcall GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode);
    HRESULT  __stdcall CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed);
    HRESULT  __stdcall CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat);
    HRESULT  __stdcall CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels);
    HRESULT  __stdcall CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat);
    HRESULT  __stdcall CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat);
    HRESULT  __stdcall GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps);
    HMONITOR __stdcall GetAdapterMonitor(UINT Adapter);
    HRESULT  __stdcall CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);

    rpsDirect3D9(IDirect3D9 *inst);
    ~rpsDirect3D9();
};


class rpsDirect3DDevice9 : public rpsTUnknown<IDirect3DDevice9>
{
typedef rpsTUnknown<IDirect3DDevice9> super;
public:
    HRESULT __stdcall TestCooperativeLevel();
    UINT    __stdcall GetAvailableTextureMem();
    HRESULT __stdcall EvictManagedResources();
    HRESULT __stdcall GetDirect3D(IDirect3D9** ppD3D9);
    HRESULT __stdcall GetDeviceCaps(D3DCAPS9* pCaps);
    HRESULT __stdcall GetDisplayMode(UINT iSwapChain,D3DDISPLAYMODE* pMode);
    HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters);
    HRESULT __stdcall SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap);
    void    __stdcall SetCursorPosition(int X,int Y,DWORD Flags);
    BOOL    __stdcall ShowCursor(BOOL bShow);
    HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain);
    HRESULT __stdcall GetSwapChain(UINT iSwapChain,IDirect3DSwapChain9** pSwapChain);
    UINT    __stdcall GetNumberOfSwapChains();
    HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
    HRESULT __stdcall Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
    HRESULT __stdcall GetBackBuffer(UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
    HRESULT __stdcall GetRasterStatus(UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus);
    HRESULT __stdcall SetDialogBoxMode(BOOL bEnableDialogs);
    void    __stdcall SetGammaRamp(UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp);
    void    __stdcall GetGammaRamp(UINT iSwapChain,D3DGAMMARAMP* pRamp);
    HRESULT __stdcall CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle);
    HRESULT __stdcall CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle);
    HRESULT __stdcall CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle);
    HRESULT __stdcall CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle);
    HRESULT __stdcall CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle);
    HRESULT __stdcall CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
    HRESULT __stdcall CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
    HRESULT __stdcall UpdateSurface(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint);
    HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture);
    HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface);
    HRESULT __stdcall GetFrontBufferData(UINT iSwapChain,IDirect3DSurface9* pDestSurface);
    HRESULT __stdcall StretchRect(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter);
    HRESULT __stdcall ColorFill(IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color);
    HRESULT __stdcall CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
    HRESULT __stdcall SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget);
    HRESULT __stdcall GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget);
    HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil);
    HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface);
    HRESULT __stdcall BeginScene();
    HRESULT __stdcall EndScene();
    HRESULT __stdcall Clear(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
    HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
    HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
    HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE Type,CONST D3DMATRIX* pMatrix);
    HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9* pViewport);
    HRESULT __stdcall GetViewport(D3DVIEWPORT9* pViewport);
    HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9* pMaterial);
    HRESULT __stdcall GetMaterial(D3DMATERIAL9* pMaterial);
    HRESULT __stdcall SetLight(DWORD Index,CONST D3DLIGHT9*);
    HRESULT __stdcall GetLight(DWORD Index,D3DLIGHT9*);
    HRESULT __stdcall LightEnable(DWORD Index,BOOL Enable);
    HRESULT __stdcall GetLightEnable(DWORD Index,BOOL* pEnable);
    HRESULT __stdcall SetClipPlane(DWORD Index,CONST float* pPlane);
    HRESULT __stdcall GetClipPlane(DWORD Index,float* pPlane);
    HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State,DWORD Value);
    HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State,DWORD* pValue);
    HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB);
    HRESULT __stdcall BeginStateBlock();
    HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** ppSB);
    HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus);
    HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* pClipStatus);
    HRESULT __stdcall GetTexture(DWORD Stage,IDirect3DBaseTexture9** ppTexture);
    HRESULT __stdcall SetTexture(DWORD Stage,IDirect3DBaseTexture9* pTexture);
    HRESULT __stdcall GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue);
    HRESULT __stdcall SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
    HRESULT __stdcall GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue);
    HRESULT __stdcall SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value);
    HRESULT __stdcall ValidateDevice(DWORD* pNumPasses);
    HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber,CONST PALETTEENTRY* pEntries);
    HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries);
    HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber);
    HRESULT __stdcall GetCurrentTexturePalette(UINT *PaletteNumber);
    HRESULT __stdcall SetScissorRect(CONST RECT* pRect);
    HRESULT __stdcall GetScissorRect(RECT* pRect);
    HRESULT __stdcall SetSoftwareVertexProcessing(BOOL bSoftware);
    BOOL    __stdcall GetSoftwareVertexProcessing();
    HRESULT __stdcall SetNPatchMode(float nSegments);
    float   __stdcall GetNPatchMode();
    HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
    HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount);
    HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
    HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
    HRESULT __stdcall ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags);
    HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl);
    HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl);
    HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl);
    HRESULT __stdcall SetFVF(DWORD FVF);
    HRESULT __stdcall GetFVF(DWORD* pFVF);
    HRESULT __stdcall CreateVertexShader(CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader);
    HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9* pShader);
    HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9** ppShader);
    HRESULT __stdcall SetVertexShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
    HRESULT __stdcall GetVertexShaderConstantF(UINT StartRegister,float* pConstantData,UINT Vector4fCount);
    HRESULT __stdcall SetVertexShaderConstantI(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount);
    HRESULT __stdcall GetVertexShaderConstantI(UINT StartRegister,int* pConstantData,UINT Vector4iCount);
    HRESULT __stdcall SetVertexShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount);
    HRESULT __stdcall GetVertexShaderConstantB(UINT StartRegister,BOOL* pConstantData,UINT BoolCount);
    HRESULT __stdcall SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride);
    HRESULT __stdcall GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride);
    HRESULT __stdcall SetStreamSourceFreq(UINT StreamNumber,UINT Setting);
    HRESULT __stdcall GetStreamSourceFreq(UINT StreamNumber,UINT* pSetting);
    HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9* pIndexData);
    HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9** ppIndexData);
    HRESULT __stdcall CreatePixelShader(CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader);
    HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* pShader);
    HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** ppShader);
    HRESULT __stdcall SetPixelShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
    HRESULT __stdcall GetPixelShaderConstantF(UINT StartRegister,float* pConstantData,UINT Vector4fCount);
    HRESULT __stdcall SetPixelShaderConstantI(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount);
    HRESULT __stdcall GetPixelShaderConstantI(UINT StartRegister,int* pConstantData,UINT Vector4iCount);
    HRESULT __stdcall SetPixelShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount);
    HRESULT __stdcall GetPixelShaderConstantB(UINT StartRegister,BOOL* pConstantData,UINT BoolCount);
    HRESULT __stdcall DrawRectPatch(UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo);
    HRESULT __stdcall DrawTriPatch(UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo);
    HRESULT __stdcall DeletePatch(UINT Handle);
    HRESULT __stdcall CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery);

    rpsDirect3DDevice9(IDirect3DDevice9 *inst);
    ~rpsDirect3DDevice9();
};


class rpsDirect3DSwapChain9 : public rpsTUnknown<IDirect3DSwapChain9>
{
typedef rpsTUnknown<IDirect3DSwapChain9> super;
public:
    HRESULT __stdcall Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags);
    HRESULT __stdcall GetFrontBufferData(IDirect3DSurface9* pDestSurface);
    HRESULT __stdcall GetBackBuffer(UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
    HRESULT __stdcall GetRasterStatus(D3DRASTER_STATUS* pRasterStatus);
    HRESULT __stdcall GetDisplayMode(D3DDISPLAYMODE* pMode);
    HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice);
    HRESULT __stdcall GetPresentParameters(D3DPRESENT_PARAMETERS* pPresentationParameters);

    rpsDirect3DSwapChain9(IDirect3DSwapChain9 *inst);
    ~rpsDirect3DSwapChain9();
};


class rpsDirect3DTexture9 : public rpsTDirect3DBaseTexture9<IDirect3DTexture9>
{
typedef rpsTDirect3DBaseTexture9<IDirect3DTexture9> super;
public:
    HRESULT __stdcall GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc);
    HRESULT __stdcall GetSurfaceLevel(UINT Level,IDirect3DSurface9** ppSurfaceLevel);
    HRESULT __stdcall LockRect(UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags);
    HRESULT __stdcall UnlockRect(UINT Level);
    HRESULT __stdcall AddDirtyRect(CONST RECT* pDirtyRect);

    rpsDirect3DTexture9(IDirect3DTexture9 *inst);
    ~rpsDirect3DTexture9();
};


class rpsDirect3DVolumeTexture9 : public rpsTDirect3DBaseTexture9<IDirect3DVolumeTexture9>
{
typedef rpsTDirect3DBaseTexture9<IDirect3DVolumeTexture9> super;
public:
    HRESULT __stdcall GetLevelDesc(UINT Level,D3DVOLUME_DESC *pDesc);
    HRESULT __stdcall GetVolumeLevel(UINT Level,IDirect3DVolume9** ppVolumeLevel);
    HRESULT __stdcall LockBox(UINT Level,D3DLOCKED_BOX* pLockedVolume,CONST D3DBOX* pBox,DWORD Flags);
    HRESULT __stdcall UnlockBox(UINT Level);
    HRESULT __stdcall AddDirtyBox(CONST D3DBOX* pDirtyBox);

    rpsDirect3DVolumeTexture9(IDirect3DVolumeTexture9 *inst);
    ~rpsDirect3DVolumeTexture9();
};


class rpsDirect3DCubeTexture9 : public rpsTDirect3DBaseTexture9<IDirect3DCubeTexture9>
{
typedef rpsTDirect3DBaseTexture9<IDirect3DCubeTexture9> super;
public:
    HRESULT __stdcall GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc);
    HRESULT __stdcall GetCubeMapSurface(D3DCUBEMAP_FACES FaceType,UINT Level,IDirect3DSurface9** ppCubeMapSurface);
    HRESULT __stdcall LockRect(D3DCUBEMAP_FACES FaceType,UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags);
    HRESULT __stdcall UnlockRect(D3DCUBEMAP_FACES FaceType,UINT Level);
    HRESULT __stdcall AddDirtyRect(D3DCUBEMAP_FACES FaceType,CONST RECT* pDirtyRect);

    rpsDirect3DCubeTexture9(IDirect3DCubeTexture9 *inst);
    ~rpsDirect3DCubeTexture9();
};


class rpsDirect3DVertexBuffer9 : public rpsTDirect3DResource9<IDirect3DVertexBuffer9>
{
typedef rpsTDirect3DResource9<IDirect3DVertexBuffer9> super;
public:
    HRESULT __stdcall Lock(UINT OffsetToLock,UINT SizeToLock,void** ppbData,DWORD Flags);
    HRESULT __stdcall Unlock();
    HRESULT __stdcall GetDesc(D3DVERTEXBUFFER_DESC *pDesc);

    rpsDirect3DVertexBuffer9(IDirect3DVertexBuffer9 *inst);
    ~rpsDirect3DVertexBuffer9();
};


class rpsDirect3DIndexBuffer9 : public rpsTDirect3DResource9<IDirect3DIndexBuffer9>
{
typedef rpsTDirect3DResource9<IDirect3DIndexBuffer9> super;
public:
    HRESULT __stdcall Lock(UINT OffsetToLock,UINT SizeToLock,void** ppbData,DWORD Flags);
    HRESULT __stdcall Unlock();
    HRESULT __stdcall GetDesc(D3DINDEXBUFFER_DESC *pDesc);

    rpsDirect3DIndexBuffer9(IDirect3DIndexBuffer9 *inst);
    ~rpsDirect3DIndexBuffer9();
};

class rpsDirect3DSurface9 : public rpsTDirect3DResource9<IDirect3DSurface9>
{
typedef rpsTDirect3DResource9<IDirect3DSurface9> super;
public:
    HRESULT __stdcall GetContainer(REFIID riid,void** ppContainer);
    HRESULT __stdcall GetDesc(D3DSURFACE_DESC *pDesc);
    HRESULT __stdcall LockRect(D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags);
    HRESULT __stdcall UnlockRect();
    HRESULT __stdcall GetDC(HDC *phdc);
    HRESULT __stdcall ReleaseDC(HDC hdc);

    rpsDirect3DSurface9(IDirect3DSurface9 *inst);
    ~rpsDirect3DSurface9();
};

class rpsDirect3DStateBlock9 : public rpsTUnknown<IDirect3DStateBlock9>
{
typedef rpsTUnknown<IDirect3DStateBlock9> super;
public:
    HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice);
    HRESULT __stdcall Capture();
    HRESULT __stdcall Apply();

    rpsDirect3DStateBlock9(IDirect3DStateBlock9 *inst);
    ~rpsDirect3DStateBlock9();
};

class rpsDirect3DVertexDeclaration9 : public rpsTUnknown<IDirect3DVertexDeclaration9>
{
typedef rpsTUnknown<IDirect3DVertexDeclaration9> super;
public:
    HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice);
    HRESULT __stdcall GetDeclaration(D3DVERTEXELEMENT9* pElement,UINT* pNumElements);

    rpsDirect3DVertexDeclaration9(IDirect3DVertexDeclaration9 *inst);
    ~rpsDirect3DVertexDeclaration9();
};


class rpsDirect3DVertexShader9 : public rpsTUnknown<IDirect3DVertexShader9>
{
typedef rpsTUnknown<IDirect3DVertexShader9> super;
public:
    HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice);
    HRESULT __stdcall GetFunction(void*,UINT* pSizeOfData);

    rpsDirect3DVertexShader9(IDirect3DVertexShader9 *inst);
    ~rpsDirect3DVertexShader9();
};


class rpsDirect3DPixelShader9 : public rpsTUnknown<IDirect3DPixelShader9>
{
typedef rpsTUnknown<IDirect3DPixelShader9> super;
public:
    HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice);
    HRESULT __stdcall GetFunction(void*,UINT* pSizeOfData);

    rpsDirect3DPixelShader9(IDirect3DPixelShader9 *inst);
    ~rpsDirect3DPixelShader9();
};


class rpsDirect3DQuery9 : public rpsTUnknown<IDirect3DQuery9>
{
typedef rpsTUnknown<IDirect3DQuery9> super;
public:
    HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice);
    D3DQUERYTYPE __stdcall GetType();
    DWORD __stdcall GetDataSize();
    HRESULT __stdcall Issue(DWORD dwIssueFlags);
    HRESULT __stdcall GetData(void* pData,DWORD dwSize,DWORD dwGetDataFlags);

    rpsDirect3DQuery9(IDirect3DQuery9 *inst);
    ~rpsDirect3DQuery9();
};

