
#include "electric_net.h"




namespace electric_net
{

	ElectricNet::ElectricNet (string nm)
		: title(nm)    { ; }


	 

	void ElectricNet::addInput (Key name, VarKind type, double cvValue)
	{
		isStoragedResultsActual = false;

		auto newInput_ptr = make_shared<Input>(type, cvValue);
		net.addRoot(name, newInput_ptr);

		if (name == default_frist_input_name)
			actualDefaultInputNumber++;
	}

	void ElectricNet::addConverter (Key name, Key sourceName, ConverterType type, VarKind cvType, double cvValue, double efficiency)
	{
		isStoragedResultsActual = false;

		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.pushBackLeaf(name, sourceName, newConverter_ptr);
	}


	void ElectricNet::addConverter (Key name, ConverterType type, VarKind cvType, double cvValue, double efficiencyParam)
	{
		isStoragedResultsActual = false;

		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiencyParam);
		net.addRoot(name, newConverter_ptr);
	}


	void ElectricNet::insertConverter (Key name, Key sourceName, ConverterType type, VarKind cvType, double cvValue, double efficiency)
	{
		isStoragedResultsActual = false;

		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.insertDesc(name, sourceName, newConverter_ptr);
	}


	void ElectricNet::insertConverter (Key name, Key sourceName, Key sinkName, ConverterType type, VarKind cvType, double cvValue, double efficiency)
	{
		isStoragedResultsActual = false;

		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.insertDesc(name, sourceName, sinkName, newConverter_ptr);
	}


	void ElectricNet::addLoad (Key name, Key sourceName, LoadType type, double param)
	{
		if (type == LoadType::DIODE)     throw exception("Diode load must have two parameters");


		isStoragedResultsActual = false;

		auto newLoad_ptr = make_shared<OneParamLoad>(type, param);
		net.pushBackLeaf(name, sourceName, newLoad_ptr);
	}


	void ElectricNet::addLoad (Key name, LoadType type, double param)
	{
		if (type == LoadType::DIODE)     throw exception("Diode load must have two parameters");


		isStoragedResultsActual = false;

		auto newLoad_ptr = make_shared<OneParamLoad>(type, param);
		net.addRoot(name, newLoad_ptr);
	}


	void ElectricNet::addLoad (Key name, Key sourceName, LoadType type, double mainParam, double secondaryParam)
	{
		if (type == LoadType::RESISTIVE)     throw exception("Resistive load can have only one parameter");
		if (type == LoadType::CONSTANT_CURRENT)    throw exception("Constant current load can have only one parameter");


		isStoragedResultsActual = false;

		auto newLoad_ptr = make_shared<TwoParamsLoad>(type, mainParam, secondaryParam);
		net.pushBackLeaf(name, sourceName, newLoad_ptr);
	}


	void ElectricNet::addLoad (Key name, LoadType type, double mainParam, double secondaryParam)
	{
		if (type == LoadType::RESISTIVE)     throw exception("Resistive load can have only one parameter");
		if (type == LoadType::CONSTANT_CURRENT)    throw exception("Constant current load can have only one parameter");


		isStoragedResultsActual = false;

		auto newLoad_ptr = make_shared<TwoParamsLoad>(type, mainParam, secondaryParam);
		net.addRoot(name, newLoad_ptr);
	}


	void ElectricNet::deleteInput (Key name, Key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveAllDesces(name, newSourceName);
		net.popFrontRoot(name);
	}


	void ElectricNet::deleteInput (Key name)
	{
		isStoragedResultsActual = false;

		net.popFrontRoot(name);
	}

	void ElectricNet::deleteConverter (Key name, Key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveAllDesces(name, newSourceName);
		net.popBackLeaf(name);
	}


	void ElectricNet::deleteConverter (Key name)
	{
		isStoragedResultsActual = false;

		net.freeAllDesces(name);
		net.popBackLeaf(name);
	}


	void ElectricNet::deleteLoad (Key name)
	{
		isStoragedResultsActual = false;

		net.popBackLeaf(name);
	}


	void ElectricNet::deleteNode (Key name, Key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveAllDesces(name, newSourceName);
		net.popBackLeaf(name);
	}


	void ElectricNet::deleteNode (Key name)
	{
		isStoragedResultsActual = false;

		net.freeAllDesces(name);

		if ( net.isRoot(name) )
			net.popFrontRoot(name);
		else
			net.popBackLeaf(name);
	}


	void ElectricNet::deleteSubnet (Key headerName)
	{
		isStoragedResultsActual = false;

		net.popBackSubtree(headerName);
	}


	void ElectricNet::deleteAllSinks (Key sourceName)
	{
		isStoragedResultsActual = false;

		net.eraseAllDesces(sourceName);
	}


	void ElectricNet::moveConverter (Key name, Key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveNode(name, newSourceName);
	}


	void ElectricNet::moveConverter (Key name, Key newSourceName, Key newSinksSourceName)
	{
		isStoragedResultsActual = false;

		net.moveNode(name, newSourceName, newSinksSourceName);
	}


	void ElectricNet::freeConverter (Key name)
	{
		isStoragedResultsActual = false;

		net.freeNode(name);
	}
	 

	void ElectricNet::freeConverter (Key name, Key newSinksSourceName)
	{
		isStoragedResultsActual = false;

		net.freeNode(name, newSinksSourceName);
	}
	 

	void ElectricNet::moveLoad (Key name, Key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveLeaf(name, newSourceName);
	}
	 

	void ElectricNet::freeLoad (Key name)
	{
		isStoragedResultsActual = false;

		net.freeLeaf(name);
	}


	void ElectricNet::moveSubnet (Key headerName, Key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveSubtree(headerName, newSourceName);
	}


	void ElectricNet::freeSubnet (Key headerName)
	{
		isStoragedResultsActual = false;

		net.freeSubtree(headerName);
	}


	void ElectricNet::moveNode (Key name, Key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveNode(name, newSourceName);
	}


	void ElectricNet::moveNode (Key name, Key newSourceName, Key newSinksSourceName)
	{
		isStoragedResultsActual = false;

		net.moveNode(name, newSourceName, newSinksSourceName);
	}


	void ElectricNet::freeNode (Key name)
	{
		isStoragedResultsActual = false;

		net.freeNode(name);
	}


	void ElectricNet::freeNode (Key name, Key newSinksSourceName)
	{
		isStoragedResultsActual = false;

		net.freeNode(name, newSinksSourceName);
	}


	void ElectricNet::renameNode (Key name, Key newName)
	{
		isStoragedResultsActual = false;

		net.renameNode(name, newName);
	}


	void ElectricNet::setSourceCvType (Key name, VarKind newType)
	{
		isStoragedResultsActual = false;

		auto source_ptr = dynamic_pointer_cast<Source>( net[name] );
		source_ptr->cvKind = newType;
	}


	void ElectricNet::setSourceCvValue (Key name, double newValue)
	{
		isStoragedResultsActual = false;

		auto source_ptr = dynamic_pointer_cast<Source>(net[name]);
		source_ptr->cvValue = newValue;
	}


	void ElectricNet::setConverterType (Key name, ConverterType newType)
	{
		isStoragedResultsActual = false;

		auto converter_ptr = dynamic_pointer_cast<Converter>(net[name]);
		converter_ptr->type = newType;
	}


	void ElectricNet::setConverterEfficiencyParam (Key name, double efficiencyParam)
	{
		isStoragedResultsActual = false;

		auto converter_ptr = dynamic_pointer_cast<Converter>(net[name]);
		converter_ptr->efficiencyParam = efficiencyParam;
	}


	void ElectricNet::setLoadType (Key name, LoadType newType)
	{
		isStoragedResultsActual = false;

		auto load_ptr = dynamic_pointer_cast<Load>( net[name] );
		load_ptr->type = newType;
	}


	void ElectricNet::setLoadResistance (Key name, double resistance)
	{
		isStoragedResultsActual = false;

		auto load_ptr = dynamic_pointer_cast<OneParamLoad>( net[name] );
		load_ptr->param = resistance;
	}


	void ElectricNet::setLoadCurrent (Key name, double current)
	{
		isStoragedResultsActual = false;

		auto load_ptr = dynamic_pointer_cast<OneParamLoad>( net[name] );
		load_ptr->param = current;
	}


	void ElectricNet::setLoadForawrdVoltage (Key name, double forwardVoltage)
	{
		isStoragedResultsActual = false;

		auto load_ptr = dynamic_pointer_cast<TwoParamsLoad>( net[name] );
		load_ptr->mainParam = forwardVoltage;
	}


	void ElectricNet::setLoadForwardCurrent (Key name, double forwardCurrent)
	{
		isStoragedResultsActual = false;

		auto load_ptr = dynamic_pointer_cast<TwoParamsLoad>( net[name] );
		load_ptr->secondaryParam = forwardCurrent;
	}


	DeviceType ElectricNet::getNodeType (Key name) const
	{
		auto node_ptr = net.at(name);
		auto type = node_ptr->type;
		return type;
	}


	InputData ElectricNet::getInputData (Key inputName) const
	{
		InputData data;

		AUTO_CONST_REF node = net.at(inputName);
		AUTO_CONST_REF input = dynamic_pointer_cast<Input>(node);

		data.name = inputName;

		data.cvKind = input->cvKind;
		data.value = input->cvValue;

		return data;
	}


	ConverterData ElectricNet::getConverterData (Key converterName) const
	{
		ConverterData data;

		AUTO_CONST_REF node = net.at(converterName);
		AUTO_CONST_REF converter = dynamic_pointer_cast<Converter>(node);

		data.name = converterName;
		data.nestingLevel = net.getNestingLevel(converterName);

		data.cvKind = converter->Source::cvKind;
		data.value = converter->cvValue;
		data.type = converter->type;
		data.efficiency = converter->efficiencyParam;

		return data;
	}


	ResistiveLoadData ElectricNet::getResistiveLoadData (Key loadName) const
	{
		ResistiveLoadData data;

		AUTO_CONST_REF node = net.at(loadName);
		AUTO_CONST_REF load = dynamic_pointer_cast<OneParamLoad>(node);

		data.name = loadName;
		data.nestingLevel = net.getNestingLevel(loadName);

		data.resistance = load->param;

		return data;
	}


	ConstantCurrentLoadData ElectricNet::getConstantCurrentLoadData (Key loadName) const
	{
		ConstantCurrentLoadData data;

		AUTO_CONST_REF node = net.at(loadName);
		AUTO_CONST_REF load = dynamic_pointer_cast<OneParamLoad>(node);

		data.name = loadName;
		data.nestingLevel = net.getNestingLevel(loadName);

		data.current = load->param;

		return data;
	}


	DiodeLoadData ElectricNet::getDiodeLoadData (Key loadName) const
	{
		DiodeLoadData data;

		AUTO_CONST_REF node = net.at(loadName);
		AUTO_CONST_REF load = dynamic_pointer_cast<TwoParamsLoad>(node);

		data.name = loadName;
		data.nestingLevel = net.getNestingLevel(loadName);

		data.forwardVoltage = load->mainParam;
		data.forwardCurrent = load->secondaryParam;

		return data;
	}


	VarKind ElectricNet::getConverterVarKind (Key converterName) const
	{
		ConverterData data;

		AUTO_CONST_REF node = net.at(converterName);
		AUTO_CONST_REF converter = dynamic_pointer_cast<Converter>(node);

		return converter->cvKind;
	}


	bool ElectricNet::isLoadExsist (Key name) const
	{
		if (net.isExsist(name))
		{
			auto type = net.at(name) ->type;
			if (type == DeviceType::LOAD)
				return true;
		}
		return false;
	}


	LoadType ElectricNet::getLoadType (Key name) const
	{
		auto load_ptr = dynamic_pointer_cast<Load>( net.at(name) );
		auto type = load_ptr->type;
		return type;
	}


	string ElectricNet::getTitle () const
	{
		return title;
	}


	void ElectricNet::rename (string newTitle)
	{
		title = newTitle;
	}


	void ElectricNet::calculte () const
	{
		if (!isStoragedResultsActual)
		{
			updateCalculations();
			isStoragedResultsActual = true;
		}
	}


	InputResults ElectricNet::getInputResults (Key inputName) const
	{
		InputResults results;

		AUTO_CONST_REF node = net.at(inputName);
		AUTO_CONST_REF input = dynamic_pointer_cast<Input>(node);

		results.name = inputName;

		results.type = input->cvKind;
		results.value = input->cvValue;

		results.avValue = input->avValue;

		return results;
	}


	ConverterResults ElectricNet::getConverterResults (Key convertertName) const
	{
		ConverterResults results;

		AUTO_CONST_REF node = net.at(convertertName);
		AUTO_CONST_REF converter = dynamic_pointer_cast<Converter>(node);

		results.name = convertertName;
		results.nestingLevel = net.getNestingLevel(convertertName);

		results.cvType = converter->Source::cvKind;
		results.value = converter->cvValue;
		results.type = converter->type;

		results.avValue = converter->avValue;
		results.inputValue = converter->inputValue;

		return results;
	}


	ResistiveLoadResults ElectricNet::getResistiveLoadResults (Key loadName) const
	{
		ResistiveLoadResults results;

		AUTO_CONST_REF node = net.at(loadName);
		AUTO_CONST_REF load = dynamic_pointer_cast<OneParamLoad>(node);

		results.name = loadName;
		results.nestingLevel = net.getNestingLevel(loadName);

		results.resistance = load->param;

		results.inputValue = load->inputValue;
		results.inputVarKind = calcInputVarKindByParent(loadName);

		return results;
	}


	ConstantCurrentLoadResults ElectricNet::getConstantCurrentLoadResults (Key loadName) const
	{
		ConstantCurrentLoadResults results;

		AUTO_CONST_REF node = net.at(loadName);
		AUTO_CONST_REF load = dynamic_pointer_cast<OneParamLoad>(node);

		results.name = loadName;
		results.nestingLevel = net.getNestingLevel(loadName);

		results.current = load->param;

		results.inputVoltage = load->inputValue;

		return results;
	}
		
	
	DiodeLoadResults ElectricNet::getDiodeLoadResults (Key loadName) const
	{
		DiodeLoadResults results;

		AUTO_CONST_REF node_ptr = net.at(loadName);
		AUTO_CONST_REF load_ptr = dynamic_pointer_cast<TwoParamsLoad>(node_ptr);

		results.name = loadName;
		results.nestingLevel = net.getNestingLevel(loadName);

		results.forwardVoltage = load_ptr->mainParam;
		results.forwardCurrent = load_ptr->secondaryParam;

		return results;
	}


	void ElectricNet::iterateAndExecuteForEach (function<void (Key)> functor) const
	{
		for (AUTO_CONST_REF node : const_cast<ElectricForest &>(net))
			functor(node.first);
	}


	Key ElectricNet::getDefaultNodeName (DeviceType type) const
	{
		string name;
		switch (type)
		{
			case DeviceType::INPUT:
			{
				name = default_input_name_body + to_string(actualDefaultInputNumber);
				actualDefaultInputNumber++;
				return name;
			}

			case DeviceType::CONVERTER:
			{
				name = default_converter_name_body + to_string(actualDefaultConverterNumber);
				actualDefaultConverterNumber++;
				return name; 
			}

			case DeviceType::LOAD:
			{
				name = default_load_name_body + to_string(actualDefaultLoadNumber);
				actualDefaultLoadNumber++;
				return name;
			}


			default:
				throw exception("Invalid type of device");
		}
	}


#ifdef DEBUG
	bool ElectricNet::operator != (const ElectricNet & other) const
	{
		for (auto it = net.cbegin(); it != net.cend(); it++)
		{

			auto isNodeEqualTo = [it](pair<Key, Node_ptr> otherPair) -> bool
			{
				if ((*it).first != otherPair.first)
					return false;
				if (!isNodesEqualByContent((*it).second, otherPair.second))
					return false;

				return true;
			};



			if (find_if(other.net.cbegin(), other.net.cend(), isNodeEqualTo) == other.net.cend())
				return true;
		}

		return false;
	}
#endif




	void ElectricNet::updateCalculations () const
	{
		for (ElectricForest::const_desces_group_iterator root_it = net.cdgbegin(); root_it != net.cdgend(); root_it++)
			 calculateAndUpdateGivenParams(root_it);
	}


	double ElectricNet::calculateAndUpdateGivenParams (ConstDesc_it source_it) const
	{
		auto sourceVarKind = dynamic_pointer_cast<Source>((*source_it).second)->cvKind;

		switch (sourceVarKind)
		{
			case VarKind::VOLTAGE:
			{
				double accOutputCurrent = 0.0;
				for (ElectricForest::const_desces_group_iterator desc_it = net.cdgbegin((*source_it).first); desc_it != net.cdgend((*source_it).first); 
					                                                                                         desc_it++)
					accOutputCurrent += calculateConsumption (desc_it, source_it);

				writeAvValueToSource(accOutputCurrent, (*source_it).first);

				return accOutputCurrent;
			}

			case VarKind::CURRENT:
			{
				auto firstDesc_it = net.cdgbegin((*source_it).first);
				auto descesType = dynamic_pointer_cast<Load>((*firstDesc_it).second) ->type;

				switch (descesType)
				{
					case LoadType::RESISTIVE:
					{
						double accLoadsTotalConductivity = 0.0;
						for (ElectricForest::const_desces_group_iterator desc_it = net.cdgbegin((*source_it).first); 
							                                             desc_it != net.cdgend((*source_it).first); desc_it++)
						{
							double loadResistance = dynamic_pointer_cast<OneParamLoad>((*desc_it).second) ->param;
							accLoadsTotalConductivity += 1 / loadResistance;
						}
						
						double sourceCurrent = dynamic_pointer_cast<Converter>((*source_it).second) ->cvValue;
						double outputVoltage = sourceCurrent / accLoadsTotalConductivity;
						writeAvValueToSource(outputVoltage, (*source_it).first);
						
						for (ElectricForest::const_desces_group_iterator desc_it = net.cdgbegin((*source_it).first);
							                                             desc_it != net.cdgend((*source_it).first); desc_it++)
						{
							double loadResistance = dynamic_pointer_cast<OneParamLoad>((*desc_it).second) ->param;
							double loadCurrent = outputVoltage / loadResistance;
							writeInputValueToResistiveLoad(loadCurrent, (*desc_it).first);
						}
						
						return outputVoltage;
					}

					case LoadType::CONSTANT_CURRENT:
					{
						throw exception("\"calculateAndUpdateGivenParams\": Constant current load can't be supplied from current source");
					}

					case LoadType::DIODE:
					{
						double forwardVoltage = dynamic_pointer_cast<TwoParamsLoad>((*firstDesc_it).second) ->mainParam;
						return forwardVoltage;
					}


					default:
						throw exception("Invalid type of load");
				}
			}


			default:
				throw exception("invalind kind of controlled variable");
		}
	}
	

	void ElectricNet::writeAvValueToSource (double newAvValue, Key sourceName) const
	{
		AUTO_CONST_REF source = dynamic_pointer_cast<Source>(net.at(sourceName));
		source->avValue = newAvValue;
	}
	

	void ElectricNet::writeInputValueToConverter (double newInputValue, Key converterName) const
	{
		AUTO_CONST_REF source = dynamic_pointer_cast<Converter>(net.at(converterName));
		source->inputValue = newInputValue;
	}


	void ElectricNet::writeInputValueToResistiveLoad (double newInputValue, Key loadName) const
	{
		AUTO_CONST_REF source = dynamic_pointer_cast<OneParamLoad>(net.at(loadName));
		source->inputValue = newInputValue;
	}


	double ElectricNet::calculateConsumption (ConstDesc_it consumer_it, ConstDesc_it source_it) const
	{
		DeviceType type = (*consumer_it).second->type;

		switch (type)
		{
			case DeviceType::CONVERTER:
			{
				calculateAndUpdateGivenParams(consumer_it);
				double inputValue = reduceOutputToInput(consumer_it, source_it);
				writeInputValueToConverter(inputValue, (*consumer_it).first);
				return inputValue;
			}

			case DeviceType::LOAD:
			{
				double loadCurrent = calculateLoadConsumptionDrivenByVoltageSource(consumer_it, source_it);
				return loadCurrent;
			}


			default:
				throw exception("Input doesn't consume");
		}
	}


	double ElectricNet::reduceOutputToInput (ConstDesc_it sink_it, ConstDesc_it source_it) const
	{
		auto source = dynamic_pointer_cast<Source>( (*source_it).second );
		auto converter = dynamic_pointer_cast<Converter>( (*sink_it).second );
		auto converterType = converter->type;
		auto converterVarKind = converter->cvKind;

		double outputPower = (converter->cvValue) * (converter->avValue);
		double efficiency = converter->efficiencyParam;
		double sourcesCv = source->cvValue;

		double inputValue;
		switch (converterType)
		{
			case ConverterType::PULSE:
				inputValue = 100 * outputPower / (efficiency * sourcesCv);
				break;

			case ConverterType::LINEAR:
				inputValue = converter->efficiencyParam;
				switch (converterVarKind)
				{
					case VarKind::VOLTAGE:
						inputValue += converter->avValue;
						break;

					case VarKind::CURRENT:
						inputValue += converter->cvValue;
						break;


					default:
						throw exception("Invalind kind of controlled variable");
				}
				break;


			default:
				throw exception("Invalid type of converter");
		}
		
		return inputValue;
	}


	double ElectricNet::calculateLoadConsumptionDrivenByVoltageSource (ConstDesc_it load_it, ConstDesc_it source_it) const
	{
		auto loadType = dynamic_pointer_cast<Load>((*load_it).second) ->type;

		switch (loadType)
		{
			case LoadType::RESISTIVE:
			{
				double resistance = dynamic_pointer_cast<OneParamLoad>((*load_it).second) ->param;
				double voltage = dynamic_pointer_cast<Source>((*source_it).second) ->cvValue;

				double loadCurrent = voltage / resistance;
				writeInputValueToResistiveLoad(loadCurrent, (*load_it).first);
				return loadCurrent;
			}

			case LoadType::CONSTANT_CURRENT:
			{
				auto current = dynamic_pointer_cast<OneParamLoad>((*load_it).second) ->param;
				return current;
			}

			case LoadType::DIODE:
			{
				auto current = dynamic_pointer_cast<TwoParamsLoad>((*load_it).second) ->secondaryParam;
				return current;
			}

			default:
				throw exception("Invalid type of load");
		}
	}


	VarKind ElectricNet::calcInputVarKindByParent (Key loadName) const
	{
		auto parent_ptr = net.getParent(loadName);
		auto type = dynamic_pointer_cast<Source>(parent_ptr)->cvKind;
		return type;
	}




#ifdef DEBUG
	bool ElectricNet::isNodesEqualByContent (Node_ptr first_ptr, Node_ptr second_ptr)
	{
		const auto & first = *first_ptr;
		const auto & second = *second_ptr;

		if (first.type != second.type)
			return false;

		switch (first.type)
		{
			case DeviceType::INPUT:
			{
				const auto & firstInput = dynamic_cast<const Input &>(first);
				const auto & secondInput = dynamic_cast<const Input &>(second);

				if (firstInput.cvKind != secondInput.cvKind)
					return false;
				if (firstInput.cvValue != secondInput.cvValue)
					return false;

				break;
			}

			case DeviceType::CONVERTER:
			{
				const auto & firstConverter = dynamic_cast<const Converter &>(first);
				const auto & secondConverter = dynamic_cast<const Converter &>(second);

				if (firstConverter.cvKind != secondConverter.cvKind)
					return false;
				if (firstConverter.cvValue != secondConverter.cvValue)
					return false;
				if (firstConverter.type != secondConverter.type)
					return false;

				if (firstConverter.type == ConverterType::PULSE)
					if (firstConverter.efficiencyParam != secondConverter.efficiencyParam)
						return false;


				break;
			}

			case DeviceType::LOAD:
			{
				const auto & firstLoad = dynamic_cast<const Load &>(first);
				const auto & secondLoad = dynamic_cast<const Load &>(second);

				if (firstLoad.type != secondLoad.type)
					return false;

				switch (firstLoad.type)
				{
					case LoadType::RESISTIVE: [[__fallthrough]]
					case LoadType::CONSTANT_CURRENT:
					{
						const auto & firstOneParamLoad = dynamic_cast<const OneParamLoad &>(first);
						const auto & secondOneParamLoad = dynamic_cast<const OneParamLoad &>(second);

						if (firstOneParamLoad.param != secondOneParamLoad.param)
							return false;


						break;
					}

					case LoadType::DIODE:
					{
						const auto & firstTwoParamsLoad = dynamic_cast<const TwoParamsLoad &>(first);
						const auto & secondTwoParamsLoad = dynamic_cast<const TwoParamsLoad &>(second);

						if (firstTwoParamsLoad.mainParam != secondTwoParamsLoad.mainParam)
							return false;
						if (firstTwoParamsLoad.secondaryParam != secondTwoParamsLoad.secondaryParam)
							return false;


						break;
					}
						

					default:
						throw exception("Invalid type of load");
				}

				
				break;
			}


			default:
				throw exception("Invalid type of device");
		}

		return true;
	}
#endif

	








	ElectricNet::ElectricNode::ElectricNode (DeviceType tp)
		: type(tp)    {;}





	ElectricNet::Source::Source (DeviceType devType, VarKind tp, double value)
		: ElectricNode(devType), cvKind(tp), cvValue(value)    {;}





	ElectricNet::Input::Input (VarKind type, double cvValue)
		: Source(DeviceType::INPUT, type, cvValue)    {;}





	ElectricNet::Converter::Converter (VarKind cvType, double value, ConverterType tp, double effcnc)
		: Source(DeviceType::CONVERTER, cvType, value), type(tp), efficiencyParam(effcnc)    {;}





	ElectricNet::Load::Load (LoadType tp)
		: ElectricNode(DeviceType::LOAD), type(tp)    {;}





	ElectricNet::OneParamLoad::OneParamLoad (LoadType tp, double param)
		: Load(tp), param(param)    
	{
		if (tp == LoadType::DIODE)     throw exception("Diode load must have two parameters");
	}
	




	ElectricNet::TwoParamsLoad::TwoParamsLoad(LoadType tp, double firstParam, double secondParam)
		: Load(tp), mainParam(firstParam), secondaryParam(secondParam)    
	{
		if (tp == LoadType::RESISTIVE)    throw exception("Resistive load can have only one parameter");
		if (tp == LoadType::CONSTANT_CURRENT)    throw exception("Constant current load can have only one parameter");
	}

}
