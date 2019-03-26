#define     NAME    "WINDOW"  //タイトルバーに表示するテキスト
#include    <windows.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#include <Windows.h>
#include <memory>

#include "Device.h"
#include "GameMain.h"
#include "RenderPass.h"
using namespace std;

//プロトタイプ宣言
LRESULT  CALLBACK   WndProc(HWND, UINT, WPARAM, LPARAM);
int      WINAPI     WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

//Windws イベント用関数
LRESULT  CALLBACK  WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	//渡された message から、イベントの種類を解析する
	switch (msg) {
		//----終了処理----
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		//----デフォルトの処理----
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0L;
}

//Windows Main 関数
int  WINAPI  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow) {

	HWND        hWnd;
	MSG         msg;

	// Set up and register window class
	WNDCLASS wc = { CS_CLASSDC,
		WndProc,                                //イベントcallback関数
		0,
		0,
		hInstance,
		NULL,                                   //アイコン
		LoadCursor(NULL, IDC_ARROW),          //マウスカーソル
		(HBRUSH)GetStockObject(WHITE_BRUSH), //背景色
		NULL,                                   //メニュー
		NAME };                                 //クラス名
	if (RegisterClass(&wc) == 0) return FALSE;

	//ウインドウ生成
	hWnd = CreateWindow(NAME,                  //タイトルバーテキスト
		NAME,                  //クラス名
		WS_OVERLAPPEDWINDOW,   //ウインドウスタイル
		GetSystemMetrics(SM_CXSCREEN) / 4,         //ウインドウ左上x座標
		GetSystemMetrics(SM_CYSCREEN) / 4,         //ウインドウ左上y座標
		WINDOW_WIDTH,         //ウインドウ幅
		WINDOW_HEIGHT,         //ウインドウ高さ
		NULL,                  //親ウインドウのハンドル
		NULL,
		hInstance,
		NULL);
	if (!hWnd) return FALSE;

	ShowWindow(hWnd, nCmdShow);     //Window を表示
	UpdateWindow(hWnd);             //表示を初期化
	SetFocus(hWnd);                 //フォーカスを設定
	
	if (!DeviceUser::Initialize())
		return 0;

	unique_ptr<PassManager> pass(new PassManager());
	unique_ptr<GameMain> game(new GameMain());

	if (!pass->Initialize(hWnd, WINDOW_WIDTH, WINDOW_HEIGHT, true))
		return 0;

	if (!game->Init(hWnd, WINDOW_WIDTH, WINDOW_HEIGHT))
		return 0;

	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) {
			break;
		}
		else {
			if (!game->Run())
				break;

			if (!pass->Render())
				break;

			if (GetAsyncKeyState(VK_ESCAPE))
				break;
		}
	}

	game.reset();

	return msg.wParam;
}