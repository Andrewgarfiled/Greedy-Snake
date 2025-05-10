#include <windows.h>
#include <graphics.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

// 便于统一更改
#define WIDTH 800
#define HEIGHT 600
#define BLOCK_SIZE 20//固定的标准宽度
#define MAX_SNAKE_LENGTH 100
#define SPEED_UP_DURATION 5000  
#define NORMAL_SPEED 100        
#define SPEED_UP_SPEED 50       
#define FOOD_SCORE 10           
#define SPEED_UP_ITEM_SCORE 30  

struct SnakeNode {
    int x;
    int y;
};

struct Food {
    int x;
    int y;
};

struct SpeedUpItem {//加速道具
    int x;
    int y;
    bool active;
};


struct Portal {//传送门
    SnakeNode portal1;
    SnakeNode portal2;
};


void drawRectangle(int x, int y, COLORREF color) {
    setfillcolor(color);
    fillrectangle(x, y, x + BLOCK_SIZE, y + BLOCK_SIZE);
}


void initSnake(SnakeNode snake[], int& length) {//所有的初始化
    length = 1;
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;
}
void initFood(Food& food) {
    srand(static_cast<unsigned>(time(nullptr)));
    food.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
    food.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
}

void initSpeedUpItem(SpeedUpItem& speedUpItem) {
    speedUpItem.active = false;
}

void initPortal(Portal& portal) {   //传送门
    portal.portal1.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
    portal.portal1.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
    portal.portal2.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
    portal.portal2.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
}

// 绘制
void draw(SnakeNode snake[], int length, const Food& food, const SpeedUpItem& speedUpItem, const Portal& portal, int score) {
    cleardevice();

    for (int i = 0; i < length; i++) {
        drawRectangle(snake[i].x, snake[i].y, GREEN);
    }

    drawRectangle(food.x, food.y, RED);

    if (speedUpItem.active) {
        drawRectangle(speedUpItem.x, speedUpItem.y, YELLOW);
    }

    drawRectangle(portal.portal1.x, portal.portal1.y, CYAN);
    drawRectangle(portal.portal2.x, portal.portal2.y, CYAN);
}


void moveSnake(SnakeNode snake[], int& length, Food& food, SpeedUpItem& speedUpItem, Portal& portal, int& direction, int& currentSpeed, DWORD& speedUpStartTime, int& score) {
    SnakeNode head = snake[0];
    switch (direction) {
    case 0: // 上
        head.y -= BLOCK_SIZE;
        break;
    case 1: // 下
        head.y += BLOCK_SIZE;
        break;
    case 2: // 左
        head.x -= BLOCK_SIZE;
        break;
    case 3: // 右
        head.x += BLOCK_SIZE;
        break;
    }

    // 检查传送门
    if (head.x == portal.portal1.x && head.y == portal.portal1.y) {
        head = portal.portal2;
    }
    else if (head.x == portal.portal2.x && head.y == portal.portal2.y) {
        head = portal.portal1;
    }

    if (head.x == food.x && head.y == food.y) { // 检测碰撞，蛇头与食物的重叠即碰撞
        if (length < MAX_SNAKE_LENGTH) { // 注意限制最大长度
            for (int i = length; i > 0; i--) {
                snake[i] = snake[i - 1];
            }
            snake[0] = head;
            length++;
        }

        food.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE; // 随机生成且永远与蛇保持齐平的食物
        food.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;

        score += FOOD_SCORE; // 吃到食物增加得分

        if (rand() % 3 == 0 && !speedUpItem.active) {
            speedUpItem.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
            speedUpItem.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
            speedUpItem.active = true;
        }
    }
    else {
        for (int i = length - 1; i > 0; i--) {
            snake[i] = snake[i - 1];
        }
        snake[0] = head; // 只移动不加长
    }

    // 判断是否吃到加速道具
    if (speedUpItem.active && head.x == speedUpItem.x && head.y == speedUpItem.y) {
        speedUpItem.active = false;
        currentSpeed = SPEED_UP_SPEED;
        speedUpStartTime = GetTickCount();
        score += SPEED_UP_ITEM_SCORE; // 吃到加速道具增加更高得分
    }

    // 检查加速时间是否结束
    if (currentSpeed == SPEED_UP_SPEED && GetTickCount() - speedUpStartTime >= SPEED_UP_DURATION) {
        currentSpeed = NORMAL_SPEED;
    }
}

// 判断游戏是否结束
bool isGameOver(SnakeNode snake[], int length) {
    SnakeNode head = snake[0];
    if (head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT) {
        return true;
    }
    for (int i = 1; i < length; i++) {
        if (snake[i].x == head.x && snake[i].y == head.y) { // 蛇头与蛇身重叠
            return true;
        }
    }
    return false;
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PSTR szCmdLine,
    int iCmdShow 
) {
    SnakeNode snake[MAX_SNAKE_LENGTH];
    int length;
    Food food;
    SpeedUpItem speedUpItem;
    Portal portal;
    int direction = 3;
    int currentSpeed = NORMAL_SPEED;
    DWORD speedUpStartTime = 0;
    int score = 0;

    initSnake(snake, length);
    initFood(food);
    initSpeedUpItem(speedUpItem);
    initPortal(portal);

    if (initgraph(WIDTH, HEIGHT) == NULL) {
        std::cerr << "Failed to initialize graphics mode." << std::endl;
        return 1;
    }
    setbkcolor(BLACK);
    cleardevice();
    int sleep = 0;
    while (true) { // 主循环，不断绘制，检测按键，移动，直到break
        // 键盘的获取 windows api
        if (GetAsyncKeyState(VK_UP) & 0x8000 && direction != 1) { // 使用 windows api的函数检测按键是否被按下，同时注意避免反向操作
            direction = 0;
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 && direction != 0) {
            direction = 1;
        }
        if (GetAsyncKeyState(VK_LEFT) & 0x8000 && direction != 3) {
            direction = 2;
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && direction != 2) {
            direction = 3;
        }

        if (sleep == currentSpeed) {
            sleep = 0;
            moveSnake(snake, length, food, speedUpItem, portal, direction, currentSpeed, speedUpStartTime, score);

            if (isGameOver(snake, length)) {
                break;
            }
            draw(snake, length, food, speedUpItem, portal, score);
        }

        Sleep(10);
        sleep += 10;
    }

    closegraph(); // 结束图形模式，恢复到控制台模式，对应initgraph
    std::cout << "Game Over! Your Score: " << score << std::endl;
    wchar_t message[100];
    swprintf_s(message, L"游戏结束!\n你的分数: %d", score);
    MessageBoxW(NULL, message, L"游戏结束", MB_OK | MB_ICONINFORMATION);
    return 0;
}