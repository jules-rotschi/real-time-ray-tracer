#pragma once

#include <stdint.h>
#include <string>

bool EncodePPM(const char* filename, uint32_t width, uint32_t height, const uint8_t* imageData);