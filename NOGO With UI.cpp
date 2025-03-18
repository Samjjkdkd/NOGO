
#pragma optimize (3)
#include <stdlib.h>
#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <direct.h>
#include <string.h>
#pragma comment(lib,"Winmm.lib")//音乐

#include <climits>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <iostream>
#include <math.h>
#include <random>
#include <string>
using namespace std;
#define height 700
#define width 1000
#define textlength2 150
#define startx2_1 310
#define startx2_2 600
#define starty2 440
#define startx3 700
#define starty3 180
#define textlarge 55
#define textheight 70
#define textlength 200
#define startx 390
#define starty 340
#define blank 15
#define offsetx 70 //绘图时x的偏移值
#define offsety 70 //绘图时x的偏移值
#define offsetcolumn 300
#define map_width 9//棋盘宽
#define map_height 9//棋盘高
#define grid_size 70	//棋盘一格的长度
#define piece_size 68 //棋子的直径

IMAGE backImg;	 //保存图片的数据类型
MOUSEMSG msg = { 0 };
int _color;
bool PlayerFirst;
int Process[83] = { 0 };

const double D1 = 1, D2 = -1, D3 = 0.5, D4 = -0.5;
const double C1 = 1, C2 = -1, C3 = -0.3;//系数
const double S = 2;
int constant = 10;
bool _switch = false;//控制开启不同的估值函数

int start, timeout;//控制时间
int depth;//模拟深度
int Player = 1, Initial_Player;//棋子颜色
int Board[9][9] = { 0 };//棋盘。黑为1，白为-1，
bool visited_By_Air_Judge[9][9];//标记有没有被搜气函数搜到过
int Avail[2][81]; //双方可落子点 1为黑，0为白
int len[2]; //可落子点数量 1为黑，0为白
int Initial_Avail[2][81]; //保存初状态
int Initial_len[2]; //保存初状态
int _index = 0;//Node[]的数组下标，保证一直增加，不会重
int Best_x, Best_y;//最后选出来的落子点。

struct MCTS
{
    int x, y;            //节点走的位置
    bool Expanded;       //是否完全拓展
    int Num_of_ChildNode; //目前已有的孩子节点的个数
    int Children[81];   //存放该节点对应的子结点的下标
    double Value;      //节点的棋盘价值
    double Times;      //节点的访问次数
} Node[5000000] = {}; //节点信息

void InitPage() //初始化页面
{
    initgraph(width, height);
    loadimage(NULL, _T("start.jpg"));
    setbkmode(TRANSPARENT);
    settextstyle(textlarge, 0, L"楷体");    //设置字体的风格

    settextcolor(RGB(94, 73, 20)); //设置字体的颜色
    outtextxy(startx, starty, L"人机对战!");   //输出文字
    outtextxy(startx, starty + textheight, L"人人对战!");
    outtextxy(startx, starty + textheight * 2, L"读档");
    outtextxy(startx, starty + textheight * 3, L"退出游戏:(");
}

void ModelShow() //模式显示
{
	msg = GetMouseMsg();  //获取鼠标的消息

	//鼠标的位置 在Begin !的位置
	if (msg.x >= startx && msg.x < startx + textlength && msg.y >= starty&& msg.y <= starty + textheight - blank)
	{
		settextcolor(RGB(255, 0, 0)); //设置字体的颜色
		outtextxy(startx, starty, L"人机对战!"); //255, 180 用于在指定位置输出字符串。
        settextcolor(RGB(94, 73, 20));
        outtextxy(startx, starty + textheight, L"人人对战!");
        outtextxy(startx, starty + textheight * 2, L"读档");
        outtextxy(startx, starty + textheight * 3, L"退出游戏:(");
	}
	else if (msg.x >= startx && msg.x < startx + textlength && msg.y >= starty + textheight&& msg.y <= starty + textheight * 2 - blank)
	{
		settextcolor(RGB(255, 0, 0));	//设置字体的颜色
		outtextxy(startx, starty + textheight, L"人人对战!");
        settextcolor(RGB(94, 73, 20));
        outtextxy(startx, starty, L"人机对战!");   //输出文字
        outtextxy(startx, starty + textheight * 2, L"读档");
        outtextxy(startx, starty + textheight * 3, L"退出游戏:(");
	}
	else if (msg.x >= startx && msg.x < startx + textlength && msg.y >= starty + textheight  * 2&& msg.y <= starty + textheight * 3 - blank)
	{
		settextcolor(RGB(255, 0, 0));	//设置字体的颜色
        outtextxy(startx, starty + textheight * 2, L"读档");
        settextcolor(RGB(94, 73, 20));
        outtextxy(startx, starty, L"人机对战!");   //输出文字
        outtextxy(startx, starty + textheight, L"人人对战!");
        outtextxy(startx, starty + textheight * 3, L"退出游戏:(");
	}
    else if (msg.x >= startx && msg.x < startx + textlength && msg.y >= starty + textheight * 3&& msg.y <= starty + textheight * 4 - blank) {
        settextcolor(RGB(255, 0, 0));	//设置字体的颜色
        outtextxy(startx, starty + textheight * 3, L"退出游戏:(");
        settextcolor(RGB(94, 73, 20));
        outtextxy(startx, starty, L"人机对战!");   //输出文字
        outtextxy(startx, starty + textheight, L"人人对战!");
        outtextxy(startx, starty + textheight * 2, L"读档");
    }
    else
	{
		settextcolor(RGB(94, 73, 20)); //设置字体的颜色
		outtextxy(startx, starty, L"人机对战!");   //输出文字
		outtextxy(startx, starty + textheight, L"人人对战!");
        outtextxy(startx, starty + textheight * 2, L"读档");
        outtextxy(startx, starty + textheight * 3, L"退出游戏:(");
	}
}
void ColumnShow() {
    msg = GetMouseMsg(); //获取一个鼠标消息

    if (msg.x >= startx3 && msg.x < startx3 + textlength && msg.y >= starty3 && msg.y <= starty3 + textheight - blank)
    {
        settextcolor(RGB(255, 0, 0)); //设置字体的颜色
        outtextxy(startx3, starty3, L"存档"); //255, 180 用于在指定位置输出字符串。
        settextcolor(RGB(94, 73, 20));
        outtextxy(startx3, starty3 + textheight, L"悔棋");
        outtextxy(startx3, starty3 + textheight * 2, L"AI帮下");
        outtextxy(startx3, starty3 + textheight * 3, L"返回菜单");
    }
    else if (msg.x >= startx3 && msg.x < startx3 + textlength && msg.y >= starty3 + textheight && msg.y <= starty3 + textheight * 2 - blank)
    {
        settextcolor(RGB(255, 0, 0));	//设置字体的颜色
        outtextxy(startx3, starty3 + textheight, L"悔棋");
        settextcolor(RGB(94, 73, 20));
        outtextxy(startx3, starty3, L"存档");   //输出文字
        outtextxy(startx3, starty3 + textheight * 2, L"AI帮下");
        outtextxy(startx3, starty3 + textheight * 3, L"返回菜单");
    }
    else if (msg.x >= startx3 && msg.x < startx3 + textlength && msg.y >= starty3 + textheight * 2 && msg.y <= starty3 + textheight * 3 - blank)
    {
        settextcolor(RGB(255, 0, 0));	//设置字体的颜色
        outtextxy(startx3, starty3 + textheight * 2, L"AI帮下");
        settextcolor(RGB(94, 73, 20));
        outtextxy(startx3, starty3, L"存档");   //输出文字
        outtextxy(startx3, starty3 + textheight, L"悔棋");
        outtextxy(startx3, starty3 + textheight * 3, L"返回菜单");
    }
    else if (msg.x >= startx3 && msg.x < startx3 + textlength && msg.y >= starty3 + textheight * 3 && msg.y <= starty3 + textheight * 4 - blank) {
        settextcolor(RGB(255, 0, 0));	//设置字体的颜色
        outtextxy(startx3, starty3 + textheight * 3, L"返回菜单");
        settextcolor(RGB(94, 73, 20));
        outtextxy(startx3, starty3, L"存档");   //输出文字
        outtextxy(startx3, starty3 + textheight, L"悔棋");
        outtextxy(startx3, starty3 + textheight * 2, L"AI帮下");
    }
    else
    {
        settextcolor(RGB(94, 73, 20)); //设置字体的颜色
        outtextxy(startx3, starty3, L"存档");   //输出文字
        outtextxy(startx3, starty3 + textheight, L"悔棋");
        outtextxy(startx3, starty3 + textheight * 2, L"AI帮下");
        outtextxy(startx3, starty3 + textheight * 3, L"返回菜单");
    }
}
//void drawPiece0(int x, int y) {
//    setfillcolor(BROWN); 
//    solidcircle(y* grid_size + offsetx, x* grid_size + offsety, piece_size / 2);
//}
//void PositionShow() {
//    msg = GetMouseMsg();
//    int x1 = 0, y1 = 0;
//    cleardevice();
//    loadimage(NULL, _T("board.jpg"));
//    setbkmode(TRANSPARENT);
//    settextstyle(55, 0, L"楷体");    //设置字体的风格
//    settextcolor(RGB(94, 73, 20)); //设置字体的颜色
//    outtextxy(startx3, starty3, L"存档");   //输出文字
//    outtextxy(startx3, starty3 + textheight, L"悔棋");
//    outtextxy(startx3, starty3 + textheight * 2, L"AI帮下");
//    outtextxy(startx3, starty3 + textheight * 3, L"返回菜单");
//    if (msg.x <= (map_width - 1) * grid_size + offsetx * 2)
//    {
//        int y = (msg.x - offsetx + grid_size / 2) / grid_size;
//        int x = (msg.y - offsety + grid_size / 2) / grid_size;
//        if (x <= 8 && y <= 8 && Board[x][y] == 0) {
//            drawPiece0(x, y);
//        }
//    }
//}
void initGame() //游戏初始化
{
	_color = 1;//初始棋子为黑
	//把棋盘置空
	for (int i = 0; i < map_height; i++)
	{
		for (int j = 0; j < map_width; j++)
		{
			Board[i][j] = 0;
		}
	}
}
void initView() {
    cleardevice();
    loadimage(NULL, _T("board.jpg"));
    setbkmode(TRANSPARENT);
    settextstyle(55, 0, L"楷体");    //设置字体的风格
    settextcolor(RGB(94, 73, 20)); //设置字体的颜色
    outtextxy(startx3, starty3, L"存档");   //输出文字
    outtextxy(startx3, starty3 + textheight, L"悔棋");
    outtextxy(startx3, starty3 + textheight * 2, L"AI帮下");
    outtextxy(startx3, starty3 + textheight * 3, L"返回菜单");
}
//void initView() //初始化界面
//{
//	initgraph((map_width - 1) * grid_size + offsetx * 2 + offsetcolumn, (map_height - 1) * grid_size + offsety * 2);
//	//加载背景
//	loadimage(NULL, _T("2.jpg"), map_width * grid_size + offsetx * 2 + offsetcolumn, map_height * grid_size + offsety * 2);// 读取棋盘图片至绘图窗口 
//    setbkmode(TRANSPARENT);  //设置背景的风格 tranparent（透明的）
//    settextstyle(55, 0, L"楷体");    //设置字体的风格
//	//画线
//	setlinecolor(BLACK);
//	for (int i = 0; i < map_width; i++)//竖线
//	{
//		line(i * grid_size + offsetx, offsety, i * grid_size + offsetx, offsety + (map_height - 1) * grid_size);
//	}
//	for (int i = 0; i < map_height; i++)//横线
//	{
//		line(offsetx, offsety + i * grid_size, (map_width - 1) * grid_size + offsetx, offsety + i * grid_size);
//	}
//}

void DecideWhoFirst() {
    cleardevice();
    loadimage(NULL, _T("choose.png"));
    setbkmode(TRANSPARENT);
    settextstyle(textlarge, 0, L"楷体");    //设置字体的风格
    settextcolor(RGB(94, 73, 20)); //设置字体的颜色
    outtextxy(startx2_1, starty2, L"执黑!");   //输出文字
    outtextxy(startx2_2, starty2, L"执白!");
	while (1)
	{
		msg = GetMouseMsg();  //获取鼠标的消息

		//鼠标的位置 在Begin !的位置
		if (msg.x >= startx2_1 && msg.x < startx2_1 + textlength2 && msg.y >= starty2 && msg.y <= starty2 + textheight)
		{
			settextcolor(RGB(255, 0, 0)); //设置字体的颜色
            outtextxy(startx2_1, starty2, L"执黑!"); //255, 180 用于在指定位置输出字符串。
		}
		else if (msg.x >= startx2_2 && msg.x < startx2_2 + textlength2 && msg.y >= starty2 && msg.y <= starty2 + textheight)
		{
			settextcolor(RGB(255, 0, 0));	//设置字体的颜色
            outtextxy(startx2_2, starty2, L"执白!");
		}
		else
		{
			settextcolor(RGB(94, 73, 20)); //设置字体的颜色
            outtextxy(startx2_1, starty2, L"执黑!");   //输出文字
            outtextxy(startx2_2, starty2, L"执白!");
		}
		switch (msg.uMsg)
		{
		case WM_LBUTTONDOWN: //鼠标左键按下
			if (msg.x >= startx2_1 && msg.x < startx2_1 + textlength2 && msg.y >= starty2 && msg.y <= starty2 + textheight) //在人机对战区域按下
			{
				PlayerFirst = true;
				return;
			}
			if (msg.x >= startx2_2 && msg.x < startx2_2 + textlength2 && msg.y >= starty2 && msg.y <= starty2 + textheight) //在人人对战区域按下
			{
				PlayerFirst = false;
				return;
			}
		}
	}
}

void SaveBoard() {
    FILE* fp;
    errno_t err = fopen_s(&fp, "file1.txt", "wb");
    if (err) {
        MessageBox(GetHWnd(), L"存档失败", L"错误！", MB_OK);
        return;
    }
    fwrite(Process, sizeof(1), 83, fp);
    fclose(fp);
    MessageBox(GetHWnd(), L"存档成功", L"恭喜！", MB_OK);
}

void LoadBoard() {
    FILE* fp;
    errno_t err = fopen_s(&fp, "file1.txt", "rb");
    if (err) {
        MessageBox(GetHWnd(), L"读档失败", L"错误！", MB_OK);
        return;
    }
    fread(Process, sizeof(1), 83, fp);
    fclose(fp);
}

void drawPiece(int x, int y, int color) //绘制棋子
{
    Board[x][y] = color;
    if (color == -1)
    {
        setfillcolor(WHITE); //设置当前的填充颜色。 WHITE  0xFFFFFF 白色
    }
    else if (color == 1)
    {
        setfillcolor(BLACK); //黑色
    }
    solidcircle(y * grid_size + offsetx, x * grid_size + offsety, piece_size / 2); //用于画填充圆（无边框）
}

void drawPiece1(int x, int y, int color) //绘制棋子
{
	Board[x][y] = color;
    setlinestyle(PS_SOLID, 2);
	if (color == -1)
	{
        setlinecolor(RED);
		setfillcolor(WHITE); //设置当前的填充颜色。 WHITE  0xFFFFFF 白色
	}
	else if (color == 1)
	{
        setlinecolor(YELLOW);
		setfillcolor(BLACK); //黑色
	}
	fillcircle(y * grid_size + offsetx, x * grid_size + offsety, piece_size / 2); //用于画填充圆（无边框）
    mciSendString(_T("close bkmusic"), NULL, 0, NULL);
    mciSendString(_T("open drop.mp3 alias bkmusic"), NULL, 0, NULL);
    mciSendString(_T("play bkmusic"), NULL, 0, NULL);
    
}

void drawPiece2(int x, int y, int color) {
    setlinestyle(PS_SOLID, 2);
    if (color == 1)
    {
        setlinecolor(WHITE); //设置当前的填充颜色。 WHITE  0xFFFFFF 白色
    }
    else if (color == -1)
    {
        setlinecolor(BLACK); //黑色
    }
    circle(y * grid_size + offsetx, x * grid_size + offsety, piece_size/ 2);
}

inline void Store()
{
    memcpy(Initial_Avail, Avail, sizeof(Avail));
    memcpy(Initial_len, len, sizeof(len));
}
inline void Recover()
{
    memcpy(Avail, Initial_Avail, sizeof(Avail));
    memcpy(len, Initial_len, sizeof(len));
}

bool HaveAir(int x, int y, int Player) //判断点x，y与周围同色子是否有气
{
    if (x == -1 || y == -1 || x == 9 || y == 9 || Board[x][y] == -Player) //越界或者为另一色棋子——无气
        return 0;
    if (Board[x][y] == 0) //没子——有气
        return 1;
    Board[x][y] = -Player;//把走过的地方换成另一色棋子避免回头
    bool b = HaveAir(x, y - 1, Player) || HaveAir(x, y + 1, Player) //看这个子周围的棋子没有气
        || HaveAir(x - 1, y, Player) || HaveAir(x + 1, y, Player);
    Board[x][y] = Player;//复原棋盘
    return b;
}

bool Put_Available(int x, int y, int Player) //判断点x,y能否由player下子
{
    if (x < 0 || y < 0 || x > 8 || y > 8 || Board[x][y] != 0)//越界或者该点已经有子——不行
        return 0;
    bool b = 1;
    Board[x][y] = Player;//模拟放入该子的棋盘状况
    if (!HaveAir(x, y, Player) //自杀
        || (x + 1 <= 8 && Board[x + 1][y] == -Player && !HaveAir(x + 1, y, -Player))
        || (x - 1 >= 0 && Board[x - 1][y] == -Player && !HaveAir(x - 1, y, -Player))
        || (y + 1 <= 8 && Board[x][y + 1] == -Player && !HaveAir(x, y + 1, -Player))
        || (y - 1 >= 0 && Board[x][y - 1] == -Player && !HaveAir(x, y - 1, -Player))) //使对手无气
        b = 0;
    Board[x][y] = 0;//复原棋盘
    return b;
}

bool judge(int color)
{
	int right = 0;
	for (int x = 0; x < 9; ++x)
	{
		for (int y = 0; y < 9; ++y)
		{
			if (Put_Available(x, y, color))
			{
				return false;
			}
		}
	}
	return true;

}

bool isHukou(int x, int y, int Player) //判断某点是不是虎口
{
    if (Board[x][y] != 0) return false;//有子则不是
    int q = 0;
    int next_x, next_y;
    if (x - 1 >= 0 && Board[x - 1][y] == 0) { //周边四个位置依次判断，首先判断是否为有气点
        if (q == 0) { q++; next_x = x - 1; next_y = y; }//若是有气点，则判断是否是第一个有气点，若不是，则该点不是虎口
        else return 0;
    }
    else if (x - 1 >= 0 && Board[x - 1][y] == -Player)//另外如果旁边有对方子，也不是虎口
        return 0;

    if (y - 1 >= 0 && Board[x][y - 1] == 0) {
        if (q == 0) { q++; next_x = x; next_y = y - 1; }
        else return 0;
    }
    else if (y - 1 >= 0 && Board[x][y - 1] == -Player)
        return 0;

    if (x + 1 <= 8 && Board[x + 1][y] == 0) {
        if (q == 0) { q++; next_x = x + 1; next_y = y; }
        else return 0;
    }
    else if (x + 1 <= 8 && Board[x + 1][y] == -Player)
        return 0;

    if (y + 1 <= 8 && Board[x][y + 1] == 0) {
        if (q == 0) { q++; next_x = x; next_y = y + 1; }
        else return 0;
    }
    else if (y + 1 <= 8 && Board[x][y + 1] == -Player)
        return 0;
    return (q == 1 && Put_Available(next_x, next_y, Player));//满足只有一口气，且自己下一步能成眼
}
bool isYan(int x, int y, int Player)
{
    if (Board[x][y] == 0  //空点且我自己下在这之后有气
        && (x + 1 > 8 || Board[x + 1][y] == Player)
        && (x - 1 < 0 || Board[x - 1][y] == Player)
        && (y + 1 > 8 || Board[x][y + 1] == Player)
        && (y - 1 < 0 || Board[x][y - 1] == Player)) //旁边是棋盘边界或自己子
    {
        Board[x][y] = Player;//模拟
        if (HaveAir(x, y, Player)) {//我可以走在自己的眼处，才为真（此处无法判断有两个假眼连在一起的情况，应为只有一个真眼，但会认为有两个真眼）
            Board[x][y] = 0;//复原
            return true;
        }
        else {
            Board[x][y] = 0;//复原
            return false;
        }
    }
    else
        return false;
}

double Boardvalue(int color) {//第一种Boardvalue仅在正常计算的基础上阻碍对面做眼
    int N1 = 0, N2 = 0, N3 = 0;
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 9; y++) {
            if (!Put_Available(x, y, -color))
                N1++;
            if (!Put_Available(x, y, color))
                N2++;
            if (isYan(x, y, -color))
                N3++;
        }
    }
    return C1 * N1 + C2 * N2 + C3 * N3;
}

double Boardvalue1(int color) {//第二种Boardvalue同时阻碍对面做眼和使自己做眼
    int N1 = 0, N2 = 0, N3 = 0, N4 = 0;
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 9; y++) {
            if (!Put_Available(x, y, -color))
                N1++;
            if (!Put_Available(x, y, color))
                N2++;
            if (isYan(x, y, color))
                N3++;
            else if (isYan(x, y, -color))
                N4++;
        }
    }
    return D1 * N1 + D2 * N2 + D3 * N3 + D4 * N4;
}

int UCB1(int now) {
    int next_move = 0;
    if (!Node[now].Expanded) {//若没有完全拓展
        int* pos;
        int number = int(Player == 1);//将1/-1转化成1/0
        int tempAvail[81];
        int templen;
        templen = len[number];
        memcpy(tempAvail, Avail[number], sizeof(tempAvail));

        for (int i = 1; i <= Node[now].Num_of_ChildNode; i++) {//先在tempAvail中将已经访问过的节点删除
            int deleteAvail = Node[Node[now].Children[i]].x * 10 + Node[Node[now].Children[i]].y;
            pos = find(tempAvail, tempAvail + templen, deleteAvail);
            if (pos != tempAvail + templen)
            {
                *pos = tempAvail[templen - 1];
                templen--;
            }
        }
        while (1) {
            if (templen == 0 && Node[now].Num_of_ChildNode == 0)//我方没有落子点，且没有访问过的点
                return -1;
            else if (templen == 0 && Node[now].Num_of_ChildNode != 0)//我方没有落子点，且有访问过的点
            {
                Node[now].Expanded = 1;//拓展完毕
                goto choose_by_ucb;//用ucb选择
            }
            int select = rand() % templen;
            int sx = tempAvail[select] / 10;
            int sy = tempAvail[select] % 10;//在tempavail中随机选择一点
            if (Put_Available(sx, sy, Player))//若可走，就走这步
            {
                pos = find(Avail[1 - number], Avail[1 - number] + len[1 - number], tempAvail[select]);
                if (pos != Avail[1 - number] + len[1 - number])//将对面的这个点删掉。
                {
                    *pos = Avail[1 - number][len[1 - number] - 1];
                    len[1 - number]--;
                }
                next_move = tempAvail[select];//走这步
                break;
            }
            else
            {
                pos = find(Avail[number], Avail[number] + len[number], tempAvail[select]);
                *pos = Avail[number][len[number] - 1];//若不可走，就在我方的可走点中将该点删掉。
                len[number]--;
                tempAvail[select] = tempAvail[templen - 1];
                templen--;
            }
        }

        Node[now].Num_of_ChildNode++;
        _index++;
        Node[now].Children[Node[now].Num_of_ChildNode] = _index;//生成子结点
        Node[_index] = { next_move / 10, next_move % 10 };

        pos = find(Avail[number], Avail[number] + len[number], next_move);//在我方可走点中将此点删掉
        *pos = Avail[number][len[number] - 1];
        len[number]--;

        return _index;
    }
choose_by_ucb:
    double max = INT_MIN;
    int next_branch = 0;
    for (int i = 1; i <= Node[now].Num_of_ChildNode; i++)
    {
        int child__index = Node[now].Children[i];
        double value = Node[child__index].Value / Node[child__index].Times + S * sqrt(log(Node[now].Times) / Node[child__index].Times);
        if (value > max)//找出ucb值最大的子节点。
        {
            max = value;
            next_branch = child__index;
        }
    }
    return next_branch;
}

void CreateRoot() {//创建根节点
    Node[0] = { -1, -1 ,false,0 };
    len[1] = len[0] = 0;
    memset(Avail, 0, sizeof(Avail));
    for (int i = 0; i <= 8; i++)
        for (int j = 0; j <= 8; j++)
        {
            if (Put_Available(i, j, 1))//记录目前黑白的所有可走点。
                Avail[1][len[1]++] = i * 10 + j;
            if (Put_Available(i, j, -1))
                Avail[0][len[0]++] = i * 10 + j;
        }
}

void UCT() {//根据棋盘状态返回一个落子点
    int Trace[82];//记录搜索路径
    _index = 0;//下标归零
    CreateRoot();//创建根节点
    Store();//保存初始状态
    while (clock() - start < timeout) {
        int i = 1;
        int Initial_Branch = UCB1(0);//选择第一步
        int Branch;
        for (int t = 1; t <= constant; t++) {//从第二步开始连续进行十次（部分消除随机性带来的误判）
            i = 1;
            Branch = Initial_Branch;
            Trace[i] = Branch;//记录搜索路径
            Board[Node[Branch].x][Node[Branch].y] = Player;//下子
            Player *= -1;//变换持方
            while (!(len[int(Player == 1)] == 0) && i <= depth) {//对面没得下了，或者到达规定深度
                Branch = UCB1(Branch);
                if (Branch == -1) break;//终局(我没得下了)
                i++;
                Trace[i] = Branch;//记录搜索路径
                Board[Node[Branch].x][Node[Branch].y] = Player;//下子
                Player *= -1;//变换持方
            }
            double FinalValue;
            if (_switch)FinalValue = Boardvalue1(-Player);
            else FinalValue = Boardvalue(-Player);//计算最终节点的value
            for (int j = i; j > 0; j--) {
                Node[Trace[j]].Times++;
                Node[Trace[j]].Value += FinalValue;//backpropagation
                FinalValue *= -1;
                Board[Node[Trace[j]].x][Node[Trace[j]].y] = 0;//回溯棋盘状态
            }
            Node[0].Times++;
            Player = Initial_Player;
            Recover();
        }
    }
    double max = INT_MIN;
    int maxnode = 0;
    for (int i = 1; i <= Node[0].Num_of_ChildNode; i++)
    {
        int children = Node[0].Children[i];
        if (Node[children].Times != 0)
        {
            double childrenvalue = Node[children].Value / Node[children].Times;//选择平均value更大的子节点
            if (childrenvalue > max)
            {
                max = childrenvalue;
                maxnode = children;
            }
        }
    }
    Best_x = Node[maxnode].x;
    Best_y = Node[maxnode].y;
    Player = Initial_Player;
}

int AlphaBeta(int Player, int nAlpha, int nBeta)
{
    int tempAvail_x[81];
    int tempAvail_y[81];
    int num = 0;
    for (int i = 0; i <= 8; i++)
        for (int j = 0; j <= 8; j++)
            if (Put_Available(i, j, Player)) {
                tempAvail_x[num] = i;
                tempAvail_y[num] = j;
                num++;
            }
    if (num == 0) {
        if (Player == Initial_Player) return -1;
        else return 1;
    }
    if (Player == -Initial_Player) {
        int score = INT_MAX;
        for (int i = 0; i < num; i++)
        {
            Board[tempAvail_x[i]][tempAvail_y[i]] = Player;//模拟
            int temp_score = AlphaBeta(-Player, nAlpha, nBeta); //递归搜索子节点
            Board[tempAvail_x[i]][tempAvail_y[i]] = 0;         //复原
            if (temp_score < score)
                score = temp_score;
            if (score < nBeta)
                nBeta = score; //取极小值
            if (nAlpha >= nBeta)
                break; //alpha剪枝，抛弃后继节点
            if (clock() - start >= timeout)
                return nBeta;
        }
        return nBeta; //返回最小值
    }
    else {
        int score = INT_MIN;
        for (int i = 0; i < num; i++)
        {
            Board[tempAvail_x[i]][tempAvail_y[i]] = Player;    //模拟
            int temp_score = AlphaBeta(-Player, nAlpha, nBeta); //递归搜索子节点
            Board[tempAvail_x[i]][tempAvail_y[i]] = 0;  //复原
            if (temp_score > score)
                score = temp_score;
            if (score > nAlpha)
                nAlpha = score; //取极大值
            if (nAlpha >= nBeta)
                break; //nBeta剪枝，抛弃后继节点
            if (clock() - start >= timeout)
                return nAlpha;
        }
        return nAlpha; //返回最大值            
    }
}

void minmax() {
    int tempAvail_x[81];
    int tempAvail_y[81];
    int max_value = INT_MIN;
    for (int i = 0; i <= 8; i++)
        for (int j = 0; j <= 8; j++)
            if (Put_Available(i, j, Player)) {
                tempAvail_x[_index] = i;
                tempAvail_y[_index] = j;
                _index++;
            }
    for (int i = 0; i < _index; i++) {
        Board[tempAvail_x[i]][tempAvail_y[i]] = Player;
        int temp = AlphaBeta(-Player, INT_MIN, INT_MAX);
        if (max_value < temp)
        {
            max_value = temp;
            Best_x = tempAvail_x[i];
            Best_y = tempAvail_y[i];
        }
        Board[tempAvail_x[i]][tempAvail_y[i]] = 0;
    }
}

void AI()
{
    _index = 0;
    start = clock();
    timeout = (int)(0.98 * (double)CLOCKS_PER_SEC);
    memset(Node, 0, sizeof(Node));
    if (!PlayerFirst)
        Player = 1;
    else
        Player = -1;
    Initial_Player = Player;
    int available = 0;
    int num_of_hukou = 0;
    for (int i = 0; i <= 8; i++)
        for (int j = 0; j <= 8; j++) {
            available += Put_Available(i, j, Player);
            num_of_hukou += isHukou(i, j, Player);
        }
    if (available > 30) depth = 400 / available + 1;
    else depth = -2.0 / 13.0 * available + 225.0 / 13.0;
    if (num_of_hukou >= 1 && available < 55 && available > 20) {//若场上有虎口，更改估值函数
        _switch = true;
    }
    if (available > 8)
        UCT();
    else {
        minmax();
    }
}


void playerVSAI(int x, int y, int _turn) //人机对战
{
    int gamemode = 1;
    Process[0] = gamemode;
    Process[1] = int(PlayerFirst);
    int x1 = 0, y1 = 0;
	if (!PlayerFirst) {
        goto AImove;
	}	
    while (1)
    {
        ColumnShow();
        //PositionShow();
        switch (msg.uMsg)
        {
        case WM_LBUTTONDOWN:
            if (msg.x >= startx3 && msg.x < startx3 + textlength && msg.y >= starty3 && msg.y <= starty3 + textheight - blank) {
                SaveBoard();
            }
            if (msg.x >= startx3 && msg.x < startx3 + textlength && msg.y >= starty3 + textheight && msg.y <= starty3 + textheight * 2 - blank) {
                Board[Process[_turn] / 10][Process[_turn] % 10 - 1] = 0;
                Process[_turn--] = 0;
                Board[Process[_turn] / 10][Process[_turn] % 10 - 1] = 0;
                Process[_turn--] = 0;
                initView();
                initGame();
                int i;
                for (i = 2; i < 83; i++) {
                    if (Process[i] == 0) {
                        if (i != 2) drawPiece1(Process[i - 1] / 10, Process[i - 1] % 10 - 1, -_color);
                        break;
                    }
                    else {
                        drawPiece(Process[i] / 10, Process[i] % 10 - 1, _color);
                        _color *= -1;
                    }
                }
                PlayerFirst = true;
                if (i > 2)   playerVSAI(Process[i - 1] / 10, Process[i - 1] % 10 - 1, i - 1);
                else playerVSAI(-1, -1, 1);
            }
            if (msg.x >= startx3 && msg.x < startx3 + textlength && msg.y >= starty3 + textheight * 2 && msg.y <= starty3 + textheight * 3 - blank) {
                AI();
                if (x >= 0) {
                    drawPiece2(x, y, _color);
                }
                x = Best_x;
                y = Best_y;
                drawPiece1(x, y, _color);
                Process[++_turn] = x * 10 + y + 1;
                if (judge(-_color)) //分出胜负则退出
                {
                    if (_color == 1)
                    {
                        MessageBox(GetHWnd(), L"黑子胜出！(白方无处可下)", L"游戏结束：", MB_OK);
                    }
                    else
                    {
                        MessageBox(GetHWnd(), L"白子胜出！(黑方无处可下)", L"游戏结束：", MB_OK);
                    }
                    goto end;
                }
                //改变棋子颜色
                _color *= -1;

                goto AImove;
            }
            if (msg.x >= startx3 && msg.x < startx3 + textlength && msg.y >= starty3 + textheight * 3 && msg.y <= starty3 + textheight * 4 - blank) {
                goto end;
            }
            if (msg.x <= (map_width - 1) * grid_size + offsetx * 2) //左键按下
            {
                y1 = (msg.x - offsetx + grid_size / 2) / grid_size;
                x1 = (msg.y - offsety + grid_size / 2) / grid_size;
                if (x1 <= 8 && y1 <= 8 && Board[x1][y1] == 0) {
                    if (x >= 0) {
                        drawPiece2(x, y, _color);
                    }
                    x = x1; y = y1;
                    if (!Put_Available(x, y, _color)) {
                        if (_color == -1)
                        {
                            MessageBox(GetHWnd(), L"黑子胜出！(白方非法落子)", L"游戏结束：", MB_OK);
                        }
                        else
                        {
                            MessageBox(GetHWnd(), L"白子胜出！(黑方非法落子)", L"游戏结束：", MB_OK);
                        }
                        goto end;
                    }
                    else {
                        drawPiece1(x, y, _color);
                        Process[++_turn] = x * 10 + y + 1;
                        if (judge(-_color)) //分出胜负则退出
                        {
                            if (_color == 1)
                            {
                                MessageBox(GetHWnd(), L"黑子胜出！(白方无处可下)", L"游戏结束：", MB_OK);
                            }
                            else
                            {
                                MessageBox(GetHWnd(), L"白子胜出！(黑方无处可下)", L"游戏结束：", MB_OK);
                            }
                            goto end;
                        }
                    }
                    //改变棋子颜色
                    _color *= -1;
                    //AI行动
                AImove:                 
                    AI();
                    if (x >= 0) {
                        drawPiece2(x, y, _color);
                    }
                    x = Best_x;
                    y = Best_y;
                    drawPiece1(x, y, _color);
                    Process[++_turn] = x * 10 + y + 1;
                    if (judge(-_color)) //分出胜负则退出
                    {
                        if (_color == 1)
                        {
                            MessageBox(GetHWnd(), L"黑子胜出！(白方无处可下)", L"游戏结束：", MB_OK);
                        }
                        else
                        {
                            MessageBox(GetHWnd(), L"白子胜出！(黑方无处可下)", L"游戏结束：", MB_OK);
                        }
                        goto end;
                    }
                    //改变棋子颜色
                    _color *= -1;
                }
            }
        }
    }
end:closegraph();
}

void playerVSplayer(int x, int y, int _turn) //人人对战
{
    int gamemode = 0;
    Process[0] = gamemode;
    int x1 = 0, y1 = 0;
    while (1)
    {
        ColumnShow();
        switch (msg.uMsg)
        {
        case WM_LBUTTONDOWN:
            if (msg.x >= startx3 && msg.x < startx3 + textlength && msg.y >= starty3 && msg.y <= starty3 + textheight - blank) {
                SaveBoard();
            }
            if (msg.x >= startx3 && msg.x < startx3 + textlength && msg.y >= starty3 + textheight && msg.y <= starty3 + textheight * 2 - blank) {
                Board[Process[_turn] / 10][Process[_turn] % 10 - 1] = 0;
                Process[_turn--] = 0;
                initView();
                initGame();
                int i;
                for (i = 2; i < 83; i++) {
                    if (Process[i] == 0) {
                        if (i != 2) drawPiece1(Process[i - 1] / 10, Process[i - 1] % 10 - 1, -_color);
                        break;
                    }
                    else {
                        drawPiece(Process[i] / 10, Process[i] % 10 - 1, _color);
                        _color *= -1;
                    }
                }
                if (i > 2)   playerVSplayer(Process[i - 1] / 10, Process[i - 1] % 10 - 1, i - 1);
                else playerVSplayer(-1, -1, 1);
            }
            if (msg.x >= startx3 && msg.x < startx3 + textlength && msg.y >= starty3 + textheight * 2 && msg.y <= starty3 + textheight * 3 - blank) {
                AI();
                if (x >= 0) {
                    drawPiece2(x, y, _color);
                }
                x = Best_x;
                y = Best_y;
                drawPiece1(x, y, _color);
                Process[++_turn] = x * 10 + y + 1;
                if (judge(-_color)) //分出胜负则退出
                {
                    if (_color == 1)
                    {
                        MessageBox(GetHWnd(), L"黑子胜出！(白方非法落子)", L"游戏结束：", MB_OK);
                    }
                    else
                    {
                        MessageBox(GetHWnd(), L"白子胜出！(黑方无处可下)", L"游戏结束：", MB_OK);
                    }
                    goto end;
                }
                //改变棋子颜色
                _color *= -1;
            }
            if (msg.x >= startx3 && msg.x < startx3 + textlength && msg.y >= starty3 + textheight * 3 && msg.y <= starty3 + textheight * 4 - blank) {
                goto end;
            }
            if (msg.x <= (map_width - 1) * grid_size + offsetx * 2) 
            {
                y1 = (msg.x - offsetx + grid_size / 2) / grid_size;
                x1 = (msg.y - offsety + grid_size / 2) / grid_size;
                if (x1 <= 8 && y1<= 8 && Board[x1][y1] == 0) {
                    if (x >= 0) {
                        drawPiece2(x, y, _color);
                    }
                    x = x1; y = y1;
                    if (!Put_Available(x, y, _color)) {
                        if (_color == -1)
                        {
                            MessageBox(GetHWnd(), L"黑子胜出！(白方非法落子)", L"游戏结束：", MB_OK);
                        }
                        else
                        {
                            MessageBox(GetHWnd(), L"白子胜出！(黑方非法落子)", L"游戏结束：", MB_OK);
                        }
                        goto end;
                    }
                    else {
                        drawPiece1(x, y, _color);
                        Process[++_turn] = x * 10 + y + 1;
                        if (judge(-_color)) //分出胜负则退出
                        {
                            if (_color == 1)
                            {
                                MessageBox(GetHWnd(), L"黑子胜出！(白方无处可下)", L"游戏结束：", MB_OK);
                            }
                            else
                            {
                                MessageBox(GetHWnd(), L"白子胜出！(黑方无处可下)", L"游戏结束：", MB_OK);
                            }
                            goto end;
                        }
                    }
                    //改变棋子颜色
                    _color *= -1;
                }
            }
        }
    }
end:closegraph();
}

void LoadGame() {
    int i;
    
    for (i = 2; i < 83; i++) {
        if (Process[i] == 0) {
            if(i != 2) drawPiece1(Process[i - 1] / 10, Process[i - 1] % 10 - 1, -_color);
            break;
        }
        else {
            drawPiece(Process[i] / 10, Process[i] % 10 - 1, _color);
            _color *= -1;
        }
    }
    PlayerFirst = true;
    if (Process[0] == 1)
        playerVSAI(Process[i - 1] / 10, Process[i - 1] % 10 - 1, i - 1);
    else
        playerVSplayer(Process[i - 1] / 10, Process[i - 1] % 10 - 1, i - 1);
}

int main()
{
	InitPage(); //初始化页面
	while (1)
	{
        int Process[81] = { 0 };
		ModelShow(); //模式显示
		switch (msg.uMsg)
		{
		case WM_LBUTTONDOWN: //鼠标左键按下
			if (msg.x >= startx && msg.x < startx + textlength && msg.y >= starty && msg.y <= starty + textheight - blank) //在人机对战区域按下
			{
				DecideWhoFirst();//决定先手
                initGame(); //游戏初始化
                initView(); //页面初始化
				playerVSAI(-1, -1, 1); //人机对战
				InitPage(); //重新显示页面
				break;
			}
			if (msg.x >= startx && msg.x < startx + textlength && msg.y >= starty + textheight && msg.y <= starty + textheight * 2 - blank) //在人人对战区域按下
			{
                initGame(); //游戏初始化
                initView(); //页面初始化
                playerVSplayer(-1, -1, 1); //人人对战
				InitPage(); //重新显示页面
				break;
			}
            if (msg.x >= startx && msg.x < startx + textlength && msg.y >= starty + textheight * 2 && msg.y <= starty + textheight * 3 - blank) {
                initGame(); //游戏初始化
                initView(); //页面初始化
                LoadBoard();
                LoadGame();
                InitPage();
                break;
            }
			if (msg.x >= startx && msg.x < startx + textlength && msg.y >= starty + textheight * 3 && msg.y <= starty + textheight * 4 - blank) //在退出游戏区域按下
			{
				return 0;
			}
		}
	}
}