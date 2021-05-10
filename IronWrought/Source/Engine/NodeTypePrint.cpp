#include "stdafx.h"
#include "NodeTypePrint.h"
#include "NodeInstance.h"
#include <iostream>

CNodeTypePrint::CNodeTypePrint()
{
	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Flow));	// Pin Index 0, this is an input
	myPins.push_back(SPin("Text", SPin::EPinTypeInOut::PinTypeInOut_IN,SPin::EPinType::String)); // Pin Index 1, this is an input
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::PinTypeInOut_OUT)); // Pin Index 2, this is an output
}

int CNodeTypePrint::OnEnter(class CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize); // Get data on pin index 1, this index is relative to what you push in the constructor
	
	// As this node can be connected to float, bool, inte, string, wee need to go through them and look what we got
	if (outSize > 0)
	{
		if (outType == SPin::EPinType::Int)
		{
			// Yay an integer! Get the data and print it
			int c = NodeData::Get<int>(someData);
			std::cout << "Data: " << c << std::endl;
		}
		else if (outType == SPin::EPinType::String)
		{
			// Yay a string! Get the data and print it
			std::cout << "Data: " << static_cast<char*>(someData) << std::endl;
		}
		else if (outType == SPin::EPinType::Bool)
		{
			// Yay a bool! Get the data and print it
			bool c = NodeData::Get<bool>(someData);
			std::cout << "Data: " << c << std::endl;
		}
		else if (outType == SPin::EPinType::Float)
		{
			// Yay a float! Get the data and print it
			float c = NodeData::Get<float>(someData);
			std::cout << "Data: " << c << std::endl;
		}
	}

	// Now we want to continue the execution in our node tree, lets send the flow on pin index 2
	// If we dont want the execution to continue, return -1 (see the Math Add node for reference)
	return 2;  // We want to continue execution in the tree on the pin 2 in the pushed vector in the constructor ("OUT")
}
