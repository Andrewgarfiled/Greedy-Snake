#include <windows.h>
#include <graphics.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
//预处理，编译时自动替换
#define WIDTH 800
#define HEIGHT 600
#define BLOCK_SIZE 20
#define MAX_SNAKE_LENGTH 100

struct SnakeNode {
    int x;
    int y;
};

struct Food {
    int x;
    int y;
};

void init(SnakeNode snake[], int& length, Food& food) {
    
    length = 1;
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;

    
    srand((unsigned)time(NULL));
    food.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
    food.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;

    
    initgraph(WIDTH, HEIGHT);
    setbkcolor(BLACK);
    cleardevice();
}


void draw(SnakeNode snake[], int length, const Food& food) {
    cleardevice();
    
    for (int i = 0; i < length; i++) {
        setfillcolor(GREEN);
        fillrectangle(snake[i].x, snake[i].y, snake[i].x + BLOCK_SIZE, snake[i].y + BLOCK_SIZE);
    }
    
    setfillcolor(RED);
    fillrectangle(food.x, food.y, food.x + BLOCK_SIZE, food.y + BLOCK_SIZE);
}


void move(SnakeNode snake[], int& length, Food& food, int& direction) {//引用传参//
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
   
    if (head.x == food.x && head.y == food.y) {//检测碰撞，蛇头与食物的重叠即碰撞
        
        if (length < MAX_SNAKE_LENGTH) {//注意限制最大长度
            for (int i = length; i > 0; i--) {
                snake[i] = snake[i - 1];
            }
            snake[0] = head;
            length++;
        }

        
        food.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;//随机生成且永远与蛇保持齐平的食物
        food.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
    }
    else {
        
        for (int i = length - 1; i > 0; i--) {
            snake[i] = snake[i - 1];
        }
        snake[0] = head;//只移动不加长
    }
}


bool isGameOver(SnakeNode snake[], int length) {
    SnakeNode head = snake[0];
    
    if (head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT) {
        return true;
    }
    
    for (int i = 1; i < length; i++) {
        if (snake[i].x == head.x && snake[i].y == head.y) {//蛇头与蛇身重叠
            return true;
        }
    }
    return false;
}

int main() {
    SnakeNode snake[MAX_SNAKE_LENGTH];
    int length;
    Food food;
    int direction = 3; 

    
    init(snake, length, food);

    
    while (true) {//主循环，不断绘制，检测按键，移动，直到break
        //键盘的获取 windows api
        if (GetAsyncKeyState(VK_UP) & 0x8000 && direction!=1) {//使用 windows api的函数检测按键是否被按下，同时注意避免反向操作
            direction = 0;
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000&& direction!=0) {
            direction = 1;
        }
        if (GetAsyncKeyState(VK_LEFT) & 0x8000 && direction != 3) {
            direction = 2; 
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000&& direction != 2) {
            direction = 3;
        }

        
        move(snake, length, food, direction);

       
        if (isGameOver(snake, length)) {
            break;
        }

        
        draw(snake, length, food);

        
        Sleep(100);
    }

    closegraph();// 结束图形模式，恢复到控制台模式，对应initgraph
    std::cout << "Game Over!" << std::endl;
    return 0;
}

