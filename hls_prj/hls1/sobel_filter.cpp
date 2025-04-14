#include <ap_int.h>

#define WIDTH 128
#define HEIGHT 128
#define ARRAY_SIZE (WIDTH * HEIGHT)

static ap_uint<8> memory[ARRAY_SIZE];
static ap_uint<8> processed_memory[ARRAY_SIZE];

void sobel_filter(ap_uint<8> data_in, ap_uint<1> write_enable, ap_uint<1> read_enable, ap_uint<8>& data_out, ap_uint<1>& done) {
    #pragma HLS INTERFACE ap_none port=data_in
    #pragma HLS INTERFACE ap_none port=write_enable
    #pragma HLS INTERFACE ap_none port=read_enable
    #pragma HLS INTERFACE ap_none port=data_out
    #pragma HLS INTERFACE ap_none port=done
    #pragma HLS INTERFACE ap_ctrl_none port=return

    static ap_uint<14> counter = 0;
    static ap_uint<1> memory_filled = 0;
    static ap_uint<1> processing_done = 0;

    #pragma HLS pipeline

    if (write_enable && !memory_filled) {
        memory[counter] = data_in;
        counter++;
        if (counter == ARRAY_SIZE) {
            counter = 0;
            memory_filled = 1;
            processing_done = 0;
            done = 0;
        }
    } else if (memory_filled && !processing_done) {
        const char Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
        const char Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

        for (int y = 1; y < HEIGHT - 1; y++) {
            for (int x = 1; x < WIDTH - 1; x++) {
                #pragma HLS pipeline
                int Gx_sum = 0;
                int Gy_sum = 0;

                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        #pragma HLS unroll
                        int pixel = memory[(y + ky) * WIDTH + (x + kx)];
                        Gx_sum += pixel * Gx[ky + 1][kx + 1];
                        Gy_sum += pixel * Gy[ky + 1][kx + 1];
                    }
                }

                int magnitude = (Gx_sum < 0 ? -Gx_sum : Gx_sum) + (Gy_sum < 0 ? -Gy_sum : Gy_sum);
                processed_memory[y * WIDTH + x] = (magnitude > 255) ? (ap_uint<8>)255 : (ap_uint<8>)magnitude;
            }
        }
        processing_done = 1;
        done = 1;
    } else if (read_enable && processing_done) {
        data_out = processed_memory[counter];
        counter++;
        if (counter == ARRAY_SIZE) {
            counter = 0;
            memory_filled = 0;
            processing_done = 0;
            done = 1;
        } else {
            done = 0;
        }
    } else {
        data_out = 0;
        done = 0;
    }
}
