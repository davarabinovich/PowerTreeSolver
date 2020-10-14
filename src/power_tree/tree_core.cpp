
//#include "tree/tree.h"
#include "power_tree/power_tree_core.h"




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


void PowerTree::Source::setCvType (CvType type)
{
	cvType = type;
}


void PowerTree::Source::setValue (double value)
{
	cvValue = value;
}





double PowerTree::Converter::calculateConsumption (double parentCvValue, CvType parentCvType) const
{
	parentCvType;

	double load = calculateLoad();
	double consumption = reduceLoadToInput(parentCvValue, load) + getSelfConsumption(load);
	return consumption;
}





double PowerTree::Converter::getSelfConsumption (double load) const
{
	double selfConsumption = (1 - efficiency) * cvValue * load;
	return selfConsumption;
}





double PowerTree::PulseConverter::reduceLoadToInput (double parentCvValue, double load) const
{
	double reducedLoad = load * cvValue / (parentCvValue * efficiency);
	return reducedLoad;
}





double PowerTree::LinearConverter::reduceLoadToInput (double parentCvValue, double load) const
{
	parentCvValue;

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




void PowerTree::addInput (key inputName)
{
	ensureIsNameNotEmpty(inputName, "node");
	ensureIsNodeNotExsisting(inputName);


	inputs[inputName] = make_shared<Input>();
}

 
void PowerTree::addConverter (key converterName, key parentName, ConverterType type)
{
	validateArgsForNewSink(converterName, parentName);


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
	converters[converterName] = newConverter_ptr;

	writeNewSinkToSource(converterName, parentName, newConverter_ptr);
}


void PowerTree::addLoad (key loadName, key parentName, LoadType type)
{
	validateArgsForNewSink(loadName, parentName);


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
	loads[loadName] = newLoad_ptr;

	writeNewSinkToSource(loadName, parentName, newLoad_ptr);
}


void PowerTree::moveSubnetTo (key headerName, key newParentName)
{
	ensureIsSinkExsisting(headerName);
	ensureIsSourceExsisting(newParentName);
	ensureIsSecondNotFirstsDesc(headerName, newParentName);


	disconnectSubnetWithoutChecking(headerName);
	auto header_ptr = findSinkByKey(headerName);
	writeNewSinkToSource(headerName, newParentName, header_ptr);
}


void PowerTree::disconnectSubnet (key headerName)
{
#pragma exercises check deleting via console messages in destructors
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


void PowerTree::deleteNode (key nodeName, key descesNewParentName)
{
	ensureIsNodeExsisting(nodeName);
	if (descesNewParentName != "")    ensureIsSourceExsisting(descesNewParentName);


	disconnectSubnetWithoutChecking(nodeName);

	if (isSuchLoad(nodeName))
	{
		loads.erase(nodeName);
		return;
	}

	auto desces = findDescesByKey(nodeName);
	for (AUTO_CONST_REF desc : desces)
	{
		desces.erase(desc.first);
		writeNewSinkToSource(desc.first, descesNewParentName, desc.second);
	}

	if (isSuchConverter(nodeName))
		converters.erase(nodeName);
	else if (isSuchInput(nodeName))
		inputs.erase(nodeName);
}


void PowerTree::deleteSubnet (key headerName)
{
	ensureIsNodeExsisting(headerName);


	disconnectSubnetWithoutChecking(headerName);
	deleteSubnetPointers(headerName);
}


void PowerTree::setNodeName (key oldName, key newName)
{
	ensureIsNodeNotExsisting(newName);

	if (isSuchInput(oldName))
	{
		inputs[newName] = inputs[oldName];
		inputs.erase(oldName);
		return;
	}

	auto parentDesces = findParent(oldName) ->getAllDesces();
	parentDesces[newName] = parentDesces[oldName];
	parentDesces.erase(oldName);

	if (isSuchConverter(oldName))
	{
		converters[newName] = converters[oldName];
		converters.erase(oldName);
		return;
	}
	if (isSuchLoad(oldName))
	{
		loads[newName] = loads[oldName];
		loads.erase(oldName);
		return;
	}
}


void PowerTree::setSourceCvType (key inputName, CvType type, double value)
{
	auto source_ptr = findSourceByKey(inputName);
	source_ptr->setCvType(type);
	if ( !isnan(value) )
		source_ptr->setValue(value);
}


void PowerTree::setSourceValue (key inputName, double value)
{
	auto source_ptr = findSourceByKey(inputName);
	source_ptr->setValue(value);
}


void PowerTree::setConverterType (key converterName, ConverterType type)
{

}


void PowerTree::setConverterEfficiency (key converterName, double efficiency)
{

}


void PowerTree::setLoadValue (key loadName, double value)
{

}


void PowerTree::calculate () const
{
	for (const auto & input_ptr : inputs)
	{
		const auto & input = *input_ptr.second;
		input.calculateLoad();
	}
}


void PowerTree::validateArgsForNewSink (key sinkName, key parentName) const
{
	ensureIsNameNotEmpty(sinkName, "node");
	ensureIsNodeNotExsisting(sinkName);
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

	AUTO_CONST_REF headersDesces = findDescesByKey (headerName);
	for (AUTO_CONST_REF desc : headersDesces)
	{
		deleteSubnetPointers(desc.first);
	}

	if (isSuchConverter(headerName))
		converters.erase(headerName);
	else if (isSuchInput(headerName))
		inputs.erase(headerName);
}


shared_ptr<PowerTree::Source> PowerTree::findParent (key sinkName) const
{
	for (AUTO_CONST_REF converter : converters)
		if (converter.second ->isSuchDesc(sinkName))
			return converter.second;

	for (AUTO_CONST_REF input : inputs)
		if (input.second->isSuchDesc(sinkName))
			return input.second;

	throw exception ("There is node without a parent");
}


const shared_ptr<PowerTree::Source> PowerTree::findSourceByKey (key sourceName) const
{
	if (isSuchConverter(sourceName))
		return converters.at(sourceName);
	else if (isSuchInput(sourceName))
		return inputs.at(sourceName);

	throw exception("There is no such a source");
}


const shared_ptr<PowerTree::Sink> PowerTree::findSinkByKey (key sinkName) const
{
	if (isSuchLoad(sinkName))
		return loads.at(sinkName);
	else if (isSuchConverter(sinkName))
		return converters.at(sinkName);

	throw exception("There is np such a sink");
}


const PowerTree::Source::DescesDictionary & PowerTree::findDescesByKey (key sourceName) const
{
	auto header = findSourceByKey(sourceName);
	return header->getAllDesces();
}


void PowerTree::ensureIsNodeExsisting (key nodeName) const
{
	bool isNodeExsisting = false;
	if (isSuchInput(nodeName))
		isNodeExsisting = true;
	else if (isSuchConverter(nodeName))
		isNodeExsisting = true;
	else if (isSuchLoad(nodeName))
		isNodeExsisting = true;

	if (!isNodeExsisting)
		throw exception(string("A node with a name " + nodeName + " isn't exsisting").c_str());
}


void PowerTree::ensureIsNodeNotExsisting (key nodeName) const
{
	string exsistingNodeType_str = "";
	if (isSuchInput(nodeName))
		exsistingNodeType_str = "input";
	else if (isSuchConverter(nodeName))
		exsistingNodeType_str = "converter";
	else if (isSuchLoad(nodeName))
		exsistingNodeType_str = "load";

	if (exsistingNodeType_str != "")
		throw exception( string("The " + exsistingNodeType_str + "with such a name already exsist").c_str() );
}


void PowerTree::ensureIsSourceExsisting (key sourceName) const
{
	if (isSuchSource(sourceName))
		return;

	throw exception( string("There is no sources with the name " + sourceName).c_str() );
}


void PowerTree::ensureIsSinkExsisting (key sinkName) const
{
	if (isSuchSink(sinkName))
		return;

	throw exception(string("There is no sinks with the name " + sinkName).c_str());
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


bool PowerTree::isSuchSource (key sourceName) const
{
	bool result = (isSuchInput(sourceName) && isSuchConverter(sourceName));
	return result;
}


bool PowerTree::isSuchSink (key sinkName) const
{
	bool result = (isSuchConverter(sinkName) && isSuchLoad(sinkName));
	return result;
}


bool PowerTree::isSuchInput (key inputName) const
{
	bool result = (inputs.count(inputName) > 0);
	return result;
}


bool PowerTree::isSuchConverter (key converterName) const
{
	bool result = (converters.count(converterName) > 0);
	return result;
}

bool PowerTree::isSuchLoad (key loadName) const
{
	bool result = (loads.count(loadName) > 0);
	return result;
}
