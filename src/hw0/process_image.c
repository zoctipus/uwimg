#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

void rgb_to_hsv_pixel(float R, float G, float B, float *H, float *S, float *V);
void hsv_to_rgb_pixel(float H, float S, float V, float *R, float *G, float *B);

float get_pixel(image im, int c, int h, int w)
{

    int c_ = c;
    int h_ = h;
    int w_ = w;
    if(c < 0) { c_ = 0; }
    if(h < 0) { h_ = 0; }
    if(w < 0) { w_ = 0; }
    if(c >= im.c) { c_ = im.c - 1; }
    if(h >= im.h) { h_ = im.h - 1; }
    if(w >= im.w) { w_ = im.w - 1; }
    float res = im.data[c_ * (im.h * im.w) + h_ * im.w + w_];
    return res;
}

void set_pixel(image im, int c, int h, int w, float v)
{
    if(c < 0 || c > 2){}
    else if (h < 0 || w < 0 || h > im.h || w > im.w){}
    else {
        int pixel_index = c * (im.h * im.w) + h * im.w + w;
        im.data[pixel_index] = v;
    }
}

image copy_image(image im)
{
    image copy = make_image(im.c, im.h, im.w);
    for(int i = 0; i < im.c; i++) {
        for(int j = 0; j < im.h; j++) {
            for(int k = 0; k < im.w; k++) {
                int pixel_index = i * (im.h * im.w) + j * im.w + k;
                copy.data[pixel_index] = im.data[pixel_index];
            }
        }
    }
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(1, im.h, im.w);
    
    for(int j = 0; j < im.h; j++) {
        for(int k = 0; k < im.w; k++) {
            float R = get_pixel(im, 0, j, k);
			float G = get_pixel(im, 1, j, k);
            float B = get_pixel(im, 2, j, k);
            float Y = 0.299*R + 0.587*G + 0.114*B;
            set_pixel(gray, 0, j, k, Y);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for(int j = 0; j < im.h; j++) {
        for(int k = 0; k < im.w; k++) {
            float new_v = get_pixel(im, c, j, k) + v;
            set_pixel(im, c, j, k, new_v);
        }
    }	
}

void clamp_image(image im)
{
    for(int i = 0; i < im.c; i++) {
        for(int j = 0; j < im.h; j++) {
            for(int k = 0; k < im.w; k++) {
                if(get_pixel(im, i, j, k) < 0){
				    set_pixel(im, i, j, k, 0);
                }
			    if(get_pixel(im, i, j, k) > 1){
                    set_pixel(im, i, j, k, 1);
                }
            }
        }
    }
}

// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    float H, S, V;
    for(int j = 0; j < im.h; j++) {
        for(int k = 0; k < im.w; k++) {
            float R = get_pixel(im, 0, j, k);
			float G = get_pixel(im, 1, j, k);
            float B = get_pixel(im, 2, j, k);

            rgb_to_hsv_pixel(R, G, B, &H, &S, &V);

            set_pixel(im, 0, j, k, H);
            set_pixel(im, 1, j, k, S);
            set_pixel(im, 2, j, k, V);
        }
    }
}

void hsv_to_rgb(image im)
{
    float H, S, V, R, G, B;
    for(int j = 0; j < im.h; j++) {
        for(int k = 0; k < im.w; k++) {
            H = get_pixel(im, 0, j, k);
            S = get_pixel(im, 1, j, k);
            V = get_pixel(im, 2, j, k);

            hsv_to_rgb_pixel(H, S, V, &R, &G, &B);

            set_pixel(im, 0, j, k, R);
            set_pixel(im, 1, j, k, G);
            set_pixel(im, 2, j, k, B);
        }
    }
}

void rgb_to_hsv_pixel(float R, float G, float B, float *H, float *S, float *V) {
    float h, s, v, _h;
    v = three_way_max(R, G, B);
    float c = v - three_way_min(R, G, B);
    
    if (v == 0) {
        s = 0;
    } else {
        s = c / v;
    }

    if(c == 0) {
        _h = 0;
    } else if (v == R) {
        _h = (G - B)/c;
    } else if (v == G) {
        _h = (B - R)/c + 2;
    } else {
        _h = (R - G)/c + 4;
    }

    if(_h < 0) {
        h = _h/6 + 1;
    } else {
        h = _h/6;
    }

    *H = h;
    *S = s;
    *V = v;
}

void hsv_to_rgb_pixel(float H, float S, float V, float *R, float *G, float *B) {
    float _H, C, m, r, g, b;
    _H = H * 6;
    C = V * S;
    m = V - C;
    float X = C * (1 - fabs(fmod(_H, 2) - 1));
    switch ((int)floor(_H))
    {
        case 0: r = C; g = X; b = 0; break;
        case 1: r = X; g = C; b = 0; break;
        case 2: r = 0; g = C; b = X; break;
        case 3: r = 0; g = X; b = C; break;
        case 4: r = X; g = 0; b = C; break;
        default: r = C; g = 0; b = X; break;
    }

    *R = r + m;
    *G = g + m;
    *B = b + m;
}
