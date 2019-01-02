# Pixelization
This is a program designed to take any image and recreate it in the art style of pixel art.  The primary constraints of
this style of artwork is the low resolution or the output, and the limited palette that it must adhere to.
This is done through an interleaved usage of the SLIC (Simple Linear Iterative Clustering) superpixel, 
and the MCDA (Mass Constrained Determininstic Annealing) algorithms.  The former controls the pixels associated from each
input pixel to output (super)pixel, and the latter controls the colors in the palette that the output must adhere to.


### To run the program:
Download the directory, and open in Microsoft Visual Studio.  Resolve any library dependencies, and proceed to run.


### Additional notes:
- Supported image types include most common image file types, but see 
[here](https://docs.opencv.org/3.0-beta/modules/imgcodecs/doc/reading_and_writing_images.html#imread)
for full list.
- Primary reference paper: https://dl.acm.org/citation.cfm?id=2330154
- Coded for CSE 522 Final Project, completing the requirements for CSE 528 Computer Graphics.
