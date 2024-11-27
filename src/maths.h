#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "image.h"

class Kernel {
public:
  Kernel(std::vector<std::vector<double>> Data);
  unsigned int SizeX, SizeY;
  std::vector<std::vector<double>> KernelData;
  double Min = 0.0;
  double Max = 0.0;

private:
  void GetMinAndMax();
};


// Scale the output from kernel min/max to 0->255
void ConvertToUnsignedChar(double &Num, Kernel Knl);

void Convolution2D(Image &Img, Kernel Knl);

struct Point {
  int x, y;
};

struct Spline {
  double a, b, c, d, x;
};


std::vector<Spline> CubicSpline(std::vector<Point> C);
