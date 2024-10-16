#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    // Configuramos la consola para usar UTF-8 en Windows
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    // Definimos el laberinto con el símbolo especial '&' y otros caracteres
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

    // Mensaje de bienvenida
    std::cout << "Welcome to the maze game!\n";
    std::cout << "You are represented by the character 'C'.\n";
    std::cout << "The exit is represented by the character '&' with different colors.\n";
    std::cout << "You can move up, down, left, or right.\n";
    std::cout << "Good luck!\n";

    // Colores para los &: rojo, rosado, anaranjado, celeste
    std::string colors[] = {"\033[31m", "\033[35m", "\033[32m", "\033[36m"};

    // Imprimir el laberinto fila por fila con colores
    int ampersandCount = 0;  // Contador para los diferentes colores de '&'
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 55; j++) {
            char currentChar = maze[i][j];

            if (currentChar == '#' || currentChar == '|') {
                // Azul para las paredes (# y |)
                std::cout << "\033[34m" << currentChar << "\033[0m";
            } else if (currentChar == '&') {
                // Colores variados para cada '&'
                std::cout << colors[ampersandCount % 4] << currentChar << "\033[0m";
                ampersandCount++;
            } else if (currentChar == 'C') {
                // Amarillo para el jugador (C)
                std::cout << "\033[33m" << currentChar << "\033[0m";
            } else if (currentChar == ' ') {
                // Intercalar las 'o' entre filas: en filas pares usar 'o', en filas impares usar ' ' (vacío)
                if (j % 2 == 1 || i == 7) {
                    std::cout << "\033[37m" << ' ' << "\033[0m";  // Dejar espacio en filas impares
                } else {
                    std::cout << "\033[37m" << '.' << "\033[0m";  // Blanco para las 'o'
                }
            } else {
                // Sin color para los demás caracteres
                std::cout << currentChar;
            }
        }
        std::cout << std::endl; // Nueva línea después de cada fila
    }
    return 0;
}