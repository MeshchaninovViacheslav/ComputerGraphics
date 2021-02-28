#ifndef MAIN_IMAGE_H
#define MAIN_IMAGE_H

#include <string>

constexpr int tileSize = 16;
constexpr int numChannels = 4;

struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

bool operator==(const Pixel &pix1, const Pixel &pix2);

Pixel Fade(Pixel pixel, double alpha);

constexpr Pixel backgroundColor{0, 0, 0, 0};

struct Image {
    explicit Image(const std::string &a_path);

    Image(int a_width, int a_height, int a_channels);

    int Save(const std::string &a_path);

    int Width() const { return width; }

    int Height() const { return height; }

    int Channels() const { return channels; }

    size_t Size() const { return size; }

    Pixel *Data() { return data; }

    Pixel GetPixel(int x, int y) { return data[width * y + x]; }

    void PutPixel(int x, int y, const Pixel &pix) { data[width * y + x] = pix; }

    void InsertImage(Image image);

    bool HasZeroPixel();

    ~Image();

private:
    int width = -1;
    int height = -1;
    int channels = 3;
    size_t size = 0;
    Pixel *data = nullptr;
    bool self_allocated = false;
};


#endif //MAIN_IMAGE_H
