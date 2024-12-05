#pragma once
#include "maths.h"


Kernel::Kernel(std::vector<std::vector<double>> Data)
  :KernelData(Data) {
  SizeY = KernelData.size();
  SizeX = KernelData[0].size();
  GetMinAndMax();
};


void Kernel::GetMinAndMax() {
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




std::vector<Spline> CubicSpline(std::vector<Point> C) {
  // From https://en.wikipedia.org/wiki/Spline_(mathematics)#Algorithm_for_computing_natural_cubic_splines
  // Input: set of coordinates C, with |C| = n + 1
  int SizeC = C.size();
  int n = SizeC;
  // Output: set splines which is composed of n 5-tuples.

  //     Create new array a of size n + 1 and for i = 0, …, n set a_{i}=y_{i}
  std::vector<int> a;
  a.reserve(n + 1);
  for (int i = 0; i < n + 1; i++) {
    a.push_back(C[i].y);
  }
  //     Create new arrays b and d, each of size n.
  std::vector<int> b, d;
  b.reserve(n);
  d.reserve(n);
  //     Create new array h of size n and for i = 0, …, n – 1 set h_{i}=x_{i+1}-x_{i}
  std::vector<int> h;
  h.reserve(n);
  for (int i = 0; i < n; i++) {
    h.push_back(C[i + 1].x - C[i].x);
  }
  //     Create new array α of size n and for i = 1, …, n – 1 set \alpha _{i}={\frac {3}{h_{i}}}(a_{i+1}-a_{i})-{\frac {3}{h_{i-1}}}(a_{i}-a_{i-1}).}
  std::vector<int> alpha;
  alpha.reserve(n);
  for (int i = 1; i < n; i++) {
    double output = (3 / h[i])*(a[i+1] - a[i]) - (3 / h[i-1])*(a[i] - a[i-1]);
    alpha.push_back(output);
  }
  //     Create new arrays c, l, μ, z, each of size n + 1.
  std::vector<double> c, l, mu, z;
  c.reserve(n + 1);
  l.reserve(n + 1);
  mu.reserve(n + 1);
  z.reserve(n + 1);
  //     Set l 0 = 1 , μ 0 = z 0 = 0 {\displaystyle l_{0}=1,{\mu }_{0}=z_{0}=0\,}
  l.push_back(0.0);
  mu.push_back(0.0);
  z.push_back(0.0);
  //     For i = 1, …, n – 1 set the following:
  for (int i = 1; i < n; i++) {
    // l_{i} = 2(x_{i+1}-x_{i-1})-h_{i-1}\mu _{i-1}
    l[i] = 2*(C[i+1].x - C[i-1].x - h[i-1] * mu[i-1]);
    // mu _{i}&={\frac {h_{i}}{l_{i}}}
    mu[i] = h[i]/l[i];
    // z_{i}&={\frac {\alpha _{i}-h_{i-1}z_{i-1}}{l_{i}}}
    z[i] = (alpha[i] - h[i-1] * z[i-1])/l[i];
  }
  
  //     Set l_{n}=1;z_{n}=c_{n}=0
  l[n] = 1;
  z[n] = 0;
  c[n] = 0;

  //     For j = n – 1, n – 2, …, 0, set the following:
  for (int i = 1; i < n; i++) {
    int j = n - i;
    // c_{j}&=z_{j}-\mu _{j}c_{j+1}
    c[j] = z[j] - mu[j] * c[j+1];
    // b_{j}&={\frac {a_{j+1}-a_{j}}{h_{j}}}-{\frac {h_{j}(c_{j+1}+2c_{j})}{3}}
    b[j] = ((a[j+1] - a[j]) / (h[j])) * ((h[j] * (c[j+1] + 2*c[j])) / (3.0));
    // d_{j}&={\frac {c_{j+1}-c_{j}}{3h_{j}}}
    d[j] = (c[j+1]-c[j]) / (3.0*h[j]);
  }
  //     Create new set Splines and call it output_set. Populate it with n splines S.
  std::vector<Spline> OutputSet;
  //     For i = 0, …, n – 1 set the following:
  for (int i = 0; i < n; i++) {
    Spline S;
    // S_{i,a}=a_{i}
    S.a = a[i];
    // S_{i,b}=b_{i}
    S.b = b[i];
    // S_{i,c}=c_{i}
    S.c = c[i];
    // S_{i,d}=d_{i}
    S.d = d[i];
    // S_{i,x}=x_{i}
    S.x = C[i].x;

    OutputSet.push_back(S);
  }
  // Output output_set
  return OutputSet;
};


double Magnitude(Point P) { return sqrt((P.x ^ 2) + (P.y ^ 2)); };

Point Subtract(Point P1, Point P2) {
  Point P;
  P.x = P1.x - P2.x;
  P.y = P1.y - P2.y;
  return P;
}

Point Add(Point P1, Point P2) {
  Point P;
  P.x = P1.x + P2.x;
  P.y = P1.y + P2.y;
  return P;
}


