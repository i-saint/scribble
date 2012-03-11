#ifndef _FXAA3_11_D3D11_h_
#define _FXAA3_11_D3D11_h_
#include <d3d11.h>

bool FXAAD3_11D11Initialize(ID3D11Device *pDevice);
void FXAAD3_11D11Finalize();

void FXAAD3_11D11Render(ID3D11DeviceContext *pContext);

#endif // _FXAA3_11_D3D11_h_
