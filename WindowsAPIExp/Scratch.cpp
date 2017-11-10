#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <ole2.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <shlobj.h>

HINSTANCE g_hinst;
HWND g_hwndChild;


void
OnSize(HWND hwnd, UINT state, int cx, int cy) {
	if (g_hwndChild) {
		MoveWindow(g_hwndChild, 0, 0, cx, cy, TRUE);
	}
}


BOOL
OnCreate(HWND hwnd, LPCREATESTRUCT lpcs) {
	return TRUE;
}


void
OnDestroy(HWND hwnd) {
	PostQuitMessage(0);
}


void
PaintContent(HWND hwn, PAINTSTRUCT *pps) {}


void
OnPaint(HWND hwnd) {
	PAINTSTRUCT ps;
	BeginPaint(hwnd, &ps);
	PaintContent(hwnd, &ps);
	EndPaint(hwnd, &ps);
}



void
OnPrintClient(HWND hwnd, HDC hdc) {
	PAINTSTRUCT ps;
	ps.hdc = hdc;
	GetClientRect(hwnd, &ps.rcPaint);
	PaintContent(hwnd, &ps);
}



LRESULT CALLBACK
WndProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
	switch (uiMsg) {
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hwnd, WM_SIZE, OnSize);
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
	case WM_PRINTCLIENT: OnPrintClient(hwnd, (HDC)wParam);
	}

	return DefWindowProc(hwnd, uiMsg, wParam, lParam);
}

BOOL
InitApp(void) {
	WNDCLASS wc;

	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hinst;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("Scratch");

	if (!RegisterClass(&wc)) {
		return FALSE;
	}

	//InitCommonControlsEx();

	return TRUE;
}


HRESULT GetUIObjectOfFIle(HWND hwnd, LPCWSTR pszPath, REFIID riid, void **ppv) {
	*ppv = NULL;
	HRESULT hr;
	LPITEMIDLIST pidl;
	SFGAOF sfgao;
	if (SUCCEEDED(hr = SHParseDisplayName(pszPath, NULL, &pidl, 0, &sfgao))) {
		IShellFolder *psf;
		LPCITEMIDLIST pidlChild;
		if (SUCCEEDED(hr = SHBindToParent(pidl, IID_IShellFolder, (void**)&psf, &pidlChild))) {
			hr = psf->GetUIObjectOf(hwnd, 1, &pidlChild, riid, NULL, ppv);
			psf->Release();
		}
		CoTaskMemFree(pidl);
	}
	return hr;
}


int WINAPI
WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdline, int nShowCmd) {
	MSG msg;
	HWND hwnd;

	g_hinst = hinst;

	if (!InitApp()) {
		return 0;
	}
	
	if (SUCCEEDED(OleInitialize(NULL))) {
		hwnd = CreateWindow(
			TEXT("Scratch"),
			TEXT("Scratch"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			NULL,
			NULL,
			hinst,
			0
		);

		ShowWindow(hwnd, nShowCmd);

		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		OleUninitialize();
	}

	return 0;
}