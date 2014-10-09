#include <cstdio>

__declspec(dllexport) void DLLFunction()
{
	// TestMain プロジェクトをデバッグなしで開始したのち、
	// ここを編集して TestDLL プロジェクトをビルドすると変更が反映される。
	// (ビルド後イベントで PatchLibraryBridge を呼んで更新を反映させている)
	printf("update me\n");
}
