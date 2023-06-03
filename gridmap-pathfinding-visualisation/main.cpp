
#include "gridmap.h"
#include "controlbar.h"
#include <vector>
#include <string>

#define VIEW_MOUSE 0 // 1����ʾ����ƶ��ĵ�ͷ���

//�ɵ�����
const int N = 30; // ��ͼĬ������
const int M = 30; // ��ͼĬ������
const int WIDTH = 800; // ���ڿ��
const int HEIGHT = 600; // ���ڸ߶�
const int MAP_X = 50; // ��ͼ���Ͻ�����
const int MAP_Y = 50; // ��ͼ���Ͻ�����
const int MAP_WIDTH = 500; // ��ͼ���
const int MAP_HEIGHT = 500; // ��ͼ�߶�
const int BAR_X = 600; // ���������Ͻ�����
const int BAR_Y = 50; // ���������Ͻ�����
const int BAR_WIDTH = 200; // ���������
const int BAR_HEIGHT = 500; // �������߶�


int main()
{
	initgraph(WIDTH, HEIGHT);

	ControlBar bar(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, 13);
	bar.insert(80, 30, "���õ�ͼ" ); //����0
	bar.insert(80, 30, "��յ�ͼ" ); //����1
	bar.insert(70, 30, "DFS", SHAPE_CIRC);//2
	bar.insert(70, 30, "BFS", SHAPE_CIRC);//3
	bar.insert(70, 30, "SPFA", SHAPE_CIRC);//4
	bar.insert(70, 30, "Dijkstra", SHAPE_CIRC);//5
	bar.insert(70, 30, "A*", SHAPE_CIRC);//6
	bar.insert(80, 30, "����켣"); //7
	bar.insert(0, 400, 55, 20, "�м�һ", SHAPE_RECT); // 8
	bar.insert(65, 400, 55, 20, "�м�һ", SHAPE_RECT); // 9
	bar.insert(0, 435, 55, 20, "�м�һ", SHAPE_RECT); // 10
	bar.insert(65, 435, 55, 20, "�м�һ", SHAPE_RECT); // 11
	bar.draw();

	GridMap G(N, M, MAP_X, MAP_Y, MAP_WIDTH, MAP_HEIGHT);
	G.draw();

	ExMessage msg;
	bool isLPress = false; // ����Ƿ���
	bool isRPress = false; // �Ҽ��Ƿ���
	bool isExit = false; // �Ƿ��˳�

	std::string commond = "����\"���õ�ͼ\"������������Ҽ������ϰ���..."; //����������ʾ��
	bool new_commond = 1; //�Ƿ���²���������ʾ��
	
	bool new_Press[7] = { 0,0,0,0,0,0 };

	while (!isExit)
	{

		while (peekmessage(&msg, EM_KEY | EM_MOUSE)) // ��ȡһ�����򰴼���Ϣ,��������
		{
			if (new_commond) { //�������������ʾ��
				new_commond = 0;
				clearrectangle(MAP_X, 0, MAP_X + MAP_WIDTH, MAP_Y);
				RECT r = { MAP_X, 0, MAP_X+ MAP_WIDTH,   MAP_Y };
				TCHAR* tstr = new TCHAR[commond.length() + 1];
				MultiByteToWideChar(CP_ACP, 0, commond.c_str(), -1, tstr, static_cast<int>(commond.length() + 1));
				drawtext(tstr, &r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				delete[] tstr;
			}

			bar.UpdateStatus(msg.x, msg.y, msg.lbutton); // ���°�ť״̬
			if (bar.GetStatus(0) == STATE_OPEN) // ����0�����õ�ͼ������
			{
				G.UpdateNodeType(msg.x, msg.y, msg.lbutton, isLPress, msg.rbutton, isRPress, (msg.message == WM_KEYDOWN && msg.vkcode == 0x53), (msg.message == WM_KEYDOWN && msg.vkcode == 0x45)); // ���½��״̬
				commond = "���¼���\"S\"��\"E\"����������յ�...";
				new_commond = 1;
				for (int j = 2; j <= 6; j++)
					if (bar.GetStatus(j) == STATE_OPEN)
						bar.reset(j);
			}
			if (bar.GetStatus(1) == STATE_OPEN) // ����1����յ�ͼ������
			{
				G.reset();
				bar.reset(1);
				commond = "��ͼ����գ�";
				new_commond = 1;
				for (int j = 2; j <= 6; j++)
					if (bar.GetStatus(j) == STATE_OPEN)
						bar.reset(j);
			}

			for (int i = 2; i <= 6; i++) //�㷨��ť
			{
				if (bar.GetStatus(i) == STATE_OPEN && G.nullcheck()) { //����Ƿ�������յ�
					commond = "�붨�����S���յ�E��";
					new_commond = 1;
					bar.reset(i);
				}
				if (bar.GetStatus(i) == STATE_OPEN && new_Press[i] == 0) //�����㷨��ť
				{
					for (int j = 2; j <= 6; j++) //���������㷨��ť
						if (j != i && bar.GetStatus(j) == STATE_OPEN)
							bar.reset(j);
					new_Press[i] = 1;
					G.init(); //��ʼ���㷨����
					G.draw(); //�ػ��ͼ
					switch (i) {
					case 2: G.dfs(); break;
					case 3:	G.bfs(); break;
					case 4: G.SPFA(); break;
					case 5: G.Dijsktra(); break;
					case 6: G.Astar(); break;
					}
					printf("Performed a pathfinding.\n");
					G.DrawPath(); //����·��
					G.DrawAccess(); //��������
					commond = "���Ե��\"����켣\"��ֱ�ӵ���л������㷨...";
					new_commond = 1;
				} else if (bar.GetStatus(i) != STATE_OPEN && new_Press[i] == 1){
					new_Press[i] = 0;
				}
			}

			if (bar.GetStatus(7) == STATE_OPEN)
			{
				G.draw();
				bar.reset(7);
				commond = "�켣����գ�";
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


			if (msg.message == WM_KEYDOWN) // ������Ϣ
			{
				if (msg.vkcode == VK_ESCAPE) isExit = true; // ����ESC���˳�
			}
			else if (msg.message == WM_MOUSEMOVE || msg.message == WM_LBUTTONDOWN || msg.message == WM_LBUTTONUP || msg.message == WM_RBUTTONDOWN || msg.message == WM_RBUTTONUP) // �����Ϣ
			{
				if (!isLPress && msg.lbutton)  // �������
				{
					isLPress = true;
					// ��һ���󷽿�
					if (VIEW_MOUSE) {
						setlinecolor(RGB(255, 0, 0));
						rectangle(msg.x - 10, msg.y - 10, msg.x + 10, msg.y + 10);
					}
				}
				else if (isLPress && msg.lbutton) // �϶����
				{
					// ����ƶ���ʱ�򻭺�ɫ��С��
					if(VIEW_MOUSE)
						putpixel(msg.x, msg.y, RED);
				}
				else if (isLPress && !msg.lbutton) // �ɿ����
				{
					isLPress = false;
					// ��һ��С����
					if (VIEW_MOUSE) {
						setlinecolor(RGB(255, 0, 0));
						rectangle(msg.x - 5, msg.y - 5, msg.x + 5, msg.y + 5);
					}
						
				}
				else if (!isRPress && msg.rbutton) // �����Ҽ�
				{
					isRPress = true;
				}
				else if (isRPress && msg.rbutton) // �϶��Ҽ�
				{

				}
				else if (isRPress && !msg.rbutton) // �ɿ��Ҽ�
				{
					isRPress = false;
				}
			}
		}//End While ��ȡ��������Ϣ
	} //�˳�
	closegraph();
	return 0;
}

