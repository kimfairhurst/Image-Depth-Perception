Image-Depth-Perception
======================

An attempt at simulating depth perception using image analysis (C and MIPS).

This program generates a depth map that calculates the differences in coloration between two images.

calcDepthNaive.c -> Offers a naive solution to the problem. <br>
calcDepthOptimized.c -> Optimizes the solution using preprocessor directives (#pragma parallelization), loop optimization, etc. <br>

make_qtree.c -> The program also creates a "quadtree," a recursive data structure that makes a tree-like representation of a depth-map using its color values. <br>

quad2matrix.c -> Converts the quadtree into its original matrix representation. <br>

lfsr_random.s -> a random number generator using a linear feedback shift register. <br>



###Followup
For more details and specs, see:
<br> http://inst.eecs.berkeley.edu/~cs61c/fa14/projs/01/
<br> http://inst.eecs.berkeley.edu/~cs61c/fa14/projs/01/#part-2
