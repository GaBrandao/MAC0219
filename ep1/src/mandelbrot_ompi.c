#include "mpi.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 128

double c_x_min;
double c_x_max;
double c_y_min;
double c_y_max;

double pixel_width;
double pixel_height;

int iteration_max = 200;

int image_size;
unsigned char **image_buffer;
unsigned int *iteration_buffer;

int i_x_max;
int i_y_max;
int image_buffer_size;

int gradient_size = 16;
int colors[17][3] = {
                        {66, 30, 15},
                        {25, 7, 26},
                        {9, 1, 47},
                        {4, 4, 73},
                        {0, 7, 100},
                        {12, 44, 138},
                        {24, 82, 177},
                        {57, 125, 209},
                        {134, 181, 229},
                        {211, 236, 248},
                        {241, 233, 191},
                        {248, 201, 95},
                        {255, 170, 0},
                        {204, 128, 0},
                        {153, 87, 0},
                        {106, 52, 3},
                        {16, 16, 16},
                    };

int n_threads;
pthread_t threads[MAX_THREADS];

struct args {
    int y_min;
    int y_max;
};

struct time_track {
    struct timespec c_begin;
    struct timespec c_end;

    struct timespec w_begin;
    struct timespec w_end;
};

void start_time_tracking(struct time_track *t) {
    clock_gettime(CLOCK_REALTIME, &t->w_begin);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t->c_begin);
}

void end_time_tracking(struct time_track *t) {
    clock_gettime(CLOCK_REALTIME, &t->w_end);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t->c_end);
}

double get_wall_time(struct time_track *t) {
    long seconds = t->w_end.tv_sec - t->w_begin.tv_sec;
    long nanoseconds = t->w_end.tv_nsec - t->w_begin.tv_nsec;

    return seconds + nanoseconds*1e-9;
}

double get_clock_time(struct time_track *t) {
    long seconds = t->c_end.tv_sec - t->c_begin.tv_sec;
    long nanoseconds = t->c_end.tv_nsec - t->c_begin.tv_nsec;

    return seconds + nanoseconds*1e-9;
}

void allocate_image_buffer(){
    int rgb_size = 3;
    image_buffer = (unsigned char **) malloc(sizeof(unsigned char *) * image_buffer_size);

    for(int i = 0; i < image_buffer_size; i++){
        image_buffer[i] = (unsigned char *) malloc(sizeof(unsigned char) * rgb_size);
    };
};

void allocate_iteration_buffer(){
    iteration_buffer = (unsigned int *) malloc(sizeof(unsigned int) * image_buffer_size);
}

void update_rgb_buffer(){
    int color, iteration;

    for (int y = 0; y < image_size; ++y)
        for (int x = 0; x < image_size; ++x) {
            iteration = iteration_buffer[(i_y_max * y) + x];
            if(iteration == iteration_max){
                image_buffer[(i_y_max * y) + x][0] = colors[gradient_size][0];
                image_buffer[(i_y_max * y) + x][1] = colors[gradient_size][1];
                image_buffer[(i_y_max * y) + x][2] = colors[gradient_size][2];
            }
            else{
                color = iteration % gradient_size;

                image_buffer[(i_y_max * y) + x][0] = colors[color][0];
                image_buffer[(i_y_max * y) + x][1] = colors[color][1];
                image_buffer[(i_y_max * y) + x][2] = colors[color][2];
            };
        }
};

void write_to_file(){
    FILE * file;
    char * filename               = "output.ppm";
    char * comment                = "# ";

    int max_color_component_value = 255;
    update_rgb_buffer();

    file = fopen(filename,"wb");

    fprintf(file, "P6\n %s\n %d\n %d\n %d\n", comment,
            i_x_max, i_y_max, max_color_component_value);

    for(int i = 0; i < image_buffer_size; i++){
        fwrite(image_buffer[i], 1 , 3, file);
    };

    fclose(file);
};

void mandelbrot_task(int start, int tasksize){
    double z_x;
    double z_y;
    double z_x_squared;
    double z_y_squared;
    double escape_radius_squared = 4;

    int iteration;
    int i_x;
    int i_y;

    double c_x;
    double c_y;

    int i;

    // int y_max = y_min + tasksize;

    for(i = start; i < start + tasksize; i++){
        i_y = i / i_y_max;
        i_x = i % i_y_max;

        c_y = c_y_min + i_y * pixel_height;

        if(fabs(c_y) < pixel_height / 2){
            c_y = 0.0;
        };

        c_x         = c_x_min + i_x * pixel_width;

        z_x         = 0.0;
        z_y         = 0.0;

        z_x_squared = 0.0;
        z_y_squared = 0.0;

        for(iteration = 0;
            iteration < iteration_max && \
            ((z_x_squared + z_y_squared) < escape_radius_squared);
            iteration++){
            z_y         = 2 * z_x * z_y + c_y;
            z_x         = z_x_squared - z_y_squared + c_x;

            z_x_squared = z_x * z_x;
            z_y_squared = z_y * z_y;
        };

        iteration_buffer[i] = iteration;
    };
};

void init(){
    c_x_min = -0.188;
    c_x_max = -0.012;

    c_y_min = 0.554;
    c_y_max = 0.754;

    image_size = 4096;

    i_x_max           = image_size;
    i_y_max           = image_size;
    image_buffer_size = image_size * image_size;

    pixel_width       = (c_x_max - c_x_min) / i_x_max;
    pixel_height      = (c_y_max - c_y_min) / i_y_max;
};


int main(int argc, char *argv[]){
    int numtasks, taskid, start, end, tasksize, blocksize;

    double computing_start, computing_end;
    
    MPI_Status status;

    /***** Initializations *****/
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

    init();
    allocate_iteration_buffer();
    
    blocksize = image_buffer_size / (numtasks - 1);

    if (taskid == 0) {
        computing_start = MPI_Wtime();

        allocate_image_buffer();

        for (int i = 1; i < numtasks; ++i) {
            start = (i - 1) * blocksize;
            end = start + blocksize;

            if (end > image_buffer_size) {
                end = image_buffer_size;
            }

            tasksize = end - start + 1;

            MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&tasksize, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&iteration_buffer[start], tasksize, MPI_UNSIGNED, i, 2, 
                     MPI_COMM_WORLD);
        }

        for (int i = 1; i < numtasks; ++i) {
            MPI_Recv(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&tasksize, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&iteration_buffer[start], tasksize, MPI_UNSIGNED, i, 2,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        computing_end = MPI_Wtime();
    }
    
    else {
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&tasksize, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&iteration_buffer[start], tasksize, MPI_UNSIGNED, 0, 2, 
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        mandelbrot_task(start, tasksize);

        MPI_Send(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&tasksize, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        MPI_Send(&iteration_buffer[start], tasksize, MPI_UNSIGNED, 0, 2, 
                 MPI_COMM_WORLD);
    }

    if (taskid == 0) {
        write_to_file();
        printf("%d,%d,%d,", image_size, numtasks, 1);
        printf("%3.4lf\n", computing_end - computing_start);
    }

    MPI_Finalize();

    return 0;
};