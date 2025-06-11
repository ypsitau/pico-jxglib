#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Stream.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace jxglib;

void write_to_stream(void* context, void* data, int size)
{
    Stream* pStream = reinterpret_cast<Stream*>(context);
    pStream->Write(data, size);
}

int main()
{
    stdio_init_all();
    Stream* pStream;
    int x, y, channels;
    ::stbi_load_from_callbacks(nullptr, pStream, &x, &y, &channels, 0);
    ::stbi_write_bmp_to_func(write_to_stream, pStream, 0, 0, 0, nullptr);
    ::stbi_write_png_to_func(write_to_stream, pStream, 0, 0, 0, nullptr, 0);
    ::stbi_write_jpg_to_func(write_to_stream, pStream, 0, 0, 0, nullptr, 90);
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
