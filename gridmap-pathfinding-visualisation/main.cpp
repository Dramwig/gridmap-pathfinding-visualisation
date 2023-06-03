
#include "gridmap.h"
#include "controlbar.h"
#include <vector>
#include <string>

#define VIEW_MOUSE 0 // 1：显示鼠标移动的点和方块

//可调参数
const int N = 30; // 地图默认行数
const int M = 30; // 地图默认列数
const int WIDTH = 800; // 窗口宽度
const int HEIGHT = 600; // 窗口高度
const int MAP_X = 50; // 地图左上角坐标
const int MAP_Y = 50; // 地图左上角坐标
const int MAP_WIDTH = 500; // 地图宽度
const int MAP_HEIGHT = 500; // 地图高度
const int BAR_X = 600; // 控制栏左上角坐标
const int BAR_Y = 50; // 控制栏左上角坐标
const int BAR_WIDTH = 200; // 控制栏宽度
const int BAR_HEIGHT = 500; // 控制栏高度


int main()
{
	initgraph(WIDTH, HEIGHT);

	ControlBar bar(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, 13);
	bar.insert(80, 30, "设置地图" ); //按键0
	bar.insert(80, 30, "清空地图" ); //按键1
	bar.insert(70, 30, "DFS", SHAPE_CIRC);//2
	bar.insert(70, 30, "BFS", SHAPE_CIRC);//3
	bar.insert(70, 30, "SPFA", SHAPE_CIRC);//4
	bar.insert(70, 30, "Dijkstra", SHAPE_CIRC);//5
	bar.insert(70, 30, "A*", SHAPE_CIRC);//6
	bar.insert(80, 30, "清除轨迹"); //7
	bar.insert(0, 400, 55, 20, "行加一", SHAPE_RECT); // 8
	bar.insert(65, 400, 55, 20, "行减一", SHAPE_RECT); // 9
	bar.insert(0, 435, 55, 20, "列加一", SHAPE_RECT); // 10
	bar.insert(65, 435, 55, 20, "列减一", SHAPE_RECT); // 11
	bar.draw();

	GridMap G(N, M, MAP_X, MAP_Y, MAP_WIDTH, MAP_HEIGHT);
	G.draw();

	ExMessage msg;
	bool isLPress = false; // 左键是否按下
	bool isRPress = false; // 右键是否按下
	bool isExit = false; // 是否退出

	std::string commond = "单点\"设置地图\"，尝试左键和右键增减障碍物..."; //操作引导提示词
	bool new_commond = 1; //是否更新操作引导提示词
	
	bool new_Press[7] = { 0,0,0,0,0,0 };

	while (!isExit)
	{

		while (peekmessage(&msg, EM_KEY | EM_MOUSE)) // 获取一个鼠标或按键消息,立即返回
		{
			if (new_commond) { //输出操作引导提示词
				new_commond = 0;
				clearrectangle(MAP_X, 0, MAP_X + MAP_WIDTH, MAP_Y);
				RECT r = { MAP_X, 0, MAP_X+ MAP_WIDTH,   MAP_Y };
				TCHAR* tstr = new TCHAR[commond.length() + 1];
				MultiByteToWideChar(CP_ACP, 0, commond.c_str(), -1, tstr, static_cast<int>(commond.length() + 1));
				drawtext(tstr, &r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				delete[] tstr;
			}

			bar.UpdateStatus(msg.x, msg.y, msg.lbutton); // 更新按钮状态
			if (bar.GetStatus(0) == STATE_OPEN) // 按键0“设置地图”按下
			{
				G.UpdateNodeType(msg.x, msg.y, msg.lbutton, isLPress, msg.rbutton, isRPress, (msg.message == WM_KEYDOWN && msg.vkcode == 0x53), (msg.message == WM_KEYDOWN && msg.vkcode == 0x45)); // 更新结点状态
				commond = "按下键盘\"S\"或\"E\"以设置起点终点...";
				new_commond = 1;
				for (int j = 2; j <= 6; j++)
					if (bar.GetStatus(j) == STATE_OPEN)
						bar.reset(j);
			}
			if (bar.GetStatus(1) == STATE_OPEN) // 按键1“清空地图”按下
			{
				G.reset();
				bar.reset(1);
				commond = "地图已清空！";
				new_commond = 1;
				for (int j = 2; j <= 6; j++)
					if (bar.GetStatus(j) == STATE_OPEN)
						bar.reset(j);
			}

			for (int i = 2; i <= 6; i++) //算法按钮
			{
				if (bar.GetStatus(i) == STATE_OPEN && G.nullcheck()) { //检查是否有起点终点
					commond = "请定义起点S和终点E！";
					new_commond = 1;
					bar.reset(i);
				}
				if (bar.GetStatus(i) == STATE_OPEN && new_Press[i] == 0) //按下算法按钮
				{
					for (int j = 2; j <= 6; j++) //弹起其他算法按钮
						if (j != i && bar.GetStatus(j) == STATE_OPEN)
							bar.reset(j);
					new_Press[i] = 1;
					G.init(); //初始化算法参数
					G.draw(); //重绘地图
					switch (i) {
					case 2: G.dfs(); break;
					case 3:	G.bfs(); break;
					case 4: G.SPFA(); break;
					case 5: G.Dijsktra(); break;
					case 6: G.Astar(); break;
					}
					printf("Performed a pathfinding.\n");
					G.DrawPath(); //绘制路径
					G.DrawAccess(); //绘制区域
					commond = "尝试点击\"清除轨迹\"或直接点击切换其他算法...";
					new_commond = 1;
				} else if (bar.GetStatus(i) != STATE_OPEN && new_Press[i] == 1){
					new_Press[i] = 0;
				}
			}

			if (bar.GetStatus(7) == STATE_OPEN)
			{
				G.draw();
				bar.reset(7);
				commond = "轨迹已清空！";
				new_commond = 1;
				for (int j = 2; j <= 6; j++)
					if (bar.GetStatus(j) == STATE_OPEN)
						bar.reset(j);
			}
			if (bar.GetStatus(8) == STATE_OPEN)
			{
				G.AddN();
				bar.reset(8);
				for (int j = 2; j <= 6; j++)
					if (bar.GetStatus(j) == STATE_OPEN)
						bar.reset(j);
			}
			if (bar.GetStatus(9) == STATE_OPEN) 
			{
				G.LesN();
				bar.reset(9);
				for (int j = 2; j <= 6; j++)
					if (bar.GetStatus(j) == STATE_OPEN)
						bar.reset(j);
			}
			if (bar.GetStatus(10) == STATE_OPEN) 
			{
				G.AddM();
				bar.reset(10);
				for (int j = 2; j <= 6; j++)
					if (bar.GetStatus(j) == STATE_OPEN)
						bar.reset(j);
			}
			if (bar.GetStatus(11) == STATE_OPEN) 
			{
				G.LesM();
				bar.reset(11);
				for (int j = 2; j <= 6; j++)
					if (bar.GetStatus(j) == STATE_OPEN)
						bar.reset(j);
			}


			if (msg.message == WM_KEYDOWN) // 按键消息
			{
				if (msg.vkcode == VK_ESCAPE) isExit = true; // 按下ESC键退出
			}
			else if (msg.message == WM_MOUSEMOVE || msg.message == WM_LBUTTONDOWN || msg.message == WM_LBUTTONUP || msg.message == WM_RBUTTONDOWN || msg.message == WM_RBUTTONUP) // 鼠标消息
			{
				if (!isLPress && msg.lbutton)  // 按下左键
				{
					isLPress = true;
					// 画一个大方块
					if (VIEW_MOUSE) {
						setlinecolor(RGB(255, 0, 0));
						rectangle(msg.x - 10, msg.y - 10, msg.x + 10, msg.y + 10);
					}
				}
				else if (isLPress && msg.lbutton) // 拖动左键
				{
					// 鼠标移动的时候画红色的小点
					if(VIEW_MOUSE)
						putpixel(msg.x, msg.y, RED);
				}
				else if (isLPress && !msg.lbutton) // 松开左键
				{
					isLPress = false;
					// 画一个小方块
					if (VIEW_MOUSE) {
						setlinecolor(RGB(255, 0, 0));
						rectangle(msg.x - 5, msg.y - 5, msg.x + 5, msg.y + 5);
					}
						
				}
				else if (!isRPress && msg.rbutton) // 按下右键
				{
					isRPress = true;
				}
				else if (isRPress && msg.rbutton) // 拖动右键
				{

				}
				else if (isRPress && !msg.rbutton) // 松开右键
				{
					isRPress = false;
				}
			}
		}//End While 获取鼠标键盘信息
	} //退出
	closegraph();
	return 0;
}

