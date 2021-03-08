#pragma once
#include <vector>

using namespace std;

namespace phi
{
	class CompressedNode
	{
		double value;
		double khList;
		double sted[4];
		vector<double> connectionWeights;
	};
}