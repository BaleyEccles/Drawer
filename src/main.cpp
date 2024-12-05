#include <cmath>
#include <string>
#include <vector>
#include "image.cpp"
#include "image.h"
#include "maths.cpp"
#include "maths.h"


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
    SharpenData = {{ 1.0, 1.0, 1.0},
                   {1.0, 1.0, 1.0},
                   { 1.0, 1.0, 1.0}};
  Kernel Sharpen(SharpenData);

  std::vector<std::vector<double>>
    RemoveNoiseData = {{ 1.0, 1.0, 1.0},
                       {1.0, 0.0, 1.0},
                       { 1.0, 1.0, 1.0}};
  Kernel RemoveNoise(RemoveNoiseData);

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

  Convolution2D(Img, Sharpen);
  Img.SaveImage("Output/SharpenEdge.png");
  
  Img.ImageToEdgeBits(127);
  Img.SaveImage("Output/EdgeBits.png");
};


int main() {
  // Gets the edge bit image
  Image Img("Images/Totoro2.png");
  Img.SaveImage("Output/Input.png");
  CreateBitImage(Img);

  
  return 0;
}
