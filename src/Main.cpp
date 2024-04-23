#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <format>

struct Point {
    int x;
    int y;
};

struct ScanPoint : public Point {
    COLORREF color;
};

HWND getHwndOfPid(DWORD pid) {
    
    HWND currentHwnd = nullptr;
    do {
        currentHwnd = FindWindowEx(nullptr, currentHwnd, nullptr, nullptr);
        DWORD currentPid = 0;
        GetWindowThreadProcessId(currentHwnd, &currentPid);
        if (currentPid == pid) {
            RECT hwndRect;
            GetWindowRect(currentHwnd, &hwndRect);
            if (hwndRect.right > 0) {
                return currentHwnd;
            }
        }
    } while (currentHwnd != nullptr);

    return nullptr;
}

inline void log(const std::string &msg) {
    std::cout << msg << std::endl;
}

int main(int argc, char *argv[]) {
    
    constexpr ScanPoint SCAN_POINTS[] = {
        {566, 256, 0x000000},
        {566, 257, 0x000000},
        {566, 258, 0x000000},
        {566, 259, 0x000000}
    };

    std::unordered_map<std::string, int> STAT_ORDER{
        { "STR", 0 },
        { "DEX", 1 },
        { "INT", 2 },
        { "LUK", 3 }
    };

    constexpr int STAT_OFFSET = 20;
    constexpr int RETRY_MS = 600;
    constexpr Point CLICK_POINT{ 632, 318 };

    std::vector<int> requestedOffsets;
    for (int i = 2; i < argc; i++) {
        requestedOffsets.push_back(STAT_ORDER[std::string(argv[i])] * STAT_OFFSET);
    }
    
    HWND mapleHwnd = getHwndOfPid(atoi(argv[1]));

    if (!mapleHwnd) {
        log("PID given is invalid.");
        return EXIT_FAILURE;
    }

    HDC mapleDC = GetDC(mapleHwnd);

    SetForegroundWindow(mapleHwnd);

    POINT clickPoint{ CLICK_POINT.x, CLICK_POINT.y };
    ClientToScreen(mapleHwnd, &clickPoint);
    SetCursorPos(clickPoint.x, clickPoint.y);

    log("Starting brute force!");

    int attempts = 0;

    bool found = false;
    while (!found) {

        POINT curr;
        GetCursorPos(&curr);
        if (curr.x != clickPoint.x || curr.y != clickPoint.y) {
            log("Stopped the process due to mouse movement. Got impatient huh?");
            return 0;
        }

        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        ++attempts;

        Sleep(RETRY_MS);

        found = true;
        for (int offset : requestedOffsets) {
            for (const auto &scanPoint : SCAN_POINTS) {
                found = found && GetPixel(mapleDC, scanPoint.x, scanPoint.y + offset) == scanPoint.color;
            }
        }
    }

    log(std::format("Done. Took {} attempts. Enjoy your perfectly crafted character!", attempts));
    return 0;
}