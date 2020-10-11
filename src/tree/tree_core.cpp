
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





void PowerTree::Source::connectDesc (key descName, shared_ptr<Sink> sink_ptr)
{
	desces[descName] = sink_ptr;
}


void PowerTree::Source::deleteDesc (key descName)
{
	desces.erase(descName);
}


const PowerTree::Source::DescesDictionary & PowerTree::Source::getAllDesces () const
{
	return desces;
}


double PowerTree::Source::calculateLoad () const
{
	double load = 0.0;
	for (const auto & sink_ptr : desces)
	{
		const auto & sink = *sink_ptr.second;
		load += sink.calculateConsumption(cvValue, cvType);
	}
	return load;
}


bool PowerTree::Source::isSuchDesc (key descName) const
{
	if (desces.count(descName) > 0)
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
	ensureIsSinkExsisting(headerName);
	ensureIsSourceExsisting(newParentName);
	ensureIsSecondNotFirstsDesc(headerName, newParentName);


	disconnectSubnetWithoutChecking(headerName);
	auto newParentsDesces = findDescesByKey(newParentName);
	auto header_ptr = findSinkByKey(headerName);
	writeNewSinkToSource(headerName, newParentName, header_ptr);
}


void PowerTree::disconnectSubnet (key headerName)
{
#pragma todo check deleting via console messages in destructors
	ensureIsSinkExsisting(headerName);
	disconnectSubnetWithoutChecking(headerName);
}


void PowerTree::moveAllDescesTo (key parentName, key newParentName)
{
	ensureIsSourceExsisting(parentName);
	ensureIsSourceExsisting(newParentName);
	ensureIsSecondNotFirstsDesc(newParentName, parentName);

	auto parent_ptr = findSourceByKey(parentName);
	auto desces = parent_ptr->getAllDesces();
	for (AUTO_CONST_REF desc : desces)
	{
		disconnectSubnetWithoutChecking(desc.first, parentName);
		writeNewSinkToSource(desc.first, newParentName, desc.second);
	}
}


void PowerTree::disconnectAllDesces (key parentName)
{
	ensureIsSourceExsisting(parentName);

	auto parent_ptr = findSourceByKey(parentName);
	auto desces = parent_ptr->getAllDesces();
	for (AUTO_CONST_REF desc : desces)
		disconnectSubnetWithoutChecking(desc.first, parentName);
}


void PowerTree::deleteNode (key name, key descesNewParentName)
{
	ensureIsNodeExsisting(name);
	if (descesNewParentName != "")    ensureIsSourceExsisting(descesNewParentName);


	disconnectSubnetWithoutChecking(name);

	if (isSuchLoad(name))
	{
		loads.erase(name);
		return;
	}

	auto desces = findDescesByKey(name);
	for (AUTO_CONST_REF desc : desces)
	{
		desces.erase(desc.first);
		writeNewSinkToSource(desc.first, descesNewParentName, desc.second);
	}

	if (isSuchConverter(name))
		converters.erase(name);
	else if (isSuchInput(name))
		inputs.erase(name);
}


void PowerTree::deleteSubnet (key headerName)
{
	ensureIsNodeExsisting(headerName);


	disconnectSubnetWithoutChecking(headerName);
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
			converters[parentName] ->connectDesc(sinkName, newSink_ptr);
		if (isSuchInput(parentName))
			inputs[parentName] ->connectDesc(sinkName, newSink_ptr);
	}
}


void PowerTree::disconnectSubnetWithoutChecking (key headerName)
{
	auto source = findParent(headerName);
	source->deleteDesc(headerName);
}


void PowerTree::disconnectSubnetWithoutChecking (key headerName, key parentName)
{
	auto source_ptr = findSourceByKey(headerName);
	source_ptr->deleteDesc(headerName);
}


void PowerTree::deleteSubnetPointers(key headerName)
{
	if (isSuchLoad(headerName))
	{
		loads.erase(headerName);
		return;
	}

	AUTO_CONST_REF headersDesces = findDescesByKey(headerName);
	for (AUTO_CONST_REF desc : headersDesces)
	{
		deleteSubnetPointers(desc.first);
	}

	if (isSuchConverter(headerName))
		converters.erase(headerName);
	else if (isSuchInput(headerName))
		inputs.erase(headerName);
}


shared_ptr<PowerTree::Source> PowerTree::findParent (key name) const
{
	for (AUTO_CONST_REF converter : converters)
		if (converter.second ->isSuchDesc(name))
			return converter.second;

	for (AUTO_CONST_REF input : inputs)
		if (input.second->isSuchDesc(name))
			return input.second;
}


const shared_ptr<PowerTree::Source> PowerTree::findSourceByKey (key name) const
{
	if (isSuchConverter(name))
		return converters.at(name);
	else if (isSuchInput(name))
		return inputs.at(name);
}


const shared_ptr<PowerTree::Sink> PowerTree::findSinkByKey (key name) const
{
	if (isSuchLoad(name))
		return loads.at(name);
	else if (isSuchConverter(name))
		return converters.at(name);
}


const PowerTree::Source::DescesDictionary
& PowerTree::findDescesByKey (key name) const
{
	auto header = findSourceByKey(name);
	return header->getAllDesces();
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


void PowerTree::ensureIsSinkExsisting(key name) const
{
	if (isSuchSink(name))
		return;

	throw exception(string("There is no sinks with the name " + name).c_str());
}


void PowerTree::ensureIsSecondNotFirstsDesc(key firstName, key secondName) const
{
	if (isSuchLoad(firstName) || isSuchInput(secondName))
		return;
	ensureIsConvertersDescNotExsisting(secondName, firstName);
}


void PowerTree::ensureIsConvertersDescNotExsisting (key checkedDescName, key converterName) const
{
	if (converterName == checkedDescName)    throw exception("Such an operation follows to closing a converter on itself");

	AUTO_CONST_REF convertersDesces = converters.at(converterName) ->getAllDesces();

	for (AUTO_CONST_REF convertersDesc : convertersDesces)
	{
		if (isSuchLoad(convertersDesc.first))
			continue;

		if (convertersDesc.first == checkedDescName)    throw exception ("Such an operation follows to closing a loop on the tree's structure");

		ensureIsConvertersDescNotExsisting(checkedDescName, convertersDesc.first);
	}
}


bool PowerTree::isSuchSource (key name) const
{
	bool result = (isSuchInput(name) && isSuchConverter(name));
	return result;
}


bool PowerTree::isSuchSink(key name) const
{
	bool result = (isSuchConverter(name) && isSuchLoad(name));
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
