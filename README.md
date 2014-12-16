SimpleGA
========
A very simple Genetic Algorithm for Max/MSP and Pd.

Input a target string (alphanumeric, numeric or binary) and supply bang messages to evolve a new population.

Fitness is computed using hamming distance or the swap distance. Swap distance currently only supports binary strings and
is intended to operate on binary rhythmic patterns. See [1] for more information on this.

Installing/Building
===================
Binaries for Mac are available in the bin folder along with help files.

Source code is available, needs Flext (http://grrrr.org/research/software/flext/) and Boost Graph Library (http://www.boost.org/doc/libs/1_57_0/libs/graph/doc/).

References
==========
[1] Guastavino, C., Gómez, F., Toussaint, G., Marandola, F., & Gómez, E. (2009). Measuring Similarity between Flamenco Rhythmic Patterns. Journal of New Music Research, 38(2), 129–138.