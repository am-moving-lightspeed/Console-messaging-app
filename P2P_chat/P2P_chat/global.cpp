#include "global.h"



void p2p_chat::global::clearScreen() {

    COORD coord00 = { 0, 0 };
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO sbi;
    DWORD tmp;

    GetConsoleScreenBufferInfo(hConsole, &sbi);

    FillConsoleOutputCharacterA(hConsole,
                                ' ',
                                sbi.dwSize.X * sbi.dwSize.Y,
                                coord00,
                                &tmp);

    //FillConsoleOutputAttribute(
    //    hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
    //    sbi.dwSize.X * sbi.dwSize.Y, coord00, &tmp
    //);

    SetConsoleCursorPosition(hConsole, coord00);
}
