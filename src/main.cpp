#include <string>
#include <vector>
#include "image.cpp"
#include "maths.cpp"


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

  // Turn into a set of cubic equations



  
  return 0;
}
