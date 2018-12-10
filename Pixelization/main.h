#pragma once
#include "base.h"
#include "SuperPixel.h"
#include "Slic.h"
#include "SPImage.h"
#include "PicImage.h"
#include "Palette.h"
#include "Pixel.h"

#define IMG_PATH "../Images/"
#define RESULTS_PATH "../Results/"
#define FINAL_TEMP 1
#define SP_DATA_XCOOR 0
#define SP_DATA_YCOOR 1
#define SP_DATA_LVAL 2
#define SP_DATA_AVAL 3
#define SP_DATA_BVAL 4
#define SP_DATA_COUNT 5

cv::Scalar getMeanColor(cv::Mat image);
double initPCA(cv::Mat image, cv::Scalar* start1, cv::Scalar* start2);
void initializeSP(PicImage* pimage, SPImage* spImage, int size);
void runAlgo(std::string filename, int spSize, int displayScale);