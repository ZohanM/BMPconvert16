#include <stdio.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define widthLoc 18
#define heightLoc 22
#define offsetLoc 10
#define sizeLoc 34
#define pixelsizeloc 28



struct Bitmap{
    uint32_t width;
    uint32_t height;
    uint32_t size;
    uint8_t pixelsize;
    uint8_t* colors;
    uint16_t* pixels;
    uint32_t totalpixels;
};

void bits24(Bitmap* bmp);
void bits32(Bitmap* bmp);
void conversion(Bitmap* bmp);

void destroy(Bitmap* m) {
    free(m->pixels);
    free(m->colors);
}

void imageData(char* filename, Bitmap* bmp) {
    FILE *f = fopen(filename, "rb");

    uint8_t header[54] = {0};
    fread(header, sizeof(uint8_t), 54, f);

    bmp->width = header[widthLoc];
    bmp->height = header[heightLoc];
    int offset = (header[offsetLoc + 3] << (3 * 8)) + (header[offsetLoc + 2] << (2 * 8))
                 + (header[offsetLoc + 1] << (1 * 8)) + header[offsetLoc] - 54;
    bmp->size = (header[sizeLoc + 3] << (3 * 8)) + (header[sizeLoc + 2] << (2 * 8))
                + (header[sizeLoc + 1] << (1 * 8)) + header[sizeLoc];
    bmp->pixelsize = header[pixelsizeloc];
    char *trash = (char *) (malloc(offset));
    fread(trash, sizeof(char), offset, f);
    free(trash);            //gibberish data

    bmp->colors = (uint8_t*)malloc(bmp->size);
    fread(bmp->colors, sizeof(uint8_t), bmp->size, f);

    if(bmp->pixelsize == 24){
        bmp->totalpixels = bmp->size/3;
        bits24(bmp);
    }else if(bmp->pixelsize == 32){
        bmp->totalpixels = bmp->size/4;
        bits32(bmp);
    }else{
        assert(0);
    }



}


void bits24(Bitmap* bmp){
    int i = 0;
    int pad = 0;
    int totalpadding = 0;
    if((pad = (3 * bmp->width) % 4) != 0) {
        while (i < bmp->size) {
            if((i+3)%(bmp->width + 1) == 0){
                totalpadding += pad;
            }
            bmp->colors[i + 0] = bmp->colors[i + 0 + totalpadding];
            bmp->colors[i + 1] = bmp->colors[i + 1 + totalpadding];
            bmp->colors[i + 2] = bmp->colors[i + 2 + totalpadding];
            i += 3;
        }
        bmp->totalpixels -= bmp->height;
    }
    conversion(bmp);
}

void bits32(Bitmap* bmp){
    conversion(bmp);
}

void conversion(Bitmap* bmp){
    uint32_t newSize = bmp->totalpixels * sizeof(uint16_t);
    bmp->pixels = (uint16_t*)malloc(newSize);
    int i = 0;
    uint8_t r8,g8,b8;
    uint16_t r5,g6,b5;
    while(i < bmp->totalpixels){
        r8 = bmp->colors[3 * i];
        g8 = bmp->colors[3 * i + 1];
        b8 = bmp->colors[3 * i + 2];
        r5 = (r8 * 249 + 1014) >> 11;
        g6 = (g8 * 253 + 505) >> 10;
        b5 = (b8 * 249 + 1014) >> 11;
        r5 = r5 << 11;
        g6 = g6 << 5;
        b5 = b5 & 0x0000007F;
        uint16_t color = r5 + g6 + b5;
        bmp->pixels[i] = color;
        i++;
    }
}

void printPix(Bitmap* bmp){
    for(int i = 0; i < bmp->totalpixels; i++) {
        if (i % 15 == 0) {
            printf("\n");
        }
        printf("0x%04X, ", bmp->pixels[i]);
    }
}

int main() {
    Bitmap* parrot = (Bitmap*)malloc(sizeof(Bitmap) * 1);
    imageData("parrot2.bmp",parrot);
    printPix(parrot);
    return 0;
}