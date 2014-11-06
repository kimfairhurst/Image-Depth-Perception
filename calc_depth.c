#include "calc_depth.h"
#include "utils.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "feature.h"

#define MAX(a,b) (( a> b)? a : b)
#define SQUARED(a) (a * a)
#define ABS(x) (((x) < 0) ? (-(x)) : (x))

int squared(int f1);
int isEdge(int xcoord, int ycoord, int feature_width, int feature_height, int image_width, int image_height);
void makeFeature(unsigned char *newfeature, unsigned char *image, int feature_width, int feature_height, int x, int y, int image_width); 
int findMostSimilar(unsigned char *mostSimilar, unsigned char *left_feature, unsigned char *right_image, int feature_width, int feature_height, int image_width, int image_height, int maximum_displacement, int pixelX, int pixelY);
int max(int a, int b);
unsigned int euclideanSquaredDistance (unsigned char *f1, unsigned char *f2, int feature_width, int feature_height);

/* Implements the normalized displacement function */
unsigned char normalized_displacement(int dx, int dy,
        int maximum_displacement) {

    double squared_displacement = dx * dx + dy * dy;
    double normalized_displacement = round(255 * sqrt(squared_displacement) / sqrt(2 * maximum_displacement * maximum_displacement));
    return (unsigned char) normalized_displacement;

}

void calc_depth(unsigned char *depth_map, unsigned char *left,
        unsigned char *right, int image_width, int image_height,
        int feature_width, int feature_height, int maximum_displacement) {
	/* specialty case where max displacement == 0, all values of depth_map == 0
			if (maximum_displacement == 0){
				set all depth map vals = 0;
			}
 		*/ 

	// unsigned char a[] = { 3, 3, 3, 3};
	// unsigned char b[] = { 3, 2, 1, 0};
	// unsigned int result = euclideanSquaredDistance(a, b, feature_width, feature_height);
	// printf("%s", "euclid result is: ");
	// printf("%d", result);
	// printf("\n");

	if (maximum_displacement == 0){
		for (int a = 0; a < image_height; a++){
			for (int b = 0; b < image_width; b++){
				depth_map[b + (a * image_width)] = 0;
			}
	}
	/* For every pixel in *left, 
		if !(isEdge(currentPixel)){
			makeFeature(int x, int y, currentPixel);
			find_most_similar(...);
			normalized_displacement(...);
			insertInDepthMap(...);
		} else {
			set depth map = 0 
	*/
	} else {

			unsigned char leftFeature[(feature_width*2+1)*(feature_height*2+1)];
			unsigned char mostSimilar[(feature_width*2+1)*(feature_height*2+1)];

			for (int y = 0; y < image_height; y++){ 
				for (int x = 0; x < image_width; x++){ 

					if (!isEdge(x, y, feature_width, feature_height, image_width, image_height)){

						//leftFeature = (feature*)malloc(sizeof(feature)); 
						makeFeature(leftFeature, left, feature_width, feature_height, x, y, image_width);
						//mostSimilar = (feature*) malloc(sizeof(feature));

						int displacement;

						displacement = findMostSimilar(mostSimilar, leftFeature, right, feature_width, feature_height, image_width, image_height, maximum_displacement, x, y);

						// SOMETHING OFF WHEN CALCULATING DX/DY IS OFF:
						// 		-not updating when similarities are same? (checked)
						// 		-doesn't iterate through loop?
						// 		-something wrong with FindMostSimilar X, Y coords 

						depth_map[(y * image_width) + x] = displacement;

					} else {
						depth_map[(y * image_width) + x] = 0;
					}
				}
			}
		}

	}


/* See if pixel is on edge of image */
/* If possible to make a feature around the current pixel, do so; otherwise, set depth_map of current pixel = 0*/

int isEdge(int xcoord, int ycoord, int feature_width, int feature_height, int image_width, int image_height){
	int xLeftDiff, xRightDiff, yUpDiff, yDownDiff; 
	xLeftDiff = xcoord - feature_width;
	xRightDiff = xcoord + feature_width;
	yUpDiff = ycoord + feature_height;
	yDownDiff = ycoord - feature_height;

	if (xLeftDiff < 0 || xRightDiff >= image_width || yUpDiff >= image_height || yDownDiff < 0){
		return 1;
	} return 0;
}

int isFeatureEdge(int centerX, int centerY, int xcoord, int ycoord, int max_displacement, int image_width, int image_height){
	int xDiff = centerX + xcoord - max_displacement;
	int yDiff = centerY - ycoord - max_displacement;

	if (xDiff < 0 || xDiff >= image_width || yDiff < 0 || yDiff >= image_height){
		return 0;
	} else {
		return 1;
	}


}


 //Make a feature around the current pixel 
void makeFeature(unsigned char *newfeature, unsigned char *image, int feature_width, int feature_height, int x, int y, int image_width){
	//IMPORTANT 

	for (int a = 0; a < (2 * feature_width) + 1 ; a++){
		for (int b = 0; b < (2 * feature_height)+1; b++){
			newfeature[(b * feature_height) + a] = image[x + a + (y + b) * image_width];
		}
	}

}

//mostSimilar = findMostSimilar(leftFeature, right, 0, 0, 3, 3, 1, 1, 1);

/* Returns the most similar feature from the right image. Calls euclidean_square_distance */
int findMostSimilar(unsigned char *mostSimilar, unsigned char *left_feature, unsigned char *right_image, int feature_width, int feature_height, int image_width, int image_height, int maximum_displacement, int pixelX, int pixelY){

	int startX = pixelX - maximum_displacement;
	int startY = pixelY - maximum_displacement;

	unsigned char currentRightFeature[(feature_width*2+1)*(feature_height*2+1)];

	unsigned int smallestSoFar = 255;
	int displacement = 0;
	int temp_displacement;

			// printf("%s", "Pixel (X, Y): ");
			// printf("%d", pixelX);
			// printf("%d", pixelY);
			// printf("\n");

        for (int y = 0; y < 2 * maximum_displacement + 1; y++) {
          for (int x = 0; x < 2 * maximum_displacement + 1; x++) {

			if (isFeatureEdge(pixelX, pixelY, x, y, maximum_displacement, image_width, image_height)){
				temp_displacement = 255;
			}

			makeFeature(currentRightFeature, right_image, feature_width, feature_height, x, y, image_width);

			int dx = ABS(pixelX - x); 
			int dy = ABS(pixelY - y); 

			int distance = euclideanSquaredDistance(left_feature, currentRightFeature, feature_width, feature_height);

			// if (x == 1 && y == 1){
			// 	printf("%s", "Point (X, Y): ");
			// 	printf("%d", x);
			// 	printf("%d", y);
			// 	printf("\n");
			// 	printf("%d", distance);
			// 	printf("\n");
			// }

			temp_displacement = normalized_displacement(x - maximum_displacement, y - maximum_displacement, maximum_displacement);

			if (distance < smallestSoFar) {
				smallestSoFar = distance; 
				mostSimilar = currentRightFeature;
				displacement = normalized_displacement(dx, dy, maximum_displacement);

			} else if (distance == smallestSoFar){
				if (temp_displacement < displacement){
					mostSimilar = currentRightFeature; 
				} 
			}
		}
	}

	return displacement;
}

/* Calculates the Euclidean Squared Distance for a given pixel*/
/*returns int smallestDistance*/
unsigned int euclideanSquaredDistance (unsigned char *f1, unsigned char *f2, int feature_width, int feature_height){

	int distance = 0;
	int temp = 0;

    for (int y = 0; y < 2 * feature_height + 1; y++) {
        for (int x = 0; x < 2 * feature_width + 1; x++) {
               temp = (f1[x + y * (2 * feature_width + 1)] - f2[x + y * (2 * feature_width + 1)]);
               distance += (temp * temp);

        }      
	}

	return distance;
}

