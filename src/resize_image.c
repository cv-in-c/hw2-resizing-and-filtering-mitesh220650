#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    int x1 = (int)round(x);
    int y1 = (int)round(y);
    return get_pixel(im,x1,y1,c);
}

image nn_resize(image im, int w, int h)
{
    image resize = make_image(w,h,im.c);
    float x_div = (float)im.w/w;
    float y_div = (float)im.h/h;

    int i,j,k;
    for (k=0;k<im.c;k++){
        for(j=0;j<h;j++){
            for(i=0;i<w;i++){
                 float x = (i+0.5)*x_div - 0.5;
                 float y = (j+0.5)*y_div - 0.5;
                 set_pixel(resize,i,j,k,nn_interpolate(im,x,y,k));
            }
        }
    }

    return resize;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
   int x0 = (int)floor(x);
   int y0 = (int)floor(y);
   int x1 = x0 + 1;
   int y1 = y0 + 1;

   float pix_00  = get_pixel(im,x0,y0,c);
   float pix_01  = get_pixel(im,x0,y1,c);
   float pix_10  = get_pixel(im,x1,y0,c);
   float pix_11  = get_pixel(im,x1,y1,c);

   float pix_x1 = pix_00*(x1-x) + pix_10*(x-x0); // simply expanding linear interpolation in 2D
   float pix_x2 = pix_01*(x1-x) + pix_11*(x-x0);

   float pix_val = pix_x1*(y1-y)+pix_x2*(y-y0); // in 1D it is length , In 2D Area is used for interpolation

    return pix_val;
}

image bilinear_resize(image im, int w, int h)
{
   image resize = make_image(w,h,im.c);
   float x_div = (float)im.w/w;
   float y_div = (float)im.h/h;

   int i,j,k;
    for (k=0;k<im.c;k++){
        for(j=0;j<h;j++){
            for(i=0;i<w;i++){
                 float x = (i+0.5)*x_div-0.5 ;
                 float y = (j+0.5)*y_div-0.5 ;
                 float pix_val = bilinear_interpolate(im,x,y,k);
                 set_pixel(resize,i,j,k,pix_val);
            }
        }
    }

    return resize;
}

