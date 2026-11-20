#include "PhysicsTools/ParticleSystem.h"
#include "PhysicsTools/Projectile.h"
#if defined(_WIN32)
#pragma comment(lib, "dwmapi.lib")
extern "C" {
__declspec(dllimport) long __stdcall DwmSetWindowAttribute(
    void *hwnd, unsigned long attr, const void *value, unsigned long size);
}
#endif

#include "Coordinate.h"
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


#include <cstdlib>
#include <cstring>

#include "PhysicsTools/PhysicsTools.h"

int main(int argc, char* argv[])
{
    
    
    bool testScreenshot = false;
    float customThickness = -1.0f;
    const char* screenshotFile = "screenshot_out.png";

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--test-screenshot") == 0) testScreenshot = true;
        if (strncmp(argv[i], "--thickness=", 12) == 0) customThickness = (float)atof(argv[i] + 12);
        if (strncmp(argv[i], "--out=", 6) == 0) screenshotFile = argv[i] + 6;
    }
    // Window properties
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_WINDOW_UNDECORATED);

    // Window size
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Physics Playground");
    
    

    // Make relative paths always resolve next to the executable
    ChangeDirectory(GetApplicationDirectory());

    // Window icon
    Image icon = LoadImage("resources/app.png");
    SetWindowIcon(icon);

    // Title-bar texture loaded directly from decoded image data
    Texture2D iconTex = LoadTextureFromImage(icon);
    UnloadImage(icon); // Free CPU memory now that texture is on GPU
    SetTextureFilter(iconTex, TEXTURE_FILTER_BILINEAR);

    const char* appTitle = "Physics Playground";
    const float iconSize = 20.0f;   // icon is drawn at this size (px)
    const float barPad = 8.0f;      // left padding of the bar

    // Custom font
    Font myFont = LoadFontEx(
        "resources/fonts/BigShoulders_18pt-Regular.ttf",
        32,
        nullptr,
        0
    );

    // Smooth scaling, because the font is loaded at 32 but drawn at 16-20
    SetTextureFilter(myFont.texture, TEXTURE_FILTER_BILINEAR);

    // GUI style
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

#if defined(_WIN32)
    // DWMWA_WINDOW_CORNER_PREFERENCE = 33, DWMWCP_ROUND = 2
    int pref = 2;
    DwmSetWindowAttribute(GetWindowHandle(), 33, &pref, sizeof(pref));
#endif
    
    // Background color
    Color color = {20,20,20,0};

    // Framerate
    SetTargetFPS(60);
    

    // Unit scale default (pixels per physics unit)
    float pixelsPerUnit = 55.0f;

    // Gravity default
    float gravity = 9.8f;

    // Grid line thickness default
    float gridThickness = 0.25f;
    if (customThickness > 0.0f) gridThickness = customThickness;

    int testFrameCount = 0;

    // Labels on the ticks
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
    
    
    /*---------------------------------------------------------------- TEST*/
    ParticleSystem2D particleSystem;
    float spawnRate = 60.0f; // particles per second
    /*---------------------------------------------------------------- TEST*/
    

    // Main game loop
    while (!WindowShouldClose() && !closeRequested)
    {
        // --- Window dragging (only from the top bar) ---
        Vector2 mouse = GetMousePosition();   // relative to the window
        float screenW = (float)GetScreenWidth();
        float screenH = (float)GetScreenHeight();
        
        
        // Define UI rectangles once per frame (the grid slider depends on window size)
        Rectangle unitSlider    = { 80, 40, 120, 8 };
        Rectangle gravitySlider = { 80, 65, 120, 8 };
        Rectangle gridSlider    = { screenW - 180.0f, screenH - 30.0f, 120.0f, 8.0f };

        
        // Mouse wheel zoom for pixelsPerUnit
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f)
        {
            pixelsPerUnit += wheel * 2.0f;
            if (pixelsPerUnit < 10.0f) pixelsPerUnit = 10.0f;
            if (pixelsPerUnit > 100.0f) pixelsPerUnit = 100.0f;
        }

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
        if (shown == EDGE_L || shown == EDGE_R)                             SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
        else if (shown == EDGE_T || shown == EDGE_B)                        SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
        else if (shown == (EDGE_L | EDGE_T) || shown == (EDGE_R | EDGE_B))  SetMouseCursor(MOUSE_CURSOR_RESIZE_NWSE);
        else if (shown == (EDGE_R | EDGE_T) || shown == (EDGE_L | EDGE_B))  SetMouseCursor(MOUSE_CURSOR_RESIZE_NESW);
        else                                                                SetMouseCursor(MOUSE_CURSOR_DEFAULT);

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
        float buttonsX = screenW - 3 * btnW;

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
        
        
        // Coordinate calculations
        Vector2 origin = Coordinate::origin(screenW, screenH);
        Vector2 left = Coordinate::middle_edge(screenW, screenH, Coordinate::positions::left);
        Vector2 right = Coordinate::middle_edge(screenW, screenH, Coordinate::positions::right);
        Vector2 top = Coordinate::middle_edge(screenW, screenH, Coordinate::positions::top);
        Vector2 bottom = Coordinate::middle_edge(screenW, screenH, Coordinate::positions::bottom);

        float step = pixelsPerUnit;
        
        // ---------------------------------------------------------------------------- TEST
        Vec2D SpawnPos = {
            (mouse.x - origin.x) / pixelsPerUnit,
            (origin.y - mouse.y) / pixelsPerUnit
        };
        
        particleSystem.SetGravity(gravity);
        particleSystem.SetEmitter(MakeProjectile2D(SpawnPos, {RandomRange(-5.0f,5.0f) ,RandomRange(5.0, 10.0f)}, RandomRange(1.0f,2.0f)), spawnRate);
        float dt = GetFrameTime();
        particleSystem.Update(dt);
        // ---------------------------------------------------------------------------- TEST

        BeginDrawing();
        
        ClearBackground(color);
        
        // ---------------------------------------------------------------------------- TEST
        for (const auto& p : particleSystem.GetAll())
        {
            float screenX = origin.x + p.position.x * pixelsPerUnit;
            float screenY = origin.y - p.position.y * pixelsPerUnit;
            float t = p.NormalizedAge();
            float radius = 4.0f * (1.0f - t);
            DrawCircleV({ screenX, screenY }, radius, RED);
        }
        // ---------------------------------------------------------------------------- TEST
        


        // --- 1. Background Grid Lines (Subtle Cartesian Grid) ---
        Color gridColor = Fade(LIGHTGRAY, 0.3f);

        // Vertical grid lines
        for (float x = origin.x + step; x <= right.x; x += step)
            DrawLineEx({ x, top.y }, { x, bottom.y }, gridThickness, gridColor);
        for (float x = origin.x - step; x >= left.x; x -= step)
            DrawLineEx({ x, top.y }, { x, bottom.y }, gridThickness, gridColor);

        // Horizontal grid lines
        for (float y = origin.y - step; y >= top.y; y -= step)
            DrawLineEx({ left.x, y }, { right.x, y }, gridThickness, gridColor);
        for (float y = origin.y + step; y <= bottom.y; y += step)
            DrawLineEx({ left.x, y }, { right.x, y }, gridThickness, gridColor);

        // --- 2. Main Coordinate Axes ---
        DrawLineV(left, right, LIGHTGRAY);
        DrawLineV(bottom, top, LIGHTGRAY);

        // Origin Point & Label
        DrawCircleV(origin, 4.0f, LIGHTGRAY);
        DrawTextEx(myFont, "ORIGIN (0, 0)", { origin.x + 10.0f, origin.y + 6.0f }, 16.0f, 1.0f, LIGHTGRAY);

        // Positive X Ticks & Labels
        for (int i = 1; origin.x + i * step <= right.x; i++)
        {
            float x = origin.x + i * step;
            DrawLineV({ x, origin.y - 5.0f }, { x, origin.y + 5.0f }, LIGHTGRAY);

            if (i % labelEvery == 0)
                Coordinate::DrawTickLabel(myFont, i, { x, origin.y + 8.0f }, 16.0f, LIGHTGRAY);
        }

        // Negative X Ticks & Labels
        for (int i = 1; origin.x - i * step >= left.x; i++)
        {
            float x = origin.x - i * step;
            DrawLineV({ x, origin.y - 5.0f }, { x, origin.y + 5.0f }, LIGHTGRAY);

            if (i % labelEvery == 0)
                Coordinate::DrawTickLabel(myFont, -i, { x, origin.y + 8.0f }, 16.0f, LIGHTGRAY);
        }

        // Positive Y Ticks & Labels (up on screen)
        for (int i = 1; origin.y - i * step >= top.y; i++)
        {
            float y = origin.y - i * step;
            DrawLineV({ origin.x - 5.0f, y }, { origin.x + 5.0f, y }, LIGHTGRAY);

            if (i % labelEvery == 0)
            {
                const char* text = TextFormat("%d", i);
                Vector2 size = MeasureTextEx(myFont, text, 16.0f, 1.0f);
                DrawTextEx(myFont, text, { origin.x + 10.0f, y - size.y * 0.5f }, 16.0f, 1.0f, LIGHTGRAY);
            }
        }

        // Negative Y Ticks & Labels (down on screen)
        for (int i = 1; origin.y + i * step <= bottom.y; i++)
        {
            float y = origin.y + i * step;
            DrawLineV({ origin.x - 5.0f, y }, { origin.x + 5.0f, y }, LIGHTGRAY);

            if (i % labelEvery == 0)
            {
                const char* text = TextFormat("%d", -i);
                Vector2 size = MeasureTextEx(myFont, text, 16.0f, 1.0f);
                DrawTextEx(myFont, text, { origin.x + 10.0f, y - size.y * 0.5f }, 16.0f, 1.0f, LIGHTGRAY);
            }
        }

        
        // --- 3. UI Sliders Panel ---
        GuiSliderBar(
            Rectangle{unitSlider},
            "Unit Scale",
            TextFormat("%d px", (int)pixelsPerUnit),
            &pixelsPerUnit,
            10.0f,
            100.0f
        );

        GuiSliderBar(
            Rectangle{gravitySlider},
            "Gravity",
            TextFormat("%.1f", gravity),
            &gravity,
            0.0f,
            20.0f
        );

        // Grid line thickness slider (bottom right)
        GuiSliderBar(
            Rectangle{ gridSlider },
            "Grid Thickness",
            TextFormat("%.2f px", gridThickness),
            &gridThickness,
            0.25f,
            0.5f
        );

        // --- 4. Title Bar (Rendered on top of background) ---
        // Top draggable bar background
        DrawRectangle(0, 0, (int)screenW, (int)dragBarHeight, Fade(BLACK, 0.55f));

        // Icon + Title (top left)
        float iconY = (dragBarHeight - iconSize) * 0.5f;
        DrawTexturePro(
            iconTex,
            Rectangle{ 0.0f, 0.0f, (float)iconTex.width, (float)iconTex.height },
            Rectangle{ barPad, iconY, iconSize, iconSize },
            Vector2{ 0.0f, 0.0f }, 0.0f, WHITE
        );

        float fontSize = 18.0f;
        DrawTextEx(myFont, appTitle,
                   Vector2{ barPad + iconSize + 8.0f, (dragBarHeight - fontSize) * 0.5f },
                   fontSize, 1.0f, ORANGE);

        // Title Bar Window Control Buttons (top right)
        float bx = screenW - 3 * btnW;

        if (GuiButton(Rectangle{ bx, 0, btnW, dragBarHeight }, "-"))
            MinimizeWindow();

        if (GuiButton(Rectangle{ bx + btnW, 0, btnW, dragBarHeight }, "[]"))
        {
            if (IsWindowMaximized()) RestoreWindow();
            else MaximizeWindow();
        }

        if (GuiButton(Rectangle{ bx + 2 * btnW, 0, btnW, dragBarHeight }, "X"))
            closeRequested = true;
        
        /*---------------------------------------------- TEST*/

        /*---------------------------------------------- TEST*/
        

        EndDrawing();

        if (testScreenshot)
        {
            testFrameCount++;
            if (testFrameCount >= 5)
            {
                TakeScreenshot(screenshotFile);
                break;
            }
        }
    }

    // Cleanup resources
    UnloadTexture(iconTex);
    UnloadFont(myFont);
    CloseWindow();

    return 0;
}
