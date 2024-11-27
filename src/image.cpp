#include "image.h"

Image::Image(std::string FileName) : FileName(FileName) {
  int x, y, n;
  Data = stbi_load(FileName.c_str(), &x, &y, &n, 0);
  Height = y;
  Width = x;
  Channels = n;
  ConvertToVec();
};
Image::~Image() {};
void Image::ImageToGreyScale() {
  for (int y = 0; y < Height; y++) {
    for (int x = 0; x < Width; x++) {
      int Pixel = 0;
      Pixel += PixelVec[y][x].R;
      Pixel += PixelVec[y][x].G;
      Pixel += PixelVec[y][x].B;
      Pixel = Pixel/3;
        
      PixelVec[y][x].R = Pixel;
      PixelVec[y][x].G = Pixel;
      PixelVec[y][x].B = Pixel;
        
    }
  }
};
void Image::ImageToEdgeBits(int Threshold) {
  for (int y = 0; y < Height; y++) {
    for (int x = 0; x < Width; x++) {
      if (PixelVec[y][x].R < Threshold) {
        PixelVec[y][x].R = 0;
      } else {
        PixelVec[y][x].R = 255;
      }
      if (PixelVec[y][x].G < Threshold) {
        PixelVec[y][x].G = 0;
      } else {
        PixelVec[y][x].G = 255;
      }
      if (PixelVec[y][x].B < Threshold) {
        PixelVec[y][x].B = 0;
      } else {
        PixelVec[y][x].B = 255;
      }
    }
  }
}

  
void Image::SaveImage(std::string Name) {
  ConvertToCharArray();
  stbi_write_png(Name.c_str(), Width, Height, 3, Data, Width * 3);
}
void Image::ConvertToVec() {
  for(int i = 0; i < Height; i++) {
    int y = Width*Channels*i;
    std::vector<RGBData> PixelLine;
    for(int x = 0; x < Width*Channels; x += 3) {
      RGBData CurrentPixel;
      CurrentPixel.R = Data[x + y + 0];
      CurrentPixel.G = Data[x + y + 1];
      CurrentPixel.B = Data[x + y + 2];
      PixelLine.push_back(CurrentPixel);
    }
    PixelVec.push_back(PixelLine);
  }
  //std::cout << PixelVec[0][0].R << std::endl;
  //std::cout << (int)Data[2] << std::endl;
}
void Image::ConvertToCharArray() {
  for(int y = 0; y < Height; y++) {
    for(int x = 0; x < Width; x++) {
      int PixelNum = x + y*Width;
      Data[PixelNum*Channels + 0] = PixelVec[y][x].R;
      Data[PixelNum*Channels + 1] = PixelVec[y][x].G;
      Data[PixelNum*Channels + 2] = PixelVec[y][x].B;
    }
  }
}
