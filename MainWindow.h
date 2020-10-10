#pragma once
#include <d2d1.h>
#include "BaseWindow.h"

class MainWindow : public BaseWindow
{
	ID2D1Factory            *pFactory;
	ID2D1HwndRenderTarget   *pRenderTarget;
	ID2D1SolidColorBrush    *pBrush;
	D2D1_ELLIPSE            ellipse;
	ID2D1SolidColorBrush    *pStroke;
	ID2D1SolidColorBrush    *pStroke1;
	ID2D1SolidColorBrush    *pStroke2;
	ID2D1SolidColorBrush    *pStroke3;

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	void    DiscardGraphicsResources();
	void    OnPaint();
	void    Resize();
	void DrawClockHand(float fHandLength, float fAngle, float fStrokeWidth, ID2D1SolidColorBrush *pStroke);
	void RenderScene();

public:

	MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL)
	{
	}

	PCWSTR  ClassName() const { return L"Analog Clock Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
