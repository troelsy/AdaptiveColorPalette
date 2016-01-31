// Quick run: gcc main.c lodepng.h lodepng.c -Wall && ./a.out

#include <stdio.h>
#include <stdlib.h>

#include "lodepng.h"
#include "uthash.h"


struct kv_pair {
    int key;
    int value;
    UT_hash_handle hh;
};

struct kv_pair *elements = NULL;

void add_element(int element_key, int value){
    struct kv_pair *s;

    // Check if element already exists
    HASH_FIND_INT(elements, &element_key, s);
    if (s == NULL) {
        s = (struct kv_pair*) malloc(sizeof(struct kv_pair));
        s->key = element_key;
        HASH_ADD_INT(elements, key, s);
    }

    s->value = value;
}

void inc_element(int element_key){
    struct kv_pair *s;

    // Check if element already exists
    HASH_FIND_INT(elements, &element_key, s);
    if (s == NULL) {
        add_element(element_key, 1);
    } else {
        s->value += 1;
    }
}

void delete_all(){
    struct kv_pair *current_element, *tmp;

    HASH_ITER(hh, elements, current_element, tmp) {
        // Delete element
        HASH_DEL(elements, current_element);
        // Free memory
        free(current_element);
    }
}

void find_peaks(int n, int distance){
    // Find up to 'n' colors. Inorder to group colors that look alike, a new color must meet the
    // distance criteria. When checking the criteria, RGB is summed (no average) and compared by
    // calculating the distance to any existing colors in the array. The array is expected to be
    // sorted by size, in which case, this function will output the 'n' peaks of a histogram.

    int buffer[n][2];
    int current = -1;
    int cmp;
    int inrange;
    struct kv_pair *s;

    for(s = elements; s != NULL; s=(struct kv_pair*)(s->hh.next)) {
        if (current == -1){
            // The first and largest element in the array is always a peak point
            buffer[0][0] = s->key;
            buffer[0][1] = (s->key >> 16) + (s->key >> 8 & 255) + (s->key & 255);
            current++;
        } else {
            // Translate the key (color) to a sum of colors, which can be used to compare
            cmp = ((s->key >> 16) + (s->key >> 8 & 255) + (s->key & 255));

            // Test if existing color is too close
            inrange = 0;
            for(int i = 0; i < current+1; i++){
                if (abs(buffer[i][1] - cmp) < distance){
                    inrange = 1;
                    break;
                }
            }

            // If the colors has no resemblance to existing colors: Add it to the array
            if (inrange == 0){
                current++;
                buffer[current][0] = s->key;
                buffer[current][1] = (s->key >> 16) + (s->key >> 8 & 255) + (s->key & 255);
            }
        }

        // Stop if number of colors found is sufficient
        if (current == n-1){
            break;
        }
    }

    for (int i = 0; i < current+1; i++){
        printf("%d, %d, %d\n", buffer[i][0] >> 16,
                               buffer[i][0] >> 8 & 255,
                               buffer[i][0] & 255);
    }
}

int value_sort(struct kv_pair *a, struct kv_pair *b){
    return a->value < b->value;
}

void sort_by_value(){
    HASH_SORT(elements, value_sort);
}

void processPNG(const char* filename){
    unsigned error;
    unsigned char* image;
    unsigned width, height;

    error = lodepng_decode32_file(&image, &width, &height, filename);
    if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

    //printf("(%d, %d)\n", width, height);

    int key;
    for(unsigned i = 0; i < width * height; i += 4){
        // Alpha channel is discarded
        // RGB-key = (R << 16) + (G << 8) + B
        key = ((int)image[i+0] << 16) + ((int)image[i+1] << 8) + (int)image[i+2];
        inc_element(key);
    }

    sort_by_value();
    find_peaks(3, 135); // Get n colors - at most.

    delete_all();
    free(image);
}

int main(int argc, char *argv[]){
    const char* filename = argc > 1 ? argv[1] : "../pic.png";
    processPNG(filename);

    return 0;
}
