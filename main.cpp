#include <bitset>
#include <iostream>
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

class Kernel {
public:
  Kernel(std::vector<std::vector<double>> Data)
    :KernelData(Data) {
    SizeY = KernelData.size();
    SizeX = KernelData[0].size();
    GetMinAndMax();
  }
  unsigned int SizeX, SizeY;
  std::vector<std::vector<double>> KernelData;
  double Min = 0.0;
  double Max = 0.0;

private:
  void GetMinAndMax() {
    for (int y = 0; y < SizeY; y++) {
      for (int x = 0; x < SizeX; x++) {
        if (KernelData[y][x] < 0 ) {
          Min += KernelData[y][x];
        }
        if (KernelData[y][x] > 0 ) {
          Max += KernelData[y][x];
        }
      }
    }
  };
};

struct RGBData {
  int R, G, B;
};

class Image {
public:
  Image(std::string FileName) : FileName(FileName) {
    int x, y, n;
    Data = stbi_load(FileName.c_str(), &x, &y, &n, 0);
    Height = y;
    Width = x;
    Channels = n;
    ConvertToVec();
  };
  ~Image() {};
  void ImageToGreyScale() {
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
  void ImageToEdgeBits(int Threshold) {
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

  int Height, Width, Channels;
  unsigned char *Data;
  std::vector<std::vector<RGBData>> PixelVec;
  std::string FileName;
  
  void SaveImage(std::string Name) {
    ConvertToCharArray();
    stbi_write_png(Name.c_str(), Width, Height, 3, Data, Width * 3);
  }
private:
  void ConvertToVec() {
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
  void ConvertToCharArray() {
    for(int y = 0; y < Height; y++) {
      for(int x = 0; x < Width; x++) {
        int PixelNum = x + y*Width;
        Data[PixelNum*Channels + 0] = PixelVec[y][x].R;
        Data[PixelNum*Channels + 1] = PixelVec[y][x].G;
        Data[PixelNum*Channels + 2] = PixelVec[y][x].B;
      }
    }
  }
};


// Scale the output from kernel min/max to 0->255
void ConvertToUnsignedChar(double &Num, Kernel Knl) {
  Num = (Num - Knl.Min); // 0 -> min + max
  Num = Num/(Knl.Max - Knl.Min); // 0 -> 1
  Num = floor(Num*255.0); // 0 -> 255
}

void Convolution2D(Image &Img, Kernel Knl) {
  std::vector<std::vector<RGBData>> TempVec = Img.PixelVec;

  for(int y = 0; y < Img.Height; y++) {
    for(int x = 0; x < Img.Width; x++)  {
      double CurrentPixelR = 0.0;
      double CurrentPixelG = 0.0;
      double CurrentPixelB = 0.0;
      for(int i = 0; i < Knl.SizeY; i++) {
        for(int j = 0; j < Knl.SizeX; j++) {
          int CurrentY = y + (-floor((double)Knl.SizeY/2.0) + i);
          int CurrentX = x + (-floor((double)Knl.SizeX/2.0) + j);

          if((CurrentX < 0 || CurrentY < 0) || (CurrentX > Img.Width-1 || CurrentY > Img.Height-1)) {
            //std::cout << 0.0 << std::endl;
            CurrentPixelR += 0.0;
            CurrentPixelG += 0.0;
            CurrentPixelB += 0.0;
          } else {
            CurrentPixelR += ((double)Img.PixelVec[CurrentY][CurrentX].R/255.0)*(Knl.KernelData[j][i]);
            CurrentPixelG += ((double)Img.PixelVec[CurrentY][CurrentX].G/255.0)*(Knl.KernelData[j][i]);
            CurrentPixelB += ((double)Img.PixelVec[CurrentY][CurrentX].B/255.0)*(Knl.KernelData[j][i]);
          }          
        }
      }
      ConvertToUnsignedChar(CurrentPixelR, Knl);
      ConvertToUnsignedChar(CurrentPixelG, Knl);
      ConvertToUnsignedChar(CurrentPixelB, Knl);
      TempVec[y][x].R = CurrentPixelR;
      TempVec[y][x].G = CurrentPixelG;
      TempVec[y][x].B = CurrentPixelB;
    }
  }
  Img.PixelVec = TempVec;
};


void CreateBitImage(Image &Img) {
  std::vector<std::vector<double>>
    EdgeData = {{ -1.0, -1.0, -1.0},
                {-1.0, 8.0, -1.0},
                { -1.0, -1.0, -1.0}};
  Kernel EdgeDetect(EdgeData);

  std::vector<std::vector<double>>
    BlurData = {{ 1.0, 1.0, 1.0},
                {1.0, 1.0, 1.0},
                { 1.0, 1.0, 1.0}};
  Kernel Blur(BlurData);

  std::vector<std::vector<double>>
    GaussianBlurData = {{1, 4, 6, 4, 1},
                        {4, 16, 24, 16, 4},
                        {6, 24, 36, 24, 6},
                        {4, 16, 24, 16, 4},
                        {1, 4, 6, 4, 1}};
  Kernel GaussianBlur(GaussianBlurData);


  Img.ImageToGreyScale();
  Img.SaveImage("Output/Grey1.png");

  Convolution2D(Img, GaussianBlur);
  Img.SaveImage("Output/Blur1.png");
  
  Convolution2D(Img, EdgeDetect);
  Img.SaveImage("Output/EdgeDetect.png");

  Img.ImageToEdgeBits(127);
  Img.SaveImage("Output/EdgeBits.png");
}




int main() {
  // Gets the edge bit image
  Image Img("Images/Spirited1.png");
  std::cout << Img.Channels << std::endl;
  Img.SaveImage("Output/Input.png");
  CreateBitImage(Img);

  // Turn into a SVG
  
  return 0;
}
