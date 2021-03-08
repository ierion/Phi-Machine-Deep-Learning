#pragma once
#include <random>
#include <vector>

using namespace std;

class randomClass
{
public:

	int variation = 0;
	vector<float> random = { };
	unsigned int rdmIndex = 0;

	randomClass(int variationAmt, unsigned int layers, vector<unsigned int> nodes, unsigned int iterationAmt)
	{
		std::default_random_engine generator;
		std::uniform_int_distribution<int> distribution(0 - variationAmt * 100, variationAmt * 100);
		int dice_roll = distribution(generator);  // generates number in the range 1..6

		for (unsigned int layer = 0; layer < layers; layer++)
		{
			for (int i = 0; i < nodes[layer] * 6; i++)
			{
				random.push_back(distribution(generator) / 100);
			}
		}
		for (unsigned int i = 0; i < iterationAmt; i++)
		{
			for (signed int i = 0; i < nodes[layers - 1]; i++)
			{
				random.push_back(distribution(generator));
			}
			for (unsigned int node = 0; node < nodes[0]; node++)
			{
				random.push_back(distribution(generator) / 100);
			}
		}
		for (int i = 0; i < 100; i++)
		{
			random.push_back(distribution(generator) / 100);
		}
	}

	float rdmGenerator()
	{
		float something = random[rdmIndex];
		rdmIndex++;
		return something;
	}
};
