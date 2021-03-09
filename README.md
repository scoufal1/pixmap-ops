# pixmap-ops

Image manipulation demos based on the PPM image format.

TODO: Add a representative image for your project here

## How to build

*Windows*

Open git bash to the directory containing this repository.

```
pixmap-ops $ mkdir build
pixmap-ops $ cd build
pixmap-ops/build $ cmake -G "Visual Studio 16 2019" ..
pixmap-ops/build $ start Pixmap-Ops.sln
```

Your solution file should contain two projects: `pixmap_art` and `pixmap_test`.
To run from the git bash command shell, 

```
pixmap-ops/build $ ../bin/Debug/pixmap_test
pixmap-ops/build $ ../bin/Debug/pixmap_art
```

*macOS*

Open terminal to the directory containing this repository.

```
pixmap-ops $ mkdir build
pixmap-ops $ cd build
pixmap-ops/build $ cmake ..
pixmap-ops/build $ make
```

To run each program from build, you would type

```
pixmap-ops/build $ ../bin/pixmap_test
pixmap-ops/build $ ../bin/pixmap_art
```

## Image operators

<b>Original Image:</b>

<img src="https://user-images.githubusercontent.com/48161551/110398828-a3d87180-8042-11eb-83bd-9c7876de85fe.jpg" alt="oboe" width="300"/>

```
ppm_image invert();
```
Each color channel is subtracted from the maximum value to invert the colors.

<img src="https://user-images.githubusercontent.com/48161551/110400053-f74bbf00-8044-11eb-9ae5-1eb356c16b1f.jpg" alt="oboe" width="300"/>

```
ppm_image sobel();
```

Applies the Sobel operator using two kernels.

<img src="https://user-images.githubusercontent.com/48161551/110400566-d59f0780-8045-11eb-90e1-8092d10c1bc2.jpg" alt="oboe" width="300"/>

```
ppm_image box_blur();
```

Each pixel is set to the average of its neighboring pixels to create a blurring effect.

 <img src="https://user-images.githubusercontent.com/48161551/110399353-b7380c80-8043-11eb-9467-001e4555de61.jpg" alt="oboe" width="300"/>

```
ppm_image swirl_colors();
```

Color channels are rotated such that the red channel becomes the blue channel, the green becomes red, and the blue becomes green.

 <img src="https://user-images.githubusercontent.com/48161551/110400946-9e7d2600-8046-11eb-86ed-c4d6942e648d.jpg" alt="oboe" width="300"/>
 
```
ppm_image swirl_colors2();
```

Color channels are rotated such that the red channel becomes the green channel, the green becomes blue, and the blue becomes red.

 <img src="https://user-images.githubusercontent.com/48161551/110400966-accb4200-8046-11eb-9174-f10daf8b24d6.jpg" alt="oboe" width="300"/>
 
```
ppm_image rainbow();
```

Overlays horizontal rainbow stripes on image.

 <img src="https://user-images.githubusercontent.com/48161551/110401381-780bba80-8047-11eb-8966-826a1cf4058d.jpg" alt="oboe" width="300"/>
 
```
ppm_image black_border(int thickness);
```

Applies a black border of given thickness on image.

 <img src="https://user-images.githubusercontent.com/48161551/110401648-f49e9900-8047-11eb-93c9-7524a53c055d.jpg" alt="oboe" width="300"/>
 
```
ppm_image combine(const ppm_image& other);
```
Puts given image to the right of original image. Assumes heights of both images are equal.

## Results

Unique image created by applying box blur, both swirl colors operators, sobel, combine, and black border.

<img src="https://user-images.githubusercontent.com/48161551/110401988-98884480-8048-11eb-8fd0-e4a9c8790869.jpg" alt="oboe" width="900"/>


