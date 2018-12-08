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
#define TEMP_CHANGE_THRESH 0.005

cv::Scalar getMeanColor(cv::Mat image);
double getStartTemp(cv::Mat image);