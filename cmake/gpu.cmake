# Use NVIDIA-SMI tool to automatically find the
# compute capability of the GPU
function(set_cc)
	# Obtain the git hash and store it to a variable
	execute_process(
		COMMAND nvidia-smi --query-gpu=compute_cap --format=csv
		COMMAND tail -n 1
		COMMAND tr -d .
		OUTPUT_VARIABLE GPU_CC
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	message("GPU compute capability detected: cc" ${GPU_CC})
	set(GPU_CC ${GPU_CC} PARENT_SCOPE)
endfunction()

# Use NVCC to automatically find the
# CUDA version
function(set_cuda)
	# Obtain the git hash and store it to a variable
	execute_process(
		COMMAND nvcc --version
		COMMAND grep release
		COMMAND cut -d " " -f 5
		COMMAND cut -d "," -f 1
		OUTPUT_VARIABLE GPU_CUDA
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	message("CUDA version detected: " ${GPU_CUDA})
	set(GPU_CUDA ${GPU_CUDA} PARENT_SCOPE)
endfunction()