
#include "tree_core.h"




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





double PowerTree::CurrentLoad::calculateConsumption (double parentCvValue, CvType parentCvType) const
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





void PowerTree::Source::connectNewDescendant (key newDescendantName, shared_ptr<Sink> sink_ptr)
{
	descendants[newDescendantName] = sink_ptr;
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





string PowerTree::Input::getTypeOfNode_str () const
{
	return "input";
}





string PowerTree::Converter::getTypeOfNode_str () const
{
	return "converter";
}


double PowerTree::Converter::calculateConsumption (double parentCvValue, CvType parentCvType) const
{
	double load = calculateLoad();
	double consumption = recudeLoadToInput(parentCvValue, parentCvType) + getSelfConsumption(parentCvValue, parentCvType, load);
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
	ensureIsNameNotEmpty(name, "node");
	ensureIsNodeNotExsisting(name);
	if (parentName != "")    ensureIsSourceExsisting(parentName);

	shared_ptr<Sink> newConverter_ptr;
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

	if (parentName != "")
	{
		if (isSuchConverter(parentName))
			converters[parentName] ->connectNewDescendant(name, newConverter_ptr);
		if (isSuchInput(parentName))
			inputs[parentName] ->connectNewDescendant(name, newConverter_ptr);
	}
}


void PowerTree::addLoad (key name, key parentName)
{
	ensureIsNameNotEmpty(name, "node");
	ensureIsNodeNotExsisting(name);
	if (parentName != "")    ensureIsSourceExsisting(parentName);

	shared_ptr<Sink> newConverter_ptr;
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

	if (parentName != "")
	{
		if (isSuchConverter(parentName))
			converters[parentName]->connectNewDescendant(name, newConverter_ptr);
		if (isSuchInput(parentName))
			inputs[parentName]->connectNewDescendant(name, newConverter_ptr);
	}
}


void PowerTree::moveSubnetTo (key subnetHeadName, key newParentName)
{

}


void PowerTree::disconnectSubnet (key subnetHeadName)
{

}


void PowerTree::moveAllDescendantsTo (key parentName, key newParentName)
{

}


void PowerTree::disconnectAllDescendants (key parentName)
{

}


void PowerTree::deleteNode (key name, key descendantsNewParentName)
{

}


void PowerTree::deleteSubnet (key name)
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


void PowerTree::ensureIsNodeNotExsisting (key name) const
{
	string exsistingNodeType_str;
	if (isSuchInput(name))
		exsistingNodeType_str = "input";
	else if (isSuchConverter(name))
		exsistingNodeType_str = "converter";
	else if (IsSuchLoad(name))
		exsistingNodeType_str = "load";

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

bool PowerTree::IsSuchLoad (key name) const
{
	bool result = (loads.count(name) > 0);
	return result;
}
