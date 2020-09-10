
#include "tree_core.h"




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





double PowerTree::Source::calculateLoad () const
{
	double load = 0.0;
	for (const auto& sink_ptr : descendants)
	{
		const auto& sink = *sink_ptr.second;
		load += sink.calculateConsumption(cvValue, cvType);
	}
	return load;
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





PowerTree::PowerTree (key nm)
{
	if (nm == "")    throw exception("Name of tree can't be empty");

	name = nm;
}




void PowerTree::addInput (key name)
{
	if (name == "")    throw exception("Name of node can't be empty");
	if (inputs.count(name) > 0) throw

	inputs[name];
}

 
void PowerTree::addConverter (key name, key parentName)
{

}


void PowerTree::addLoad (key name, key parentName)
{

}


void PowerTree::moveSubnetTo (key subnetHeadName, key newParentName)
{

}


void PowerTree::disconnectSubnet (key subnetHeadName)
{

}


PowerTree::DescendantsMap & PowerTree::getDescendants (key headName)
{
#todo pragma add validation
	return DescendantsMap();
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

#pragma todo needs to return result yet
}
