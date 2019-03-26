#define     NAME    "WINDOW"  //�^�C�g���o�[�ɕ\������e�L�X�g
#include    <windows.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#include <Windows.h>
#include <memory>

#include "Device.h"
#include "GameMain.h"
#include "RenderPass.h"
using namespace std;

//�v���g�^�C�v�錾
LRESULT  CALLBACK   WndProc(HWND, UINT, WPARAM, LPARAM);
int      WINAPI     WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

//Windws �C�x���g�p�֐�
LRESULT  CALLBACK  WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	//�n���ꂽ message ����A�C�x���g�̎�ނ���͂���
	switch (msg) {
		//----�I������----
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		//----�f�t�H���g�̏���----
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0L;
}

//Windows Main �֐�
int  WINAPI  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow) {

	HWND        hWnd;
	MSG         msg;

	// Set up and register window class
	WNDCLASS wc = { CS_CLASSDC,
		WndProc,                                //�C�x���gcallback�֐�
		0,
		0,
		hInstance,
		NULL,                                   //�A�C�R��
		LoadCursor(NULL, IDC_ARROW),          //�}�E�X�J�[�\��
		(HBRUSH)GetStockObject(WHITE_BRUSH), //�w�i�F
		NULL,                                   //���j���[
		NAME };                                 //�N���X��
	if (RegisterClass(&wc) == 0) return FALSE;

	//�E�C���h�E����
	hWnd = CreateWindow(NAME,                  //�^�C�g���o�[�e�L�X�g
		NAME,                  //�N���X��
		WS_OVERLAPPEDWINDOW,   //�E�C���h�E�X�^�C��
		GetSystemMetrics(SM_CXSCREEN) / 4,         //�E�C���h�E����x���W
		GetSystemMetrics(SM_CYSCREEN) / 4,         //�E�C���h�E����y���W
		WINDOW_WIDTH,         //�E�C���h�E��
		WINDOW_HEIGHT,         //�E�C���h�E����
		NULL,                  //�e�E�C���h�E�̃n���h��
		NULL,
		hInstance,
		NULL);
	if (!hWnd) return FALSE;

	ShowWindow(hWnd, nCmdShow);     //Window ��\��
	UpdateWindow(hWnd);             //�\����������
	SetFocus(hWnd);                 //�t�H�[�J�X��ݒ�
	
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