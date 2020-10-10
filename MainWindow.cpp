#include <Windows.h>
#include "MainWindow.h"
#pragma comment(lib, "d2d1")

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

// �������� ����������� ��� ��������� ������� ����.
void MainWindow::CalculateLayout()
{
	if (pRenderTarget != NULL)
	{
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		const float x = size.width / 2;
		const float y = size.height / 2;
		const float radius = min(x, y);
		ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
	}
}

HRESULT MainWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

			if (SUCCEEDED(hr))
			{
				const D2D1_COLOR_F handcolor = D2D1::ColorF(0, 0, 0);
				hr = pRenderTarget->CreateSolidColorBrush(handcolor, &pStroke);
				if (SUCCEEDED(hr))
				{
					// ��� ������� �������
					const D2D1_COLOR_F handcolor = D2D1::ColorF(1, 0, 0);
					hr = pRenderTarget->CreateSolidColorBrush(handcolor, &pStroke1);
					if (SUCCEEDED(hr))
					{
						// ��� �������� �������
						const D2D1_COLOR_F handcolor = D2D1::ColorF(0, 1, 0);
						hr = pRenderTarget->CreateSolidColorBrush(handcolor, &pStroke2);
						if (SUCCEEDED(hr))
						{
							// ��� ��������� �������
							const D2D1_COLOR_F handcolor = D2D1::ColorF(0, 0, 1);
							hr = pRenderTarget->CreateSolidColorBrush(handcolor, &pStroke3);
							if (SUCCEEDED(hr))
							{
								CalculateLayout();
							}
						}
					}
				}
			}
		}
	}
	return hr;
}

void MainWindow::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBrush);
}

void MainWindow::OnPaint()
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();

		RenderScene();

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}

		EndPaint(m_hwnd, &ps);
	}
}

void MainWindow::Resize()
{
	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		pRenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

void MainWindow::DrawClockHand(float fHandLength, float fAngle, float fStrokeWidth, ID2D1SolidColorBrush *pStroke)
{
	pRenderTarget->SetTransform(
		D2D1::Matrix3x2F::Rotation(fAngle, ellipse.point)
	);

	// endPoint ���������� ���� ����� �������.
	D2D_POINT_2F endPoint = D2D1::Point2F(
		ellipse.point.x,
		ellipse.point.y - (ellipse.radiusY * fHandLength)
	);

	// ���������� ������ �� ������ ������� � endPoint.
	pRenderTarget->DrawLine(
		ellipse.point, endPoint, pStroke, fStrokeWidth);

	// ����� �������
	D2D_POINT_2F left_endPoint = D2D1::Point2F(ellipse.point.x - 15, ellipse.point.y - (ellipse.radiusY * fHandLength) + 25);
	pRenderTarget->DrawLine(endPoint, left_endPoint, pStroke, fStrokeWidth);

	D2D_POINT_2F right_endPoint = D2D1::Point2F(ellipse.point.x + 15, ellipse.point.y - (ellipse.radiusY * fHandLength) + 25);
	pRenderTarget->DrawLine(endPoint, right_endPoint, pStroke, fStrokeWidth);
}

void MainWindow::RenderScene()
{
	// �������
	SYSTEMTIME time;
	GetLocalTime(&time);

	// ��� �������� ���������
	const float h = time.wHour / 24.0f;
	const float m = (time.wMinute) / 60.0f;
	const float s = (time.wSecond) / 60.0f;


	//pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF(h, m, s)));

	pRenderTarget->FillEllipse(ellipse, pBrush);
	pRenderTarget->DrawEllipse(ellipse, pStroke);
	

	// 60 ����� = 30 ��������, 1 ������ = 0.5 �������
	const float fHourAngle = (360.0f / 12) * (time.wHour) + (time.wMinute * 0.5f);
	const float fMinuteAngle = (360.0f / 60) * (time.wMinute);
	const float fSecondAngle = (360.0f / 60) * (time.wSecond);

	DrawClockHand(0.6f, fHourAngle, 6, pStroke1);
	DrawClockHand(0.85f, fMinuteAngle, 4, pStroke2);
	DrawClockHand(0.95f, fSecondAngle, 2, pStroke3);

	// ������������ ������������ �������������.
	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		// ������������� ������
		SetTimer(m_hwnd, 1, 1000, NULL);

		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1;  // ������� CreateWindowEx.
		}
		return 0;

	case WM_DESTROY:
		DiscardGraphicsResources();
		SafeRelease(&pFactory);
		KillTimer(m_hwnd, 1);
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;

	// ������������ ������
	case WM_TIMER:
		OnPaint();
		return 0;

	case WM_SIZE:
		Resize();
		return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}