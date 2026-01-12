tiling: tiling.c
	gcc -g tiling.c -o tiling.a -lm
tiling_weight_stationary: tiling_weight_stationary.c
	gcc -g tiling_weight_stationary.c -o tiling_weight_stationary.a -lm
tiling_input_share: tiling_input_share.c
	gcc -g tiling_input_share.c -o tiling_input_share.a -lm
# tiling_weight_input: tiling_weight_input.c
# 	gcc -g tiling_weight_input.c -o tiling_weight_input.a -lm
clean:
	rm *.a