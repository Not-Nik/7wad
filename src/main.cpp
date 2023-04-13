// 7wad (c) Nikolas Wipper 2022

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <raylib.h>

#include "wad/Archive.h"
#include "wad/File.h"

int main() {
    int screenWidth = 1080;
    int screenHeight = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "7wad");
    SetTargetFPS(60);

    std::optional<Archive> opened_archive = {};

    int scroll = 0;

    while (!WindowShouldClose()) {
        if (IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();

            opened_archive = Archive(std::string(droppedFiles.paths[0]));

            UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
        }

        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();

        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (opened_archive.has_value()) {
            scroll += GetMouseWheelMoveV().y * 4;
            if (scroll > 0) scroll = 0;

            int height = scroll + 10;
            for (auto [name, off] : opened_archive->files) {
                DrawRectangleLines(10, height, screenWidth - 20, 30, BLACK);
                DrawText(name.c_str(), 15, height + 5, 20, BLACK);
                height += 35;
            }
        }

        EndDrawing();
    }

    return 0;
}
