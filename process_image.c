#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // TODO Fill this in
    // Bound checking and clamp padding the image
    if(x<0) x=0;
    if(y<0) y=0;
    if(c<0) c=0;
    if(x>=im.w) x=im.w-1;
    if(y>=im.h) y=im.h-1;
    if(c>=im.c) c=im.c-1;

    int index = c*im.w*im.h + y*im.w+x;
    return im.data[index];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    // TODO Fill this in
    // Checking for Invalid Coordinates
    if(x<0||y<0||c<0||x>=im.w||y>im.h||c>=im.c){
        return;
    }
    int index = c*im.w*im.h + y*im.w+x;
    im.data[index] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // TODO Fill this in
    //Another way of copying pixel using loops
    /* for(int c=0;c<im.c;c++){
        for(int y=0;y<im.h;y++){
            for(int x=0;x<im.w;x++){
                index = c*im.w*im.h + y*im.w +x;
                copy.data[index]=im.data[index];
            }
        }
    }*/
    memcpy(copy.data ,im.data ,im.w*im.h*im.c*sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    // TODO Fill this in
    for(int y=0;y<im.h;y++){
        for(int x=0;x<im.w;x++){
           float r = get_pixel( im,x,y,0);
           float g = get_pixel( im,x,y,1);
           float b = get_pixel( im,x,y,2);
           float Y = 0.299*r + 0.587*g + 0.114*b;

           set_pixel(gray,x,y,0,Y);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    // TODO Fill this in
    for(int y=0;y<im.h;y++){
        for(int x=0;x<im.w;x++){
          float pixel_value=get_pixel(im,x,y,c);
          int index = c*im.w*im.h + y*im.w+x;
          im.data[index] = pixel_value + v;
           }
    }
}

void clamp_image(image im)
{
    // TODO Fill this in
    for(int c=0;c<im.c;c++){
       for(int y=0;y<im.h;y++){
          for(int x=0;x<im.w;x++){
          float pixel_value=get_pixel(im,x,y,c);
         /* int index = c*im.w*im.h + y*im.w+x;
          if(im.data[index]<0){
            im.data[index]=0;
          }else if(im.data[index]>1){
            im.data[index]=1;
          }*/

          pixel_value = (pixel_value < 0)? 0:pixel_value;
          pixel_value = (pixel_value > 1)? 1:pixel_value;
          set_pixel(im,x,y,c,pixel_value);
          
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

void rgb_to_hsv(image im){
    // TODO Fill this in
    for(int y=0;y<im.h;y++){
       for(int x =0;x<im.w;x++){
         float r = get_pixel( im,x,y,0);
         float g = get_pixel( im,x,y,1);
         float b = get_pixel( im,x,y,2);

         //Calculating value
         float V = three_way_max(r,g,b);
         float min = three_way_min(r,g,b);
         float C = V - min;

         //Calculating Saturation
         float S = (r==0 && g==0 && b==0)?0:C/V;
         
         float h_ =0;
         //Calculating Hue
         if(C==0){ h_=0;}
         else if(V==r){ h_ = (g-b)/C;}
         else if(V==g) {h_ = (b-r)/C +2;}
         else if(V==b) {h_ = (r-g)/C + 4;}
           
         float H = (h_<0)? H = h_/6 +1 : h_/6 ; 

         set_pixel(im,x,y,0,H);
         set_pixel(im,x,y,1,S);
         set_pixel(im,x,y,2,V);
         }
    }

}

void hsv_to_rgb(image im){
    // TODO Fill this in
    float r=0,g=0,b=0;
    for(int y=0;y<im.h;y++){
        for(int x=0;x<im.w;x++){
            float h = get_pixel(im,x,y,0);
            float s = get_pixel(im,x,y,1);
            float v = get_pixel(im,x,y,2);

            if(s==0){
                 r=g=b=v;
                 }else{
                  h=h*6.0;//Scaling h
                  int i = floor(h);
                 float f=h-i;
                 float a = v*(1-s) ; //a=min
                 float p = v*(1-s*f);
                 float c = v*(1-s*(1-f));
                   switch(i){
                   case 0 : r=v;
                   g=c;
                   b=a;
                  break;
                 case 1 : r=p;
                  g=v;
                  b=a;
                  break;
                  case 2 : r=a;
                  g=v;
                  b=c;
                  break;
                  case 3: r=a;
                  g=p;
                  b=v;
                  break;
                  case 4 : r=c;
                  g=a;
                  b=v;
                  break;
                  case 5 :
                  r=v;
                  g=a;
                  b=p;
                 }
            }
            set_pixel(im,x,y,0,r);
            set_pixel(im,x,y,1,g);
            set_pixel(im,x,y,2,b);
         }

    }
    
}
