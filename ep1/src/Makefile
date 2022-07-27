OUTPUT=mandelbrot

IMAGE=.ppm

CC=gcc
MPICC=mpicc
# CC_OPT=-std=c17

CC_OMP=-fopenmp
CC_PTH=-pthread

.PHONY: all
all: $(OUTPUT)_omp $(OUTPUT)_pth $(OUTPUT)_seq $(OUTPUT)_ompi $(OUTPUT)_ompi_pth

$(OUTPUT)_omp: $(OUTPUT)_omp.c
	$(CC) -o $(OUTPUT)_omp $(CC_OMP) $(OUTPUT)_omp.c

$(OUTPUT)_pth: $(OUTPUT)_pth.c
	$(CC) -o $(OUTPUT)_pth $(CC_PTH) $(OUTPUT)_pth.c

$(OUTPUT)_seq: $(OUTPUT)_seq.c
	$(CC) -o $(OUTPUT)_seq $(OUTPUT)_seq.c

$(OUTPUT)_ompi: $(OUTPUT)_ompi.c
	$(MPICC) -o $(OUTPUT)_ompi $(OUTPUT)_ompi.c

$(OUTPUT)_ompi_pth: $(OUTPUT)_ompi_pth.c
	$(MPICC) -o $(OUTPUT)_ompi_pth $(OUTPUT)_ompi_pth.c

.PHONY: clean
clean:
	rm $(OUTPUT)_omp $(OUTPUT)_pth $(OUTPUT)_seq $(OUTPUT)_ompi $(OUTPUT)_ompi_pth *$(IMAGE)
