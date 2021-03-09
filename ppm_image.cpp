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
      //new row is height of image - old row
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
    //iterates through limited portion of original image
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
      //if row is out of range of original image dimensions, continue
      if(row >= wid) {
         continue;
      }
       for(int col = startx; col < startx + w; col++) {
          if(col >= hgt) {
             i++;
             continue;
          }
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

ppm_image ppm_image::invert() const
{
   ppm_image result(wid,hgt);
   result.image_arr = new ppm_pixel[wid*hgt];
   ppm_pixel pixel;
   for(int i = 0; i < wid*hgt; i++) {
      pixel = image_arr[i];
      result.image_arr[i] = ppm_pixel{(unsigned char)(255-pixel.r), (unsigned char)(255-pixel.g),(unsigned char)(255-pixel.b)};
   }
   return result;
}

ppm_image ppm_image::sobel() const
{
   ppm_image result(wid,hgt);
   result.image_arr = new ppm_pixel[wid*hgt];
   int left, right, up, down;
   int row, col;
   int r1, g1, b1, r2, g2, b2, r, g, b;
   for(int i = 0; i < wid*hgt; i++) {
      row = floor(i/wid);
      col = i%wid;
      left = col-1;
		right = col+1;
		up = row-1;
		down = row+1;
      //limit index values if they are outside range of image
		if(left < 0) {
		    left = 0;
		}
		if(up < 0) {
		    up = 0;
		}
		if(right >= this->width()) {
		    right = this->width()-1;
		}
		if(down >= this->height()) {
		    down = this->height()-1;
		}
      //do calculation using two kernels and given formula
      r1 = (int)this->get(up, left).r - (int)this->get(up, right).r + 2*(int)this->get(row, left).r 
         - 2*(int)this->get(row, right).r + (int)this->get(down, left).r - (int)this->get(down, right).r;
      g1 = (int)this->get(up, left).g - (int)this->get(up, right).g + 2*(int)this->get(row, left).g 
         - 2*(int)this->get(row, right).g + (int)this->get(down, left).g - (int)this->get(down, right).g;
      b1 = (int)this->get(up, left).b - (int)this->get(up, right).b + 2*(int)this->get(row, left).b 
         - 2*(int)this->get(row, right).b + (int)this->get(down, left).b - (int)this->get(down, right).b;
      r2 = (int)this->get(up, left).r + 2*(int)this->get(up, col).r + (int)this->get(up, right).r 
         - (int)this->get(down, left).r - 2*(int)this->get(down, col).r - (int)this->get(down, right).r;
      g2 = (int)this->get(up, left).g + 2*(int)this->get(up, col).g + (int)this->get(up, right).g 
         - (int)this->get(down, left).g - 2*(int)this->get(down, col).g - (int)this->get(down, right).g;
      b2 = (int)this->get(up, left).b + 2*(int)this->get(up, col).b + (int)this->get(up, right).b 
         - (int)this->get(down, left).b - 2*(int)this->get(down, col).b - (int)this->get(down, right).b;

      r = pow(r1,2) + pow(r2, 2);
      r = int(pow(r, 0.5));
      g = pow(g1,2) + pow(g2, 2);
      g = int(pow(g, 0.5));
      b = pow(b1,2) + pow(b2, 2);
      b = int(pow(b, 0.5));

      r = limit_value(r);
      g = limit_value(g);
      b = limit_value(b);

      result.set(row,col,ppm_pixel{(unsigned char)r,(unsigned char)g,(unsigned char)b});
   }  
   return result;
}

ppm_image ppm_image::box_blur() const
{
   ppm_image result(wid,hgt);
   result.image_arr = new ppm_pixel[wid*hgt];
   int left, right, up, down;
   int row, col;
   int r, g, b;
   float weight = 1.0/9.0;

   for(int i = 0; i < wid*hgt; i++) {
      row = floor(i/wid);
      col = i%wid;
      left = col-1;
		right = col+1;
		up = row-1;
		down = row+1;
		if(left < 0) {
		    left = 0;
		}
		if(up < 0) {
		    up = 0;
		}
		if(right >= this->width()) {
		    right = this->width()-1;
		}
		if(down >= this->height()) {
		    down = this->height()-1;
		}
      //each pixel is the average of its neighbors
      r = weight*(this->get(up, left).r + this->get(up, col).r + this->get(up, right).r 
      + this->get(row, left).r + this->get(row, col).r + this->get(row, right).r 
      + this->get(down, left).r + this->get(down, col).r + this->get(down, right).r);

      g = weight*(this->get(up, left).g + this->get(up, col).g + this->get(up, right).g 
      + this->get(row, left).g + this->get(row, col).g + this->get(row, right).g 
      + this->get(down, left).g + this->get(down, col).g + this->get(down, right).g);

      b = weight*(this->get(up, left).b + this->get(up, col).b + this->get(up, right).b 
      + this->get(row, left).b + this->get(row, col).b + this->get(row, right).b 
      + this->get(down, left).b + this->get(down, col).b + this->get(down, right).b);

      r = limit_value(r);
      g = limit_value(g);
      b = limit_value(b);

      result.set(row,col,ppm_pixel{(unsigned char)r,(unsigned char)g,(unsigned char)b});
   }
   return result;
}

ppm_image ppm_image::swirl_colors() const
{
   ppm_image result(wid,hgt);
   result.image_arr = new ppm_pixel[wid*hgt];
   ppm_pixel pixel;
   unsigned char r,g,b;
   for(int i = 0; i < wid*hgt; i++) {
      pixel = image_arr[i];
      //rotate color channels
      r = pixel.g;
      g = pixel.b;
      b = pixel.r;
      result.image_arr[i] = ppm_pixel{r,g,b};
   }
   return result;
}

ppm_image ppm_image::swirl_colors2() const
{
   ppm_image result(wid,hgt);
   result.image_arr = new ppm_pixel[wid*hgt];
   ppm_pixel pixel;
   unsigned char r,g,b;
   for(int i = 0; i < wid*hgt; i++) {
      pixel = image_arr[i];
      //rotate color channels
      r = pixel.b;
      g = pixel.r;
      b = pixel.g;
      result.image_arr[i] = ppm_pixel{r,g,b};
   }
   return result;
}

ppm_image ppm_image::rainbow() const
{
   ppm_image result(wid,hgt);
   result.image_arr = new ppm_pixel[wid*hgt];
   ppm_pixel pixel;
   int row;
   //draw horizontal rainbow stripes, thickness based on height of image
   for(int i = 0; i < wid*hgt; i++) {
      row = floor(i/wid);
      pixel = image_arr[i];
      if(row < hgt/6.0) {
         //red
         result.image_arr[i] = ppm_pixel{255, 0, 0};
      }
      else if(row < 2*hgt/6.0) {
         //orange
         result.image_arr[i] = ppm_pixel{255, 128, 0};
      }
      else if(row < 3*hgt/6.0) {
         //yellow
         result.image_arr[i] = ppm_pixel{255, 255, 0};
      }
      else if(row < 4*hgt/6.0) {
         //green
         result.image_arr[i] = ppm_pixel{0, 255, 0};
      }
      else if(row < 5*hgt/6.0) {
         //blue
         result.image_arr[i] = ppm_pixel{0, 0, 255};
      }
      else {
         //purple
         result.image_arr[i] = ppm_pixel{128, 0, 255};
      }
   }
   //use alpha blend to overlay rainbow image on original image
   result = this->alpha_blend(result, 0.5f);
   return result;
}

ppm_image ppm_image::black_border(int thickness) const
{
   ppm_image result(wid,hgt);
   result.image_arr = new ppm_pixel[wid*hgt];
   int row,col;
   for(int i = 0; i < wid*hgt; i++) {
      row = floor(i/wid);
      col = i%wid;
      //if pixel is on the edge of image, color in black
      if(row < thickness || row > hgt - thickness || col < thickness || col > wid - thickness) {
         result.image_arr[i] = ppm_pixel{0,0,0};
      } else {
         result.image_arr[i] = image_arr[i];
      }
   }
   return result;
}


ppm_image ppm_image::combine(const ppm_image& other) const 
{
   int wid2 = other.width();
   int hgt2 = other.height();
   if(hgt != hgt2){
      cout << "incompatible heights" << endl;
      //return empty ppm_image object if heights are not the same
      return ppm_image();
   }
   ppm_image result(wid+wid2,hgt);
   result.image_arr = new ppm_pixel[(wid+wid2)*hgt];
   int row,col;
   ppm_pixel pixel;
   for(int i = 0; i < (wid+wid2)*hgt; i++) {
      row = floor(i/(wid+wid2));
      col = i%(wid+wid2);
      //if column number is less than the width of original image, set pixel of original image
      if(col<wid) {
         pixel = this->get(row,col);
         result.set(row,col,pixel);
         //if column number is greater than the width of original image, set pixel to second image
      } else {
         pixel = other.get(row,col-wid);
         result.set(row,col,pixel);
      }
   }
   return result;
}

int ppm_image::limit_value(int value) const
{
   if(value > 255) {
      return 255;
   }
   else if(value < 0) {
      return 0;
   }
   else {
      return value;
   }
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