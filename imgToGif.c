#include <stdio.h>
#include <stdlib.h>

// Define implementations before including the headers
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"

#include "libs/gif.h"

int main(int argc, char *argv[]) {
    // Check for correct number of arguments
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <path_to_image> <output_path (Include the whatever_name_you_want.gif as part of the path)>\n", argv[0]);
        return -1;
    }

    // Load the image using stb_image
    int width, height, channels;
    unsigned char *img = stbi_load(argv[1], &width, &height, &channels, 3); 
    if (img == NULL) {
        fprintf(stderr, "Error loading image: %s\n", argv[1]);
        return -1;
    }
    
    // Create a buffer with RGBA format for the GIF library
    uint8_t *gif_buffer = (uint8_t *)malloc(width * height * 4);
    if (gif_buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory for the GIF buffer\n");
        stbi_image_free(img);
        return -1;
    }

    // Convert RGB to RGBA (the alpha will be used by the GIF library for palette indexing)
    for (int i = 0; i < width * height; i++) {
        gif_buffer[i * 4 + 0] = img[i * 3 + 0]; 
        gif_buffer[i * 4 + 1] = img[i * 3 + 1]; 
        gif_buffer[i * 4 + 2] = img[i * 3 + 2]; 
        gif_buffer[i * 4 + 3] = 255;            
    }

    // Write the GIF using the library's built-in palette generation and dithering
    GifWriter gif;
    int bitDepth = 8; // Maximum color depth (256 colors)
    bool dither = true; // Enable dithering for better color representation - line 56
    
    // Initialize the GIF writer
    if (!GifBegin(&gif, argv[2], width, height, 100, bitDepth, false)) {
        fprintf(stderr, "Error creating GIF file: %s\n", argv[2]);
        free(gif_buffer);
        stbi_image_free(img);
        return -1;
    }
    
    // Write the first frame
    if (!GifWriteFrame(&gif, gif_buffer, width, height, 100, bitDepth, dither)) {
        fprintf(stderr, "Error writing frame to GIF\n");
        free(gif_buffer);
        stbi_image_free(img);
        GifEnd(&gif);
        return -1;
    }
    
    // Finalize the GIF and free up memory
    GifEnd(&gif);
    free(gif_buffer);
    stbi_image_free(img);
    
    printf("Successfully converted image to GIF: %s\n", argv[2]);
    return 0;
}