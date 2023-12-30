#pragma once

#include "./base/application.h"

#include <windows.h>

#define BITMAP_ID 0x4D42

bool newScreenShot(int width, int height, const char *filePath);