#pragma comment(lib, "dwmapi.lib")

#include "raylib.h"
#include "coordinate.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Declared manually to avoid windows.h name clashes with raylib
// (link with: -ldwmapi  or  dwmapi.lib)
extern "C" {
__declspec(dllimport) long __stdcall DwmSetWindowAttribute(
    void *hwnd, unsigned long attr, const void *value, unsigned long size);
}



int WinMain(void)
{
    // Window properties
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_WINDOW_UNDECORATED);
    
    
    
    // Window size
    const int screenWidth = 1280;
    const int screenHeight = 720;
    
    // Creating origin class
    coordinate* co = new coordinate();
    

    InitWindow(screenWidth, screenHeight, "Physics Playground");
    
    
    // Make relative paths always resolve next to the executable
    ChangeDirectory(GetApplicationDirectory());
    
    // Window icon
    Image icon = LoadImage("resources/app.png");
    SetWindowIcon(icon);
    
    
    // Title-bar texture loaded straight from file (no shared Image)
    Texture2D iconTex = LoadTexture("resources/app.png");
    SetTextureFilter(iconTex, TEXTURE_FILTER_BILINEAR);
    
    
    const char* appTitle = "Physics Playground";
    const float iconSize = 16.0f;   // icon is drawn at this size (px)
    const float barPad = 8.0f;      // left padding of the bar
    
    // Custom font
    Font myFont = LoadFontEx(
    "resources/fonts/BigShoulders_18pt-Regular.ttf",
    32,
    nullptr,
    0
    );
    
    // Smooth scaling, because the font is loaded at 32 but drawn at 20
    SetTextureFilter(myFont.texture, TEXTURE_FILTER_BILINEAR);
    
    
    // GUI stayle
    GuiSetFont(myFont);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(LIGHTGRAY));
    
    // Flat, transparent title-bar buttons (affects only GuiButton)
    GuiSetStyle(BUTTON, BORDER_WIDTH, 0);                                      // no border
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL,  ColorToInt(BLANK));                // no background
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(Fade(WHITE, 0.15f)));   // subtle hover
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(Fade(WHITE, 0.30f)));   // subtle press
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL,  ColorToInt(LIGHTGRAY));            // icon always visible
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(WHITE));
    
    
    // DWMWA_WINDOW_CORNER_PREFERENCE = 33, DWMWCP_ROUND = 2
    int pref = 2;
    DwmSetWindowAttribute(GetWindowHandle(), 33, &pref, sizeof(pref));
    
    // Framerate
    SetTargetFPS(60);
    
    
    // Unit scale default
    float pixelsPerUnit = 20.0f;
    
    // Gravity default
    float gravity = 9.8f;
    
    // Labale on the ticks
    const int labelEvery = 5;
    
    // Height of the draggable strip at the top of the window
    const float dragBarHeight = 28.0f;
    
    // Drag the windows properties
    bool dragging = false;
    Vector2 grab = { 0 };
    
    // Set to true by the close button; checked by the main loop
    bool closeRequested = false;

    // Size of each title-bar button
    const float btnW = 32.0f;
    
    // Resize settings
    const int border = 6;                  // thickness of the invisible resize zone
    const int minW = 400, minH = 300;      // minimum window size

    // Edge flags (can be combined for corners)
    const int EDGE_L = 1, EDGE_R = 2, EDGE_T = 4, EDGE_B = 8;

    bool resizing = false;
    int resizeEdge = 0;
    Vector2 startMouse = { 0 };    // mouse position in SCREEN space when resize started
    Vector2 startPos = { 0 };      // window position when resize started
    Vector2 startSize = { 0 };     // window size when resize started

    // Main game loop
    while (!WindowShouldClose() && !closeRequested)    // Detect window close button or ESC key
    {
        // --- Window dragging (only from the top bar) ---
        Vector2 mouse = GetMousePosition();   // relative to the window
        
        // --- Window resizing (manual, since the window is undecorated) ---
        int w = GetScreenWidth();
        int h = GetScreenHeight();

        // Which edge(s) is the mouse over?
        int hover = 0;
        if (!IsWindowMaximized())
        {
            if (mouse.x <= border)     hover |= EDGE_L;
            if (mouse.x >= w - border) hover |= EDGE_R;
            if (mouse.y <= border)     hover |= EDGE_T;
            if (mouse.y >= h - border) hover |= EDGE_B;
        }

        // Cursor feedback
        int shown = resizing ? resizeEdge : hover;
        if (shown == EDGE_L || shown == EDGE_R)                         SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
        else if (shown == EDGE_T || shown == EDGE_B)                    SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
        else if (shown == (EDGE_L | EDGE_T) || shown == (EDGE_R | EDGE_B)) SetMouseCursor(MOUSE_CURSOR_RESIZE_NWSE);
        else if (shown == (EDGE_R | EDGE_T) || shown == (EDGE_L | EDGE_B)) SetMouseCursor(MOUSE_CURSOR_RESIZE_NESW);
        else                                                            SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        // Start resizing
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hover != 0)
        {
            resizing = true;
            resizeEdge = hover;
            Vector2 wp = GetWindowPosition();
            startMouse = { wp.x + mouse.x, wp.y + mouse.y };
            startPos = wp;
            startSize = { (float)w, (float)h };
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) resizing = false;

        // Apply resizing
        if (resizing)
        {
            Vector2 wp = GetWindowPosition();
            Vector2 now = { wp.x + mouse.x, wp.y + mouse.y };   // mouse in screen space
            float dx = now.x - startMouse.x;
            float dy = now.y - startMouse.y;

            float nx = startPos.x, ny = startPos.y;
            float nw = startSize.x, nh = startSize.y;

            if (resizeEdge & EDGE_R) nw = startSize.x + dx;
            if (resizeEdge & EDGE_B) nh = startSize.y + dy;
            if (resizeEdge & EDGE_L) { nw = startSize.x - dx; }
            if (resizeEdge & EDGE_T) { nh = startSize.y - dy; }

            if (nw < minW) nw = minW;
            if (nh < minH) nh = minH;

            // For left/top edges, keep the opposite edge fixed
            if (resizeEdge & EDGE_L) nx = startPos.x + startSize.x - nw;
            if (resizeEdge & EDGE_T) ny = startPos.y + startSize.y - nh;

            SetWindowPosition((int)nx, (int)ny);
            SetWindowSize((int)nw, (int)nh);
        }
        
        // Buttons occupy the right-most 3 * btnW pixels of the bar
        float buttonsX = GetScreenWidth() - 3 * btnW;

        // Start dragging only if the click started inside the top bar
        // (and not on the buttons, and not while maximized)
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
            && !IsWindowMaximized()
            && hover == 0
            && mouse.y >= 0 && mouse.y <= dragBarHeight
            && mouse.x < buttonsX)
        {
            dragging = true;
            grab = mouse;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) dragging = false;

        if (dragging)
        {
            Vector2 win = GetWindowPosition();
            SetWindowPosition((int)(win.x + mouse.x - grab.x),
                              (int)(win.y + mouse.y - grab.y));
        }
        
        
        // Get the origin of the screen each frame
        Vector2 origin = co->origin(GetScreenWidth(), GetScreenHeight());
        
        // Middle of the edge of the screen
        Vector2 left = co->middle_edge(GetScreenWidth(), GetScreenHeight(), coordinate::positions::left);
        Vector2 right = co->middle_edge(GetScreenWidth(), GetScreenHeight(), coordinate::positions::right);
        Vector2 top = co->middle_edge(GetScreenWidth(), GetScreenHeight(), coordinate::positions::top);
        Vector2 bottom = co->middle_edge(GetScreenWidth(), GetScreenHeight(), coordinate::positions::bottom);
        

        BeginDrawing();

        ClearBackground(DARKGRAY);
        

        
        // --- Title bar buttons (top right) ---
        float bx = GetScreenWidth() - 3 * btnW;

        // Minimize
        if (GuiButton(Rectangle{ bx, 0, btnW, dragBarHeight }, "-"))
            MinimizeWindow();

        // Maximize / Restore
        if (GuiButton(Rectangle{ bx + btnW, 0, btnW, dragBarHeight }, "[]"))
        {
            if (IsWindowMaximized()) RestoreWindow();
            else MaximizeWindow();
        }

        // Close
        if (GuiButton(Rectangle{ bx + 2 * btnW, 0, btnW, dragBarHeight }, "X"))
            closeRequested = true;

        // Origin & Text
        DrawCircle(origin.x, origin.y, 4, LIGHTGRAY); // Origin point
        GuiLabel(Rectangle{origin.x + 10,origin.y + 5, 120, 30}, "ORIGIN");
        
        // Division amount slider
        GuiSliderBar(
            Rectangle{ 80, 40, 100, 10 },
            "Unit Scale",
            TextFormat("%d px", (int)pixelsPerUnit),
            &pixelsPerUnit,
            20,
            30
        );
        
        // gravity amount slider
        GuiSliderBar(
            Rectangle{ 80, 60, 100, 10 },
            "Gravity",
            TextFormat("%.1f", gravity),
            &gravity,
            0.0f,
            20.0f
        );
        
        float step = pixelsPerUnit;
        
        // Coordinate lines
        DrawLine(left.x, left.y, right.x, right.y, LIGHTGRAY);
        
        // Positive X
        for (int i = 1; origin.x + i * step <= right.x; i++)
        {
            float x = origin.x + i * step;
            DrawLineV({x, origin.y - 5}, {x, origin.y + 5}, LIGHTGRAY);

            if (i % labelEvery == 0)
                co->DrawTickLabel(myFont, i, {x, origin.y + 8}, 16, LIGHTGRAY);
        }

        // Negative X
        for (int i = 1; origin.x - i * step >= left.x; i++)
        {
            float x = origin.x - i * step;
            DrawLineV({x, origin.y - 5}, {x, origin.y + 5}, LIGHTGRAY);

            if (i % labelEvery == 0)
                co->DrawTickLabel(myFont, -i, {x, origin.y + 8}, 16, LIGHTGRAY);
        }


        
        
        DrawLine(bottom.x, bottom.y, top.x, top.y, LIGHTGRAY);
        
        // Positive Y (up on screen)
        for (int i = 1; origin.y - i * step >= top.y; i++)
        {
            float y = origin.y - i * step;
            DrawLineV({origin.x - 5, y}, {origin.x + 5, y}, LIGHTGRAY);

            if (i % labelEvery == 0)
                DrawTextEx(myFont, TextFormat("%d", i), {origin.x + 10, y - 8}, 16, 1, LIGHTGRAY);
        }

        // Negative Y (down on screen)
        for (int i = 1; origin.y + i * step <= bottom.y; i++)
        {
            float y = origin.y + i * step;
            DrawLineV({origin.x - 5, y}, {origin.x + 5, y}, LIGHTGRAY);

            if (i % labelEvery == 0)
                DrawTextEx(myFont, TextFormat("%d", -i), {origin.x + 10, y - 8}, 16, 1, LIGHTGRAY);
        }
        
        // Visual hint for the drag area
        DrawRectangle(0, 0, GetScreenWidth(), (int)dragBarHeight, Fade(BLACK, 0.5f));
        
        // --- Title bar: icon + name (top left) ---
        float iconY = (dragBarHeight - iconSize) * 0.5f;

        DrawTexturePro(
            iconTex,
            Rectangle{ 0, 0, (float)iconTex.width, (float)iconTex.height },   // source: whole image
            Rectangle{ barPad, iconY, iconSize, iconSize },                   // destination: scaled
            Vector2{ 0, 0 }, 0.0f, WHITE
        );

        // Vertically center the text inside the bar
        float fontSize = 16.0f;
        DrawTextEx(myFont, appTitle,
                   Vector2{ barPad + iconSize + 8.0f, (dragBarHeight - fontSize) * 0.5f },
                   fontSize, 1, LIGHTGRAY);
        

        EndDrawing();

    }
    UnloadImage(icon);
    UnloadTexture(iconTex);
    UnloadFont(myFont);
    CloseWindow();        // Close window and OpenGL context
    
    return 0;
}