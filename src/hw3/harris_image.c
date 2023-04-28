#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#include "matrix.h"
#include <time.h>

// Frees an array of descriptors.
// descriptor *d: the array.
// int n: number of elements in array.
void free_descriptors(descriptor *d, int n)
{
    int i;
    for(i = 0; i < n; ++i){
        free(d[i].data);
    }
    free(d);
}

// Create a feature descriptor for an index in an image.
// image im: source image.
// int i: index in image for the pixel we want to describe.
// returns: descriptor for that index.
descriptor describe_index(image im, int i)
{
    int w = 5;
    descriptor d;
    d.p.x = i%im.w;
    d.p.y = i/im.w;
    d.data = calloc(w*w*im.c, sizeof(float));
    d.n = w*w*im.c;
    int c, dx, dy;
    int count = 0;
    // If you want you can experiment with other descriptors
    // This subtracts the central value from neighbors
    // to compensate some for exposure/lighting changes.
    for(c = 0; c < im.c; ++c){
        float cval = im.data[c*im.w*im.h + i];
        for(dx = -w/2; dx < (w+1)/2; ++dx){
            for(dy = -w/2; dy < (w+1)/2; ++dy){
                float val = get_pixel(im, c, i/im.w+dy, i%im.w+dx);
                d.data[count++] = cval - val;
            }
        }
    }
    return d;
}

// Marks the spot of a point in an image.
// image im: image to mark.
// ponit p: spot to mark in the image.
void mark_spot(image im, point p)
{
    int x = p.x;
    int y = p.y;
    int i;
    for(i = -9; i < 10; ++i){
        set_pixel(im, 0,   y, x+i, 1);
        set_pixel(im, 0, y+i,   x, 1);
        set_pixel(im, 1,   y, x+i, 0);
        set_pixel(im, 1, y+i,   x, 0);
        set_pixel(im, 2,   y, x+i, 1);
        set_pixel(im, 2, y+i,   x, 1);
    }
}

// Marks corners denoted by an array of descriptors.
// image im: image to mark.
// descriptor *d: corners in the image.
// int n: number of descriptors to mark.
void mark_corners(image im, descriptor *d, int n)
{
    int i;
    for(i = 0; i < n; ++i){
        mark_spot(im, d[i].p);
    }
}

// Creates a 1d Gaussian filter.
// float sigma: standard deviation of Gaussian.
// returns: single row image of the filter.
image make_1d_gaussian(float sigma)
{
    int w = ((int)(6*sigma))|1;
    image f = make_image(1, 1, w);
    // TODO: optional, make separable 1d Gaussian.

    return f;
}

// Smooths an image using separable Gaussian filter.
// image im: image to smooth.
// float sigma: std dev. for Gaussian.
// returns: smoothed image.
image smooth_image(image im, float sigma)
{
    if(1){
        image g = make_gaussian_filter(sigma);
        image s = convolve_image(im, g, 1);
        free_image(g);
        return s;
    } else {
        // TODO: optional, use two convolutions with 1d gaussian filter.
        // If you implement, disable the above if check.
        return copy_image(im);
    }
}

// Calculate the structure matrix of an image.
// image im: the input image.
// float sigma: std dev. to use for weighted sum.
// returns: structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2,
//          third channel is IxIy.
image structure_matrix(image im, float sigma)
{
    image S = make_image(3, im.h, im.w);
    // TODO: calculate structure matrix for im.

    image Ix = convolve_image(im, make_gx_filter(), 0);
    image Iy = convolve_image(im, make_gy_filter(), 0);

    int i, j;
    for (i = 0; i < im.h; i++) {
        for (j = 0; j < im.w; j++) {
            float Ix_val = get_pixel(Ix, 0, i, j);
            float Iy_val = get_pixel(Iy, 0, i, j);
            set_pixel(S, 0, i, j, Ix_val * Ix_val);
            set_pixel(S, 1, i, j, Iy_val * Iy_val);
            set_pixel(S, 2, i, j, Ix_val * Iy_val);
        }
    }

    free_image(Ix);
    free_image(Iy);

    image smoothed_S = smooth_image(S, sigma);

    return smoothed_S;
}

// Estimate the cornerness of each pixel given a structure matrix S.
// image S: structure matrix for an image.
// returns: a response map of cornerness calculations.
image cornerness_response(image S)
{
    image R = make_image(1, S.h, S.w);
    // TODO: fill in R, "cornerness" for each pixel using the structure matrix.
    // We'll use formulation det(S) - alpha * trace(S)^2, alpha = .06.

    float alpha = 0.06;

    for (int x = 0; x < S.h; x++) {
        for (int y = 0; y < S.w; y++) {
            float s_11 = get_pixel(S, 0, x, y); // Ix^2
            float s_22 = get_pixel(S, 1, x, y); // Iy^2
            float s_12 = get_pixel(S, 2, x, y); // Ix * Iy
            float s_21 = s_12; // In our case, S is symmetric, so  S_12 = S_21

            float det = s_11 * s_22 - s_12 * s_21;
            float trace = s_11 + s_22;
            float response = det - alpha * trace * trace;

            set_pixel(R, 0, x, y, response);
        }
    }

    return R;
}

// Perform non-max supression on an image of feature responses.
// image im: 1-channel image of feature responses.
// int w: distance to look for larger responses.
// returns: image with only local-maxima responses within w pixels.
image nms_image(image im, int w)
{
    // TODO: perform NMS on the response map.
    // for every pixel in the image:
    //     for neighbors within w:
    //         if neighbor response greater than pixel response:
    //             set response to be very low (I use -999999 [why not 0??])
    image r = copy_image(im);
    for (int x = 0; x < r.h; x++) {
        for (int y = 0; y < r.w; y++) {
            for (int i = x - w; i <= x + w; i++) {
                for (int j = y - w; j <= y + w; j++) {
                    if (get_pixel(r, 0, i, j) > get_pixel(r, 0, x, y)) {
                        set_pixel(r, 0, x, y, -999999);
                        goto done;
                    }
                }
            }
            done:;
        }
    }
    return r;

}

// Perform harris corner detection and extract features from the corners.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
// int *n: pointer to number of corners detected, should fill in.
// returns: array of descriptors of the corners in the image.
descriptor *harris_corner_detector(image im, float sigma, float thresh, int nms, int *n)
{
    // Calculate structure matrix
    image S = structure_matrix(im, sigma);

    // Estimate cornerness
    image R = cornerness_response(S);

    // Run NMS on the responses
    image Rnms = nms_image(R, nms);


    //TODO: count number of responses over threshold
    int count = 0;
    for(int x = 0; x < Rnms.h; x++){
        for(int y = 0; y < Rnms.w; y++){
            if (get_pixel(Rnms, 0, x, y) > thresh) {
                count++;
            }
        }
    }
    
    *n = count; // <- set *n equal to number of corners in image.

    descriptor *d = calloc(count, sizeof(descriptor));
    //TODO: fill in array *d with descriptors of corners, use describe_index.

    int idx = 0;
    for(int x = 0; x < Rnms.h; x++){
        for(int y = 0; y < Rnms.w; y++){
            if (get_pixel(Rnms, 0, x, y) > thresh) {
                int i = x * Rnms.w + y;
                d[idx++] = describe_index(im, i);  
            }
        }
    }

    free_image(S);
    free_image(R);
    free_image(Rnms);
    return d;
}

// Find and draw corners on an image.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
void detect_and_draw_corners(image im, float sigma, float thresh, int nms)
{
    int n = 0;
    descriptor *d = harris_corner_detector(im, sigma, thresh, nms, &n);
    mark_corners(im, d, n);
}
