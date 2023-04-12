#include <math.h>
#include "image.h"

float nn_interpolate(image im, int c, float h, float w)
{
    // TODO
    int nearest_h = round(h);
    int nearest_w = round(w);
    return get_pixel(im, c, nearest_h, nearest_w);
}

image nn_resize(image im, int h, int w)
{
    float h_ratio = (float) (im.h) / h;
    float w_ratio = (float) (im.w) / w;
    // TODO
    image new = make_image(im.c, h, w);
    for (int i = 0; i < im.c; i++) {
        for (int j = 0; j < h; j++) {
            for (int k = 0; k < w; k++) {
                float h_ = (j + 0.5) * h_ratio - 0.5;
                float w_ = (k + 0.5) * w_ratio - 0.5;
                float pixel = nn_interpolate(im, i, h_, w_);
                set_pixel(new, i, j, k, pixel);
            }
        }
    }
    return new;
}

float bilinear_interpolate(image im, int c, float h, float w)
{   
    // TODO
    int pointTop = (int) floor(h);
    int pointBottom = (int) ceil(h);
    int pointLeft = (int) floor(w);
    int pointRight = (int) ceil(w);
    // float area1 = (pointBottom - h) * (pointRight - w);
    // float area2 = (pointBottom - h) * (w - pointLeft);
    // float area3 = (h - pointTop) * (pointRight - w);
    // float area4 = (h - pointTop) * (w - pointLeft);
    float d1 = w - pointLeft;
    float d2 = pointRight - w;
    float d3 = h - pointTop;
    float d4 = pointBottom - h;
    float pixel1 = get_pixel(im, c, pointTop, pointLeft);
    float pixel2 = get_pixel(im, c, pointTop, pointRight);
    float pixel3 = get_pixel(im, c, pointBottom, pointLeft);
    float pixel4 = get_pixel(im, c, pointBottom, pointRight);
    // float pixel = area1 * pixel1 + area2 * pixel2 + area3 * pixel3 + area4 * pixel4;
    float q1 = pixel1 * d2 + pixel2 * d1;
    float q2 = pixel3 * d2 + pixel4 * d1;
    float pixel = q1 * d4 + q2 * d3;
    return pixel;
}

image bilinear_resize(image im, int h, int w)
{
    // TODO
    image new = make_image(im.c, h, w);
    for (int i = 0; i < im.c; i++) {
        for (int j = 0; j < h; j++) {
            for (int k = 0; k < w; k++) {
                float h_ratio = (float)(im.h) / h;
                float w_ratio = (float)(im.w) / w;
                float h_ = (j + 0.5) * h_ratio - 0.5;
                float w_ = (k + 0.5) * w_ratio - 0.5;
                float pixel = bilinear_interpolate(im, i, h_, w_);
                set_pixel(new, i, j, k, pixel);
            }
        }
    }
    return new; // <- probably delete this
}

