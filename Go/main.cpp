#include "Game.h"
#include <windows.h>

int main()
{
   HWND hWnd = GetConsoleWindow();
   ShowWindow(hWnd, SW_HIDE);


    Game game = Game();

    game.run();

    return 0;
}