#pragma once
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

namespace phi
{
	class Node // ~ 900 nodes (with prev node at 900) = 1mb
	{
	public:

		float value;
		float movingInp;							// needed to make backprop more efficiently
		vector<float> khList;						// lines between (k,h) is the derivative of bell curve (should have and may change to list of ks and hs
		float endCords[2] = { 0, 0 };				// start cords and end cords

		vector<float> avDeviation;					// average deviation between last x and x (x, averageDeviation)
		vector<short int> connWeight;				// weight of connection (uncompressed via /10000)
		signed int connWeightAmt;					// general value of iterations that occurred

		float dimSmoothing;
		float dimHeight;
		int movingTrueVal;							// correct value that is used for back propagation
		int movingValWeighted;						// signed amount | value that the node contributed to the end result

		signed int compCordAmt;
		vector<float> xCompCords;
		vector<float> yCompCords;
		vector<float> xCordList;
		vector<float> yCordList;

		// Value Throughput Functions (like sigmoid)
		void DirivMerge()
		{
		}
		int BCConstructor(bool debug = false)
		{
			if (debug) { cout << "\n\nBCConstructor Called:"; }
			//constraint checker
			for (signed int i = 0; i < xCompCords.size() - 1; i += 3) // FIX THIS
			{
				if (debug) {
					cout << "\nInputs: " << xCompCords[i] << " | " << yCompCords[i] << " | " << xCompCords[i + 1] << " | " << yCompCords[i + 1] << " | " << xCompCords[i + 2] << " | " << yCompCords[i + 2];
					cout << "\n   Out of Bounds Checker: \n      Cord size: " << xCompCords.size() << " and " << yCompCords.size();
					cout << "\n      Index Number: " << i;
				}
				if (xCompCords[i] == xCompCords[i + 1] || xCompCords[i] == xCompCords[i + 2] || yCompCords[i] == yCompCords[i + 1] || yCompCords[i] == yCompCords[i + 2])
				{
					if (debug) { cout << "\n      ERROR: OVERLAPING POINTS"; }
					return 1;
				}
				// checking for upper constraint ( \frac{\left(x-x_{0}\right)\left(y_{2}-y_{0}\right)\left(2-\frac{x-x_{0}}{x_{2}-x_{0}}\right)}{x_{2}-x_{0}}+y_{0} )
				if (((xCompCords[i + 1] - xCompCords[i]) * (yCompCords[i + 2] - yCompCords[i]) * (2 - ((xCompCords[i + 1] - xCompCords[i]) / (xCompCords[i + 2] - xCompCords[i]))))
					/ (xCompCords[i + 2] - xCompCords[i]) + yCompCords[i] <= yCompCords[i + 1])
				{
					if (debug) { cout << "\n      ERROR: OUT OF BOUNDS (top)\n"; }
					return 1;
				}
				// checking for lower constraint ( \frac{h\left(x-x_{0}\right)^{2}}{2\left(x_{2}-x_{0}\right)}+y_{0} )
				if (((yCompCords[i + 2]) - yCompCords[i]) * (pow(xCompCords[i + 1] - xCompCords[0], 2)) / (pow(xCompCords[i + 2] - xCompCords[i], 2)) + yCompCords[i] >= yCompCords[i + 1])
				{
					if (debug) { cout << "\n      ERROR: OUT OF BOUNDS (bottom)\n"; }
					return 1;
				}
			}
			khList.clear();
			vector<float> tempkhList;
			for (signed int i = 0; i < xCompCords.size(); i += 3)
			{
				// relative k0 & h0 (origin):
				if (debug) {
					cout << "\n   calc for index " << i;
					cout << "\n       KH Origin: " << xCompCords[i] << ", " << yCompCords[i];
				}
				tempkhList.push_back(xCompCords[i]);
				tempkhList.push_back(yCompCords[i]);
				// relative k1 & h1 (derivative vertex):
				if (0 > yCompCords[i + 1] - yCompCords[i] - ((yCompCords[i + 2] - yCompCords[i]) * (xCompCords[i + 1] - xCompCords[i])) / (xCompCords[i + 2] - xCompCords[i]))
				{
					tempkhList.push_back((((xCompCords[i + 2] - xCompCords[i]) * (yCompCords[i + 1] - yCompCords[i]) + (yCompCords[i + 2] - yCompCords[i]) * (xCompCords[i + 1] - xCompCords[i]) *
						((xCompCords[i + 1] - xCompCords[i]) / ((xCompCords[i + 2] - xCompCords[i]) - 2))) / (yCompCords[i + 1] - yCompCords[i + 2])));
				}
				else
				{
					tempkhList.push_back((((yCompCords[i + 2] - yCompCords[i]) * (xCompCords[i + 1] - xCompCords[i]) * (xCompCords[i + 1] - xCompCords[i])) / ((yCompCords[i + 1] - yCompCords[i]) * // no ^ 2 because it doesn't
						(xCompCords[i + 2] - xCompCords[i]))));																															 // accept float types
				}
				tempkhList.push_back((2 * (yCompCords[i + 2] - yCompCords[i])) / (xCompCords[i + 2] - xCompCords[i]));
				if (debug) { cout << "\n      KH Middle: " << tempkhList[2] << ", " << tempkhList[3]; }
				// relative k2 & h2 (where derivitive == 0)
				tempkhList.push_back(xCompCords[i + 2]);
				tempkhList.push_back(yCompCords[i + 2]);
				addKHCords(tempkhList);
				if (debug) { cout << "\n      KH Last: " << xCompCords[i + 2] << ", " << yCompCords[i + 2]; }
			}
			if (debug) { cout << "\n   BCConstructor Finished\n"; }
		}
		void pointGrouper(signed int amountOfOutputPoints, const bool debug = false) // MAKE SURE IS DEVISABLE BY THREE, DON'T CALCULATE MORE POINTS THAN PREV IT
		{
			if (debug) { cout << "Point Grouper Called"; cout << "\n   number of y cords: " << yCordList.size() << "\n   number of x cords: " << xCordList.size(); }
			// Segmented into 3 via x cord amt and then averaged (I.E. | 1,1,2 | 5,6,9 | 10,15,100 | ). This is a bad way of going about this.
			// This needs to be changed into a function that is better
			// Doesn't conform to constraints of BCConstructor
			signed int outPoints = amountOfOutputPoints;
			float xVal = 0;
			float yVal = 0;
			signed int lastPos = 0;
			vector<float> xTempCordList;
			vector<float> yTempCordList;
			for (signed int slice = 0; slice < outPoints; slice++)
			{
				signed int index = lastPos;
				if (debug) { cout << "\n Slice #:" << slice << "\n   number of y cords: " << yCordList.size() << "\n   number of x cords: " << xCordList.size(); }
				for (; index < xCordList.size() / outPoints + lastPos; index++)
				{																	// first third is used as an iteration amount here
					if (debug) {
						cout << "\n                 " << xCordList[index];
						cout << "\n                 " << yCordList[index];
					}
					yCordList[index];
					xVal += xCordList[index];										//
					yVal += yCordList[index];										//
				}																	// and then by the end, it equals xCordList.size() / 3 rounded up
				if (xCompCords.size() == amountOfOutputPoints)
				{
					compCordAmt += index;
					if (debug) { cout << "\n   Pushing Back (" << (xCompCords[slice] * compCordAmt + xVal) / (compCordAmt) << "), (" << (yCompCords[slice] * compCordAmt + yVal) / (compCordAmt) << ")"; }
					xTempCordList.push_back((xCompCords[slice] * compCordAmt + xVal) / (compCordAmt));
					yTempCordList.push_back((yCompCords[slice] * compCordAmt + yVal) / (compCordAmt));
				}
				else
				{
					if (debug) { cout << "\n   Pushing Back (" << xVal / index << "), (" << yVal / index << ")"; }
					xTempCordList.push_back(xVal / index);
					yTempCordList.push_back(yVal / index);
				}
				lastPos = index;
			}
			if (debug)
			{
				cout << "\n   Point Grouper: " << "\n      xCompCord Size: " << xTempCordList.size() << "\n      yCompCord Size: " << yTempCordList.size();
				cout << "\n   Cords: \n";
				for (int i = 0; i < xTempCordList.size(); i++)
				{
					cout << " (" << xTempCordList[i];
					cout << ", " << yTempCordList[i] << "), ";
				}
			}
			xCompCords = { xTempCordList };
			yCompCords = { yTempCordList };
		}
		int addOrderedCords(vector<float> xcords, vector<float> ycords, const bool debug = false)	// not most efficient
		{
			if (debug) {
				cout << "\n OrderedPairAdder Called: ";
				cout << "\n xcords:";
				for (float xcord : xcords)
				{
					cout << "\n" << xcord;
				}
				cout << "\n ycords:";
				for (float ycord : ycords)
				{
					cout << "\n" << ycord;
				}
			}
			if (xcords.size() != ycords.size())
			{
				if (debug) { cout << "\n    xcords don't match ycords"; }
				return 1;
			}
			if (xCordList.empty())
			{
				if (debug) {
					cout << "\n   XCordList Empty... Following protocol";
					cout << "\n      starting empty protocol";
					cout << "\n      adding " << xcords[0] << ", " << ycords[0];
				}
				xCordList.push_back(xcords[0]);
				yCordList.push_back(ycords[0]);
				xcords.erase(xcords.begin());
				ycords.erase(ycords.begin());
			}
			if (xcords.empty()) { return 0; }
			auto xposition = xCordList.begin();
			auto yposition = yCordList.begin();
			for (unsigned int index = 0; index < xcords.size(); index++)
			{
				if (xcords[index] < xCordList[xCordList.size() - 1])
				{
					if (debug) { cout << "\n   xCordListSize: " << xCordList.size(); }
					while (xcords[index] > *xposition)
					{
						if (debug) {
							cout << "\n   position value: " << *xposition;
							cout << "\n   xCordList value: " << xcords[index];
						}
						++xposition;
						++yposition;
					}
					if (debug) {
						cout << "\n   xcords:";
						for (float xcord : xcords)
						{
							cout << xcord << ", ";
						}
						cout << "\n   ycords:";
						for (float ycord : ycords)
						{
							cout << ycord << ", ";
						}
						cout << "\n   index: " << index;
						cout << "\n   xcords" << xcords[index];
						cout << "\n   ycords" << ycords[index];
					}
					xCordList.insert(xposition, xcords[index]);
					yCordList.insert(yposition, ycords[index]);
					if (debug) { cout << "\n    Adding " << xcords[index] << ", " << ycords[index] << " via insert"; }
				}
				else
				{
					xCordList.push_back(xcords[index]);
					yCordList.push_back(ycords[index]);
					if (debug) { cout << "\n    Adding " << xcords[index] << ", " << ycords[index]; }
				}
			}
			if (debug)
			{
				cout << "\nxCordList Size at end of orderedPairAdder" << xCordList.size();
			}
			return 0;
		}
		void addKHCords(vector<float> cords, const bool debug = false)
		{
			// note some weirdness with 2 ++positions/1 --position and differences between -1 and -2 and etc are to make sure that upon adding values the position is at the h before the k after value
			auto position = khList.begin();

			for (int i = 0; i < cords.size() - 1; i += 2)		// through extensive bug fixing, there is no longer any bugs
			{
				if (khFirstPass)
				{
					if (debug) { cout << "_3b \n"; }
					khList.push_back(cords[i]);
					khList.push_back(cords[i + 1]);
					khFirstPass = true;
				}
				else
				{
					position = khList.begin();
					if (khList.size() > 10)	//1a																				// this is for potential optimization of process ( check efficiency of ++position to validate/invalidate )
					{
						if (debug) { cout << "1a \n"; }
						if (cords[i] < khList[khList.size() - 2] && cords[i] > khList[0])
						{
							float guess = cords[i] / khList[khList.size() - 2];
							for (int i = 0; i < round(guess / 2) * 2; i++) { ++position; }	 									// division & multiplication of 2 is to make sure that it is even so that it corresponds to k
							if (cords[i] > *position) { while (cords[i] > *position) { ++position; ++position; } }				// --position because insert pushes indexed value back ( so it is like putting value
							else { while (cords[i] < *position) { --position; --position; } ++position; ++position; }			// between position and position++) ( and thus position should be list k before input k )
							--position;
						}
						else if (cords[i] > khList[khList.size() - 2])									// For in case that
						{																				// the inputed K values
							for (int i = 0; i < khList.size(); i++)									// are either greater
							{																			// or smaller than
								++position;																// the respective greatest
							}																			// or smallest ks already in list
						}
					}
					else //1b
					{
						if (debug) { cout << "1b \n"; }
						if (cords[i] < khList[khList.size() - 2] && cords[i] > khList[0])
						{
							for (int i = 0; cords[i] > *position && i < khList.size(); i += 2) //2.1a
							{
								if (debug) { cout << "2.1a \n"; }
								++position;
								++position;
							}
							--position;
						}
						else if (cords[i] > khList[khList.size() - 2])
						{
							for (int i = 0; i < khList.size(); i++)
							{
								++position;
							}
						}
					}
					if (debug) { cout << "_3a \n"; }
					if (cords[i] == *position)//3.1a				// in case that they share the same x value,
					{												// average current y and new y. Note: if x values correspond too often,
						if (debug) { cout << "_3.1a \n"; }
						auto hPos = ++position;						// there will be a disproportionate amount of weight on new y over
						*position = (*hPos + cords[i + 1]) / 2;		// previous ys
					}
				}
			}
		}
	private:
		bool khFirstPass = true;
		bool indexTwoGap = true;
		bool indexOneGap = true;
	};
}
