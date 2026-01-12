#ifndef CONFIG_H
#define CONFIG_H
// --- CẤU HÌNH BÀI TOÁN ---
#define INPUT_H 112
#define INPUT_W 112
#define INPUT_C 32
#define KERNEL_H 3
#define KERNEL_W 3
#define OUTPUT_F 1 
#define OUTPUT_H 112
#define OUTPUT_W 112
#define STRIDE 1
#define PADDING 1

// --- CẤU HÌNH PHẦN CỨNG (HW SPEC) ---
#define NUM_PE 48               
#define MACS_PER_PE 3           
#define BUFFER_SIZE_BYTES 144   // 48 PE * 3 inputs * 1 byte
#define PARALLEL_CHANNELS 16    // Số channel xử lý song song

// --- CẤU HÌNH HIỆU NĂNG (PERFORMANCE METRICS) ---
#define SYSTEM_FREQ_MHZ 100.0   // Tần số hoạt động: 100 MHz
#define DRAM_BUS_WIDTH_BYTES 8  // Bus 64-bit (8 bytes/cycle)
#define PE_COMPUTE_CYCLES 1     // Số cycle để PE array hoàn thành tính toán (Pipelined)

#endif