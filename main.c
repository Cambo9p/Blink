#include "renderer/raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "renderer/clay_renderer_raylib.c"

const int FONT_ID_BODY_16 = 0;

/* --------------- Colors --------------- */
Clay_Color COLOR_WHITE = { 255, 255, 255, 255};
Clay_Color COLOR_TEXT = { 28, 27, 25, 255};
Clay_Color COLOR_MAIN = { 237, 231, 219, 255};
Clay_Color COLOR_BROWN = { 125, 46, 29, 255};

const Clay_Color COLOR_LIGHT_HOVER = (Clay_Color) {224, 215, 210, 255};
const Clay_Color COLOR_DARK_HOVER = (Clay_Color) {148, 46, 8, 255};

const int MAX_INPUT_LENGTH = 65;

Clay_TextElementConfig headerTextConfig = (Clay_TextElementConfig) { .fontId = 2, .fontSize = 24, .textColor = {61, 26, 5, 255} };

typedef struct {
    char* content;
    bool isPressed;
} SearchInfo;

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

void HandleSearchBarInteraction(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData) {
    SearchInfo* si = (SearchInfo*)userData;
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        printf("TEST\n");
        si->isPressed = true;
   }
}

void HandleSearchButton(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData) {
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        printf("you searched for something\n");
   }
}

void SearchButton(SearchInfo* searchInfo) {
    CLAY({
            .backgroundColor = Clay_Hovered() ? COLOR_LIGHT_HOVER : COLOR_DARK_HOVER,
            .cornerRadius = CLAY_CORNER_RADIUS(10),
            .layout = {
                .sizing = {
                .width = CLAY_SIZING_FIXED(90),
                .height = CLAY_SIZING_FIXED(50)
                },
            }
        }) {
            Clay_OnHover(HandleSearchButton, (uintptr_t)&searchInfo);
            CLAY({ 
                .layout = {
                .padding =  CLAY_PADDING_ALL(16),
                .childGap = 16,
                }}) {

                CLAY_TEXT(CLAY_STRING("Search"),
                    CLAY_TEXT_CONFIG({
                        .fontSize = 24,
                        .textColor = {61, 26, 5, 255},
                        .fontId = FONT_ID_BODY_16,
                }));
            }
    }; 

}

void TextBox(SearchInfo* searchInfo) {
    char* test = "tes";
    Clay_String s = ( Clay_String ) {.length = strlen(test), .chars =test};
    CLAY({
            .backgroundColor = Clay_Hovered() ? COLOR_LIGHT_HOVER : COLOR_DARK_HOVER,
            .cornerRadius = CLAY_CORNER_RADIUS(10),
            .layout = {
                .sizing = {
                .width = CLAY_SIZING_FIXED(500),
                .height = CLAY_SIZING_FIXED(50)
                },
            }
        }) {
            Clay_OnHover(HandleSearchBarInteraction, (uintptr_t)&searchInfo);
            CLAY({ 
                .layout = {
                .padding =  CLAY_PADDING_ALL(16),
                .childGap = 16,
                }}) {

                CLAY_TEXT(s,
                    CLAY_TEXT_CONFIG({
                        .fontSize = 24,
                        .textColor = {61, 26, 5, 255},
                        .fontId = FONT_ID_BODY_16,
                }));
            }
    }; 
}

// builds all the UI elements
void GenerateHeirarchy(SearchInfo* searchInfo) {
    CLAY({ .id = CLAY_ID("OuterContainer"), .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } }, .backgroundColor = COLOR_MAIN }) {
        CLAY({ .id = CLAY_ID("Header"), .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(50) }, .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }, .childGap = 16, .padding = { 32, 32 } } }) {
            CLAY_TEXT(CLAY_STRING("test"),
                CLAY_TEXT_CONFIG({
                    .fontSize = 24,
                    .textColor = {61, 26, 5, 255},
                    .fontId = FONT_ID_BODY_16,
            }));
            CLAY({ .id = CLAY_ID("Spacer"), .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) } } }) {}
        }
        CLAY({
            .backgroundColor = COLOR_WHITE,
            .id = CLAY_ID("Secondary"),
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(),
                    .height = CLAY_SIZING_GROW()
                },
                .padding = CLAY_PADDING_ALL(45),
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
            }
            }
        ) {
            CLAY({
                .backgroundColor = COLOR_MAIN,
                .id = CLAY_ID("third"),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {
                        .width = CLAY_SIZING_GROW(),
                        .height = CLAY_SIZING_GROW()
                    },
                    .padding = CLAY_PADDING_ALL(45),
                    .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER},
                }
                }
            ) {
                // main content on main page
                TextBox(searchInfo);
                CLAY({ .id = CLAY_IDI("Spacer", 1), .layout = { .sizing = { .height = CLAY_SIZING_FIXED(10) } } }) {}
                SearchButton(searchInfo);
            }

        };
    };
}

int main(void) {
    Clay_Raylib_Initialize(1024, 768, "Browser", FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT); // Extra parameters to this function are new since the video was published

    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));
    Clay_Initialize(clayMemory, (Clay_Dimensions) {
       .width = GetScreenWidth(),
       .height = GetScreenHeight()
    }, (Clay_ErrorHandler) { HandleClayErrors });

    Font fonts[1];
    fonts[FONT_ID_BODY_16] = LoadFontEx("resources/Quicksand-Semibold.ttf", 48, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);

    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    SearchInfo* largeSearchBarInfo = (SearchInfo*)malloc(sizeof(SearchInfo));
    largeSearchBarInfo->content = malloc(sizeof(char) * MAX_INPUT_LENGTH);
    largeSearchBarInfo->isPressed = false;

    while (!WindowShouldClose()) {
        Clay_SetLayoutDimensions((Clay_Dimensions) {GetScreenWidth(), GetScreenHeight()});

        Vector2 raylibMousePosition = GetMousePosition();
        Clay_Vector2 mousePosition = {
            .x = raylibMousePosition.x,
            .y = raylibMousePosition.y,
        };

        Clay_SetPointerState(mousePosition, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
        Clay_UpdateScrollContainers(true, (Clay_Vector2) { GetMouseX(), GetMouseY() }, GetFrameTime());

        Clay_BeginLayout();

        GenerateHeirarchy(largeSearchBarInfo);

        Clay_RenderCommandArray renderCommands = Clay_EndLayout();
        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts);
        EndDrawing();


    }
}
