#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>


// メンバ関数ポインタはキャストすらできないようなので、union で強引に値を取得します
// ちなみに C++ の規格は関数ポインタの void* へのキャストは動作不定と規定しています
template<class T>
inline void* mfp_to_voidp(T v)
{
    union {
        T mfp;
        void *vp;
    } c;
    c.mfp = v;
    return c.vp;
}


template<class T>
inline void** get_vtable(T _this)
{
    return ((void***)_this)[0];
}

template<class T>
inline void set_vtable(T _this, void **vtable)
{
    ((void***)_this)[0] = vtable;
}



void **g_ID3D11DeviceContext_default_vtable;
void *g_ID3D11DeviceContext_hooked_vtable[115];

class DummyDeviceContext
{
public:

    // x86 と x64 で以下のように分岐しないとクラッシュする。要調査
#if defined(_WIN64)
    void DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation ) {
        ID3D11DeviceContext *_this = (ID3D11DeviceContext*)this;
#elif defined(_WIN32)
    void DrawIndexedInstanced(ID3D11DeviceContext *_this, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation ) {
#endif

        OutputDebugStringA("DummyDeviceContext::DrawIndexedInstanced()\n");

        // 一時的に vtable を元に戻して本来の動作をさせる
        set_vtable(_this, g_ID3D11DeviceContext_default_vtable);
        _this->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
        set_vtable(_this, g_ID3D11DeviceContext_hooked_vtable);
    }
};



void SetHook(ID3D11DeviceContext *pDeviceContext)
{
    size_t vtable_size = sizeof(g_ID3D11DeviceContext_hooked_vtable);
    void **vtable = get_vtable(pDeviceContext);

    // 元の vtable を保存
    g_ID3D11DeviceContext_default_vtable = vtable;

    // hook 関数を仕込んだ vtable を作成
    memcpy(g_ID3D11DeviceContext_hooked_vtable, g_ID3D11DeviceContext_default_vtable, vtable_size);
    g_ID3D11DeviceContext_hooked_vtable[20] = mfp_to_voidp(&DummyDeviceContext::DrawIndexedInstanced);

    // vtable をすり替える
    set_vtable(pDeviceContext, g_ID3D11DeviceContext_hooked_vtable);
}
