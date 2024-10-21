#include <iostream>
#include <string>
#include <pthread.h>
#include <cstdlib> // For rand() and srand()
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <semaphore.h>
using namespace std;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct GhostParams {
    char (*maze)[15][55];
    int x, y;
    char character;
    string color;
};

struct PlayerParams {
    char (*maze)[15][55];
    int x, y;
    char character;
    string color;
    Direction initialDirection;
};

pthread_barrier_t movementBarrier;
pthread_spinlock_t mazeLock;

Direction getNextDirection() {
    int randomNum = rand() % 4;  
        
    switch (randomNum){
        case 0:
            return UP;
        case 1:
            return LEFT;
        case 2:
            return RIGHT;
        case 3:
            return DOWN;
        default:
            return RIGHT;
    }
}

void ShowsCursor(bool visible) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = visible;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void SetPosition(int16_t X, int16_t Y) {
    HANDLE Screen = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD Position = { Y, X };
    SetConsoleCursorPosition(Screen, Position);
}

void PrintMaze(char (&maze)[15][55]) {
    string colors[] = { "\033[31m", "\033[35m", "\033[32m", "\033[36m" };
    int ampersandCount = 0;

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 55; j++) {
            char currentChar = maze[i][j];
            if (currentChar == '#' || currentChar == '|') {
                cout << "\033[34m" << currentChar << "\033[0m";
            } else if (currentChar == '&') {
                cout << colors[ampersandCount % 4] << currentChar << "\033[0m";
                ampersandCount++;
            } else if (currentChar == 'C') {
                cout << "\033[33m" << currentChar << "\033[0m";
            } else {
                cout << currentChar;
            }
        }
        cout << std::endl;
    }
}

void* ghostMoving(void* params) {
    GhostParams* ghost = (GhostParams*)params;
    char (*maze)[15][55] = ghost->maze;
    int x = ghost->x;
    int y = ghost->y;
    char character = ghost->character;
    string color = ghost->color;

    char previousChar = ' ';
    while (true) {
        Direction dir = getNextDirection();

        pthread_barrier_wait(&movementBarrier); 

        pthread_spin_lock(&mazeLock);

        SetPosition(x, y);
        cout << previousChar;

        int newX = x, newY = y;
        switch (dir) {
            case RIGHT:
                if ((*maze)[x][y + 1] != '#' && (*maze)[x][y + 1] != '|') {
                    newY = y + 1;
                }
                break;
            case UP:
                if ((*maze)[x - 1][y] != '#' && (*maze)[x - 1][y] != '|') {
                    newX = x - 1;
                }
                break;
            case LEFT:
                if ((*maze)[x][y - 1] != '#' && (*maze)[x][y - 1] != '|') {
                    newY = y - 1;
                }
                break;
            case DOWN:
                if ((*maze)[x + 1][y] != '#' && (*maze)[x + 1][y] != '|') {
                    newX = x + 1;
                }
                break;
        }

        if ((*maze)[newX][newY] != '&') {
            SetPosition(x, y);
            (*maze)[x][y] = previousChar; 
            previousChar = (*maze)[newX][newY]; 
            (*maze)[newX][newY] = '&'; 

            x = newX;
            y = newY;
        }

        SetPosition(x, y);
        cout << color << character << "\033[0m";

        pthread_spin_unlock(&mazeLock);

        pthread_barrier_wait(&movementBarrier);  
        Sleep(150);
    }
    return nullptr;
}

void* Move(void* params) {
    PlayerParams* player = (PlayerParams*)params;
    char (*maze)[15][55] = player->maze;
    int x = player->x;
    int y = player->y;
    char character = player->character;
    string color = player->color;
    Direction dir = player->initialDirection;

    ShowsCursor(false);

    while (true) {
        if (_kbhit()) {
            char input = _getch();
            SetPosition(x, y);
            cout << " ";

            switch (input) {
            case 72: 
                dir = UP;
                break;
            case 80: 
                dir = DOWN;
                break;
            case 75: 
                dir = LEFT;
                break;
            case 77: 
                dir = RIGHT;
                break;
            case 'q': // Quit
                SetPosition(15, 0);
                return nullptr;
            }

            pthread_spin_lock(&mazeLock);

            switch (dir) {
                case UP:
                    if ((*maze)[x - 1][y] == ' ' || (*maze)[x - 1][y] == '.') {
                        x--;
                    }
                    break;
                case DOWN:
                    if ((*maze)[x + 1][y] == ' ' || (*maze)[x + 1][y] == '.') {
                        x++;
                    }
                    break;
                case LEFT:
                    if ((*maze)[x][y - 1] == ' ' || (*maze)[x][y - 1] == '.') {
                        y--;
                    }
                    break;
                case RIGHT:
                    if ((*maze)[x][y + 1] == ' ' || (*maze)[x][y + 1] == '.') {
                        y++;
                    }
                    break;
            }

            SetPosition(x, y);
            cout << color << character << "\033[0m";
            pthread_spin_unlock(&mazeLock);
        }

        pthread_barrier_wait(&movementBarrier); 
        pthread_barrier_wait(&movementBarrier); 
        Sleep(50);
    }
    return nullptr;
}

int main() {
    pthread_barrier_init(&movementBarrier, nullptr, 5); 
    pthread_spin_init(&mazeLock, 0);

    int x = 9, y = 24;
    int x_ghost1 = 7, y_ghost1 = 21;
    int x_ghost2 = 7, y_ghost2 = 23;
    int x_ghost3 = 7, y_ghost3 = 25;
    int x_ghost4 = 7, y_ghost4 = 27;

    char maze[15][55] = {
        "|###############################################|",
        "|        |              |              |        |",
        "| |####| | |##########| | |##########| | |####| |",
        "| |                                           | |",
        "| | |####| |####| |###########| |####| |####| | |",
        "|                                             | |",
        "| | |####| |####| |#|       |#| |####| |####| | |",
        "                  |  & & & &  |                  ",
        "| | |####| |####| |###########| |####| |####| | |",
        "|          |            C            |          |",
        "| | |####| |####| |###########| |####| |####| | |",
        "| |                                           | |",
        "| |####| | |##########| | |##########| | |####| |",
        "|        |              |              |        |",
        "|###############################################|"
    };

    system("cls");
    PrintMaze(maze);

    pthread_t ghostThreads[4];
    GhostParams ghosts[4] = {
        {&maze, x_ghost1, y_ghost1, '&', "\033[31m"},
        {&maze, x_ghost2, y_ghost2, '&', "\033[35m"},
        {&maze, x_ghost3, y_ghost3, '&', "\033[32m"},
        {&maze, x_ghost4, y_ghost4, '&', "\033[36m"}
    };

    for (int i = 0; i < 4; i++) {
        pthread_create(&ghostThreads[i], nullptr, ghostMoving, &ghosts[i]);
    }

    pthread_t playerThread;
    PlayerParams player = {&maze, x, y, 'C', "\033[33m", RIGHT};
    pthread_create(&playerThread, nullptr, Move, &player);

    pthread_join(playerThread, nullptr);

    for (int i = 0; i < 4; i++) {
        pthread_join(ghostThreads[i], nullptr);
    }

    pthread_barrier_destroy(&movementBarrier);
    pthread_spin_destroy(&mazeLock);

    return 0;
}