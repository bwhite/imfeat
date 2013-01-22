#include <stdlib.h>
#include <stdio.h>
#include "dedupe_aux.h"

int main() {
    int height, width, i;
    uint8_t *image_lab;
    double hist[384];
    for (height = 4; height < 1024; height *= 2)
        for (width = 4; width < 1024; width *= 2) {
            printf("%d %d\n", height, width);
            image_lab = malloc(height * width * 3);
            for (i = 0; i < height * width * 3; ++i)
                image_lab[i] = (random() / (double)RAND_MAX) * 256;
            dedupe_image_to_feat(image_lab, hist, height, width);
            for (i = 0; i < 384; ++i)
                printf("%f ", hist[i]);
                printf("\n");
            free(image_lab);
        }
    return 0;
}
