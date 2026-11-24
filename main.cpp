#include <array>
#include <iostream>
#include "raylib.h"
#include "PrivacyScenario.h"

using App = PrivacyScenario::App;
using Permission = PrivacyScenario::Permission;

void drawTextureIn(Texture2D texture, Rectangle destination) {
    Rectangle source = {0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)};
    DrawTexturePro(texture, source, destination, {0, 0}, 0, WHITE);
}

void drawButton(Rectangle bounds, const char* label, Color color, int size = 16) {
    DrawRectangleRec(bounds, color);
    DrawText(label, static_cast<int>(bounds.x + (bounds.width - MeasureText(label, size)) / 2),
             static_cast<int>(bounds.y + (bounds.height - size) / 2), size, WHITE);
}

// New app screens are drawn with raylib primitives, so no further PNGs are needed.
void drawAppScreen(int app, const PrivacyScenario& scenario, Texture2D instagram,
                   bool musicPreview, bool shopPreview) {
    const bool allowed = scenario.getPermission() == Permission::Granted;
    const Rectangle screen = {402,221,198,371};
    if (app == 0) {
        drawTextureIn(instagram, screen);
        return;
    }
    DrawRectangleRec(screen, RAYWHITE);
    DrawRectangle(402,221,198,52,DARKBLUE);
    DrawText(scenario.getAppName(),414,233,23,WHITE);
    DrawText("FICTIONAL DEMO",414,259,10,WHITE);
    if (app == 1) {
        DrawText("Today's activity",414,292,18,DARKGRAY);
        DrawRectangle(414,325,174,72,Fade(RED,0.10f));
        DrawText("STEPS",426,337,13,MAROON);
        DrawText(allowed ? "6,240" : "--",426,359,26,MAROON);
        DrawRectangle(414,409,174,72,Fade(RED,0.10f));
        DrawText("HEART RATE",426,421,13,MAROON);
        DrawText(allowed ? "72 bpm" : "--",426,443,26,MAROON);
        DrawText(allowed ? "Sample records only" : "No connected records",414,499,14,DARKGRAY);
    } else if (app == 2) {
        DrawRectangle(414,288,174,185,Fade(GREEN,0.15f));
        for (int i=0; i<4; ++i) {
            DrawRectangle(435+i*42,288,7,185,WHITE);
            DrawRectangle(414,310+i*43,174,7,WHITE);
        }
        DrawLineEx({435,442},{478,442},5,BLUE);
        DrawLineEx({478,442},{478,335},5,BLUE);
        DrawLineEx({478,335},{560,335},5,BLUE);
        DrawCircle(560,335,8,MAROON);
        DrawCircle(435,442,8,allowed ? BLUE : GRAY);
        DrawText(allowed ? "Demo current position" : "Manual starting point",414,486,14,DARKGRAY);
        DrawText("Route: Home > Park",414,508,14,DARKGRAY);
    } else if (app == 3) {
        DrawRectangle(434,291,134,118,Fade(PURPLE,0.15f));
        DrawCircle(501,350,38,PURPLE);
        DrawLineEx({494,326},{494,370},5,WHITE);
        DrawLineEx({494,326},{517,332},5,WHITE);
        DrawCircle(486,371,9,WHITE);
        DrawText("Discover a song",414,425,19,DARKGRAY);
        DrawText(musicPreview ? "Sample: Evening Walk" : "Try the demo below",414,454,14,DARKGRAY);
        DrawText("No audio is recorded",414,476,14,DARKGRAY);
        drawButton({414,500,174,32},allowed ? "Identify (demo)" : "Browse (demo)",PURPLE,15);
    } else {
        DrawText(allowed ? "Picked for you" : "Popular items",414,291,19,DARKGRAY);
        DrawRectangle(414,325,174,65,Fade(ORANGE,0.15f));
        DrawText(allowed ? "Trail backpack" : "Canvas tote",424,339,17,DARKGRAY);
        DrawText(allowed ? "$48 - sample" : "$18 - sample",424,365,14,MAROON);
        DrawRectangle(414,403,174,65,Fade(ORANGE,0.15f));
        DrawText(allowed ? "Walking shoes" : "Travel mug",424,417,17,DARKGRAY);
        DrawText(allowed ? "$65 - sample" : "$12 - sample",424,443,14,MAROON);
        DrawText(shopPreview ? "Preview only - no order" : "Demo catalog",414,478,14,DARKGRAY);
        drawButton({414,500,174,32},"Preview item",BROWN,15);
    }
}

int main() {
    InitWindow(1400,800,"Privacy Explorer - Five Apps");
    SetTargetFPS(30);
    const std::array<const char*,7> paths = {{"phoneAssets/phone.png", "phoneAssets/chat.png",
        "phoneAssets/health.png", "phoneAssets/maps.png", "phoneAssets/music.png",
        "phoneAssets/shop.png", "phoneAssets/instagram.png"}};
    std::array<Texture2D,7> textures{};
    for (std::size_t i=0; i<textures.size(); ++i) {
        textures[i] = LoadTexture(paths[i]);
        if (textures[i].id == 0) {
            std::cerr << "Could not load " << paths[i] << ". Run from the project root.\n";
            for (auto texture : textures) if (texture.id != 0) UnloadTexture(texture);
            CloseWindow();
            return 1;
        }
    }

    std::array<PrivacyScenario,5> scenarios = {{PrivacyScenario(App::Instagram),
        PrivacyScenario(App::Health), PrivacyScenario(App::Maps),
        PrivacyScenario(App::Music), PrivacyScenario(App::Shop)}};
    const std::array<Rectangle,5> icons = {{{420,260,50,50},{530,260,50,50},
        {420,360,50,50},{530,360,50,50},{475,460,50,50}}};
    const Rectangle homeButton = {410,608,180,32};
    const Rectangle settingsButton = {412,548,178,36};
    const Rectangle allowButton = {414,376,174,42};
    const Rectangle denyButton = {414,430,174,42};
    const Rectangle backButton = {830,590,100,35};
    const Rectangle nextButton = {1220,590,100,35};
    int activeApp = -1;
    bool showPrompt = false;
    bool musicPreview = false;
    bool shopPreview = false;
    std::cout << "Privacy Explorer: choose a phone app. All records are simulated.\n";

    while (!WindowShouldClose()) {
        const Vector2 mouse = GetMousePosition();
        const bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        // Input is handled before drawing. This mutually exclusive chain prevents
        // opening an app from also choosing an overlapping permission button.
        if (activeApp == -1) {
            if (clicked) for (int i=0; i<5; ++i) {
                if (CheckCollisionPointRec(mouse,icons[i])) {
                    activeApp = i;
                    showPrompt = scenarios[i].getPermission() == Permission::Pending;
                    std::cout << "Opened " << scenarios[i].getAppName() << ": "
                              << scenarios[i].getStatus() << '\n';
                    break;
                }
            }
        } else if (clicked && CheckCollisionPointRec(mouse,homeButton)) {
            activeApp = -1;
            showPrompt = false;
        } else if (showPrompt) {
            if (clicked && (CheckCollisionPointRec(mouse,allowButton) || CheckCollisionPointRec(mouse,denyButton))) {
                auto& scenario = scenarios[activeApp];
                scenario.answerPermission(CheckCollisionPointRec(mouse,allowButton));
                showPrompt = false;
                if (activeApp == 3) musicPreview = false;
                if (activeApp == 4) shopPreview = false;
                std::cout << scenario.getAppName() << " - " << scenario.getAccessName()
                          << ": " << scenario.getStatus() << " (simulation only)\n";
            }
        } else if (clicked) {
            auto& scenario = scenarios[activeApp];
            if (CheckCollisionPointRec(mouse,settingsButton)) showPrompt = true;
            else if (CheckCollisionPointRec(mouse,backButton)) scenario.previousStep();
            else if (CheckCollisionPointRec(mouse,nextButton)) scenario.nextStep();
            else if (CheckCollisionPointRec(mouse,{414,500,174,32})) {
                if (activeApp == 3) {
                    musicPreview = true;
                    std::cout << "Music: showing a fictional song; no microphone used.\n";
                } else if (activeApp == 4) {
                    shopPreview = true;
                    std::cout << "Shop: preview only; no purchase or payment.\n";
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("PRIVACY EXPLORER",250,82,30,WHITE);
        DrawText("Five apps. Different data. Your choices.",250,121,18,LIGHTGRAY);
        // Keep the phone frame visible on every screen.
        drawTextureIn(textures[0],{250,150,500,500});
        if (activeApp == -1) {
            for (int i=0; i<5; ++i) {
                drawTextureIn(textures[i+1],icons[i]);
                const char* name = scenarios[i].getAppName();
                DrawText(name,static_cast<int>(icons[i].x+(50-MeasureText(name,13))/2),
                         static_cast<int>(icons[i].y+55),13,WHITE);
            }
            DrawText("Choose an app to explore",800,220,26,WHITE);
            DrawText("Health records, location, microphone",800,270,19,LIGHTGRAY);
            DrawText("and personalization each tell a different story.",800,300,19,LIGHTGRAY);
            DrawText("All data is fictional. No device access occurs.",800,365,18,LIGHTGRAY);
            DrawText("Choices stay saved until you close the program.",800,400,18,LIGHTGRAY);
        } else {
            const auto& scenario = scenarios[activeApp];
            drawAppScreen(activeApp,scenario,textures[6],musicPreview,shopPreview);
            drawButton(settingsButton,"Permission settings",DARKBLUE,15);
            drawButton(homeButton,"< Home",DARKGRAY);
            DrawRectangle(800,150,550,500,WHITE);
            DrawRectangleLines(800,150,550,500,GRAY);
            DrawText("Privacy Insights",830,175,28,DARKBLUE);
            DrawText(TextFormat("%s - JOHN DOE",scenario.getAppName()),830,215,18,DARKGRAY);
            DrawText("SIMULATION ONLY",830,240,12,GRAY);
            scenario.drawInsights();
            drawButton(backButton,"BACK",!showPrompt && scenario.canGoBack() ? DARKBLUE : GRAY);
            drawButton(nextButton,"NEXT",!showPrompt && scenario.canGoNext() ? DARKBLUE : GRAY);
            DrawText(TextFormat("%d / 3",scenario.getStep()+1),1075,600,16,DARKGRAY);
            if (showPrompt) {
                DrawRectangleRec({402,221,198,371},Fade(BLACK,0.65f));
                DrawRectangle(406,312,190,178,WHITE);
                DrawRectangleLines(406,312,190,178,DARKGRAY);
                DrawText(scenario.getPrompt(),414,330,16,BLACK);
                DrawText("Simulated choice",414,354,14,DARKGRAY);
                drawButton(allowButton,"Allow",DARKGREEN,18);
                drawButton(denyButton,"Don't Allow",MAROON,18);
            }
        }
        EndDrawing();
    }
    for (auto texture : textures) UnloadTexture(texture);
    std::cout << "Session summary:\n";
    for (const auto& scenario : scenarios)
        std::cout << scenario.getAppName() << " - " << scenario.getAccessName()
                  << ": " << scenario.getStatus() << '\n';
    CloseWindow();
    return 0;
}
