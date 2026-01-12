#ifndef DRAM_H
#define DRAM_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "config.h"
typedef enum DRAMType{
    IFM,
    OFM,
    WEIGHT
} DRAMType;
/*  Các tensor được trải thẳng với công thức index là idx = i0*(d1*d2*d3) + i1(d2*d3) + i2(d3) + i3 và không được thay đổi ct tính index
    IFM, OFM(B, H, W, C)
    W(C_out, H, W, C_in)*/
typedef struct DRAM{
    DRAMType type;
    int8_t * dat_8b; // Cho giá trị quanti của ifm và weight
    int32_t * dat_32b; // Cho ofm
    int d0, d1, d2, d3; // IFM(B, H, W, C), W(COUT, H, W, CIN), OFM(B, H, W, C)
} DRAM;
void initDRAM(DRAM *dram, DRAMType type, int d0, int d1, int d2, int d3){
    dram->type = type;
    dram->d0 = d0;
    dram->d1 = d1;
    dram->d2 = d2;
    dram->d3 = d3;
    dram->dat_8b = NULL;
    dram->dat_32b = NULL;
    if(type == IFM || type == WEIGHT){
        dram->dat_8b = (int8_t *)malloc(d0 * d1 * d2 * d3 * sizeof(int8_t));
    }
    else if(type == OFM){
        dram->dat_32b = (int32_t *)malloc(d0 * d1 * d2 * d3 * sizeof(int32_t));
    }
}
/*
Kiểm tra lại cách load weight nếu ra output sai
*/
void loadDRAM(DRAM *dram, const char *file){
    FILE *f = fopen(file, "r");
    if(f == NULL){
        printf("Khong the mo file %s", file);
        return;
    }
    if(dram->type == IFM){
        for(int b = 0; b < dram->d0; b++){
            for(int h = 0; h < dram->d1; h++){
                for(int w = 0; w < dram->d2; w++){
                    for(int c = 0; c < dram->d3; c++){
                        int idx = b*(dram->d1*dram->d2*dram->d3) + h*(dram->d2*dram->d3) + w*(dram->d3) + c;
                        fscanf(f, "%hhd", &dram->dat_8b[idx]);
                    }
                }
            }
        }
    }
    else if(dram->type == WEIGHT){
        for(int c_out = 0; c_out < dram->d0; c_out++){
            for(int h = 0; h < dram->d1; h++){
                for(int w = 0; w < dram->d2; w++){
                    for(int c_in = 0; c_in < dram->d3; c_in++){
                        // Chuyển sang dạng [H, W, C_in, C_OUT]
                        int idx = c_out*(dram->d1*dram->d2*dram->d3) + h*(dram->d2*dram->d3) + w*(dram->d3) + c_in;
                        // int idx = h*(dram->d2*dram->d3*dram->d0) + w*(dram->d3*dram->d0) + c_in*dram->d0 + c_out;
                        fscanf(f, "%hhd", &dram->dat_8b[idx]);
                    }
                    printf("\n");
                }
                printf("\n\n");
            }
        }
    }
    else if(dram->type == OFM){
        printf("Không impliment hàm đọc cho OFM");
    }
    fclose(f);
}
void printDRAM(DRAM *dram){
    for(int d0 = 0; d0 < dram->d0; d0++){
        for(int d1 = 0; d1 < dram->d1; d1++){
            for(int d2 = 0; d2 < dram->d2; d2++){
                for(int d3 = 0; d3 < dram->d3; d3++){
                    if(dram->dat_8b){
                        int idx = d0*(dram->d1*dram->d2*dram->d3) + d1*(dram->d2*dram->d3) + d2*(dram->d3) + d3;
                        printf("%5d ", dram->dat_8b[idx]);
                        // printf("%d", idx);
                    }
                    else{
                        int idx = d0*(dram->d1*dram->d2*dram->d3) + d1*(dram->d2*dram->d3) + d2*(dram->d3) + d3;
                        printf("%5d ", dram->dat_32b[idx]);
                        // printf("%d", idx);
                    }
                }
                printf("\n");
            }
            printf("\n\n");
        }
    }
}
void writeDRAMToFile(DRAM *dram, const char * filename){
    if(dram->dat_32b){
        FILE *f = fopen(filename, "w");
        if(f == NULL){
            printf("Ko mo duoc file");
            return;
        }
        for(int i = 0; i < dram->d0*dram->d1*dram->d2*dram->d3; i++){
            fprintf(f, "%d\n",dram->dat_32b[i]);
        }
    }  
}
void freeDRAM(DRAM *dram){
    if(dram->dat_32b){
        free(dram->dat_32b);
    }
    else if(dram->dat_8b){
        free(dram->dat_8b);
    }
}
// Kiểm tra với golden output
int checkWithGoldenOutput(DRAM *ofm, const char* golden){
    FILE *f = fopen(golden, "r");
    if(f == NULL){
        printf("Khong mo duoc file");
    }
    int32_t golden_data[OUTPUT_H*OUTPUT_W];
    for(int i = 0; i < OUTPUT_H*OUTPUT_W; i++){
        fscanf(f, "%d\n", &golden_data[i]);
    }
    for(int i = 0; i < OUTPUT_H*OUTPUT_W; i++){
        if(ofm->dat_32b[i] != golden_data[i]){
            return 0;
        }
    }
    return 1;  
}
#endif