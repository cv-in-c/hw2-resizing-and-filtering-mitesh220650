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
    for (int k = 0;k<im.c;k++){
        for(int j=0;j<im.h;j++){
            for(int i=0;i<im.w;i++){
                sum += get_pixel(im,i,j,k);
            }
        }
    }
    // eliminating the case whether the sum=0 so that division by zero is avoided 
    if(sum==0){
        for (int k = 0;k<im.c;k++){
        for(int j=0;j<im.h;j++){
            for(int i=0;i<im.w;i++){
                set_pixel(im,i,j,k,1/(im.w*im.h*im.c));
            }
        }
    }
}else{ // normalizing pixel values 
    for (int k = 0;k<im.c;k++){
        for(int j=0;j<im.h;j++){
            for(int i=0;i<im.w;i++){
                set_pixel(im,i,j,k,get_pixel(im,i,j,k)/sum);
            }
        }
    }
    }

}

image make_box_filter(int w)
{
    image box_filter = make_image(w,w,1);
    float pix_val = 1.0f/(w*w);
    for (int j=0;j<w;j++){
        for (int i=0;i<w;i++){
            set_pixel(box_filter,i,j,0,pix_val); // all pixel values are equal to 1/w*w where w is the width of the square box filter here.
        }
    }
    return box_filter;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(filter.c == im.c || filter.c == 1);

    int filter_channels = filter.c;
    int im_channels = im.c;
    int im_width = im.w;
    int im_height = im.h;

    image output;
    if (preserve) {
        output = make_image(im_width, im_height, im_channels);
    } else {
        output = make_image(im_width, im_height, 1);
    }

    for (int k = 0; k < im_channels; k++) { // loop over the complete image
        for (int i = 0; i < im_width; i++) {
            for (int j = 0; j < im_height; j++) {
                float sum = 0;
                for (int m = 0; m < filter.w; m++) { // loop over the complete filter
                    for (int n = 0; n < filter.h; n++) {
                        int x = i + m - (filter.w - 1) / 2;
                        int y = j + n - (filter.h - 1) / 2;
                        float im_val = get_pixel(im, x, y, k);
                        float filter_val;
                        if (filter_channels == 1) {
                            filter_val = get_pixel(filter, m, n, 0);
                        } else {
                            if (preserve) {
                                filter_val = get_pixel(filter, m, n, k);
                            } else {
                                filter_val = get_pixel(filter, m, n, 0);
                            }
                        }
                        sum += im_val * filter_val; // process of convolution 
                    }
                }
                if (preserve) {
                    set_pixel(output, i, j, k, sum);
                } else {
                    set_pixel(output, i, j, 0, get_pixel(output, i, j, 0) + sum);
                }
            }
        }
    }

    return output;
}



image make_highpass_filter()
{
    image filter = make_box_filter(3);
// assigning values to each pixel
    set_pixel(filter,0,0,0,0); 
    set_pixel(filter,1,0,0,-1);
    set_pixel(filter,2,0,0,0);
    set_pixel(filter,0,1,0,-1);
    set_pixel(filter,1,1,0,4);
    set_pixel(filter,2,1,0,-1);
    set_pixel(filter,0,2,0,0);
    set_pixel(filter,1,2,0,-1);
    set_pixel(filter,2,2,0,0);
    return filter;
}

image make_sharpen_filter()
{
    image filter = make_box_filter(3);
// assigning values to each pixel
    set_pixel(filter,0,0,0,0);
    set_pixel(filter,1,0,0,-1);
    set_pixel(filter,2,0,0,0);
    set_pixel(filter,0,1,0,-1);
    set_pixel(filter,1,1,0,5);
    set_pixel(filter,2,1,0,-1);
    set_pixel(filter,0,2,0,0);
    set_pixel(filter,1,2,0,-1);
    set_pixel(filter,2,2,0,0);
    return filter;
}

image make_emboss_filter()
{
    image filter = make_box_filter(3);
    // assigning values to each pixel
    set_pixel(filter,0,0,0,-2);
    set_pixel(filter,1,0,0,-1);
    set_pixel(filter,2,0,0,0);
    set_pixel(filter,0,1,0,-1);
    set_pixel(filter,1,1,0,1);
    set_pixel(filter,2,1,0,1);
    set_pixel(filter,0,2,0,0);
    set_pixel(filter,1,2,0,1);
    set_pixel(filter,2,2,0,2);
    return filter;
    
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer:Preserve should be used for each highpass,sharpen and emboss.In box-filter it is important to preserve no. of channels as it can be applied independently to each channel.In Highpass,Sharpen each filter can be applied individually to each channel and in emboss preserve is required to maintain 3D details across each channel.Not needed in box filter as it just smoothens the image and does averaging of all pixel values.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: Post-processing is not required for any of the above filter .It is only required for gaussian filter as due to the gaussian filter it may occur that the pixel values may go out of range and normalizing the values will keep the pixel values in valid range limit.

image make_gaussian_filter(float sigma)
{
    int size = ceil(6*sigma);
    if(size%2==0)size++;
    image filter = make_box_filter(size);
    float sum = 0;
    for(int j=0;j<size;j++){
        for(int i=0;i<size;i++){
            float x = i-size/2;
            float y = j-size/2;
            float value = expf(-(x*x+y*y)/(2*sigma*sigma))/(TWOPI*sigma*sigma);
            sum += value;
            set_pixel(filter,i,j,0,value);
        }
    }
    l1_normalize(filter);
    return filter;
}

image add_image(image a, image b)
{
    assert(a.h==b.h && a.w==b.w && a.c==b.c); // checking for same height,width and channels
    image output = make_image(a.w,a.h,a.c);
    
    for(int k=0;k<a.c;k++){
        for(int j=0;j<a.h;j++){
            for(int i=0;i<a.w;i++){
                float a_ = get_pixel(a,i,j,k);
                float b_ = get_pixel(b,i,j,k);
                set_pixel(output,i,j,k,a_+b_);
            }
        }
    }
    
    return output;
}

image sub_image(image a, image b)
{
    assert(a.h==b.h && a.w==b.w && a.c==b.c);
    image output = make_image(a.w,a.h,a.c);
    
    for(int k=0;k<a.c;k++){
        for(int j=0;j<a.h;j++){
            for(int i=0;i<a.w;i++){
                float a_ = get_pixel(a,i,j,k);
                float b_ = get_pixel(b,i,j,k);
                set_pixel(output,i,j,k,a_-b_);
            }
        }
    }
    
    return output;
}

image make_gx_filter()
{
    image filter = make_image(3,3,1);
    set_pixel(filter,0,0,0,-1);
    set_pixel(filter,1,0,0,0);
    set_pixel(filter,2,0,0,1);
    set_pixel(filter,0,1,0,-2);
    set_pixel(filter,1,1,0,0);
    set_pixel(filter,2,1,0,2);
    set_pixel(filter,0,2,0,-1);
    set_pixel(filter,1,2,0,0);
    set_pixel(filter,2,2,0,1);
   
    return filter;
}

image make_gy_filter()
{
   image filter = make_image(3,3,1);
    set_pixel(filter,0,0,0,-1);
    set_pixel(filter,1,0,0,-2);
    set_pixel(filter,2,0,0,-1);
    set_pixel(filter,0,1,0,0);
    set_pixel(filter,1,1,0,0);
    set_pixel(filter,2,1,0,0);
    set_pixel(filter,0,2,0,1);
    set_pixel(filter,1,2,0,2);
    set_pixel(filter,2,2,0,1);
   
    return filter;
}

void feature_normalize(image im)
{
    float min_pix = INFINITY;
    float max_pix = -INFINITY;

    for (int k=0;k<im.c;k++){
        for(int j=0;j<im.h;j++){
            for(int i=0;i<im.w;i++){
             float pix_val = get_pixel(im,i,j,k);
             min_pix = (pix_val < min_pix)?pix_val:min_pix;  //to calculate minimum
             max_pix = (max_pix>pix_val )? max_pix:pix_val;   // to calculate maximum

            }
        }
    }
    float range = max_pix - min_pix;
     // eliminating the case where range is zero i.e every pixel value is equal
    if (range==0){
        for (int k=0;k<im.c;k++){
        for(int j=0;j<im.h;j++){
            for(int i=0;i<im.w;i++){
            set_pixel(im,i,j,k,0);
             }
        }
    }
    }else{
    for (int k=0;k<im.c;k++){
        for(int j=0;j<im.h;j++){
            for(int i=0;i<im.w;i++){
            float val = get_pixel(im,i,j,k);
            set_pixel(im,i,j,k,(val-min_pix)/range);

            }
        }
    }
}
}


image *sobel_image(image im)
{
    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();
    image gx = convolve_image(im,gx_filter,0);
    image gy = convolve_image(im,gy_filter,0);
    image *output = (image *)malloc(2*sizeof(image));
    output[0]=make_image(im.w,im.h,1);
    output[1]=make_image(im.w,im.h,1);

    for(int j=0;j<im.h;j++){
        for(int i=0;i<im.w;i++){
            float gx_pix = get_pixel(gx,i,j,0);
            float gy_pix = get_pixel(gy,i,j,0);
            float magnitude = sqrtf(gx_pix*gx_pix + gy_pix*gy_pix);
            float direction = atan2f(gy_pix,gx_pix);
           

            set_pixel(output[0],i,j,0,magnitude);
            set_pixel(output[1],i,j,0,direction);
        }
    }
    free_image(gx);
    free_image(gy);

    return output;
}

image colorize_sobel(image im)
{
   image *sobel = sobel_image(im);
   image mag = sobel[0];
   image dir = sobel[1];
   feature_normalize(mag);
   image output = make_image(im.w,im.h,3);

   for(int j=0;j<im.h;j++){
    for(int i=0;i<im.w;i++){
        float mag_pix = get_pixel(mag,i,j,0);
        float dir_pix = get_pixel(dir,i,j,0);
        set_pixel(output,i,j,0,dir_pix);
        set_pixel(output,i,j,1,mag_pix);
        set_pixel(output,i,j,2,mag_pix);
    }
   }
   hsv_to_rgb(output);
   free_image(mag);
   free_image(dir);
   free(sobel);

    return output;
}
