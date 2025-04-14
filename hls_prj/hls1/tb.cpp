#include <iostream>
#include <fstream>
#include <vector>
#include "ap_int.h"

#define WIDTH 128
#define HEIGHT 128

void sobel_filter(ap_uint<8> input[WIDTH * HEIGHT], ap_uint<8> output[WIDTH * HEIGHT]);

int main() {
    ap_uint<8> input[WIDTH * HEIGHT];
    ap_uint<8> output[WIDTH * HEIGHT];

    std::ifstream infile("C:\\Users\\User1\\Desktop\\hls1\\resized_128x128_gray_image_pixels.txt");
    if (!infile) {
        std::cerr << "Error opening file!" << std::endl;
        return -1;
    }

    int pixel_value;
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            infile >> pixel_value;
            input[i * WIDTH + j] = static_cast<ap_uint<8>>(pixel_value);
        }
    }
    infile.close();

    sobel_filter(input, output);

    std::ofstream outfile("C:\\Users\\User1\\Desktop\\hls1\\sobel_output.txt");
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            outfile << static_cast<int>(output[i * WIDTH + j]) << " ";
        }
        outfile << "\n";
    }
    outfile.close();

    std::cout << "Sobel edge detection completed and output saved to sobel_output.txt." << std::endl;
    return 0;
}
