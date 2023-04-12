#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    // TODO
    for (int i = 0; i < im.c; i ++) {
        for (int j = 0; j < im.w; j ++) {
            for (int k = 0; k < im.h; k ++) {
                set_pixel(im, i, j, k, 1.0 /(im.c * im.h * im.w));
            }
        }
    }
}

image make_box_filter(int w)
{
    // TODO
    image im = make_image(1, w, w);
    l1_normalize(im);
    return im;
}

int calcSum(int i, int j, int k, image im, image filter, int preserve) {

    float sum = 0;
    int wOffset = (filter.w - 1) / 2;
    int hOffset = (filter.h - 1) / 2;

    if(preserve != 1){
        // im.c instead of filter.c because we consider the case where filter.c == 1 but im.c != 1
        for(int a = 0; a < im.c; a ++) {
            for(int b = 0; b < filter.h; b ++) {
                for(int c = 0; c < filter.w; c ++) {
                    sum += get_pixel(im, a, j + b - wOffset, k + c - hOffset) * get_pixel(filter, a, b, c);
                }
            }
        }
    } else {
        for(int b = 0; b < filter.h; b ++) {
            for(int c = 0; c < filter.w; c ++) {
                sum += get_pixel(im, i, j + b - wOffset, k + c - hOffset) * get_pixel(filter, i, b, c);
            }
        }
    }
    return sum;
}

image convolve_image(image im, image filter, int preserve)
{
    // TODO
    if(filter.w % 2 == 0 || filter.h % 2 == 0) {
        fprintf(stderr, "filter size must be odd");
    }

    float sum = 0;
    image newIm;
    if(preserve == 1) {
        newIm = make_image(im.c, im.h, im.w);
        for(int i = 0; i < im.c; i ++) {
            for(int j = 0; j < im.h; j ++) {
                for(int k = 0; k < im.w; k ++) {
                    sum = calcSum(i, j, k, im, filter, preserve);
                    set_pixel(newIm, i, j, k, sum);
                }
            }
        }
    } else {
        newIm = make_image(1, im.h, im.w);
        for(int j = 0; j < im.h; j ++) {
            for(int k = 0; k < im.w; k ++) {
                sum = calcSum(0, j, k, im, filter, preserve);
                set_pixel(newIm, 0, j, k, sum);
            }
        }
    }
    return newIm;
}

image make_highpass_filter()
{
    // TODO
    return make_image(1,1,1);
}

image make_sharpen_filter()
{
    // TODO
    return make_image(1,1,1);
}

image make_emboss_filter()
{
    // TODO
    return make_image(1,1,1);
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)
{
    // TODO
    return make_image(1,1,1);
}

image add_image(image a, image b)
{
    // TODO
    return make_image(1,1,1);
}

image sub_image(image a, image b)
{
    // TODO
    return make_image(1,1,1);
}

image make_gx_filter()
{
    // TODO
    return make_image(1,1,1);
}

image make_gy_filter()
{
    // TODO
    return make_image(1,1,1);
}

void feature_normalize(image im)
{
    // TODO
}

image *sobel_image(image im)
{
    // TODO
    return calloc(2, sizeof(image));
}

image colorize_sobel(image im)
{
    // TODO
    return make_image(1,1,1);
}
