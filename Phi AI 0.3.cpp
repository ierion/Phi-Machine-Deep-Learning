#include <iostream>
#include <vector>
#include "Node.h"
#include "Network.h"
#include <ctime>
#include <random>
#include "randomClass.h"
#include <chrono>
#include <iomanip>

using namespace std;

// DEPRICIATED
void calculateNetwork() // DEPRICIATED
{
	unsigned int input_Layers;
	vector<unsigned int> input_Nodes;
	cout << "How many layers do you wish to make? \n";
	cin >> input_Layers;
	for (int i = 0; i < input_Layers; i++)
	{
		cout << "How many nodes in layer " << i + 1 << " ? \n";
		unsigned int localInput;
		cin >> localInput;
		input_Nodes.push_back(localInput);
	}
	double output = 500;
	for (unsigned int i = 1; i < input_Layers; i++)
	{
		output += (input_Nodes[i] / 724 * input_Nodes[i - 1] / 724);
	}
	cout << "approx mem usage: " << output << "mb " << "(" << output / 1024 << "gb) \n";
	cout << "how many iterations?";
	unsigned int iterationAmt;
	cin >> iterationAmt;
	cout << "what should the input variation be?";
	unsigned int inputVariation;
	cin >> inputVariation;
	cout << "would you like to see the inputs and outputs? (slows process) (y/n)";
	char inputLetter;
	cin >> inputLetter;
	bool visualIO = inputLetter == 'y';
	cout << "Initializing Net . . . \n";
	int timeStart = time(nullptr);
	//phi::Network somethingNetwork(input_Layers, input_Nodes);
	randomClass rdmClass(inputVariation, input_Layers, input_Nodes, iterationAmt);
	phi::Network newNetwork(input_Layers, input_Nodes);
	for (unsigned short int layer = 0; layer < newNetwork.layers; ++layer)
	{
		for (unsigned int node = 0; node < newNetwork.nodes[layer]; ++node)
		{
			newNetwork.net[layer][node].addKHCords({ rdmClass.rdmGenerator(), rdmClass.rdmGenerator(), rdmClass.rdmGenerator() });
		}
	}

	cout << "Net finished in " << time(nullptr) - timeStart << " seconds. \n";
	timeStart = time(nullptr);
	cout << "Calculating x" << iterationAmt << " . . . \n";

	for (int i = 0; i < iterationAmt; i++)
	{
		if (visualIO)
		{
			cout << "\n \n \n ---------Iteration " << i << "---------- \n";
			cout << "Inputs: \n";
		}
		vector<float> tempInput;
		for (int i0 = 0; i0 < newNetwork.nodes[0]; i0++)
		{
			float temp0 = rdmClass.rdmGenerator();
			tempInput.push_back(temp0);
			if (visualIO)
			{
				cout << temp0 << ", ";
			}
		}
		newNetwork.calculate(tempInput);
		vector<float> BP_Input;
		if (visualIO)
		{
			cout << "\n \n Outputs: \n";
			for (int node = 0; node < newNetwork.nodes[input_Layers - 1]; ++node)
			{
				float something = newNetwork.net[input_Layers - 1][node].value + 0;
				BP_Input.push_back(newNetwork.net[input_Layers - 1][node].value);
				cout << something << ", ";
			}
		}
		else
		{
			for (int node = 0; node < newNetwork.nodes[input_Layers - 1]; ++node)
			{
				BP_Input.push_back(newNetwork.net[input_Layers - 1][node].value);
			}
		}
		newNetwork.backProp(BP_Input);
		if (newNetwork.net[1][0].xCordList.size() >= 3)
		{
			for (const auto& layer : newNetwork.net)
			{
				for (phi::Node node : layer)
				{
					node.pointGrouper(3, true);
					node.BCConstructor(true);
				}
			}
		}
		if (visualIO == false)
		{
			cout << "\n iteration #" << i + 1;
		}
	}

	cout << "\nFinished in " << time(nullptr) - timeStart << " seconds. \n";
	/*
		for (int node = 0; node < newNetwork.nodes[input_Layers - 1]; ++node)
		{
			float output = newNetwork.net[input_Layers - 1][node].value;
			cout << output << "\n";
		}*/
	cout << "done \n";
}
//

void DEBUG_calculateNetwork(bool debug = false, unsigned int debugNum = 0)
{
	if (debugNum > 0)
	{
		unsigned int input_Layers;
		vector<unsigned int> input_Nodes;
		unsigned int iterationAmt;
		unsigned int inputVariation;
		char inputLetter;

		input_Layers = debugNum;

		input_Nodes.reserve(debugNum);
		for (signed int  i = 0; i < debugNum; i++)
		{
			input_Nodes.push_back(debugNum);
		}

		float memAmtRaw = 500;
		for (unsigned int i = 1; i < input_Layers; i++)
		{
			memAmtRaw += (input_Nodes[i] / 724 * input_Nodes[i - 1] / 724);
		}

		cout << "approx mem usage: " << memAmtRaw << "mb " << "(" << memAmtRaw / 1024 << "gb) \n";

		iterationAmt = debugNum;

		inputVariation = 10000;

		bool visualIO = true;

		std::cout << "Initializing Net . . . \n";
		int timeStart = time(nullptr);

		randomClass rdmClass(inputVariation, input_Layers, input_Nodes, iterationAmt);
		phi::Network newNetwork(input_Layers, input_Nodes);

		for (unsigned short int layer = 0; layer < newNetwork.layers; ++layer)
		{
			for (unsigned int node = 0; node < newNetwork.nodes[layer]; ++node)
			{
				newNetwork.net[layer][node].addKHCords({ rdmClass.rdmGenerator(), rdmClass.rdmGenerator(), rdmClass.rdmGenerator() });
			}
		}

		if (debug) { std::cout << "Net finished in " << time(nullptr) - timeStart << " seconds. \n"; }
		timeStart = time(nullptr);
		if (debug) { std::cout << "Calculating x" << iterationAmt << " . . . \n"; }

		for (int i = 0; i < iterationAmt; i++)
		{
			if (visualIO)
			{
				if (debug) { std::cout << "\n \n \n ----------Iteration " << i << " ---------- \n"; }
				if (debug) { std::cout << "Inputs: \n"; }
			}
			vector<float> tempInput;

			//inputing random values
			for (int i0 = 0; i0 < newNetwork.nodes[0]; i0++)
			{
				if (visualIO)
				{
					float temp0 = rdmClass.rdmGenerator();
					std::cout << temp0 << ", ";
					tempInput.push_back(temp0);
				}
				else
				{
					tempInput.push_back(rdmClass.rdmGenerator());
				}
			}

			//says debug but is just normal
			newNetwork.DEBUG_Calculate(tempInput, debug);

			if (visualIO)
			{
				std::cout << "\n \n Outputs: \n";
			}

			//inputting output values to temp vector
			vector<float> BP_Input;
			for (int node = 0; node < newNetwork.nodes[input_Layers - 1]; ++node)
			{
				if (debug)
				{
					float inputTempVal = rdmClass.rdmGenerator() / 100;
					BP_Input.push_back(inputTempVal);
					cout << inputTempVal;
				}
				else
				{
					BP_Input.push_back(rdmClass.rdmGenerator());
				}
			}

			// back propagating the temp vector
			newNetwork.DEBUG_backProp(BP_Input, true);
			for (const auto& layer : newNetwork.net)
			{
				for (const phi::Node& node : layer)
				{
					if (debug) { std::cout << "\nfjkodips   " << node.xCordList.size(); }
					if (debug) { std::cout << "\nfjkodips   " << node.yCordList.size(); }
				}
			}

			// grouping variables and constructing the bell curve 
			if (newNetwork.net[1][0].xCordList.size() >= 3)
			{
				for (const auto& layer : newNetwork.net)
				{
					for (phi::Node node : layer)
					{
						node.pointGrouper(3, debug);
						node.BCConstructor(debug);
					}
				}
			}
			if (visualIO == false)
			{
				std::cout << "\n iteration #" << i + 1;
			}
		}
		std::cout << "\nFinished in " << time(nullptr) - timeStart << " seconds. \n";
		/*
			for (int node = 0; node < newNetwork.nodes[input_Layers - 1]; ++node)
			{
				float output = newNetwork.net[input_Layers - 1][node].value;
				cout << output << "\n";
			}*/
		std::cout << "done \n";
		return;
	}
	unsigned int input_Layers;
	vector<unsigned int> input_Nodes;
	unsigned int iterationAmt;
	unsigned int inputVariation;
	char inputLetter;

	cout << "How many layers do you wish to make? \n";
	cin >> input_Layers;

	for (int i = 0; i < input_Layers; i++)
	{
		cout << "How many nodes in layer " << i + 1 << " ? \n";
		unsigned int localInput;
		cin >> localInput;
		input_Nodes.push_back(localInput);
	}

	float memAmtRaw = 500;
	for (unsigned int i = 1; i < input_Layers; i++)
	{
		memAmtRaw += (input_Nodes[i] / 724 * input_Nodes[i - 1] / 724);
	}

	cout << "approx mem usage: " << memAmtRaw << "mb " << "(" << memAmtRaw / 1024 << "gb) \n";

	cout << "how many iterations?";
	cin >> iterationAmt;

	cout << "what should the input variation be?";
	cin >> inputVariation;

	cout << "would you like to see the inputs and outputs? (slows process) (y/n)";
	cin >> inputLetter;
	bool visualIO = inputLetter == 'y';

	std::cout << "Initializing Net . . . \n";
	int timeStart = time(nullptr);

	randomClass rdmClass(inputVariation, input_Layers, input_Nodes, iterationAmt);
	phi::Network newNetwork(input_Layers, input_Nodes);

	for (unsigned short int layer = 0; layer < newNetwork.layers; ++layer)
	{
		for (unsigned int node = 0; node < newNetwork.nodes[layer]; ++node)
		{
			newNetwork.net[layer][node].addKHCords({ rdmClass.rdmGenerator(), rdmClass.rdmGenerator(), rdmClass.rdmGenerator() });
		}
	}

	if (debug) { std::cout << "Net finished in " << time(nullptr) - timeStart << " seconds. \n"; }
	timeStart = time(nullptr);
	if (debug) { std::cout << "Calculating x" << iterationAmt << " . . . \n"; }

	for (int i = 0; i < iterationAmt; i++)
	{
		if (visualIO)
		{
			if (debug) { std::cout << "\n \n \n ----------Iteration " << i << " ---------- \n"; }
			if (debug) { std::cout << "Inputs: \n"; }
		}
		vector<float> tempInput;

		//inputing random values
		for (int i0 = 0; i0 < newNetwork.nodes[0]; i0++)
		{
			if (visualIO)
			{
				float temp0 = rdmClass.rdmGenerator();
				std::cout << temp0 << ", ";
				tempInput.push_back(temp0);
			}
			else
			{
				tempInput.push_back(rdmClass.rdmGenerator());
			}
		}
		
		//says debug but is just normal
		newNetwork.DEBUG_Calculate(tempInput, debug);

		if (visualIO)
		{
			std::cout << "\n \n Outputs: \n";
		}

		//inputting output values to temp vector
		vector<float> BP_Input;
		for (int node = 0; node < newNetwork.nodes[input_Layers - 1]; ++node)
		{
			if (debug)
			{
				float inputTempVal = rdmClass.rdmGenerator() / 100;
				BP_Input.push_back(inputTempVal);
				cout << inputTempVal;
			}
			else
			{
				BP_Input.push_back(rdmClass.rdmGenerator());
			}
		}

		// back propagating the temp vector
		newNetwork.DEBUG_backProp(BP_Input, debug);
		for (const auto& layer : newNetwork.net)
		{
			for (const phi::Node& node : layer)
			{
				if (debug) { std::cout << "\nfjkodips   " << node.xCordList.size(); }
				if (debug) { std::cout << "\nfjkodips   " << node.yCordList.size(); }
			}
		}

		// grouping variables and constructing the bell curve 
		if (newNetwork.net[1][0].xCordList.size() >= 3)
		{
			for (const auto& layer : newNetwork.net)
			{
				for (phi::Node node : layer)
				{
					node.pointGrouper(3, debug);
					node.BCConstructor(debug);
				}
			}
		}
		if (visualIO == false)
		{
			std::cout << "\n iteration #" << i + 1;
		}
	}
	std::cout << "\nFinished in " << time(nullptr) - timeStart << " seconds. \n";
	/*
		for (int node = 0; node < newNetwork.nodes[input_Layers - 1]; ++node)
		{
			float output = newNetwork.net[input_Layers - 1][node].value;
			cout << output << "\n";
		}*/
	std::cout << "done \n";
}

int main()
{
	bool thing = true;
	while (thing)
	{
		float something = 1 / 4;
		cout << "Would you like to make and run Phi Net? (y/n/b/b5) \n";
		string input;
		cin >> input;
		if (input == "n")
		{
			thing = false;
		}
		else if (input == "b")
		{
			DEBUG_calculateNetwork(true);
		}
		else if (input == "b5")
		{
			DEBUG_calculateNetwork(true, 5);
		}
		else
		{
			DEBUG_calculateNetwork(false);
		}
	}
}