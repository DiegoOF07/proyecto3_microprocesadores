#include <iostream>
#include <string>
#include <conio.h>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

enum Direction { UP, DOWN, LEFT, RIGHT };
int speed = 500;

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

int PrintMaze(char maze[15][55]) {
    // Colores para los &: rojo, rosado, anaranjado, celeste
    string colors[] = {"\033[31m", "\033[35m", "\033[32m", "\033[36m"};

    // Imprimir el laberinto fila por fila con colores
    int ampersandCount = 0;  // Contador para los diferentes colores de '&'
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 55; j++) {
            char currentChar = maze[i][j];

            if (currentChar == '#' || currentChar == '|') {
                // Azul para las paredes (# y |)
                cout << "\033[34m" << currentChar << "\033[0m";
            } else if (currentChar == '&') {
                // Colores variados para cada '&'
                cout << colors[ampersandCount % 4] << currentChar << "\033[0m";
                ampersandCount++;
            } else if (currentChar == 'C') {
                // Amarillo para el jugador (C)
                cout << "\033[33m" << currentChar << "\033[0m";
            } else if (currentChar == ' ') {
                // Intercalar las 'o' entre filas: en filas pares usar 'o', en filas impares usar ' ' (vacío)
                if (j % 2 == 1 || i == 7) {
                    cout << "\033[37m" << ' ' << "\033[0m";  // Dejar espacio en filas impares
                } else {
                    cout << "\033[37m" << '.' << "\033[0m";  // Blanco para las 'o'
                }
            } else {
                // Sin color para los demás caracteres
                cout << currentChar;
            }
        }
        cout << std::endl; // Nueva línea después de cada fila
    }
    return 0;
}

void KeepMoving(Direction dir, char maze[15][55], int& x, int& y, char character, string color) {
    bool isMoving = true;
    
    while (isMoving){
        if (_kbhit()) {
            char input = _getch();

            SetPosition(x, y);
            cout << " ";  

            switch (input) {
                case 72: // Arriba
                    dir = UP;
                    break;
                case 80: // Abajo
                    dir = DOWN;
                    break;
                case 75: // Izquierda
                    dir = LEFT;
                    break;
                case 77: // Derecha
                    dir = RIGHT;
                    break;
                case 'q':
                    isMoving = false;
                    break;
            }

            SetPosition(x, y);
            cout << color << character << "\033[0m";
        }

        SetPosition(x, y);
        cout << " ";  

        switch (dir) {
            case UP:
                if (maze[x-1][y] == ' ' || maze[x-1][y] == '.') {
                    x--;
                } else {
                    isMoving = false;
                }
                break;
            case DOWN:
                if (maze[x+1][y] == ' ' || maze[x+1][y] == '.') {
                    x++;
                } else {
                    isMoving = false;
                }
                break;
            case LEFT:
                if (maze[x][y-1] == ' ' || maze[x][y-1] == '.') {
                    y--;
                } else {
                    isMoving = false;
                }
                break;
            case RIGHT:
                if (maze[x][y+1] == ' ' || maze[x][y+1] == '.') {
                    y++;
                } else {
                    isMoving = false;
                }
                break;
        }

        SetPosition(x, y);
        cout << color << character << "\033[0m";
        Sleep(speed);  
    }    
}

void Move(char maze[15][55], int& x, int& y, char character, string color, Direction initialDirection) {
    Direction dir = initialDirection;
    bool running = true;

    ShowsCursor(false);

    while (running) {
        KeepMoving(dir, maze, x, y, character, color);
        if (_kbhit()) {
            char input = _getch();
            if (input == 'q') {
                SetPosition(15, 0);
                running = false;
            }
        }
    }
}

int main() {
    int x = 9, y = 24;
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
    Move(maze, x, y, 'C', "\033[33m", RIGHT); 

    return 0;
}