#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853



void l1_normalize(image im)
{
    float sum = 0;
    for (int i = 0; i < im.c; i ++) {
        for (int j = 0; j < im.w; j ++) {
            for (int k = 0; k < im.h; k ++) {
                sum += get_pixel(im, i, j, k);
            }
        }
    }
    for (int i = 0; i < im.c; i ++) {
        for (int j = 0; j < im.w; j ++) {
            for (int k = 0; k < im.h; k ++) {
                set_pixel(im, i, j, k, get_pixel(im, i, j, k) / sum);
            }
        }
    }
}

image make_box_filter(int w)
{
    // TODO
    image im = make_image(1, w, w);
    for(int i = 0; i < w; i ++) {
        for(int j = 0; j < w; j ++) {
            set_pixel(im, 0, i, j, 1/(float)(w*w));
        }
    }
    return im;
}

float calcSum(int i, int j, int k, image im, image filter, int preserve) {

    float sum = 0;
    // I assume filter size is odd
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

    float sum2 = 0;
    image newIm;
    if(preserve == 1) {
        newIm = make_image(im.c, im.h, im.w);
        for(int i = 0; i < im.c; i ++) {
            for(int j = 0; j < im.h; j ++) {
                for(int k = 0; k < im.w; k ++) {
                    sum2 = calcSum(i, j, k, im, filter, preserve);
                    set_pixel(newIm, i, j, k, sum2);
                }
            }
        }
    } else {
        newIm = make_image(1, im.h, im.w);
        for(int j = 0; j < im.h; j ++) {
            for(int k = 0; k < im.w; k ++) {
                sum2 = calcSum(0, j, k, im, filter, preserve);
                set_pixel(newIm, 0, j, k, sum2);
            }
        }
    }
    return newIm;
}

image make_highpass_filter()
{
    // TODO
    image new = make_image(1,3,3);
    set_pixel(new, 0, 0, 0, 0);
    set_pixel(new, 0, 0, 1, -1);
    set_pixel(new, 0, 0, 2, 0);
    set_pixel(new, 0, 1, 0, -1);
    set_pixel(new, 0, 1, 1, 4);
    set_pixel(new, 0, 1, 2, -1);
    set_pixel(new, 0, 2, 0, 0);
    set_pixel(new, 0, 2, 1, -1);
    set_pixel(new, 0, 2, 2, 0);
    return new;

}

image make_sharpen_filter()
{
    image new = make_image(1,3,3);
    set_pixel(new, 0, 0, 0, 0);
    set_pixel(new, 0, 0, 1, -1);
    set_pixel(new, 0, 0, 2, 0);
    set_pixel(new, 0, 1, 0, -1);
    set_pixel(new, 0, 1, 1, 5);
    set_pixel(new, 0, 1, 2, -1);
    set_pixel(new, 0, 2, 0, 0);
    set_pixel(new, 0, 2, 1, -1);
    set_pixel(new, 0, 2, 2, 0);
    return new;
}

image make_emboss_filter()
{
    image new = make_image(1,3,3);
    set_pixel(new, 0, 0, 0, -2);
    set_pixel(new, 0, 0, 1, -1);
    set_pixel(new, 0, 0, 2, 0);
    set_pixel(new, 0, 1, 0, -1);
    set_pixel(new, 0, 1, 1, 1);
    set_pixel(new, 0, 1, 2, 1);
    set_pixel(new, 0, 2, 0, 0);
    set_pixel(new, 0, 2, 1, 1);
    set_pixel(new, 0, 2, 2, 2);
    return new;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: 
// we should use preserve when we run convolution for the filters Emboss and Sharpen, because we want to preserve the color of the image.
// we should not use preserve when we run convolution for the filter Highpass, because we want the high frequncy indication of the image.
// the frequency should consider all color channels, so we should not use preserve.



// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: No we do not use any post-processing for the above filters, because all the post-processing is done in the convolution function.
// and the we we defined getPixel and setPixel.

float estimateGuassian(int x, int y, float sigma) {
    return 1 / (TWOPI * sigma * sigma) * exp(-(x * x + y * y) / (2 * sigma * sigma));
}


image make_gaussian_filter(float sigma)
{
    int filterSize = (int) (6 * sigma);
    if(filterSize % 2 == 0) {
        filterSize ++;
    }
    image new = make_image(1, filterSize, filterSize);

    for(int i = 0; i < filterSize; i ++) {
        for(int j = 0; j < filterSize; j ++) {
            set_pixel(new, 0, i, j, estimateGuassian(i - filterSize / 2, j - filterSize / 2, sigma));
        }
    }

    l1_normalize(new);    


    return new;
}

image add_image(image a, image b)
{
    // TODO
    image new = make_image(a.c,a.h,a.w);
    for(int i = 0; i < a.c; i ++) {
        for(int j = 0; j < a.h; j ++) {
            for(int k = 0; k < a.w; k ++) {
                set_pixel(new, i, j, k, get_pixel(a, i, j, k) + get_pixel(b, i, j, k));
            }
        }
    }
    return new;
}

image sub_image(image a, image b)
{
    // TODO
    image new = make_image(a.c,a.h,a.w);
    for(int i = 0; i < a.c; i ++) {
        for(int j = 0; j < a.h; j ++) {
            for(int k = 0; k < a.w; k ++) {
                set_pixel(new, i, j, k, get_pixel(a, i, j, k) - get_pixel(b, i, j, k));
            }
        }
    }
    return new;
}

image make_gx_filter()
{
    image new = make_image(1,3,3);
    set_pixel(new, 0, 0, 0, -1);
    set_pixel(new, 0, 0, 1, 0);
    set_pixel(new, 0, 0, 2, 1);
    set_pixel(new, 0, 1, 0, -2);
    set_pixel(new, 0, 1, 1, 0);
    set_pixel(new, 0, 1, 2, 2);
    set_pixel(new, 0, 2, 0, -1);
    set_pixel(new, 0, 2, 1, 0);
    set_pixel(new, 0, 2, 2, 1);
    return new;
}

image make_gy_filter()
{
    image new = make_image(1,3,3);
    set_pixel(new, 0, 0, 0, -1);
    set_pixel(new, 0, 0, 1, -2);
    set_pixel(new, 0, 0, 2, -1);
    set_pixel(new, 0, 1, 0, 0);
    set_pixel(new, 0, 1, 1, 0);
    set_pixel(new, 0, 1, 2, 0);
    set_pixel(new, 0, 2, 0, 1);
    set_pixel(new, 0, 2, 1, 2);
    set_pixel(new, 0, 2, 2, 1);
    return new;
}

void feature_normalize(image im)
{
    float min = get_pixel(im, 0, 0, 0);
    float max = get_pixel(im, 0, 0, 0);
    for (int i = 0; i < im.c; i++) {
        for (int j = 0; j < im.h; j++) {
            for (int k = 0; k < im.w; k++) {
                float pixel = get_pixel(im, i, j, k);
                if (pixel < min) {
                    min = pixel;
                }
                if (pixel > max) {
                    max = pixel;
                }
            }
        }
    }
    for (int i = 0; i < im.c; i++) {
        for (int j = 0; j < im.h; j++) {
            for (int k = 0; k < im.w; k++) {
                float pixel = get_pixel(im, i, j, k);
                if (max - min != 0) {
                    set_pixel(im, i, j, k, (pixel - min) / (max - min));
                } else {
                    set_pixel(im, i, j, k, 0);
                }
            }
        }
    }
}

image *sobel_image(image im)
{
    image im_gx = convolve_image(im, make_gx_filter(), 0);
    image im_gy = convolve_image(im, make_gy_filter(), 0);

    image *result = calloc(2, sizeof(image));
    result[0] = make_image(1, im.h, im.w);
    result[1] = make_image(1, im.h, im.w);

        for (int j = 0; j < im.h; j++) {
            for (int k = 0; k < im.w; k++) {
                float gx = get_pixel(im_gx, 0, j, k);
                float gy = get_pixel(im_gy, 0, j, k);
                float mag = sqrt(gx * gx + gy * gy);
                float theta = atan2(gy, gx);
                set_pixel(result[0], 0, j, k, mag);
                set_pixel(result[1], 0, j, k, theta);
            }
        }
    free_image(im_gx);
    free_image(im_gy);


    return result;
}

image colorize_sobel(image im)
{
    image im_gx = convolve_image(im, make_gx_filter(), 0);
    image im_gy = convolve_image(im, make_gy_filter(), 0);

    image result = make_image(3, im.h, im.w);


    for (int j = 0; j < im.h; j++) {
        for (int k = 0; k < im.w; k++) {
            float gx = get_pixel(im_gx, 0, j, k);
            float gy = get_pixel(im_gy, 0, j, k);
            float mag = sqrt(gx * gx + gy * gy);
            float theta = atan2(gy, gx);
            set_pixel(result, 0, j, k, theta);
            set_pixel(result, 1, j, k, 0.5);
            set_pixel(result, 2, j, k, mag);

        }
    }

    hsv_to_rgb(result);


    return result;
}
