#include "weightedrandom.h"

#include <vector>
#include <iostream>

WeightedRandom::WeightedRandom(std::vector<double> weights)
{
	cumulativeNormalisedWeights = { 0.0 };

	for (unsigned int i = 0; i < weights.size(); i++)
	{
		cumulativeNormalisedWeights.push_back(cumulativeNormalisedWeights[i] + weights[i]);
	}

	double total = cumulativeNormalisedWeights.back();

	for (unsigned int i = 0; i < cumulativeNormalisedWeights.size(); i++)
	{
		cumulativeNormalisedWeights[i] /= total;
	}
}

unsigned int WeightedRandom::sample()
{
	if (cumulativeNormalisedWeights.size() <= 1) {
		return 0;
	}

	// pick random float
	double random = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);

	int left = 0;
	int right = cumulativeNormalisedWeights.size() - 1;
	int index = right / 2;

	// binary search to get corresponding index
	while (!(cumulativeNormalisedWeights[index] <= random && cumulativeNormalisedWeights[index + 1] > random))
	{
		if (cumulativeNormalisedWeights[index] > random)
		{
			right = index;
		}
		else {
			left = index;
		}

		index = left + (right - left) / 2;
	}

	if (!(index >= 0 && index <= cumulativeNormalisedWeights.size()))
	{
		std::cout << "FAIL" << std::endl;
	}

	return index;
}