#pragma once
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "Include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Include/stb_image_write.h"


struct RGBData {
  int R, G, B;
};

class Image {
 public:
  Image(std::string FileName);
  ~Image();
  void ImageToGreyScale();
  void ImageToEdgeBits(int Threshold);
  int Height, Width, Channels;
  unsigned char *Data;
  std::vector<std::vector<RGBData>> PixelVec;
  std::string FileName;
  
  void SaveImage(std::string Name);
 private:
  void ConvertToVec();
  void ConvertToCharArray();
};
