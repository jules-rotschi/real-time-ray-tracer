#include "Encoder.h"

#include <fstream>

bool EncodePPM(const char* filename, uint32_t width, uint32_t height, const uint8_t* imageData)
{
    if (std::ifstream fileTest(filename); fileTest.good())
        return false;
    
    std::ofstream file(filename);

    file << "P3\n" << width << ' ' << height << "\n255\n";

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint8_t r = imageData[4 * (x + y * width)];
            uint8_t g = imageData[4 * (x + y * width) + 1];
            uint8_t b = imageData[4 * (x + y * width) + 2];

            file << static_cast<int>(r) << ' '
                 << static_cast<int>(g) << ' '
                 << static_cast<int>(b) << '\n';
        }
    }

    file << std::flush;

    return true;
}