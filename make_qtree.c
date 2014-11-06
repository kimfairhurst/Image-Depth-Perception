#include <stdlib.h>
#include <stdio.h>
#include "quadtree.h"
#include "make_qtree.h"
#include "utils.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

void toQuadTree(unsigned char *depth_map, int map_width, qNode *thisNode, int size, int x, int y);

//returns whether or not all pixels in given field have the same grayscale. 
//returns 256 if non -homogenous and the grayscale value otherwise. 

int homogenous(unsigned char *depth_map, int map_width, int x, int y, int section_width) {

    int grayscale_value = depth_map[x + (y * map_width)]; 

    for (int i = 0; i < section_width; i++){
        for (int j = 0; j < section_width; j++){

            // if (section_width == 2){
            //     printf("%s", "checking the pixel ");
            //     printf("%d", i);
            //     printf("%d", j);
            //     printf("\n");
            // }

            if (depth_map[(y + j) * map_width + (x + i)] != grayscale_value){
                grayscale_value = 256;
                break;
            }
        }
    }
    return grayscale_value;
}

//Determines whether a given qNode is a Leaf. 

int isLeaf(unsigned char *depth_map, int map_width, int x, int y, int section_width){
    if (homogenous(depth_map, map_width, x, y, section_width) == 256){
        return 0;
    }
    return 1; 
}

//Converts a depth map to a quad tree. Returns a pointer to root qNode. 
//Calls recursive function toQuadTree 
qNode *depth_to_quad(unsigned char *depth_map, int map_width) {

    qNode *root;
    root = malloc(sizeof(qNode));

    if (root == NULL){
        allocation_failed(); 
    }

    toQuadTree(depth_map, map_width, root, map_width, 0, 0); 

    /* malloc: if pointer returned by malloc is NULL, call allocation_failed() */
    return root;

}

void toQuadTree(unsigned char *depth_map, int map_width, qNode *thisNode, int size, int x, int y){

    if (isLeaf(depth_map, map_width, x, y, size)){

        // printf("%s", "the pixel (x, y): ");
        // printf("%d", x);
        // printf("%d", y);
        // printf("%s", " is leaf! ");    
        // printf("\n");         

        thisNode->leaf = 1;
        thisNode->size = size;
        thisNode->gray_value = homogenous(depth_map, map_width, x, y, size);
        thisNode->x = x;
        thisNode->y = y;

        thisNode->child_NW = NULL;
        thisNode->child_NE = NULL;
        thisNode->child_SE = NULL;
        thisNode->child_SW = NULL;

    }
    else {

        thisNode->leaf = 0;
        thisNode->size = size;
        thisNode->gray_value = 256;
        thisNode->x = x;
        thisNode->y = y;

        qNode *child1 = malloc(sizeof(qNode));
        qNode *child2 = malloc(sizeof(qNode));
        qNode *child3 = malloc(sizeof(qNode));
        qNode *child4 = malloc(sizeof(qNode));

        if (child1 == NULL || child2 == NULL ||child3 == NULL ||child4 == NULL){
            allocation_failed(); 
        }

        thisNode->child_NW = child1;
        thisNode->child_NE = child2;
        thisNode->child_SW = child3;
        thisNode->child_SE = child4;

        int mid = size/2;

        toQuadTree(depth_map, map_width, child1, size/2, x, y);
        toQuadTree(depth_map, map_width, child2, size/2, x + mid, y);
        toQuadTree(depth_map, map_width, child3, size/2, x, y + mid);
        toQuadTree(depth_map, map_width, child4, size/2, x + mid, y + mid);

    }
    
}


void free_qtree(qNode *qtree_node) {
    if(qtree_node) {
        if(!qtree_node->leaf){
            free_qtree(qtree_node->child_NW);
            free_qtree(qtree_node->child_NE);
            free_qtree(qtree_node->child_SE);
            free_qtree(qtree_node->child_SW);
        }
        free(qtree_node);
    }
}
