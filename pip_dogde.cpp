#include <windows.h>
#include <iostream>
#include <thread>
#include <string>


struct Coordinates // Structure to hold PiP and cursor coordinates
{
    /*
     * RECT structure is used to define the coordinates of the PiP window, which includes the left, top, right, and bottom edges of the window.
     * POINT structure is used to define the coordinates of the cursor, which includes the x and y coordinates of the cursor on the screen.
     */
    RECT rect;
    POINT cursor;
};

struct SizeOfScreen
{
    int width;
    int height;
};

struct SizeOfPiP
{
    int width;
    int height;
};

// Check PiP is exist or not
HWND IsPiPExist()
{
    // Find the PiP window by its title "Picture in Picture"
    return FindWindowA(NULL, "Picture in Picture");
}

// Get PiP and cursor coordinates
Coordinates GetCoordinates(HWND pip)
{
    /*
     * GetWindowRect function retrieves the coordinates of the PiP window and stores them in the rect member of the Coordinates structure.
     * GetCursorPos function retrieves the current position of the cursor and stores it in the cursor member
     */
    Coordinates coords;
    GetWindowRect(pip, &coords.rect);
    GetCursorPos(&coords.cursor);
    return coords;
}

// Compare the cursor coordinates with the bounding
bool IsInside(RECT rect, POINT point)
{
    return point.x >= rect.left &&
           point.x <= rect.right &&
           point.y >= rect.top &&
           point.y <= rect.bottom;
}

// Get the size of the screen
SizeOfScreen GetScreenSize()
{
    /*
     * GetSystemMetrics function retrieves the width and height of the screen and stores them in the width and height members of the SizeOfScreen structure, respectively.
     * SM_CXSCREEN and SM_CYSCREEN are the parameters used to specify that we want to retrieve the width and height of the screen, respectively.
     */
    SizeOfScreen size;
    size.width = GetSystemMetrics(SM_CXSCREEN);
    size.height = GetSystemMetrics(SM_CYSCREEN);
    return size;
}

// Get the size of the PiP
SizeOfPiP GetPiPSize(RECT rect)
{
    SizeOfPiP size;
    size.width = rect.right - rect.left;
    size.height = rect.bottom - rect.top;
    return size;
}

// Transfer the position of PiP
void TransferPiPPos(int leftP, int topP, int widthP, int heightP, int widthS, int heightS, HWND pip)
{
    int newX = (leftP < widthS / 2) ? (widthS - widthP - 1) : 0;
    int newY = (topP < heightS / 2) ? (heightS - heightP - 1) : 0;

    SetWindowPos(pip, HWND_TOPMOST, newX, newY, widthP, heightP, SWP_SHOWWINDOW);
}

int main()
{
    bool paused = false;

    // Get the size of screen
    SizeOfScreen sizeS = GetScreenSize();

    // Check if the cursor is hovering over the PiP
    bool lastKeyState = false;
    while (true)
    {
        bool currentKeyState = (GetAsyncKeyState(VK_RSHIFT) & 0x8000) != 0;
        if (currentKeyState && !lastKeyState)
        {
            paused = !paused;
        }
        lastKeyState = currentKeyState;

        HWND pip = IsPiPExist();
        if (!pip || !IsWindow(pip) || !IsWindowVisible(pip))
        {
            std::cout << "Window is not exist!" << std::endl;
            break;
        }

        if (!paused)
        {
            Coordinates coords = GetCoordinates(pip);
            bool checkInside = IsInside(coords.rect, coords.cursor);
            // if (checkInside)
            //     std::cout << "Inside!" << std::endl;
            // else
            //     std::cout << "Outside!" << std::endl;

            if (checkInside)
            {
                SizeOfPiP sizep = GetPiPSize(coords.rect);
                TransferPiPPos(
                    coords.rect.left,
                    coords.rect.top,
                    sizep.width,
                    sizep.height,
                    sizeS.width,
                    sizeS.height,
                    pip);
            }

            Sleep(50);
        }
    }

    return 0;
}