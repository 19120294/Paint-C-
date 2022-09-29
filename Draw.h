#pragma once
#include "framework.h"
#include "PaintApp.h"
#include <WindowsX.h>
#include <memory>

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

void OnDestroy(HWND hwnd);

void OnPaint(HWND hwnd);

void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);

void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);

void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
