#pragma once

bool ReadTexture(ID3D11Texture2D* tex, int width, int height, const std::function<void(void*, int)>& callback);


void TestGDI();
void TestDesktopDuplication();
void TestGraphicsCapture();
