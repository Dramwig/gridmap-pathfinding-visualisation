#pragma once

#include <cstdio>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <graphics.h>
#include "controlbar.h"
#include <windows.h>
#include <time.h>

//可调参数
#define SLEEP_TIEM 0 //输出查询点和路径的间隔时间ms，取0为直接出图

const COLORREF COLOR_WALL = COLOR_BLACK;
const COLORREF COLOR_EMPTY = COLOR_WHITE;

class Point {
private:
    int id_;
public:
    Point() : id_(-1) {}
    Point(int id) : id_(id) {
        printf("create Point %d.\n", id_);
    }
    virtual ~Point() {
        printf("clear Point %d.\n", id_);
    }
    int GetID() const {
        return id_;
    }
};

class Node;

class Path {
    friend class Node;
private:
    Node* to_;
    Path* next_;
    int val_;
public:
    Path(Node* to, Path* next = nullptr, int val = 1) : to_(to), next_(next), val_(val) {
        if (val < 0) {
            printf("[WAR]there are negative val edge!\n");
        }
        printf("!!!\n");
        printf("create edge val %d.\n",  val_);
    }
    ~Path() {
        printf("clear edge val %d.\n",  val_);
    }
    Path* Next() const {
        return next_;
    }
    int Val() const {
        return val_;
    }
    Node* GoTo() const {
        return to_;
    }
};

// 空 墙
enum NodeType {EMPTY, WALL ,SELECT};

class Node : public Point {
    friend class Path;
private:
    Path* edge_head_; // head of edge list
public:  
    NodeType type;
    
    class Iterator {
    private:
        Path* current_;
    public:
        Iterator(Path* path) : current_(path) {}
        bool operator!=(const Iterator& other) const {
            return current_ != other.current_;
        }
        Iterator& operator++() {
            current_ = current_->Next();
            return *this;
        }
        Path& operator*() const {
            return *current_;
        }
    };
    Iterator begin() {
        return Iterator(edge_head_);
    }
    Iterator end() {
        return Iterator(nullptr);
    }

    Node() {};
    Node(int id) : Point(id), type(EMPTY), edge_head_(nullptr){
        printf("create Node %d type %d, init.\n", GetID(), EMPTY);
    }
    Node(int id, Node e) : Point(id), type(e.type), edge_head_(e.edge_head_) {
		printf("create Node %d type %d, copy.\n", GetID(), type);
	}
    ~Node() {
        for (Path* p = edge_head_; p != nullptr; ) {
            Path* tmp = p;
            p = p->Next();
            delete tmp;
            if (p == nullptr) {
                break;
            }
        }
        printf("clear Node %d.\n", GetID());
    }

    void AddEdge(Node* to, int val) {
        if (edge_head_ == nullptr) {
            edge_head_ = new Path(to, nullptr, val);
        }
        else {
            edge_head_ = new Path(to, edge_head_, val);
        }
        printf("add edge %d to %d, val %d.\n", GetID(), to->GetID(), val);
    }
    void PrintEdge() {
        int cnt = 0;
        for(auto& e : *this) {
            printf("%d->%d, val %d\n", GetID(), e.GoTo()->GetID(), e.Val());
            ++cnt;
        }
        printf(". totally %d edges.\n", cnt);
    }
    
};


const int maxn = 10000 + 10;

int backwardIndexing[maxn]; // 反向索引
std::vector<int> accessTracks; // 访问轨迹
int dist[maxn]; // 距离数组
int vis[maxn]; // 访问数组


class GridMap {

private:
    int x, y, width, height;
    double iWidth, iHeight;
    int n_, m_;
    std::vector<Node> ref_p_;
    Node *start_, *end_;
protected:
    int GetMark(int x, int y) const {
        return (x - 1) * m_ + y;
    }
    void ReMark(int& x, int& y, int id) {
        x = (id - 1) / m_ + 1;
        y = (id - 1) % m_ + 1;
    }
    void BuildEdge(Node& u, Node& v, int w = 1) {
        u.AddEdge(&v, w);
    }
    void BuildEdge(GridMap& R, int p1, int p2, int w = 1) {
        BuildEdge(AskRef(p1), AskRef(p2), w);
    }
    void BuildEdge(GridMap& R, int x1, int y1, int x2, int y2, int w = 1) {
        BuildEdge(AskRef(x1, y1), AskRef(x2, y2), w);
    }
public:
    void ReSize(int n, int m) { // 重置地图大小
        ref_p_.resize(GetMark(max(n,n_), max(m,m_) )+ 1);
        for(int p = GetMark(n_, m_) + 1; p <= GetMark(n, m); ++p) 
			ref_p_[p] = Node(p);		
        // 移动结点
        if(m>=m_)
            for (int i = n; i >=1; --i)
                for (int j = m; j >= 1; --j)
                    ref_p_[(i - 1) * m + j] = Node((i - 1) * m + j,ref_p_[(i - 1) * m_ + j]);
        else
            for (int i = 1; i <= n; ++i)
                for (int j = 1; j <= m; ++j)
                    ref_p_[(i - 1) * m + j] = Node((i - 1) * m + j, ref_p_[(i - 1) * m_ + j]);
        // 移动起点
        int x, y;
        if (start_) {
            ReMark(x, y, start_->GetID());
            if (x <= n && y <= m)
                start_ = &ref_p_[(x - 1) * m + y];
            else
                start_ = NULL;
        }
        // 移动终点
        if (end_) {
            ReMark(x, y, end_->GetID());
            if (x <= n && y <= m)
                end_ = &ref_p_[(x - 1) * m + y];
            else
                end_ = NULL;
        }
        ref_p_.resize(GetMark(n, m) + 1);
        // 更新数据
		n_ = n;
		m_ = m;
        iWidth = (double)width / n_;
        iHeight = (double)height / m_;
		printf("resize ref %d*%d, init.\n", n_, m_);
        draw();
	}
    void AddN() { ReSize(n_ + 1, m_); } // 增加行
    void LesN() { if(n_ > 1) ReSize(n_ - 1, m_); } // 减少行
    void AddM() { ReSize(n_, m_ + 1); } // 增加列
    void LesM() { if(m_ > 1) ReSize(n_, m_ - 1); } // 减少列
    bool nullcheck() {
        if(!start_ || !end_)
        {
			printf("[WAR]start or end is null!\n");
			return true;
		}
        return false;
    }
    GridMap(int n, int m, int x, int y, int width, int height) : n_(n), m_(m), x(x), y(y), width(width), height(height), start_(NULL), end_(NULL), iWidth(1.0 * width / n), iHeight(1.0 * height / m){
        ref_p_.resize(GetMark(n,m) + 1);
        for (int p = GetMark(1, 1); p <= GetMark(n, m); ++p) {
            ref_p_[p] = Node(p);
        }
        printf("create ref %d*%d, init.\n", n_, m_);
    }

    ~GridMap() { printf("clear ref.\n"); }

    Node& AskRef(int p) {
        if (p < 1 || p > n_ * m_) {
            printf("[ERR]AskRef %d error!\n",p);
            exit(0);
        }
        return ref_p_[p];
    }
    Node& AskRef(int x, int y) {
        return AskRef(GetMark(x, y));
    }
    
    void draw() { // 绘制地图
        // 开始缓冲绘制
        BeginBatchDraw();

        setlinecolor(COLOR_BLACK);
        for (int i = 1; i <= n_; i++)
        {
            for (int j = 1; j <= m_; j++) {
                switch(AskRef(i, j).type)
                {
                case WALL:
                    setfillcolor(COLOR_WALL);
                    break;
                case EMPTY:
                    setfillcolor(COLOR_EMPTY);
                    break;
                case SELECT:
                    setfillcolor(COLOR_GRAY);
                    break;
                default:
                    setfillcolor(COLOR_RED);
                    break;
                }
                fillrectangle(x + iWidth*(i-1), y + iHeight*(j-1), x + iWidth*i , y + iHeight*j);
			}
		}

        settextstyle(16, 8, _T("Courier"));	// 设置字体
        if (start_ != NULL) {
            int p = start_->GetID();
            int i, j;
            ReMark(i, j, p);
            RECT r = { x + iWidth * (i - 1), y + iHeight * (j - 1), x + iWidth * i , y + iHeight * j };
            drawtext(_T("S"), &r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        }
        if (end_ != NULL) {
            int p = end_->GetID();
            int i,j;
            ReMark(i, j, p);
            RECT r = { x + iWidth * (i - 1), y + iHeight * (j - 1), x + iWidth * i , y + iHeight * j };
            drawtext(_T("E"), &r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        }

        // 结束缓冲绘制
        EndBatchDraw();

        // 绘制缓冲区内容到屏幕上
        FlushBatchDraw();
    }
    void draw(std::queue<int> &q){ // 绘制地图
        setlinecolor(COLOR_BLACK);
        while(!q.empty()){
            int p = q.front();
            q.pop();
            if (p > n_ * m_) continue;
            switch(AskRef(p).type)
            {
            case WALL:
                setfillcolor(COLOR_WALL);
                break;
            case EMPTY:
                setfillcolor(COLOR_EMPTY);
                break;
            case SELECT:
                setfillcolor(COLOR_GRAY);
                break;
            default:
                setfillcolor(COLOR_RED);
                break;
            }
            int i, j;
            ReMark(i, j, p);
            fillrectangle(x + iWidth*(i-1), y + iHeight*(j-1), x + iWidth*i , y + iHeight*j);
            if(start_!=NULL && start_->GetID() == p){
                RECT r = {x + iWidth * (i - 1), y + iHeight * (j - 1), x + iWidth * i , y + iHeight * j};
                settextstyle(16, 8, _T("Courier"));	// 设置字体
                drawtext(_T("S"), &r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            } else if (end_ != NULL && end_->GetID() == p) {
                RECT r = { x + iWidth * (i - 1), y + iHeight * (j - 1), x + iWidth * i , y + iHeight * j };
                settextstyle(16, 8, _T("Courier"));	// 设置字体
                drawtext(_T("E"), &r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            }

        }
    }

    void reset() { // 重置地图
        for (int p = GetMark(1, 1); p <= GetMark(n_, m_); ++p) {
            AskRef(p).type = EMPTY;
        }
        start_ = NULL;
        end_ = NULL;
        printf("reset the Gridmap.\n");
        draw();
    }

    int GetSelectNode(int mx, int my) { // 获取鼠标所在结点
        if(mx>=x && mx<x+width && my>=y && my<y+height) {
            //printf("GetSelectNode %d<%d %d<%d\n", mx, x + width, my, y + height);
            return GetMark((mx-x)/iWidth+1, (my-y)/iHeight+1);
        } else {
            return -1;
        }
    }

    void UpdateNodeType(int mx, int my, bool isLDown, bool isLPress,bool isRDown, bool isRPress, bool isSRress, bool isERress) { // 更新结点类型
        static int pr = -1;
        int p = GetSelectNode(mx, my);
        //printf("%d\n", p);
        std::queue<int> q;
        if ( pr != -1 && isSRress && (!start_ || start_ && start_->GetID() != pr )) { //按下S键
            if(start_)
                q.push(start_->GetID());
            start_ = &AskRef(pr);
            q.push(pr);
            printf("select start node.\n");
        }
        else if (pr != -1 && isERress && (!end_ || end_ && end_->GetID() != pr)) { //按下E键
            if(end_)
                q.push(end_->GetID());
            end_ = &AskRef(pr);
            q.push(pr);
            printf("select end node.\n");
        }
        if (p != -1) { // 鼠标在地图上
            if (isLDown || isLPress) {  // 按下左键
                if (AskRef(p).type != WALL)
                    printf("set a wall.\n");
                AskRef(p).type = WALL;
                q.push(p);
                pr = -1;
            }
            else if (isRDown || isRPress) { // 按下右键
                if (AskRef(p).type == WALL)
                    printf("delate a wall.\n");
                AskRef(p).type = EMPTY;
                q.push(p);
            }
            else if (p != pr) { // 悬空移动
                if (pr != -1) {
                    AskRef(pr).type = EMPTY;
                    q.push(pr);
                    //printf("pr:%d\n", pr);
                }
                if (AskRef(p).type == EMPTY) {
                    AskRef(p).type = SELECT;
                    q.push(p);
                    pr = p;
                    //printf("pr:%d\n", pr);
                }
            }
        } else if (pr != -1) { //鼠标不在地图上
            AskRef(pr).type = EMPTY;
            q.push(pr);
            pr = -1;
        }
        draw(q);
    }

    // 寻路算法部分

    void init() { // 寻路算法参数初始化
        memset(backwardIndexing, 0, sizeof(backwardIndexing));
        accessTracks.clear();
        memset(dist, 0x3f, sizeof(dist));
        dist[start_->GetID()] = 0;
        memset(vis, 0, sizeof(vis));
    }

    bool constraint_function(int x, int y) // 约束条件函数
    {
        // 判断状态 state 是否满足约束条件
        // 如果满足，返回 true；否则，返回 false。
        if (x > n_ || x < 1 || y >m_ || y < 1)
            return false;
        if (AskRef(x, y).type == WALL)
            return false;
        return true;
    }


    bool is_goal_state(int x, int y) // 目标状态判断函数
    {
        // 判断状态 state 是否为目标状态
        // 如果是，返回 true；否则，返回 false。
        if (end_->GetID() == AskRef(x, y).GetID())
            return true;
        else
            return false;
    }

    int heuristic_function(int x, int y) // 启发函数
    {
        // 估计从状态 state 到目标状态的距离
        // 返回估计距离的值
        int p = end_->GetID();
        int i = (p - 1) / m_ + 1;
        int j = (p - 1) % m_ + 1;
        return abs(x - i) + abs(y - j); // 曼哈顿距离
    }

    const int dx[4] = { 0, 0, 1, -1 }, dy[4] = { 1, -1, 0, 0 };

    int dfs_flag;
    void dfs(int x, int y) {
        if (dfs_flag) return;
        accessTracks.push_back(GetMark(x, y));
        if (is_goal_state(x, y)) {
            dfs_flag = 1;
            return;
        }
        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i], ny = y + dy[i];
            if (constraint_function(nx, ny) && !vis[GetMark(nx, ny)]) {
                vis[GetMark(nx, ny)] = 1;
                backwardIndexing[GetMark(nx, ny)] = GetMark(x, y);
                dfs(nx, ny);
            }
        }
    }

    void dfs() {
        int x, y;
        dfs_flag = 0;
        ReMark(x, y, start_->GetID());
        vis[start_->GetID()] = 1;
        dfs(x, y);
    }

    void bfs() {
        std::queue<int> q;
        q.push(start_->GetID());
        vis[start_->GetID()] = 1;
        while (!q.empty()) {
            int u = q.front(); q.pop(); 
            int x, y;
            ReMark(x, y, u);
            accessTracks.push_back(GetMark(x, y));
            if (is_goal_state(x, y)) {
                return;
            }
            for (int i = 0; i < 4; i++) {
                int nx = x + dx[i], ny = y + dy[i];
                if (constraint_function(nx, ny) && !vis[GetMark(nx, ny)]){
                    vis[GetMark(nx, ny)] = 1;
                    backwardIndexing[GetMark(nx, ny)] = u;
                    q.push(GetMark(nx, ny));
                }
            }
        }
    }

    void SPFA() {
        std::queue<int> q;
        q.push(start_->GetID());
        vis[start_->GetID()] = 1;
        while (!q.empty()) {
            int u = q.front(); q.pop(); vis[u] = 0;
            int x, y;
            ReMark(x, y, u);
            accessTracks.push_back(GetMark(x, y));
            if (is_goal_state(x, y)) {
                return;
            }
            for (int i = 0; i < 4; i++) {
                int nx = x + dx[i], ny = y + dy[i];
                //printf("(%d,%d)%d ->(%d,%d) %d %d\n", x, y, dist[u], nx, ny, dist[GetMark(nx, ny)], constraint_function(nx, ny));
                if (constraint_function(nx, ny) && dist[GetMark(nx, ny)] > dist[u] + 1) {
                    dist[GetMark(nx, ny)] = dist[u] + 1;
                    backwardIndexing[GetMark(nx, ny)] = u;
                    if (!vis[GetMark(nx, ny)]) {
                        vis[GetMark(nx, ny)] = 1;
                        q.push(GetMark(nx, ny));
                    }
                }
            }
        }
    }


    void Dijsktra() {
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int> >, std::greater<std::pair<int, int> > > q;
        q.push(std::make_pair(0, start_->GetID()));
        while (!q.empty()) {
            int u = q.top().second; q.pop();
            if (vis[u]) continue;
            vis[u] = 1;
            int x, y;
            ReMark(x, y, u);
            accessTracks.push_back(GetMark(x, y));
            if (is_goal_state(x, y)) {
                return;
            }
            for (int i = 0; i < 4; i++) {
                int nx = x + dx[i], ny = y + dy[i];
                if (constraint_function(nx, ny) && dist[GetMark(nx, ny)] > dist[u] + 1) {
                    dist[GetMark(nx, ny)] = dist[u] + 1;
                    backwardIndexing[GetMark(nx, ny)] = u;
                    q.push(std::make_pair(dist[GetMark(nx, ny)], GetMark(nx, ny)));
                }
            }
        }
    }

    void Astar() {
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int> >, std::greater<std::pair<int, int> > > q;
        q.push(std::make_pair(0, start_->GetID()));
        while (!q.empty()) {
            int u = q.top().second; q.pop();
            if (vis[u]) continue;
            vis[u] = 1;
            int x, y;
            ReMark(x, y, u);
            accessTracks.push_back(GetMark(x, y));
            if (is_goal_state(x, y)) {
                return;
            }
            for (int i = 0; i < 4; i++) {
                int nx = x + dx[i], ny = y + dy[i];
                if (constraint_function(nx, ny) && dist[GetMark(nx, ny)] > dist[u] + 1) {
                    dist[GetMark(nx, ny)] = dist[u] + 1;
                    backwardIndexing[GetMark(nx, ny)] = u;
                    q.push(std::make_pair(dist[GetMark(nx, ny)] + heuristic_function(nx, ny), GetMark(nx, ny)));
                }
            }
        }
    }

    void DrawMarkNode(int p, COLORREF col) {
        int i, j;
        ReMark(i, j, p);
        setlinecolor(COLOR_BLACK); 
        setfillcolor(col);
        fillrectangle(x + iWidth * (i - 1), y + iHeight * (j - 1), x + iWidth * i, y + iHeight * j);
        if (start_ != NULL && start_->GetID() == p) {
            RECT r = { x + iWidth * (i - 1), y + iHeight * (j - 1), x + iWidth * i , y + iHeight * j };
            settextstyle(16, 8, _T("Courier"));	// 设置字体
            drawtext(_T("S"), &r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        }
        else if (end_ != NULL && end_->GetID() == p) {
            RECT r = { x + iWidth * (i - 1), y + iHeight * (j - 1), x + iWidth * i , y + iHeight * j };
            settextstyle(16, 8, _T("Courier"));	// 设置字体
            drawtext(_T("E"), &r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        }
    }

    COLORREF GetColor(double q) {
        q = 1 - q;
        int r1 = 17, g1 = 0, b1 = 158;
        int r2 = 196, g2 = 176, b2 = 255;
        return RGB( q * r1 + (1 - q) * r2, q * g1 + (1 - q) * g2, q * b1 + (1 - q) * b2);

    }

    void DrawPath() { // 绘制搜索序轨迹
        int cnt = 0;
        int s = accessTracks.size();
        for (auto it : accessTracks) {
            cnt++;
            DrawMarkNode(it, GetColor(1.0*cnt/s));
            Sleep(SLEEP_TIEM);
        }
    }
    void DrawAccess() { // 绘制最终路径
        for (int p = end_->GetID(); p; p = backwardIndexing[p])// 从终点开始回溯
        {
            DrawMarkNode(p, COLOR_GREEN);
            Sleep(SLEEP_TIEM);
        }   
    }

};



