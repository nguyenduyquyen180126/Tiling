/*  Define 2 buffer 144 bits cho ifm và weight
*/
#ifndef BUFFER_H
#define BUFFER_H
#include <stdio.h>
#include <stdint.h>
#include "config.h"
#include "dram.h"
#include <math.h>
// Biến toàn cục trong buffer.h
unsigned long long total_dma_cycles = 0;
unsigned long long total_bytes = 0;
/*Hàm dịch buffer cho input share*/
void buffer_right_shift(int8_t *buffer, int num){
    int i = 0, j = BUFFER_SIZE_BYTES - 1;
    while(i < j){
        int temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
        i++;
        j--;
    }
    i = 0, j = BUFFER_SIZE_BYTES - num - 1;
    while(i < j){
        int temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
        i++;
        j--;
    }
}

/*  Hàm load weight bình thường
    Không inpliment dim batch_num mà hard code luôn do có batch_num là 1
    Đọc theo thứ tự vòng for: C->H->W
    Trả về số cycles tốn để load buffer */
void load_w_buffer(DRAM *w, int8_t *w_buffer, int pass_idx){
    int start_channel = pass_idx * PARALLEL_CHANNELS;
    int buffer_idx = 0;
    for(int c = start_channel; c < start_channel + PARALLEL_CHANNELS; c++){
        if(c >= INPUT_C){
            break;
        }
        for(int kh = 0; kh < KERNEL_H; kh++){
            for(int kw = 0; kw < KERNEL_W; kw++){
                int w_idx = kh*(w->d2*w->d3) + kw*(w->d3) + c;
                w_buffer[buffer_idx] = w->dat_8b[w_idx];
                buffer_idx++;
            }
        }
    }
    total_bytes += buffer_idx;
}
/*  Load ifm bình thường
    Đưa ifm vào buffer theo thứ tự C->H->W */
void load_ifm_buffer(DRAM *ifm, int8_t *ifm_buffer, int ho, int wo, int pass_idx){
    int start_channel = pass_idx * PARALLEL_CHANNELS;
    int buffer_idx = 0;// Chỉ số trên buffer
    for(int c = start_channel; c < start_channel + PARALLEL_CHANNELS; c++){
        if(c >= INPUT_C){
            break;
        }
        for(int kh = 0; kh < KERNEL_H; kh++){
            for(int kw = 0; kw < KERNEL_W; kw++){
                
                // Load ifm
                int hi = ho*STRIDE + kh - PADDING;
                int wi = wo*STRIDE + kw - PADDING;
                int ifm_idx = hi*(ifm->d2*ifm->d3) + wi*(ifm->d3) + c;
                // Xử lý padding
                if(hi >= 0 && hi < INPUT_H && wi >= 0 && wi < INPUT_W){
                    ifm_buffer[buffer_idx] = ifm->dat_8b[ifm_idx];
                }
                else{
                    ifm_buffer[buffer_idx] = 0;
                }
                buffer_idx++;
            }
        }
    }
    total_bytes += buffer_idx;
}
/*Load ifm theo thu tu W->H->C để thực hiện được input share*/
void load_ifm_share(DRAM *ifm, int8_t *ifm_buffer, int ho, int wo, int pass_idx){
    int start_channel = pass_idx * PARALLEL_CHANNELS;
    int buffer_idx = 0;// Chỉ số trên buffer
    int byte_transfered = 0;
    // Nếu là bắt đầu hàng thực hiện load toàn bộ
    if(wo == 0){
        for(int kw = 0; kw < KERNEL_W; kw++){
            for(int kh = 0; kh < KERNEL_H; kh++){
                for(int c = start_channel; c < start_channel + PARALLEL_CHANNELS; c++){
                    if(c >= INPUT_C){
                        return;
                    }
                    int hi = ho*STRIDE + kh - PADDING;
                    int wi = wo*STRIDE + kw - PADDING;
                    int ifm_idx = hi*(ifm->d2*ifm->d3) + wi*(ifm->d3) + c;
                    if(hi >= 0 && hi < INPUT_H && wi >= 0 && wi < INPUT_W){
                        ifm_buffer[buffer_idx] = ifm->dat_8b[ifm_idx];
                    }
                    else{
                        ifm_buffer[buffer_idx] = 0;
                    }
                    buffer_idx++;
                    byte_transfered++;
                }
            }
        }
    }
    // Nếu không phải bắt đầu hàng dịch phải và thực hiện load những cột mới
    else{
        buffer_right_shift(ifm_buffer, KERNEL_H*PARALLEL_CHANNELS);
        int buffer_idx_share = BUFFER_SIZE_BYTES - KERNEL_H*PARALLEL_CHANNELS;
        for(int kh = 0; kh < KERNEL_H; kh++){
            for(int c = start_channel; c < start_channel + PARALLEL_CHANNELS; c++){
                int hi = ho*STRIDE + kh - PADDING;
                int wi = wo*STRIDE + KERNEL_W - 1 - PADDING;
                int ifm_idx = hi*(ifm->d2*ifm->d3) + wi*(ifm->d3) + c;
                if(hi >= 0 && hi < INPUT_H && wi >= 0 && wi < INPUT_W){
                    ifm_buffer[buffer_idx_share] = ifm->dat_8b[ifm_idx];
                }
                else{
                    ifm_buffer[buffer_idx_share] = 0;
                }
                buffer_idx_share++;
                byte_transfered++;
            }
        }
    }
    total_bytes += byte_transfered;
}
/*Load w theo thu tu W->H->C để thực hiện được input share*/
void load_w_share(DRAM *w, int8_t *w_buffer, int pass_idx){
    int start_channel = pass_idx * PARALLEL_CHANNELS;
    int buffer_idx = 0;
    for(int kw = 0; kw < KERNEL_W; kw++){
        for(int kh = 0; kh < KERNEL_H; kh++){
            for(int c = start_channel; c < start_channel + PARALLEL_CHANNELS; c++){
                if(c >= INPUT_C){
                    return;
                }
                int w_idx = kh*(w->d2*w->d3) + kw*(w->d3) + c;
                w_buffer[buffer_idx] = w->dat_8b[w_idx];
                buffer_idx++;
            }
        }
    }
    total_bytes += buffer_idx;
}
void printBuffer(int8_t *buffer){
    for(int i = 0; i < BUFFER_SIZE_BYTES; i++){
        printf("%d ", buffer[i]);
    }
    printf("\n");
}

#endif