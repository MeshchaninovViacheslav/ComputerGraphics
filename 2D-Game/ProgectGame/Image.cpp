#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"
#include "Player.h"

#include <iostream>


Image::Image(const std::string &a_path) {
    if ((data = (Pixel *) stbi_load(a_path.c_str(), &width, &height, &channels, 0)) != nullptr) {
        size = width * height * channels;
    }
}

Image::Image(int a_width, int a_height, int a_channels) {
    data = new Pixel[a_width * a_height]{};

    if (data != nullptr) {
        width = a_width;
        height = a_height;
        size = a_width * a_height * a_channels;
        channels = a_channels;
        self_allocated = true;
    }
}


int Image::Save(const std::string &a_path) {
    auto extPos = a_path.find_last_of('.');
    if (a_path.substr(extPos, std::string::npos) == ".png" || a_path.substr(extPos, std::string::npos) == ".PNG") {
        stbi_write_png(a_path.c_str(), width, height, channels, data, width * channels);
    } else if (a_path.substr(extPos, std::string::npos) == ".jpg" ||
               a_path.substr(extPos, std::string::npos) == ".JPG" ||
               a_path.substr(extPos, std::string::npos) == ".jpeg" ||
               a_path.substr(extPos, std::string::npos) == ".JPEG") {
        stbi_write_jpg(a_path.c_str(), width, height, channels, data, 100);
    } else {
        std::cerr << "Unknown file extension: " << a_path.substr(extPos, std::string::npos) << "in file name" << a_path
                  << "\n";
        return 1;
    }

    return 0;
}

void Image::InsertImage(Image image) {
    Point coords = {this->Width() / 2 - image.Width() / 2, this->Height() / 2 - image.Height() / 2};
    for (int y = coords.y; y < coords.y + image.Height(); ++y) {
        for (int x = coords.x; x < coords.x + image.Width(); ++x) {
            Pixel new_px = image.GetPixel(x - coords.x, image.Height() - y + coords.y - 1);
            this->PutPixel(x, y, new_px);
        }
    }
}

Image::~Image() {
    if (self_allocated)
        delete[] data;
    else {
        stbi_image_free(data);
    }
}

bool Image::HasZeroPixel() {
    for (int i = 0; i < Height(); ++i) {
        for (int j = 0; j < Width(); ++j) {
            if (GetPixel(j, i) == backgroundColor) {
                return true;
            }
        }
    }
    return false;
}

bool operator==(const Pixel &pix1, const Pixel &pix2) {
    return pix1.r == pix2.r &&
           pix1.g == pix2.g &&
           pix1.b == pix2.b &&
           pix1.a == pix2.a;
}

Pixel Fade(Pixel pixel, double alpha) {
    return Pixel{uint8_t(pixel.r * alpha),
                 uint8_t(pixel.g * alpha),
                 uint8_t(pixel.b * alpha),
                 uint8_t(pixel.a)};
}