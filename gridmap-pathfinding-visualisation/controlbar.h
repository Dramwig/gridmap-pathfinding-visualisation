#pragma once

#include <graphics.h>
#include <vector>
#include <string>
#include <tchar.h>
#include <windows.h>

enum shape { SHAPE_RECT, SHAPE_ROUN, SHAPE_CIRC }; // 矩形、圆角矩形、椭圆
enum statu { STATE_OPEN , STATE_CLOSE, STATE_SELECT}; // 按下、闲置、悬浮

const COLORREF COLOR_RED = RGB(255, 0, 0);
const COLORREF COLOR_GREEN = RGB(0, 255, 0);
const COLORREF COLOR_BLUE = RGB(0, 0, 255);
const COLORREF COLOR_BLACK = RGB(0, 0, 0);
const COLORREF COLOR_WHITE = RGB(255, 255, 255);
const COLORREF COLOR_GRAY = RGB(128, 128, 128);

const COLORREF COLOR_CLOSE = COLOR_WHITE;
const COLORREF COLOR_OPEN = COLOR_BLUE;
const COLORREF COLOR_SELECT = COLOR_GRAY;


class Button
{
	friend class ControlBar;
	int id;
	std::string text;
	int x, y, width, height;
	shape type;
	statu state;
public:
	Button(int id, std::string text, int x, int y, int width, int height, shape type = SHAPE_ROUN) : id(id), text(text), x(x), y(y), width(width), height(height), type(type), state(STATE_CLOSE) {}
	void draw(int dx,int dy);
	statu GetStatu() {return state;}
	void reset() { state = STATE_CLOSE; }
};

class ControlBar
{
	std::vector<Button> buttons;
	int cnt;
	int x, y, width, height;
	int LILENGTH;
public:
	ControlBar(int x, int y, int width, int height, int LILENGTH = 10) : x(x), y(y), width(width), height(height), LILENGTH(LILENGTH), cnt(0) {}
	void insert(int x, int y, int width, int height, std::string text, shape type);
	void insert(int width, int height, std::string text, shape type); // 插入按钮功能
	void draw(); // 按钮绘制功能
	int GetSelectButton(int mx,int my);  // 获取选中按钮
	void UpdateStatus(int mx, int my, bool isLDown); // 更新按钮状态
	statu GetStatus(int i); // 获取按钮状态
	void reset(int i); // 重置按钮状态
};

void ControlBar::insert(int width, int height, std::string text = "Unnamed", shape type = SHAPE_ROUN){ // 插入一个按钮
	static int bound = 0;
	bound += LILENGTH;
	buttons.push_back(Button(cnt++, text, LILENGTH, bound, width, height, type));
	bound += height;
}

void ControlBar::insert(int x, int y, int width, int height, std::string text = "Unnamed", shape type = SHAPE_ROUN){ // 插入一个按钮
	buttons.push_back(Button(cnt++, text, x, y, width, height, type));
}

void Button:: draw(int dx = 0, int dy = 0){ // 绘制按钮
	// 设置状态对应颜色
	switch (state)
	{
	case STATE_CLOSE:
		setfillcolor(COLOR_CLOSE);
		break;
	case STATE_OPEN:
		setfillcolor(COLOR_OPEN);
		break;
	case STATE_SELECT:
		setfillcolor(COLOR_SELECT);
		break;
	default:
		setfillcolor(COLOR_RED);
		break;
	}
	// 绘制按钮对应形状
	switch (type)
	{
	case SHAPE_RECT:
		// 画矩形
		fillrectangle(dx + x, dy + y, dx + x + width, dy + y + height);
		break;
	case SHAPE_ROUN:
		// 画圆角矩形
		fillroundrect(dx + x, dy + y, dx + x + width, dy + y + height, 5, 5);
		break;
	case SHAPE_CIRC:
		// 画椭圆
		fillellipse(dx + x, dy + y, dx + x + width, dy + y + height);
		break;
	default:
		break;
	}
	RECT r = { dx + x, dy + y,  dx + x + width, dy + y + height };

	std::string str = text;
	TCHAR* tstr = new TCHAR[str.length() + 1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, tstr, static_cast<int>(str.length() + 1));
	drawtext(tstr, &r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	delete[] tstr;
}

void ControlBar::draw(){ // 绘制控制栏
	// 开始缓冲绘制
	BeginBatchDraw();

	setlinecolor(RGB(255, 255, 255));
	for (auto& button : buttons) {
		button.draw(x,y);
	}
	// 结束缓冲绘制
	EndBatchDraw();

	// 绘制缓冲区内容到屏幕上
	FlushBatchDraw();
}

int ControlBar::GetSelectButton(int mx, int my) {
	for (auto& button : buttons) {
		if(mx<=x+button.x+button.width&&mx>=x+button.x&&my<=y+button.y+button.height&&my>=y+button.y)
			return button.id;
	}
	return -1;
}

void ControlBar::UpdateStatus(int mx, int my,bool isLDown) {
	for (auto& button : buttons) {
		if (mx <= x + button.x + button.width && mx >= x + button.x && my <= y + button.y + button.height && my >= y + button.y)
			if(isLDown)
				button.state = (button.state==STATE_OPEN ? STATE_CLOSE : STATE_OPEN);
			else
				button.state = (button.state == STATE_OPEN ? STATE_OPEN : STATE_SELECT);
		else
			button.state = (button.state == STATE_OPEN ? STATE_OPEN : STATE_CLOSE);
	}
	draw();
}

statu ControlBar::GetStatus(int i) {
	return buttons[i].GetStatu();
}

void ControlBar::reset(int i) {
	buttons[i].reset();
	draw();
}