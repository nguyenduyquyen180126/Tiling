#ifndef PE_H
#define PE_H
#include <stdio.h>
#include <stdint.h>
#include "config.h"
#include "buffer.h"
// Biến toàn cục trong pe.h lưu số cycles cho việc tính toán
unsigned long long total_comp_cycles = 0;

/*  Thực hiện mul và acc 3 phần tử từ các buffer*/
typedef struct PE{
    int8_t* i[MACS_PER_PE]; // Nối dây từ buffer đến các PE
    int8_t* w[MACS_PER_PE];
    int32_t a;
}PE;
/*  Liên kết các PE với các bits trong buffer với con trỏ*/
void initPE(PE *pe, int8_t*ifm_buffer, int8_t *w_buffer, int startBit){
    for(int i=startBit, j = 0; i<startBit + MACS_PER_PE; i++, j++){
        pe->i[j] = &ifm_buffer[i];
        pe->w[j] = &w_buffer[i];
    }
}
/*  Chạy tính toán*/
int32_t runPE(PE *pe){
    int32_t pe_acc = 0;
    for(int i=0; i<MACS_PER_PE; i++){
        int32_t mul = *pe->i[i] * *pe->w[i];
        pe_acc += mul;
    }
    return pe->a = pe_acc;
}
void printPE(PE *pe){
    printf("ifm: %5d %5d %5d & w: %5d %5d %5d : a = %5d\n", *pe->i[0], *pe->i[1], *pe->i[2], *pe->w[0], *pe->w[1], *pe->w[2], pe->a);
}






// Mảng 48 PE
typedef struct PE_Array{
    PE pe[NUM_PE];
}PE_Array;
// Init từng pe
void initPE_Array(PE_Array *pe_array, int8_t *ifm_buffer, int8_t *w_buffer){
    for(int i=0; i<NUM_PE; i++){
        initPE(&pe_array->pe[i], ifm_buffer, w_buffer, i*MACS_PER_PE);
    }
}
// Chạy từng PE
int32_t runPE_Array(PE_Array *pe_array){
    int32_t p_sum = 0;
    for(int i=0; i<NUM_PE; i++){
        p_sum += runPE(&pe_array->pe[i]);
    }
    total_comp_cycles++;
    return p_sum;
}
void print_pe_array(PE_Array *pe_array){
    for(int i=0; i<NUM_PE; i++){
        printPE(&pe_array->pe[i]);
    }
}
#endif