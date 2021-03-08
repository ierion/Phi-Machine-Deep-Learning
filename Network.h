#pragma once
#include <vector>
#include <ctime>
#include <random>
#include <cstdlib>
#include "Node.h"

using namespace std;

namespace phi
{
	class Network
	{
	public:
		vector<vector<Node>> net;
		vector<unsigned int> nodes;
		unsigned int layers;
		unsigned int weightVariation = 10000; // max 10^x amount
		Network(unsigned int Layers, vector<unsigned int> Nodes)
		{
			layers = Layers;
			nodes = Nodes;
			Node stanNode;

			std::default_random_engine generator;													// for random weights
			std::uniform_int_distribution<int> distribution(0 - weightVariation, weightVariation);	//

			int maxConnections = 0;
			int maxNodes = 0;

			int errori = 0;

			//errori = 0;  cout << "check 1." << errori; errori++; // for checking for error location and etc

			net.push_back({});
			for (unsigned int node = 0; node < nodes[0]; node++)
			{
				net[0].push_back(stanNode);
			}
			for (int layer = 1; layer < layers; layer++) {
				net.push_back({});
				for (int node = 0; node < nodes[layer]; node++) {
					net[layer].push_back(stanNode);
					for (int connNum = 0; connNum < nodes[layer - 1]; connNum++)
					{
						int temp = distribution(generator);
						net[layer][node].connWeight.push_back(temp);	// allowing for variation in both pos and neg
						//cout << net[layer][node].connWeight[connNum] << "\n";
						//cout << net[layer][node].connWeight[connNum]; // note mem inefficiency
					}
				}
			}
		}

		void compressNet() // goes through nodes and weights and compresses the values down.
		{
			// often through the process that we will be going through, there will be ballooning of values of weights and bell curves. I think that this is what they mean
			// by diminishing gradient problem, however it is not ballooning for them because they compress all of it down via sigmoid. Anyways, for instance we want to
			// avoid where there is like:  || 2 * 124032.~ - 1 * 113509.~ -> Equation(khList = {1, .000008, 2, .000008}) -> ~1 || this can be done by condensing/expanding
			// x inputs like: || 2 * (124032 * .000008) - 1 * (113509 * .000008) -> Equaiton(khList = {1, (.000008 * 12400), 2, (.000008 * 12400)}) ||
		}

		void backProp(vector<float> outputValues) // DEPRICIATED
		{
			for (int layer = layers - 1; layer >= 0; layer--)
			{
				if (layer == layers - 1)
				{
					for (int node = 0; node < nodes[layer]; node++)
					{
						vector<float> xCords;
						vector<float> yCords;
						net[layer][node].movingTrueVal = outputValues[node];
						cout << "\nfb958" << net[layer][node].movingInp << " | " << outputValues[node];
						xCords.push_back(net[layer][node].movingInp);
						yCords.push_back(outputValues[node]);
						net[layer][node].connWeightAmt++;
						for (int conn = 0; conn < nodes[layer - 1]; conn++)
						{																																				// change this process by adding
							net[layer][node].connWeight[conn] = (net[layer][node].connWeight[conn] + net[layer][node].movingTrueVal) / net[layer][node].connWeightAmt;	// in thing to understand change
						}																																				//
						net[layer][node].addOrderedCords(xCords, yCords);																								// in bellcurve of prev function
					}
				}
				else
				{
					for (int node = 0; node < nodes[layer]; node++)
					{
						vector<float> xCords;
						vector<float> yCords;
						net[layer][node].connWeightAmt++;
						for (int conn = 0; conn < nodes[layer - 1]; conn++)
						{
							net[layer][node].connWeight[conn] = (net[layer][node].connWeight[conn] + net[layer - 1][conn].value) / net[layer][node].connWeightAmt;
						}
						float output = 0;
						xCords.push_back(net[layer][node].movingInp);
						for (int upNode = 0; upNode < nodes[layer + 1]; upNode++)									// calculating total amount that the function
						{																							// would have to have
							output += net[layer + 1][upNode].movingTrueVal * net[layer + 1][upNode].connWeight[node];	//
						}																							//
						yCords.push_back(output / nodes[layer]);													//
						net[layer][node].movingTrueVal = output / nodes[layer];
						net[layer][node].addOrderedCords(xCords, yCords);
					}
				}
			}
		}
		void calculate(vector<float> inputValues) // DEPRICIATED
		{
			for (int layer = 0; layer < layers; layer++)
			{
				for (int node = 0; node < nodes[layer]; node++)
				{
					float input = 0;
					if (layer == 0)
					{
						input = inputValues[node];
					}
					else
					{
						for (unsigned int connNum = 0; connNum < nodes[layer - 1]; connNum++)
						{
							cout << "\n    -cf3a4" << net[layer - 1][connNum].value;
							cout << "\n    -cf3b4" << net[layer][node].connWeight[connNum];
							cout << "\n    -cf3c4" << weightVariation;
							input += (net[layer - 1][connNum].value * net[layer][node].connWeight[connNum]) / weightVariation;
							cout << "cf34 " << input;
						}
					}
					cout << "\n1230 " << input;
					net[layer][node].movingInp = input;
					float output = 0;
					unsigned int calcLength;

					if (input > net[layer][node].khList[net[layer][node].khList.size() - 2])			// possible bug: if size is index, or if right side != last k in sequence for other reason
					{																					//
						cout << "\n790c";
						for (int i = 2; i < net[layer][node].khList.size() - 2; i += 2)					// this is Calc for right side beyond plotted curve
						{
							output += (net[layer][node].khList[i] - net[layer][node].khList[i - 2]) * ((net[layer][node].khList[i + 1] - net[layer][node].khList[i - 1])
								/ 2 + net[layer][node].khList[i - 1]);
							cout << "\n   90c \n      " << output << "  #ab" << i / 2;
						}
						output = (input - net[layer][node].khList[net[layer][node].khList.size() - 2]) / (net[layer][node].dimSmoothing
							+ (input - net[layer][node].khList[net[layer][node].khList.size() - 2]));
						cout << "\n   90c \n      " << output << "  #b";
					}
					else
					{
						for (calcLength = 0; input > net[layer][node].khList[calcLength]; calcLength += 2) {} // calcLength = the k after the input								// Bell curve
						cout << "\n790cb";
						if (calcLength >= 2)																																	//
						{																																						//
							for (int i = 2; i < calcLength; i += 2) // calculating all parts of bell curve that are fully defined (aka areas of whole right triangle)			// calculator
							{																																					// (This is
								cout << "\n   90cb \n      " << output << "  #ab-" << calcLength << "-" << i / 2;
								output += (net[layer][node].khList[i] - net[layer][node].khList[i - 2]) * ((net[layer][node].khList[i + 1] - net[layer][node].khList[i - 1])	//  somewhat
									/ 2 + net[layer][node].khList[i - 1]);																										//  tested
							}																																					//  could still
							// (x-k0)( ((x-k0)*(h-h0))/(2(k-k0)) + h0 ) | Finding area of part of traingle																		//  have problems)
							output += (input - net[layer][node].khList[calcLength - 2]) * (((input - net[layer][node].khList[calcLength - 2]) *									//
								(net[layer][node].khList[calcLength + 1] - net[layer][node].khList[calcLength - 1])) / (2 * (net[layer][node].khList[calcLength]				//
									- net[layer][node].khList[calcLength - 2])) + net[layer][node].khList[calcLength - 1]);														//
							cout << "\n   90cb \n      " << output << "  #b-" << calcLength << "\n\n" << input << " | " << net[layer][node].khList[calcLength - 2] << " | "
								<< net[layer][node].khList[calcLength + 1] << " | " << net[layer][node].khList[calcLength - 1] << " | " << output << "\n\n";
						}
						else
						{
							output += (net[layer][node].khList[1]) + input / (10 + abs(input));	// bad lower end approx
							cout << "\n   90cb \n      " << output << "  #c-" << " | " << input << " | " << net[layer][node].khList[1];
						}
					}
					net[layer][node].value = output;
				}
			}
		}

		// adapt this with debug or no debug
		void DEBUG_backProp(vector<float> outputValues, const bool debug = false)
		{
			if (debug) {
				cout << "\n\nInitiating Back Prorogation . . .";
			}
			for (int layer = layers - 1; layer >= 0; layer--)
			{
				if (debug) {
					cout << "\n   Layer: " << layer;
				}
				if (layer == layers - 1)
				{
					for (int node = 0; node < nodes[layer]; node++)
					{
						if (debug) {
							cout << "\n      Node: " << node;
							cout << "\n         Pushing back (" << net[layer][node].movingInp << ", " << outputValues[node] << ")";
						}
						vector<float> xCords;
						vector<float> yCords;
						net[layer][node].movingTrueVal = outputValues[node];
						xCords.push_back(net[layer][node].movingInp);
						yCords.push_back(outputValues[node]);
						net[layer][node].connWeightAmt++;
						for (int conn = 0; conn < nodes[layer - 1]; conn++)
						{																																				// change this process by adding
							net[layer][node].connWeight[conn] = (net[layer][node].connWeight[conn] + net[layer][node].movingTrueVal) / net[layer][node].connWeightAmt;	// in thing to understand change
						}																																				//
						net[layer][node].addOrderedCords(xCords, yCords, debug);																								// in bellcurve of prev function
					}
				}
				else if (layer == 0)
				{
					for (int node = 0; node < nodes[layer]; node++)
					{
						float output = 0;
						vector<float> xCords;
						vector<float> yCords;
						xCords.push_back(net[layer][node].movingInp);
						for (int upNode = 0; upNode < nodes[layer + 1]; upNode++)										// calculating total amount that the function
						{																								// would have to have
							output += net[layer + 1][upNode].movingTrueVal * net[layer + 1][upNode].connWeight[node];	//
						}																								//
						yCords.push_back(output / nodes[layer]);														//
						net[layer][node].movingTrueVal = output / nodes[layer];
						net[layer][node].addOrderedCords(xCords, yCords, debug);
						if (debug) {
							cout << "\n      BPNode: " << node;
							cout << "\n         BPPushing back (" << net[layer][node].movingInp << ", " << output / nodes[layer] << ")";
						}
					}
				}
				else
				{
					for (int node = 0; node < nodes[layer]; node++)
					{
						vector<float> xCords;
						vector<float> yCords;
						net[layer][node].connWeightAmt++;
						for (int conn = 0; conn < nodes[layer - 1]; conn++)
						{
							net[layer][node].connWeight[conn] = (net[layer][node].connWeight[conn] + net[layer - 1][conn].value) / net[layer][node].connWeightAmt;
						}
						float output = 0;
						xCords.push_back(net[layer][node].movingInp);
						for (int upNode = 0; upNode < nodes[layer + 1]; upNode++)									// calculating total amount that the function
						{																							// would have to have
							output += net[layer + 1][upNode].movingTrueVal * net[layer + 1][upNode].connWeight[node] / 10000;	//
						}																							//
						yCords.push_back(output / nodes[layer + 1]);													//
						net[layer][node].movingTrueVal = output / nodes[layer + 1];
						net[layer][node].addOrderedCords(xCords, yCords, debug);
						if (debug) {
							cout << "\n      BPNode: " << node;
							cout << "\n         BPPushing back (" << net[layer][node].movingInp << ", " << output / nodes[layer] << ")";
						}
					}
				}
			}
		}
		void DEBUG_Calculate(vector<float> inputValues, bool debug = false)
		{
			if (debug) {
				cout << "\n\nInitiating Calculation . . .";
			}
			for (int layer = 0; layer < layers; layer++)
			{
				if (debug) {
					cout << "\n   Layer: " << layer;
				}
				for (int node = 0; node < nodes[layer]; node++)
				{
					if (debug) {
						cout << "\n      Node: " << node;
						cout << "\n         inputs ";
					}
					float input = 0;
					if (layer == 0)
					{
						if (debug) { cout << input; }
						input = inputValues[node];
					}
					else
					{
						for (unsigned int connNum = 0; connNum < nodes[layer - 1]; connNum++)
						{
							input += net[layer - 1][connNum].value * net[layer][node].connWeight[connNum] / weightVariation;
						}
						if (debug) {
							cout << input;
						}
					}
					net[layer][node].movingInp = input;
					float output = 0;
					unsigned int calcLength;

					if (input > net[layer][node].khList[net[layer][node].khList.size() - 2])			// possible bug: if size is index, or if right side != last k in sequence for other reason
					{																					//
						if (debug) {
							cout << "\n         Input is beyond khList... Proceeding with diminishing end protocol";
						}//
						for (int i = 2; i < net[layer][node].khList.size() - 2; i += 2)					// this is Calc for right side beyond plotted curve
						{
							output += (net[layer][node].khList[i] - net[layer][node].khList[i - 2]) * ((net[layer][node].khList[i + 1] - net[layer][node].khList[i - 1])
								/ 2 + net[layer][node].khList[i - 1]);
						}
						output = (input - net[layer][node].khList[net[layer][node].khList.size() - 2]) / (net[layer][node].dimSmoothing
							+ (input - net[layer][node].khList[net[layer][node].khList.size() - 2]));
					}
					else
					{
						for (calcLength = 0; input > net[layer][node].khList[calcLength]; calcLength += 2) {} // calcLength = the k after the input so if input=k+.00001		// Bell curve
						if (calcLength >= 2)																  // then calcLength = 2.											//
						{																																						//
							if (debug) {
								cout << "\n         Input is within khList... ";
							}																											//
							for (int i = 2; i < calcLength; i += 2) // calculating all parts of bell curve that are fully defined (aka areas of whole right triangle)			// calculator
							{																																					// (This is
								output += (net[layer][node].khList[i] - net[layer][node].khList[i - 2]) * ((net[layer][node].khList[i + 1] - net[layer][node].khList[i - 1])	//  somewhat
									/ 2 + net[layer][node].khList[i - 1]);																										//  tested
							}																																					//  could still
							// (x-k0)( ((x-k0)*(h-h0))/(2(k-k0)) + h0 ) | Finding area of part of traingle																		//  have problems)
							output += (input - net[layer][node].khList[calcLength - 2]) * (((input - net[layer][node].khList[calcLength - 2]) *									//
								(net[layer][node].khList[calcLength + 1] - net[layer][node].khList[calcLength - 1])) / (2 * (net[layer][node].khList[calcLength]				//
									- net[layer][node].khList[calcLength - 2])) + net[layer][node].khList[calcLength - 1]);														//
						}
						else
						{
							if (debug) {
								cout << "\n         Input is beyond khList... Proceeding with diminishing end protocol";	//
							}
							output += (net[layer][node].khList[1]) + input / (10 + abs(input));	// bad lower end approx
						}
					}
					if (debug) {
						cout << "\n          Output: " << output;
					}
					net[layer][node].value = output;
				}
			}
		}
	};
}
