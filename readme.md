IFM: params/ifm.txt ,shape   : [1, 112, 112, 32]
WEIGHTS: params/weights.txt ,shape   : [1, 3, 3, 32]
OFM golden: golden_output/ofm_golden.txt ,shape   : [1, 112, 112, 1]
Stride=(1,1)

Viết lại vòng for trong phép Conv2D: có lệnh load, store, tính toán, share buffer.
Sắp xếp để tính toán sử dụng các phương pháp: tiling, weight stationary, input share.

Code bằng C -> đo latency, sử dụng bao nhiêu memory

- src/ - chứa header file định nghĩa buffer, dram, pe
- tiling.c - sử dụng tiling
- tiling_weight_stationary - sử dụng tiling + weight stationary
- tiling_input_share - sử dụng tiling + weight stationary + input share
- result.txt chứa thời gian cho từng trường hợp
