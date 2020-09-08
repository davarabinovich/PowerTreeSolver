
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


double PowerTree::PowerLoad::calculateConsumption (double parentCvValue, CvType parentCvType) const
{
	double consumption;
	switch (parentCvType)
	{
		case CvType::VOLTAGE:
			consumption = power / nomVoltatge;
			break;

		case CvType::CURRENT:
			
			break;

		default:
			throw exception("Invalid type of parent's controllable variable in the power load");
	}
	return consumption;
}















void PowerTree::Source::calculateLoad () const
{
	double tempLoad = 0.0;
	for (const auto& sink_ptr : sinks)
	{
		const auto& sink = *sink_ptr.second;
		tempLoad += sink.calculateConsumption(cvValue, cvType);
	}
	load = tempLoad;
}


double PowerTree::Converter::calculateConsumption (double parentCvValue, CvType parentCvType) const
{
	calculateLoad();
	double consumption = recudeLoadToInput(parentCvValue, parentCvType) + getSelfConsumption(parentCvValue, parentCvType);
	return consumption;
}


double PowerTree::Converter::getSelfConsumption (double parentCvValue, CvType parentCvType) const
{
	double selfConsumption = (1 - efficiency) * cvValue * load;
	return selfConsumption;
}





double PowerTree::PulseConverter::recudeLoadToInput (double parentCvValue, CvType parentCvType) const
{
	double reducedLoad = load * cvValue / (parentCvValue * efficiency);
	return reducedLoad;
}

double PowerTree::LinearConverter::recudeLoadToInput (double parentCvValue, CvType parentCvType) const
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

void PowerTree::calculate() const
{
	for (const auto & input_ptr : inputs)
	{
		const auto & input = *input_ptr.second;
		input.calculateLoad();
	}
}


