#include "src/buffer.h"
#include "src/config.h"
#include "src/dram.h"
#include "src/pe.h"
#include <math.h>

unsigned long long total_cycles = 0;
int main(){
    // Khai báo các DRAM
    DRAM ifm;
    DRAM ofm;
    DRAM w;
    initDRAM(&ifm, IFM, 1, 112, 112, 32);
    initDRAM(&ofm, OFM, 1, 112, 112, 1);
    initDRAM(&w, WEIGHT, 1, 3, 3, 32);
    loadDRAM(&ifm, "ifm.txt");
    loadDRAM(&w, "weights.txt");
    // printf("IFM\n");
    // printDRAM(&ifm);
    // printf("W\n");
    // printDRAM(&w);

    // Buffer
    int8_t ifm_buffer[BUFFER_SIZE_BYTES];
    int8_t w_buffer[BUFFER_SIZE_BYTES];
    // PE array
    PE_Array pe_array;
    initPE_Array(&pe_array, ifm_buffer, w_buffer);





    //===== Loop dể tính ofm =====
    int num_passes = ceil((float)INPUT_C / PARALLEL_CHANNELS);// Số lần load để tính cho pixel - 2 lần
    // Chạy từng output pixel
    for(int ho = 0; ho < OUTPUT_H; ho++){
        for(int wo = 0; wo < OUTPUT_W; wo++){
            // Mỗi điểm cần 2 lần tính
            int32_t p_sum = 0;
            for(int p = 0; p < num_passes; p++){
                load_ifm_buffer(&ifm, ifm_buffer, ho, wo, p);
                load_w_buffer(&w, w_buffer, p);
                p_sum += runPE_Array(&pe_array);
            }
            ofm.dat_32b[ho*(ofm.d2*ofm.d3) + wo*ofm.d3] = p_sum;
        }
    }
    // ===== Done =====



    // Tính DMA cycles
    total_dma_cycles += ceil((double)total_bytes / DRAM_BUS_WIDTH_BYTES);
    // Check output
    if(checkWithGoldenOutput(&ofm, "ofm_golden.txt") == 1){
        printf("---- Ofm tinh toan dung ----\n\n");
    }
    else{
        printf("---- Ofm tinh sai ----\n\n");
    }
    // In config và tính thời gian
    total_cycles = total_dma_cycles + total_comp_cycles;
    // Tính toán thời gian thực thi t = n * T / 1000(ms)
    double total_time_ms = (double)total_cycles / (SYSTEM_FREQ_MHZ * 1000.0);
    printf("--- STARTING SIMULATION ---\n");
    printf("Specs:\n");
    printf("  - Frequency: %.1f MHz\n", SYSTEM_FREQ_MHZ);
    printf("  - DMA Bandwidth: %d Bytes/cycle\n", DRAM_BUS_WIDTH_BYTES);
    printf("  - PE Array: %d PEs (Parallel)\n", NUM_PE);
    printf("---------------------------\n");
    // In thời gian thực thi
    printf("\n--- PERFORMANCE REPORT ---\n");
    printf("Total Output Pixels: %d\n", OUTPUT_H * OUTPUT_W);
    printf("Total Cycles: %llu\n", total_cycles);
    printf("  - DMA Cycles:     %llu (%.2f%%)\n", total_dma_cycles, (double)total_dma_cycles/total_cycles*100.0);
    printf("  - Compute Cycles: %llu (%.2f%%)\n", total_comp_cycles, (double)total_comp_cycles/total_cycles*100.0);
    printf("Estimated Time: %.4f ms\n", total_time_ms);
    printf("--------------------------\n");
    freeDRAM(&ifm);
    freeDRAM(&w);
    freeDRAM(&ofm);
    return 0;
}