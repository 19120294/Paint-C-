// PaintApp.cpp : Defines the entry point for the application.
//
#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "PaintApp.h"
#include "BasicShape.h"
#include <memory>
#include <windowsX.h>
#include <windows.h>
#include <commctrl.h>
#include <Commdlg.h>
#include <stdio.h>
#include <assert.h>
#pragma comment(lib, "Comctl32.lib")

#define MAX_LOADSTRING 100

using namespace std;
// Global Variables:
HINSTANCE hInst;                                 // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/*-------------------------------------*/
#define IMAGE_WIDTH     18
#define IMAGE_HEIGHT    18
#define BUTTON_WIDTH    0
#define BUTTON_HEIGHT   0

vector<shared_ptr<IShape>> drawStack;
bool isDrawing = false;
bool isCleaning = false;
int type = 0;
HBITMAP chooseFile;

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp);
void CreateBMPFile(LPTSTR pszFile, HBITMAP hBMP);
void SaveFile(HWND hwnd);
void GetFileName(HWND hWnd);
void DisplayFile(OPENFILENAME ofn, HWND hwnd);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnDestroy(HWND hwnd);
void OnPaint(HWND hwnd);
void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
void drawAShape(HWND hwnd, HDC hdc, shared_ptr<IShape> shape);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PAINTAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PAINTAPP));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAINTAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PAINTAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
void OnTest() {
    MessageBox(NULL, L"PAINT", L"....", MB_OK);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
        //HANDLE_MSG(hWnd, WM_PAINT, GetFileName);
        HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
        HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLButtonUp);
        HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD    cClrBits;

    // Retrieve the bitmap color format, width, and height.  
    assert(GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp));

    // Convert the color format to a count of bits.  
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1)
        cClrBits = 1;
    else if (cClrBits <= 4)
        cClrBits = 4;
    else if (cClrBits <= 8)
        cClrBits = 8;
    else if (cClrBits <= 16)
        cClrBits = 16;
    else if (cClrBits <= 24)
        cClrBits = 24;
    else cClrBits = 32;

    // Allocate memory for the BITMAPINFO structure. (This structure  
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
    // data structures.)  

    if (cClrBits < 24)
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
            sizeof(BITMAPINFOHEADER) +
            sizeof(RGBQUAD) * (1 << cClrBits));

    // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

    else
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
            sizeof(BITMAPINFOHEADER));

    // Initialize the fields in the BITMAPINFO structure.  

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24)
        pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

    // If the bitmap is not compressed, set the BI_RGB flag.  
    pbmi->bmiHeader.biCompression = BI_RGB;

    // Compute the number of bytes in the array of color  
    // indices and store the result in biSizeImage.  
    // The width must be DWORD aligned unless the bitmap is RLE 
    // compressed. 
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
        * pbmi->bmiHeader.biHeight;
    // Set biClrImportant to 0, indicating that all of the  
    // device colors are important.  
    pbmi->bmiHeader.biClrImportant = 0;
    return pbmi;
}

void CreateBMPFile(LPTSTR pszFile, HBITMAP hBMP)
{
    HANDLE hf;                 // file handle  
    BITMAPFILEHEADER hdr;       // bitmap file-header  
    PBITMAPINFOHEADER pbih;     // bitmap info-header  
    LPBYTE lpBits;              // memory pointer  
    DWORD dwTotal;              // total count of bytes  
    DWORD cb;                   // incremental count of bytes  
    BYTE* hp;                   // byte pointer  
    DWORD dwTmp;
    PBITMAPINFO pbi;
    HDC hDC;

    hDC = CreateCompatibleDC(GetWindowDC(GetDesktopWindow()));
    SelectObject(hDC, hBMP);

    pbi = CreateBitmapInfoStruct(hBMP);

    pbih = (PBITMAPINFOHEADER)pbi;
    lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    assert(lpBits);

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    assert(GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
        DIB_RGB_COLORS));

    // Create the .BMP file.  
    hf = CreateFile(pszFile,
        GENERIC_READ | GENERIC_WRITE,
        (DWORD)0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        (HANDLE)NULL);
    assert(hf != INVALID_HANDLE_VALUE);

    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
    // Compute the size of the entire file.  
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed
        * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices.  
    hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed
        * sizeof(RGBQUAD);

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    assert(WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
        (LPDWORD)&dwTmp, NULL));

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    assert(WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
        + pbih->biClrUsed * sizeof(RGBQUAD),
        (LPDWORD)&dwTmp, (NULL)));

    // Copy the array of color indices into the .BMP file.  
    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;
    assert(WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL));

    // Close the .BMP file.  
    assert(CloseHandle(hf));

    // Free memory.  
    GlobalFree((HGLOBAL)lpBits);
}
void SaveFile(HWND hwnd) {
    TCHAR szFile[100];
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 100;
    ofn.lpstrFilter = _T("BMP Files\0*.bmp");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    BOOL success = GetSaveFileName(&ofn);
    if (success) {
        RECT rect;
        HBITMAP hbitmap;
        HDC hdc[2];
        GetClientRect(hwnd, &rect);
        hdc[0] = GetDC(hwnd);
        hbitmap = CreateCompatibleBitmap(hdc[0], rect.right, rect.bottom);
        hdc[1] = CreateCompatibleDC(hdc[0]);
        SelectObject(hdc[1], hbitmap);

        BitBlt(
            hdc[1],
            0,0,
            rect.right,
            rect.bottom,
            hdc[0],
            0, 35,
            SRCCOPY
        );

        CreateBMPFile(ofn.lpstrFile, hbitmap);
    }
    else {
        MessageBox(NULL, L"PAINT", L"error", MB_OK);
    }
}
void DisplayFile(OPENFILENAME ofn, HWND hwnd) {
    HBITMAP img = (HBITMAP) LoadImage(hInst, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    HBRUSH brush = CreatePatternBrush((HBITMAP)img);
    
    BITMAP bm;
    GetObject(img, sizeof(BITMAP), &bm);
    RECT rect;
    GetClientRect(hwnd, &rect);
    HDC hdc = GetDC(hwnd);
    rect.top = 35;
    rect.left = 0;
    rect.right = bm.bmWidth;
    rect.bottom = bm.bmHeight;
    SetWindowPos(hwnd, NULL, 0, 0, bm.bmWidth, bm.bmHeight+35, NULL);
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);
    ReleaseDC(hwnd, hdc);
}

void GetFileName(HWND hwnd) {
    TCHAR szFile[100];
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 100;
    ofn.lpstrFilter = _T("BMP Files\0*.bmp");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    BOOL success = GetOpenFileName(&ofn);
    if (success) {
        DisplayFile(ofn, hwnd);
    }
    else {
        MessageBox(NULL, ofn.lpstrFile, _T("Invalid"), MB_OK);
    }
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    LOGFONT lf;
    GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    HFONT hFont = CreateFont(lf.lfHeight, lf.lfWidth,
        lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
        lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
        lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
        lf.lfPitchAndFamily, lf.lfFaceName);

    InitCommonControls();
    TBBUTTON tbButtons[] =
    {
        { STD_FILENEW,	ID_FILE_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_FILEOPEN,	ID_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_FILESAVE,	ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
    };
    HWND hToolBarWnd = CreateToolbarEx(hwnd,
        WS_CHILD | WS_VISIBLE | CCS_ADJUSTABLE | TBSTYLE_TOOLTIPS,
        ID_TOOLBAR, sizeof(tbButtons) / sizeof(TBBUTTON), HINST_COMMCTRL,
        0, tbButtons, sizeof(tbButtons) / sizeof(TBBUTTON),
        BUTTON_WIDTH, BUTTON_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT,
        sizeof(TBBUTTON));


    TBBUTTON buttonsToAdd[] =
    {
        { 0, 0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 },
        { STD_CUT,	ID_EDIT_CUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_COPY,	ID_EDIT_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_PASTE, ID_EDIT_PASTE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { STD_DELETE, ID_EDIT_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
    };
    SendMessage(hToolBarWnd, TB_ADDBUTTONS, (WPARAM)sizeof(buttonsToAdd) / sizeof(TBBUTTON),
        (LPARAM)(LPTBBUTTON)&buttonsToAdd);

    TBBUTTON userButtons[] =
    {
        { 0, 0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 },
        { 0, ID_DRAW_ELLIPSE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { 1, ID_DRAW_RECTANGLE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { 2, ID_DRAW_LINE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
        { 3, ID_DRAW_TEXT,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
    };
    //TBADDBITMAP tbBitmap1, tbBitmap2, tbBitmap3, tbBitmap4;
    //TBADDBITMAP	tbBitmap = { hInst, IDB_BITMAP1 };
    //// Thêm danh sách ảnh vào toolbar
    //int idx = SendMessage(hToolBarWnd, TB_ADDBITMAP, (WPARAM)sizeof(tbBitmap) / sizeof(TBADDBITMAP),
    //    (LPARAM)(LPTBADDBITMAP)&tbBitmap);
    ///*userButtons[1].iBitmap = idx1;
    //userButtons[2].iBitmap = idx1;
    //userButtons[3].iBitmap = idx1;
    //userButtons[4].iBitmap = idx1;
    //
    //
    //userButtons[1].iString =  SendMessage(hToolBarWnd, TB_ADDSTRING, 0, (LPARAM)TEXT("P"));
    //userButtons[2].iString =  SendMessage(hToolBarWnd, TB_ADDSTRING, 0, (LPARAM)TEXT("L"));
    //userButtons[3].iString =  SendMessage(hToolBarWnd, TB_ADDSTRING, 0, (LPARAM)TEXT("R"));
    //userButtons[4].iString =  SendMessage(hToolBarWnd, TB_ADDSTRING, 0, (LPARAM)TEXT("C"));

    //SendMessage(hToolBarWnd, TB_ADDBUTTONS, (WPARAM)sizeof(userButtons) / sizeof(TBBUTTON),
    //    (LPARAM)(LPTBBUTTON)&userButtons);*/

    return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{

    switch (id)
    {
    case ID_EDIT_CUT:
        type = 0;
        break;
    case ID_EDIT_COPY:
        type = 1;
        break;
    case ID_EDIT_PASTE:
        type = 2;
        break;
    case ID_FILE_OPEN:
        GetFileName(hwnd);
        break;
    case ID_FILE_SAVE:
        SaveFile(hwnd);
        break;
    case IDM_EXIT:
        DestroyWindow(hwnd);
        break;
    }
}

void OnDestroy(HWND hwnd)
{
    PostQuitMessage(0);
}

void drawAShape(HWND hwnd, HDC hdc, shared_ptr<IShape> shape) {
    switch (shape->type()) {
    case 0:
        MoveToEx(hdc, shape->getA().getX(), shape->getA().getY(), NULL);
        LineTo(hdc, shape->getB().getX(), shape->getB().getY());
        break;
    case 1:
        //Rectangle(hdc, shape->getA().getX(), shape->getA().getY(), 
        //    shape->getB().getX(), shape->getB().getY());
        MoveToEx(hdc, shape->getA().getX(), shape->getA().getY(), NULL);
        LineTo(hdc, shape->getB().getX(), shape->getA().getY());
        MoveToEx(hdc, shape->getA().getX(), shape->getA().getY(), NULL);
        LineTo(hdc, shape->getA().getX(), shape->getB().getY());
        MoveToEx(hdc, shape->getB().getX(), shape->getA().getY(), NULL);
        LineTo(hdc, shape->getB().getX(), shape->getB().getY());
        MoveToEx(hdc, shape->getA().getX(), shape->getB().getY(), NULL);
        LineTo(hdc, shape->getB().getX(), shape->getB().getY());
        break;
    case 2:
        Ellipse(hdc, shape->getA().getX(), shape->getA().getY(),
            shape->getB().getX(), shape->getB().getY());
    default:
        break;
    }
}

void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    HPEN hPen = CreatePen(PS_DASHDOT, 3, RGB(255, 0, 0));
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    SelectObject(hdc, hPen);
    if (drawStack.empty()) return;

    if (isCleaning) {
        for (int i = 0; i < (int)drawStack.size() - 1; ++i) {
            drawAShape(hwnd, hdc, drawStack[i]);
        }
        isCleaning = false;
    }
    shared_ptr<IShape> shape = drawStack[drawStack.size() - 1];
    drawAShape(hwnd, hdc, shape);
    EndPaint(hwnd, &ps);
}

void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{

    HDC hdc = GetDC(hwnd);
    shared_ptr<IShape> shape = ShapeFactory::getInstance()->create(type);;
    shape->setA(x, y);
    drawStack.push_back(shape);
    isDrawing = true;
    ReleaseDC(hwnd, hdc);
    OnPaint(hwnd);
}

void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
    if (drawStack.empty()) return;
    shared_ptr<IShape> shape = drawStack[drawStack.size() - 1];
    shape->setB(x, y);
    drawStack.push_back(shape);
    isDrawing = false;
    OnPaint(hwnd);
}

void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
    if (drawStack.empty() || !isDrawing) return;
    shared_ptr<IShape> shape = drawStack[drawStack.size() - 1];
    shape->setB(x, y);
    drawStack.push_back(shape);
    isCleaning = true;
    InvalidateRect(hwnd, NULL, TRUE);
    OnPaint(hwnd);
}

