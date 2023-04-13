// 7wad (c) Nikolas Wipper 2022

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <raylib.h>
#include <tinyfiledialogs.h>

#include "wad/Archive.h"
#include "wad/File.h"

int main() {
    int screenWidth = 1080;
    int screenHeight = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "7wad");
    SetTargetFPS(60);

    std::optional<Archive> openedArchive = {};
    std::vector<std::string> files = {};

    int scroll = 0;
    int totalHeight = 0;
    bool scrollbar = false;

    while (!WindowShouldClose()) {
        if (IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();

            openedArchive = Archive(std::string(droppedFiles.paths[0]));

            UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory

            totalHeight = openedArchive->files.size() * 35;

            files.clear();
            files.reserve(openedArchive->files.size());

            for (auto [name, off]: openedArchive->files) {
                files.push_back(name);
            }
        }

        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();

        BeginDrawing();

        ClearBackground(RAYWHITE);

        Vector2 mousePos = GetMousePosition();

        if (openedArchive.has_value()) {
            if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
                const char * target_folder = tinyfd_selectFolderDialog("Save WAD contents to", nullptr);

                if (target_folder) {
                    fs::path folder = target_folder;
                    for (auto [name, off]: openedArchive->files) {
                        fs::path file = folder / name;

                        fs::create_directories(file.parent_path());

                        openedArchive->OpenFile(name)->WriteToDisk(file);
                    }
                }
            }

            scroll += (int) (GetMouseWheelMoveV().y * 35);

            int size = files.size() - (int) (screenHeight / 35);

            if (scroll > 0) scroll = 0;
            else if (scroll / 35 < -size)
                scroll = -(size * 35);

            {
                int first_object = -scroll / 35;
                int height = 10;

                for (auto name = files.begin() + (int) first_object; name != files.end(); name++) {
                    if (mousePos.y > (float) height && mousePos.y < (float) height + 30 &&
                        mousePos.x < (float) screenWidth - 40) {
                        DrawRectangle(10, height, screenWidth - 40, 30, GRAY);

                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            fs::path file = *name;
                            std::string filename = file.filename().string();
                            std::string exten = std::string("*") + file.extension().string();
                            const char *patterns[] = {exten.c_str()};
                            char *path = tinyfd_saveFileDialog("Save file from WAD",
                                                               filename.c_str(),
                                                               1, patterns,
                                                               nullptr);
                            if (path) {
                                openedArchive->OpenFile(*name)->WriteToDisk(path);
                            }
                        }
                    }
                    DrawRectangleLines(10, height, screenWidth - 40, 30, BLACK);
                    DrawText(name->c_str(), 15, height + 5, 20, BLACK);

                    if (height > screenHeight) break;

                    height += 35;
                }
            }

            {
                // Draw scrollbar
                float perc = (float) -scroll / (float) totalHeight;

                int pos = (int) (perc * (float) screenHeight);
                int height = std::max(10, (int) (35.f / (float) totalHeight * (float) screenHeight));

                DrawRectangle(screenWidth - 30, pos, 30, height, GRAY);

                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    if (scrollbar) {
                        scroll = (int) -(mousePos.y / (float) screenHeight * (float) totalHeight);
                    }
                    if (mousePos.x > (float) screenWidth - 40 &&
                        mousePos.y > (float) pos &&
                        mousePos.y < (float) pos + (float) height) {
                        scrollbar = true;
                    }
                } else {
                    scrollbar = false;
                }
            }
        } else {
            constexpr int fontSize = 30;

            int width = MeasureText("Drop a .wad here", fontSize);
            DrawText("Drop a .wad here",
                     (int) (screenWidth / 2 - width / 2), (int) (screenHeight / 2 - fontSize / 2),
                     fontSize,
                     LIGHTGRAY);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
