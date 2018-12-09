#pragma once
#include "base.h"
#include "SuperPixel.h"
#include "Slic.h"
#include "SPImage.h"
#include "PicImage.h"
#include "Palette.h"
#include "Pixel.h"

#define IMG_PATH "../Images/"
#define FINAL_TEMP 1
#define TEMP_CHANGE_THRESH 0.05
#define SP_DATA_XCOOR 0
#define SP_DATA_YCOOR 1
#define SP_DATA_LVAL 2
#define SP_DATA_AVAL 3
#define SP_DATA_BVAL 4
#define SP_DATA_COUNT 5

cv::Scalar getMeanColor(cv::Mat image);
double getStartTemp(cv::Mat image);
void initializeSP(PicImage* pimage, SPImage* spImage, int size);