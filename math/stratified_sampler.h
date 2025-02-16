#include <vector>
#include "vec2.h"

class stratified_sampler {
	std::vector<std::vector<int>> strata_table; // Permutation of stratas for sample depth
	std::vector<int> strike_idxs; // Next index to strike out
	std::vector<int> sample_depths; // Sample depth from every strata
	// every sample, we have to fill 5000 elements. this is bottlenecked by sample with most bounces... 

	stratified_sampler(int max_depth, int num_strata) {
	}

	float randf_strat() {

	}

	point2 rand_point2_strat() {
		// Pick random strata
		// Jitter sample from middle of strata

	}
};