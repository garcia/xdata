#include <stdio.h>
#include <stdlib.h>

#include "xdata/xdata.h"

// Functions defined / used by examples.

int color_callback(color c) {
    printf("%d ", c);
    return 0;
}

int pos_callback(void *ptr) {
    printf("%d ", *(int *)ptr);
    return 0;
}

int pixel_callback(void *ptr) {
    printf("%p ", ptr);
    return 0;
}

// Example usage of xenum.h, illustrated via enum.color.h and enum.colortype.h.

void ex_color_index() {
    printf("%d %d", White, color_index(White)); // 10 3
}

void ex_color_str() {
    printf("%d %s", Green, color_str(Green)); // 1 Green
}

void ex_color_iter() {
    color_iter(color_callback); // 0 1 2 10 11
}
    
void ex_color_group() {
    printf("%d %s", Black, colorgroup_str(color_group(Black))); // 11 Grayscale
}

void ex_color_group_iter() {
    color_group_iter(Color, color_callback); // 0 1 2
    printf("/ "); // /
    color_group_iter(Grayscale, color_callback); // 10 11
}

// Example usage of xstruct.h, illustrated via struct.pixel.h and enum.pixeltype.h.

void ex_pixel_index() {
    pixel p = { 5, 10, Red, 0.5 };
    printf("%d", pixel_index(&p, &p.color)); // 3
}

void ex_pixel_member() {
    pixel p = { 5, 10, Red, 0.5 };
    printf("member %d = %f", 3, *(float *)pixel_member(&p, 3)); // member 3 = 0.500000
}

void ex_pixel_str() {
    pixel p = { 5, 10, Red, 0.5 };
    printf("%s = %d", pixel_str(&p, &p.x), p.x); // x = 5
}

void ex_pixel_type_str() {
    pixel p = { 5, 10, Red, 0.5 };
    printf("%s is %s", pixel_str(&p, &p.y), pixel_type_str(&p, &p.y)); // y is int
}

void ex_pixel_iter() {
    pixel p = { 5, 10, Red, 0.5 };
    pixel_iter(&p, pixel_callback); // 0x?? 0x?? 0x?? 0x??
}

void ex_pixel_print_member() {
    pixel p = { 5, 10, Red, 0.5 };
    char *str = pixel_print_member(&p, &p.y, "%%s = %%%s");
    printf("%s", str); // y = 10
    free(str);
}

void ex_pixel_print() {
    pixel p = { 5, 10, Red, 0.5 };
    char *str = pixel_print(&p, "%%s = %%%s", "; ");
    printf("%s", str); // x = 5; y = 10; color = 0x??; alpha = 0.500000
    free(str);
}

void ex_pixel_group() {
    pixel p = { 5, 10, Red, 0.5 };
    printf("in group %s", pixelgroup_str(pixel_group(&p, &p.y))); // in group pix_pos
}

void ex_pixel_group_iter() {
    pixel p = { 5, 10, Red, 0.5 };
    pixel_group_iter(&p, pix_pos, pos_callback); // 5 10
}

// Run each example.

void (*examples[])() = {
    // xenum.h
    ex_color_index,
    ex_color_str,
    ex_color_iter,
    ex_color_group,
    ex_color_group_iter,
    // xstruct.h
    ex_pixel_index,
    ex_pixel_member,
    ex_pixel_str,
    ex_pixel_type_str,
    ex_pixel_iter,
    ex_pixel_print_member,
    ex_pixel_print,
    ex_pixel_group,
    ex_pixel_group_iter,
    // sentinel
    NULL
};

int main(int argc, char *argv[]) {
    int i;

    for (i = 0; examples[i] != NULL; i++) {
        examples[i]();
        printf("\n");
    }

    return 0;
}
