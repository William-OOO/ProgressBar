#include<windows.h>
#include<CommCtrl.h>
#pragma comment(lib, "comctl32")

int step = 10;
int count = 0;
bool flag = true;

DWORD WINAPI PBThreadProc(LPVOID lpParameter)
{
	HWND hwndPB = (HWND)lpParameter;
	PBRANGE range;
	SendMessage(hwndPB, PBM_SETRANGE, (WPARAM)0, (LPARAM)(MAKELPARAM(0, 100)));
	SendMessage(hwndPB, PBM_GETRANGE, (WPARAM)TRUE, (LPARAM)&range);
	while (true) {
		SendMessage(hwndPB, PBM_DELTAPOS, (WPARAM)((range.iHigh - range.iLow) / 10), (LPARAM)0);
		count++;
		Sleep(1000);
		if (count == step) {
			flag = false;
			break;
		}
	}
	return flag;
}

LRESULT WINAPI WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int close_warn;
	static HWND hwndPB;
	HANDLE hThread;
	switch (message) {
		case WM_CREATE:
			InitCommonControls();
			hwndPB = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 0, 0, 0, 0, hwnd, (HMENU)0, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PBThreadProc, hwndPB, 0, NULL);
			CloseHandle(hThread);
			return 0;
		case WM_SIZE:
			RECT rc;
			GetClientRect(hwnd, &rc);
			MoveWindow(hwndPB, rc.left + 20, (rc.bottom - rc.top) / 3, rc.right - rc.left - 40, (rc.bottom - rc.top) / 4, TRUE);
			return 0;
		case WM_CLOSE:
			close_warn = MessageBox(hwnd, L"Close This Demo?", L"Warning", MB_YESNO | MB_ICONQUESTION);
			if (close_warn == IDYES) {
				DestroyWindow(hwnd);
			}
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

int main() {
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	WNDCLASS wndclass;
	ZeroMemory(&wndclass, sizeof(WNDCLASS));
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wndclass.lpszClassName = L"window_display";
	RegisterClass(&wndclass);
	HWND hwndParent = CreateWindow(L"window_display", L"Demo", WS_SIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 300, 160, NULL, NULL, NULL, NULL);
	ShowWindow(hwndParent, SW_SHOWDEFAULT);
	UpdateWindow(hwndParent);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (!flag) {
			DestroyWindow(hwndParent);
		}
	}
}
