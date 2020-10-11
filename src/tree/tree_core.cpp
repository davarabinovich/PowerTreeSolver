
#include "tree_core.h"




#pragma todo carry on lib
#define AUTO_CONST_REF auto const &




string PowerTree::Node::getTypeOfNode_str () const
{
	return "node";
}





string PowerTree::Load::getTypeOfNode_str () const
{
	return "load";
}





double PowerTree::ResistiveLoad::calculateConsumption (double parentCvValue, CvType parentCvType) const
{
	double consumption;
	switch (parentCvType)
	{
		case CvType::VOLTAGE:
			consumption = parentCvValue / resistance;
			break;

		case CvType::CURRENT:
			consumption = parentCvValue * resistance;
			break;

		default:
			throw exception("Invalid type of parent's controllable variable in the resistive load");
	}
	return consumption;
}





double PowerTree::ConstantCurrentLoad::calculateConsumption (double parentCvValue, CvType parentCvType) const
{
	double consumption;
	switch (parentCvType)
	{
		case CvType::VOLTAGE:
			consumption = current;
			break;

		case CvType::CURRENT:
			consumption = parentCvValue;
			break;

		default:
			throw exception("Invalid type of parent's controllable variable in the current load");
	}
	return consumption;
}





void PowerTree::Source::connectDescendant (key descendantName, shared_ptr<Sink> sink_ptr)
{
	descendants[descendantName] = sink_ptr;
}


void PowerTree::Source::deleteDescendant (key descendantName)
{
	descendants.erase(descendantName);
}


const PowerTree::Source::DescendantsMap & PowerTree::Source::getAllDescendants () const
{
	return descendants;
}


double PowerTree::Source::calculateLoad () const
{
	double load = 0.0;
	for (const auto & sink_ptr : descendants)
	{
		const auto & sink = *sink_ptr.second;
		load += sink.calculateConsumption(cvValue, cvType);
	}
	return load;
}


bool PowerTree::Source::isSuchDescendant (key descendantName) const
{
	if (descendants.count(descendantName) > 0)
		return true;
	return false;
}





string PowerTree::Input::getTypeOfNode_str () const
{
	return "input";
}




#pragma mayby delete these functions
string PowerTree::Converter::getTypeOfNode_str () const
{
	return "converter";
}


double PowerTree::Converter::calculateConsumption (double parentCvValue, CvType parentCvType) const
{
	double load = calculateLoad();
	double consumption = reduceLoadToInput(parentCvValue, parentCvType, load) + getSelfConsumption(parentCvValue, parentCvType, load);
	return consumption;
}





double PowerTree::Converter::getSelfConsumption (double parentCvValue, CvType parentCvType, double load) const
{
	double selfConsumption = (1 - efficiency) * cvValue * load;
	return selfConsumption;
}





double PowerTree::PulseConverter::reduceLoadToInput (double parentCvValue, CvType parentCvType, double load) const
{
	double reducedLoad = load * cvValue / (parentCvValue * efficiency);
	return reducedLoad;
}





double PowerTree::LinearConverter::reduceLoadToInput (double parentCvValue, CvType parentCvType, double load) const
{
	double reducedLoad;
	switch (cvType)
	{
		case CvType::VOLTAGE:
			reducedLoad = load;
			break;

		case CvType::CURRENT:
			reducedLoad = cvValue;
			break;

		default:
			throw exception("Invalid type of controllable variable of the linear converter");
	}
	return reducedLoad;
}





PowerTree::PowerTree (string nm)
{
	ensureIsNameNotEmpty(nm, "tree");

	name = nm;
}




void PowerTree::addInput (key name)
{
	ensureIsNameNotEmpty(name, "node");
	ensureIsNodeNotExsisting(name);


	inputs[name] = make_shared<Input>();
}

 
void PowerTree::addConverter (key name, key parentName, ConverterType type)
{
	validateArgsForNewSink(name, parentName);


	shared_ptr<Converter> newConverter_ptr;
	switch (type)
	{
		case ConverterType::PULSE:
			newConverter_ptr = make_shared<PulseConverter>();
			break;

		case ConverterType::LINEAR:
			newConverter_ptr = make_shared<LinearConverter>();
			break;

		default:
			throw exception("Invalid type of converter");
	}
	converters[name] = newConverter_ptr;

	writeNewSinkToSource(name, parentName, newConverter_ptr);
}


void PowerTree::addLoad (key name, key parentName, LoadType type)
{
	validateArgsForNewSink(name, parentName);


	shared_ptr<Load> newLoad_ptr;
	switch (type)
	{
		case LoadType::RESISTIVE:
			newLoad_ptr = make_shared<ResistiveLoad>();
			break;

		case LoadType::CONSTANT_CURRENT:
			newLoad_ptr = make_shared<ConstantCurrentLoad>();
			break;

		default:
			throw exception("Invalid type of converter");
	}
	loads[name] = newLoad_ptr;

	writeNewSinkToSource(name, parentName, newLoad_ptr);
}


void PowerTree::moveSubnetTo (key headerName, key newParentName)
{
	ensureIsSourceExsisting(newParentName);


	disconnectSubnet(headerName);
	auto newParentDescendants = findDescendantsByKey(newParentName);
	auto header_ptr = findSinkByKey(headerName);
	writeNewSinkToSource(headerName, newParentName, header_ptr);
}


void PowerTree::disconnectSubnet (key headerName)
{
#pragma todo check deleting via console messages in destructors
	ensureIsNodeExsisting(headerName);


	Source & source = findParent(headerName);
	source.deleteDescendant(headerName);
}


void PowerTree::moveAllDescendantsTo (key parentName, key newParentName)
{

}


void PowerTree::disconnectAllDescendants (key parentName)
{

}


void PowerTree::deleteNode (key name, key descendantsNewParentName)
{
	if (descendantsNewParentName != "")    ensureIsSourceExsisting(descendantsNewParentName);


	disconnectSubnet(name);

	if (isSuchLoad(name))
	{
		loads.erase(name);
		return;
	}

	auto descendants = findDescendantsByKey(name);
	for (AUTO_CONST_REF descendant : descendants)
	{
		descendants.erase(descendant.first);
		writeNewSinkToSource(descendant.first, descendantsNewParentName, descendant.second);
	}

	if (isSuchConverter(name))
		converters.erase(name);
	else if (isSuchInput(name))
		inputs.erase(name);
}


void PowerTree::deleteSubnet (key headerName)
{
	disconnectSubnet(headerName);
	deleteSubnetPointers(headerName);
}


void PowerTree::calculate () const
{
	for (const auto & input_ptr : inputs)
	{
		const auto & input = *input_ptr.second;
		input.calculateLoad();
	}
}


void PowerTree::validateArgsForNewSink (key name, key parentName) const
{
	ensureIsNameNotEmpty(name, "node");
	ensureIsNodeNotExsisting(name);
	if (parentName != "")    ensureIsSourceExsisting(parentName);
}


void PowerTree::writeNewSinkToSource (key sinkName, key parentName, shared_ptr<Sink> newSink_ptr)
{
	if (parentName != "")
	{
		if (isSuchConverter(parentName))
			converters[parentName] ->connectDescendant(sinkName, newSink_ptr);
		if (isSuchInput(parentName))
			inputs[parentName] ->connectDescendant(sinkName, newSink_ptr);
	}
}


void PowerTree::deleteSubnetPointers(key headerName)
{
	if (isSuchLoad(headerName))
	{
		loads.erase(headerName);
		return;
	}

	AUTO_CONST_REF headersDescendants = findDescendantsByKey(headerName);
	for (AUTO_CONST_REF descendant : headersDescendants)
	{
		deleteSubnetPointers(descendant.first);
	}

	if (isSuchConverter(headerName))
		converters.erase(headerName);
	else if (isSuchInput(headerName))
		inputs.erase(headerName);
}


PowerTree::Source & PowerTree::findParent (key name) const
{
	for (AUTO_CONST_REF converter : converters)
		if (converter.second ->isSuchDescendant(name))
			return *(converter.second);

	for (AUTO_CONST_REF input : inputs)
		if (input.second->isSuchDescendant(name))
			return *(input.second);
}


const PowerTree::Source & PowerTree::findSourceByKey (key name) const
{
	if (isSuchConverter(name))
		return *( converters.at(name) );
	else if (isSuchInput(name))
		return *( inputs.at(name) );
}


const shared_ptr<PowerTree::Sink> PowerTree::findSinkByKey (key name) const
{
	if (isSuchLoad(name))
		return loads.at(name);
	else if (isSuchConverter(name))
		return converters.at(name);
}


const PowerTree::Source::DescendantsMap & PowerTree::findDescendantsByKey (key name) const
{
	auto header = findSourceByKey(name);
	return header.getAllDescendants();
}


void PowerTree::ensureIsNodeExsisting (key name) const
{
	bool isNodeExsisting = false;
	if (isSuchInput(name))
		isNodeExsisting = true;
	else if (isSuchConverter(name))
		isNodeExsisting = true;
	else if (isSuchLoad(name))
		isNodeExsisting = true;

	if (!isNodeExsisting)
		throw exception(string("A node with a name " + name + " isn't exsisting").c_str());
}


void PowerTree::ensureIsNodeNotExsisting (key name) const
{
	string exsistingNodeType_str = "";
	if (isSuchInput(name))
		exsistingNodeType_str = "input";
	else if (isSuchConverter(name))
		exsistingNodeType_str = "converter";
	else if (isSuchLoad(name))
		exsistingNodeType_str = "load";

	if (exsistingNodeType_str != "")
		throw exception( string("The " + exsistingNodeType_str + "with such a name already exsist").c_str() );
}


void PowerTree::ensureIsSourceExsisting (key name) const
{
	if (isSuchSource(name))
		return;

	throw exception( string("There is no sources with the name " + name).c_str() );
}


bool PowerTree::isSuchSource (key name) const
{
	bool result = (isSuchInput(name) && isSuchConverter(name));
	return result;
}


bool PowerTree::isSuchInput (key name) const
{
	bool result = (inputs.count(name) > 0);
	return result;
}


bool PowerTree::isSuchConverter (key name) const
{
	bool result = (converters.count(name) > 0);
	return result;
}

bool PowerTree::isSuchLoad (key name) const
{
	bool result = (loads.count(name) > 0);
	return result;
}
