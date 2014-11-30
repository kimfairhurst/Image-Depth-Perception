
// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif
//math 
#include <limits.h>
#include <math.h>

#include <stdbool.h>
#include "utils.h"
#include "calcDepthOptimized.h"
#include "calcDepthNaive.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

#define MAX(a, b) ((a>b)? a:b)
#define MIN(a, b) ((a<b)? a:b)


void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement)
{
	//bounds for unrolling calculated outside the loop 
	// unroll by 8s, 4s, then 1s
	int first_unroll_max = (2 * featureWidth + 1)/8 * 8;
	int second_unroll_max = (2 * featureWidth + 1)/4 * 4;
	int third_unroll_max = 2 * featureWidth + 1;

	// Fill block of memory function 
	memset(depth, 0, imageWidth * imageHeight * sizeof(int));

	#pragma omp parallel 
	{
		#pragma omp for
		//middle 
		for (int y = featureHeight; y < imageHeight - featureHeight; y++)
		{
			for (int x = featureWidth; x < imageWidth - featureWidth; x++)
			{

				float minimumSquaredDifference = -1;
				int minimumDy = 0;
				int minimumDx = 0;

				int low_bound_dx =  MAX(-1 * maximumDisplacement, featureWidth - x);
				int high_bound_dx = MIN(imageWidth - featureWidth - x - 1, maximumDisplacement);

				int low_bound_dy = MAX(-1 * maximumDisplacement, featureHeight - y);
				int high_bound_dy = MIN(imageHeight - featureHeight - y - 1, maximumDisplacement);

				for (int dy = low_bound_dy; dy <= high_bound_dy; dy++)
				{
					for (int dx = low_bound_dx; dx <= high_bound_dx; dx++)
					{
						float squaredDifference = 0;
						float arr[4];
						__m128 sum = _mm_setzero_ps();

							// UNROLL BY 8's
							for (int boxX = 0; boxX < first_unroll_max; boxX+=8)
							{
								for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
								{
									int leftIndex = (y + boxY) * imageWidth + (x + boxX - featureWidth); 
									int rightIndex = (y + dy + boxY) * imageWidth + (x + dx + boxX - featureWidth); 
									// FIRST
									__m128 v0 = _mm_loadu_ps(left + leftIndex); 
									__m128 v1 = _mm_loadu_ps(right + rightIndex); 
									__m128 sub = _mm_sub_ps(v0, v1);
									__m128 mul = _mm_mul_ps(sub, sub);

									sum = _mm_add_ps(mul, sum);

									// SECOND
									v0 = _mm_loadu_ps(left + leftIndex + 4); 
									v1 = _mm_loadu_ps(right + rightIndex + 4); 
									sub = _mm_sub_ps(v0, v1);
									mul = _mm_mul_ps(sub, sub);

									sum = _mm_add_ps(mul, sum);
								}
							}
							

							// UNROLL BY 4's
							for (int boxX = first_unroll_max; boxX < second_unroll_max; boxX+=4)
							{
								for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
								{
									int leftIndex = (y + boxY) * imageWidth + (x + boxX - featureWidth); 
									int rightIndex = (y + dy + boxY) * imageWidth + (x + dx + boxX - featureWidth); 

									__m128 v0 = _mm_loadu_ps(left + leftIndex); 
									__m128 v1 = _mm_loadu_ps(right + rightIndex); 
									__m128 sub = _mm_sub_ps(v0, v1);
									__m128 mul = _mm_mul_ps(sub, sub);
									sum = _mm_add_ps(mul, sum);
								}
							}

							// UNROLL BY 1's
							for (int boxX = second_unroll_max; boxX < third_unroll_max; boxX++)
							{
								for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
								{
									int leftX = x + boxX - featureWidth;
									int leftY = y + boxY;
									int rightX = x + dx + boxX - featureWidth;
									int rightY = y + dy + boxY;

									float difference = left[leftY * imageWidth + leftX] - right[rightY * imageWidth + rightX];
									squaredDifference += difference * difference;
								}
							}


						_mm_storeu_ps(arr, sum);
						squaredDifference += arr[0] + arr[1] + arr[2] + arr[3];

						if ((minimumSquaredDifference == -1) || ((minimumSquaredDifference == squaredDifference) && (displacementNaive(dx, dy) < displacementNaive(minimumDx, minimumDy))) || (minimumSquaredDifference > squaredDifference))
						{
							minimumSquaredDifference = squaredDifference;
							minimumDx = dx;
							minimumDy = dy;
						}
					}
				}

				if ((minimumSquaredDifference ==-1) || ((minimumSquaredDifference != -1) && (maximumDisplacement == 0)))
				{
					depth[y * imageWidth + x] = 0;
				}
				else
				{
					depth[y * imageWidth + x] = displacementNaive(minimumDx, minimumDy);
				}
			}
		}
	}
}

