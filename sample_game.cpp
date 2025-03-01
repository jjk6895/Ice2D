#include "pch.h"

#include "Ice2D.h"

class Game : public Ice2D::Application
{
public:
	Game(HINSTANCE hInstance) : Application(hInstance, 800, 500, L"Le Windowe")
	{}
private:
	D2D_POINT_2F pos, vel, size;
	Ice2D::SolidBrush brush;
	Ice2D::TextFormat font;
	void Setup() override
	{
		brush = Ice2D::SolidBrush(&manager, D2D1::ColorF(1.0f));
		font = Ice2D::TextFormat(&manager, L"Impact", 72.0f);
		font.Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		font.Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

		pos = D2D1::Point2F(100.0f, 150.0f);
		vel = D2D1::Point2F(200.0f, 200.0f);
		size = D2D1::Point2F(200.0f, 100.0f);
	}

	void Update() override
	{
		if (pos.x < 0.0f || pos.x + size.x > GetClientWidth()) vel.x *= -1.0f;
		if (pos.y < 0.0f || pos.y + size.y > GetClientHeight()) vel.y *= -1.0f;

		pos.x += vel.x * deltaTime.count();
		pos.y += vel.y * deltaTime.count();
	}

	bool prevSpace = false;
	HRESULT Draw() override
	{
		auto rt = GetRT();
		rt->BeginDraw();
		
		rt->Clear();
		auto rect = D2D1::RectF(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
		brush.SetColor(0.0f, 0.0f, 1.0f);
		rt->FillRectangle(rect, brush.Get());
		brush.SetColor(1.0f);
		rt->DrawTextW(L"DVD", 4u, font.Get(), rect, brush.Get());

		return rt->EndDraw();
	}
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
	Game game(hInstance);
	int exitCode = game.Start();
	return exitCode;
}
