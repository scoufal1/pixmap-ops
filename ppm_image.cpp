#include "ppm_image.h"
#include <string>
#include <fstream>
#include <iostream>

using namespace agl;
using namespace std;

ppm_image::ppm_image() 
{
   wid = 0;
   hgt = 0;
}

ppm_image::ppm_image(int w, int h) 
{
   wid = w;
   hgt = h;
}

ppm_image::ppm_image(const ppm_image& orig)
{
   wid = orig.width();
   hgt = orig.height();
   image_arr = new ppm_pixel[wid*hgt];
   for (int i = 0; i < wid*hgt; i++) {
        image_arr[i] = orig.image_arr[i];
    }
}

ppm_image& ppm_image::operator=(const ppm_image& orig)
{
   if (&orig == this) // protect against self-assignment
   {
      return *this;
   }
   delete[] image_arr;
   wid = orig.width();
   hgt = orig.height();
   image_arr = new ppm_pixel[wid*hgt];
   for (int i = 0; i < wid*hgt; i++) {
        image_arr[i] = orig.image_arr[i];
    }

   return *this;   
}

ppm_image::~ppm_image()
{
   delete[] image_arr;
}

bool ppm_image::load(const std::string& filename)
{
   ifstream file(filename);
   if(!file) {
      cout << "Cannot load file: " << filename << endl;
      return false;
   }
   string imgType;
   file >> imgType;
   int maxColor;
   int numPixels = 0;
   if(imgType == "P3") {
      file >> wid;
      file >> hgt;
      file >> maxColor;
      delete[] image_arr;
      image_arr = new ppm_pixel[wid*hgt];
      numPixels = wid*hgt;
      int r,g,b;
      for(int i = 0; i < numPixels; i++) {
         file >> r >> g >> b;
         image_arr[i] = ppm_pixel{(unsigned char)r,(unsigned char)g,(unsigned char)b};
      }
   }
   file.close();
   return true;
}

bool ppm_image::save(const std::string& filename) const
{  
   ofstream file;
   file.open(filename);
   if(!file.is_open()) {
      cout << "Cannot open file: " << filename << endl;
      return false;
   }
   file << "P3\n" << wid << " " << hgt << "\n" << "255\n";
  
   int numPixels = wid*hgt;
   for(int i = 0; i < numPixels; i++) {
         file << (int)image_arr[i].r << " ";
         file << (int)image_arr[i].g << " ";
         file << (int)image_arr[i].b << " ";
   }  
   file.close();
   return true;
}

 ppm_image ppm_image::resize(int w, int h) const
{
   ppm_image result(w,h);
   result.image_arr = new ppm_pixel[w*h];
   int row1;
   int row2;
   int col1;
   int col2;
   ppm_pixel pixel;
   for(int i = 0; i < w*h; i++) {
      //calculate row and column number of new image
      row2 = floor(i/w);
      col2 = i%w;
      //formula based on ratios
      row1 = floor(((float)row2/(h-1))*(hgt-1));
      col1 = floor((float)col2/(w-1)*(wid-1));
      //pixel in original image
      pixel = image_arr[wid*row1 + col1];
      result.image_arr[i] = pixel;
   }
   return result;
}

ppm_image ppm_image::flip_horizontal() const
{
   ppm_image result(wid,hgt);
   result.image_arr = new ppm_pixel[wid*hgt];
   ppm_pixel pixel;
   int row1;
   int row2;
   int col;
   for(int i = 0; i < wid*hgt; i++) {
      row1 = floor(i/wid);
      row2 = hgt-row1;
      col = i%wid;
      pixel = image_arr[i];
      result.set(row2,col,pixel);
   }

   return result;
}

ppm_image ppm_image::subimage(int startx, int starty, int w, int h) const
{
    ppm_image result(w,h);
    result.image_arr = new ppm_pixel[w*h];
    ppm_pixel pixel;
    int i = 0;
    for(int row = starty; row < starty + h; row++) {
       for(int col = startx; col < startx + w; col++) {
          pixel = this->get(row,col);
          result.image_arr[i] = pixel;
          i++;
       }
    }
    return result;
}

void ppm_image::replace(const ppm_image& image, int startx, int starty)
{
   int h = image.height();
   int w = image.width();
   int i = 0;
   ppm_pixel pixel;
   for(int row = starty; row < starty + h; row++) {
       for(int col = startx; col < startx + w; col++) {
          pixel = image.image_arr[i];
          this->set(row,col,pixel);
          i++;
       }
   }
}

ppm_image ppm_image::alpha_blend(const ppm_image& other, float alpha) const
{
   ppm_image result(wid, hgt);
   ppm_pixel pixel1;
   ppm_pixel pixel2;
   result.image_arr = new ppm_pixel[wid*hgt];
   int r,g,b,r1,g1,b1,r2,b2,g2;
   for(int i = 0; i < wid*hgt; i++) {
      pixel1 = image_arr[i];
      pixel2 = other.image_arr[i];
      r1 = (int)pixel1.r;
      g1 = (int)pixel1.g;
      b1 = (int)pixel1.b;
      r2 = (int)pixel2.r;
      g2 = (int)pixel2.g;
      b2 = (int)pixel2.b;
      r = 255*((r1/255.0)*(1-alpha)+(r2/255.0)*alpha);
      g = 255*((g1/255.0)*(1-alpha)+(g2/255.0)*alpha);
      b = 255*((b1/255.0)*(1-alpha)+(b2/255.0)*alpha);
      result.image_arr[i] = ppm_pixel{(unsigned char)r,(unsigned char)g,(unsigned char)b};
   }
   return result;
}

ppm_image ppm_image::gammaCorrect(float gamma) const
{
   ppm_image result(wid, hgt);
   ppm_pixel pixel;
   result.image_arr = new ppm_pixel[wid*hgt];
   int r,g,b;
   for(int i = 0; i < wid*hgt; i++) {
      pixel = image_arr[i];
      r = (int)pixel.r;
      g = (int)pixel.g;
      b = (int)pixel.b;
      r = 255*pow(r/255.0, 1.0/gamma);
      g = 255*pow(g/255.0, 1.0/gamma);
      b = 255*pow(b/255.0, 1.0/gamma);
      result.image_arr[i] = ppm_pixel{(unsigned char)r,(unsigned char)g,(unsigned char)b};
   }
   return result;
}

ppm_image ppm_image::grayscale() const
{
   ppm_image result(wid,hgt);
   result.image_arr = new ppm_pixel[wid*hgt];
   ppm_pixel pixel;
   unsigned char weightedAvg;
   for(int i = 0; i < wid*hgt; i++) {
      pixel = image_arr[i];
      weightedAvg = (unsigned char)(0.3*pixel.r+0.59*pixel.g+0.11*pixel.b);
      result.image_arr[i] = {weightedAvg, weightedAvg, weightedAvg};
   }

   return result;
}

ppm_pixel ppm_image::get(int row, int col) const
{
   //Row i, column j in 2d array corresponds to image[column_count*i + j]
   return image_arr[wid*row + col];
}

void ppm_image::set(int row, int col, const ppm_pixel& c)
{
   image_arr[wid*row + col] = c;
}

int ppm_image::height() const
{
   return hgt;
}

int ppm_image::width() const
{
   return wid;
}
