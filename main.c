#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>

#define widthLoc 18
#define lengthLoc 22
#define offsetLoc 10
#define sizeLoc 34
#define pixelsizeloc 28

struct Bitmap{
    uint32_t width;
    uint32_t height;
    uint32_t offset;
    uint32_t size;
    uint32_t totalpixels;
    uint32_t convertSize;
    uint8_t *pixels;
    uint16_t* convert;
}Bitmap;

struct Bitmap* bmp;

void destroy(struct Bitmap* m){
    free(m->pixels);
    free(m->convert);
    free(m);
}
void image(char* filename){

    bmp = malloc(sizeof(Bitmap));

    FILE* f = fopen(filename,"rb");
    uint8_t header[54] = {0};
    fread(header, 1,54,f);
    bmp->width = header[widthLoc];
    bmp->height = header[lengthLoc];
    bmp->offset = (header[offsetLoc + 3] << (3*8)) + (header[offsetLoc + 2] << (2*8) ) + (header[offsetLoc + 1] << (1*8) ) + header[offsetLoc] - 54;
    bmp->size = (header[sizeLoc + 3] << (3*8)) + (header[sizeLoc + 2] << (2*8) ) + (header[sizeLoc + 1] << (1*8) ) + header[sizeLoc];
    bmp->totalpixels = (bmp->size * 8)/(header[pixelsizeloc]);
    char* trash = malloc(bmp->offset);
    fread(trash,1,bmp->offset,f);
    free(trash);
    assert(header[pixelsizeloc] == 24);

    bmp->pixels = malloc(bmp->size);
    fread(bmp->pixels, 1,bmp->size,f);
    fclose(f);
    for(int i = 0; i < bmp->totalpixels; i++){
        uint8_t tmp = bmp->pixels[3*i];
        bmp->pixels[3*i] = bmp->pixels[i + 2];
        bmp->pixels[3*i + 2] = tmp;
    }
    int pad;
    int extrapad = 0;
    if((pad = bmp->width%4) != 0){
        for(int i = 0; i < bmp->totalpixels; i++){
            if((i + 1)%(bmp->totalpixels + 1)){
                extrapad += pad;
            }
            bmp->pixels[3*i] = bmp->pixels[3*i + extrapad];
            bmp->pixels[3*i] = bmp->pixels[3*i + 1 + extrapad];
            bmp->pixels[3*i] = bmp->pixels[3*i + 2 + extrapad];
        }
    }
    bmp->totalpixels -= bmp->height;
}

void conversion(uint8_t* pixels, int size){
    uint32_t newSize = bmp->totalpixels * sizeof(uint16_t);
    uint16_t* pix = malloc(newSize);
    int i = 0;
    uint8_t r8,g8,b8;
    uint16_t r5,g6,b5;
    while(i < bmp->totalpixels){
        r8 = pixels[3 * i];
        g8 = pixels[3 * i + 1];
        b8 = pixels[3 * i + 2];
        r5 = (r8 * 249 + 1014) >> 11;
        g6 = (g8 * 253 + 505) >> 10;
        b5 = (b8 * 249 + 1014) >> 11;
        r5 = r5 << 11;
        g6 = g6 << 5;
        b5 = b5 & 0x0000007F;
        uint16_t color = r5 + g6 + b5;
        pix[i] = color;
        i++;
    }
    bmp->convert = pix;
    bmp->convertSize = newSize;
}

int main(void){

    image("parrots2.bmp");

    conversion(bmp->pixels, bmp->size);

    for(int i = 0; i < bmp->totalpixels;i++) {
        if(i%15 == 0){
            printf("");
        }
        printf("0x%04X, ", bmp->convert[i]);
        //printf("0x%04X Pixel %d\n", bmp->convert[i], i);
    }
    free(bmp);
    return 0;
}