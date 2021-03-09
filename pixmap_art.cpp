#include <iostream>
#include "ppm_image.h"
using namespace std;
using namespace agl;

int main(int argc, char** argv)
{
    ppm_image cat;
    cat.load("../images/oboe.ppm");
    
    ppm_image sobel = cat.sobel();
    sobel.save("oboe-sobel.ppm");

    ppm_image swirl = cat.swirl_colors();
    swirl.save("oboe-swirl.ppm");

    ppm_image swirl2 = cat.swirl_colors2();
    swirl2.save("oboe-swirl2.ppm");

    ppm_image rainbow = cat.rainbow();
    rainbow.save("oboe-rainbow.ppm");

    ppm_image blur = cat.box_blur();
    blur.save("oboe-blur.ppm");

    ppm_image invert = cat.invert();
    invert.save("oboe-invert.ppm");

    ppm_image border = cat.black_border(60);
    border.save("oboe-border.ppm");

    //artwork
    ppm_image image1 = cat.box_blur().swirl_colors().sobel();
    ppm_image image2 = cat.box_blur().sobel();
    ppm_image image3 = cat.box_blur().swirl_colors2().sobel();

    ppm_image combine = image1.combine(image2).combine(image3).black_border(60);
    combine.save("oboe-combine.ppm");
}

