#include "Draw.h"
#include "PaintApp.h"
#include <windowsx.h>
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")

#define IMAGE_WIDTH     18
#define IMAGE_HEIGHT    18
#define BUTTON_WIDTH    0
#define BUTTON_HEIGHT   0

vector<shared_ptr<IShape>> drawStack;
bool isDrawing = false;
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

    //TBBUTTON userButtons[] =
    //{
    //    { 0, 0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 },
    //    { 0, ID_DRAW_ELLIPSE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
    //    { 1, ID_DRAW_RECTANGLE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
    //    { 2, ID_DRAW_LINE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
    //    { 3, ID_DRAW_TEXT,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
    //};
    //TBADDBITMAP	tbBitmap = { hInst, IDB_BITMAP1 };
    //// Thêm danh sách ảnh vào toolbar
    //int idx = SendMessage(hToolBarWnd, TB_ADDBITMAP, (WPARAM)sizeof(tbBitmap) / sizeof(TBADDBITMAP),
    //    (LPARAM)(LPTBADDBITMAP)&tbBitmap);
    //// Xác định chỉ mục hình ảnh c ủa mỗi button từ ảnh bự liên hoàn nhiều tấm
    //userButtons[1].iBitmap += idx;
    //userButtons[2].iBitmap += idx;
    //userButtons[3].iBitmap += idx;
    //userButtons[4].iBitmap += idx;
    //// Thêm nút bấm vào toolbar
    //SendMessage(hToolBarWnd, TB_ADDBUTTONS, (WPARAM)sizeof(userButtons) / sizeof(TBBUTTON),
    //    (LPARAM)(LPTBBUTTON)&userButtons);

    return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{

}

void OnDestroy(HWND hwnd)
{
    PostQuitMessage(0);
}

void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    
    HPEN hPen = CreatePen(PS_DASHDOT, 3, RGB(255, 0, 0));
    SelectObject(hdc, hPen);
    if (drawStack.empty()) return;
    /**/
    shared_ptr<IShape> line =  drawStack[drawStack.size()-1];
    MoveToEx(hdc, line->getA().getX(), line->getA().getY(), NULL);
    LineTo(hdc, line->getB().getX(), line->getB().getY());

    EndPaint(hwnd, &ps);
}

void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{ 
   
    HDC hdc = GetDC(hwnd);
    SetPixel(hdc, x, y, RGB(255, 0, 0));
    shared_ptr<IShape> line = ShapeFactory::getInstance()->create(1);
    line->setA(x, y);
    drawStack.push_back(line);
    isDrawing = true;
    ReleaseDC(hwnd, hdc);
}

void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
    if (drawStack.empty()) return;
    shared_ptr<IShape> line = drawStack[drawStack.size() - 1];
    line->setB(x, y);
    drawStack.push_back(line);

    isDrawing = false;
    InvalidateRect(hwnd, NULL, TRUE);
}

void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
    if (drawStack.empty() || !isDrawing) return;
    shared_ptr<IShape> line = drawStack[drawStack.size() - 1];
    line->setB(x, y);
    drawStack.push_back(line);
    InvalidateRect(hwnd, NULL, TRUE);
}