Image-Depth-Perception
======================

An method of simulating depth perception using image analysis (C and MIPS).
This program generates a depth map that calculates the differences in coloration between two images that represent what is seen by the left and right eyes.

<b>calcDepthNaive.c</b> -> Offers a naive solution to the problem. <br>

<b>calcDepthOptimized.c</b> -> Optimizes the solution using preprocessor directives (#pragma parallelization), loop optimization, etc. <br>

<b>make_qtree.c</b> -> Creates a "quadtree," a recursive data structure that makes a tree-like representation of a depth-map using its color values. <br>

<b>quad2matrix.c</b> -> Converts the quadtree into its original matrix representation. <br>

<b>lfsr_random.s</b> -> a random number generator using a linear feedback shift register. <br>



###Followup
For more details and specs, see:
<br> http://inst.eecs.berkeley.edu/~cs61c/fa14/projs/01/
<br> http://inst.eecs.berkeley.edu/~cs61c/fa14/projs/01/#part-2
