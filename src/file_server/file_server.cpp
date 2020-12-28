
#pragma once


#include <cctype>
#include <utility>
#include <filesystem>


#include "lib/ciflib.h"


#include "file_server/file_server.h"
#include "file_server/file_server_language_description.h"




namespace file_server
{

	using namespace std::filesystem;

	using std::to_string;




	istream & operator >> (istream & is, ReadInput & data);
	istream & operator >> (istream & is, ReadConverter & data);
	istream & operator >> (istream & is, ReadLoad & data);




	const string file_extension = ".pts";




	
	FileWriter::FileWriter (string treeName, string fileName, string path)
	{
		size_t pathSize = path.size();
		if (pathSize != 0)
		{
			wchar_t lastChar = path[pathSize - 1];
			if (lastChar != '\\')
				path += '\\';

			create_directory(path);
		}

		string fileWithPath = path + fileName + file_extension;
		wstream.open(fileWithPath);


		wstream << treeName << endl << endl;
	}




	FileWriter & FileWriter::operator << (InputData & data)
	{
		auto [name, cvKind, value] = data;

		wstream << getNodeTagByNestingLevel(1) << " ";

		wstream << node_input_tag << " " << name << endl;
		wstream << getVarTagByVarKind(cvKind) << " " << to_string(value) << endl;
		wstream << endl;

		return *this;
	}


	FileWriter & FileWriter::operator << (ConverterData & data)
	{
		auto [name, nestingLevel, cvKind, value, type, efficiency] = data;

		wstream << getNodeTagByNestingLevel(nestingLevel) << " ";

		wstream << node_converter_tag << " " << name << endl;
		wstream << getVarTagByVarKind(cvKind) << " " << to_string(value) << " " << getConverterTypeTagByType(type);
		if (type == ConverterType::PULSE)
			wstream << " " << to_string(efficiency) << endl;

		wstream << endl;

		return *this;
	}


	FileWriter & FileWriter::operator << (ResistiveLoadData & data)
	{
		auto [name, nestingLevel, resistance] = data;

		wstream << getNodeTagByNestingLevel(1) << " ";

		wstream << node_input_tag << " " << name << endl;
		wstream << load_resistance_tag << " " << to_string(resistance) << endl;
		wstream << endl;

		return *this;
	}


	FileWriter & FileWriter::operator << (ConstantCurrentLoadData & data)
	{
		auto [name, nestingLevel, current] = data;

		wstream << getNodeTagByNestingLevel(1) << " ";

		wstream << node_input_tag << " " << name << endl;
		wstream << load_resistance_tag << " " << to_string(current) << endl;
		wstream << endl;

		return *this;
	}


	FileWriter & FileWriter::operator << (DiodeLoadData & data)
	{
		auto [name, nestingLevel, forwardVoltage, forwardCurrent] = data;

		wstream << getNodeTagByNestingLevel(1) << " ";

		wstream << node_input_tag << " " << name << endl;
		wstream << load_resistance_tag << " " << to_string(forwardVoltage) << " " << to_string(forwardCurrent) << endl;

		return *this;
	}




	string FileWriter::getNodeTagByNestingLevel (unsigned level)
	{
		string tag(node_tag_template);
		tag += to_string(level);
		return tag;
	}


	string FileWriter::getVarTagByVarKind (VarKind kind)
	{
		switch (kind)
		{
		case VarKind::VOLTAGE:
			return var_voltage_tag;
		case VarKind::CURRENT:
			return var_current_tag;

		default:
			throw exception("Invalid kind of controlled variable");
		}
	}


	string FileWriter::getConverterTypeTagByType (ConverterType type)
	{
		switch (type)
		{
		case ConverterType::PULSE:
			return converter_pulse_tag;
		case ConverterType::LINEAR:
			return converter_linear_tag;

		default:
			throw exception("Invalid type of converter");
		}
	}










	FileReader::FileReader (string fileName, string path)
	{
		size_t pathSize = path.size();
		if (pathSize != 0)
		{
			wchar_t lastChar = path[pathSize - 1];
			if (lastChar != '\\')
				path += '\\';
		}

		string fileWithPath = path + fileName + file_extension;
		rstream.open(fileWithPath);
	}




	string FileReader::getTitle ()
	{
		string title;
		rstream >> title;
		return title;
	}


	bool FileReader::hasUnreadNode () const
	{
		bool result = !(rstream.eof());
		return result;
	}


	FileReader & FileReader::operator >> (ReadNode & node)
	{
		tag nestingLevel_tag;
		rstream >> nestingLevel_tag;
		nestingLevel_tag.erase(0, 1);
		unsigned nestingLevel = stoi(nestingLevel_tag);


		tag nodeType_tag;
		rstream >> nodeType_tag;

		if (nodeType_tag == node_input_tag)
			node.type = DeviceType::INPUT;
		else if (nodeType_tag == node_converter_tag)
			node.type = DeviceType::CONVERTER;
		else if (nodeType_tag == node_load_tag)
			node.type = DeviceType::LOAD;

		else    throw exception("Invalid type of electric node");


		rstream >> node.name;


		switch (node.type)
		{
		case DeviceType::INPUT:
		{
			updateStackAndCalcParentName({ node.name, nestingLevel });

			ReadInput data;
			rstream >> data;
			node.data = data;
			break;
		}

		case DeviceType::CONVERTER:
		{
			key parentName = updateStackAndCalcParentName({ node.name, nestingLevel });

			ReadConverter data;

			if (nestingLevel > 1)
				data.commonSinkData.parentName = parentName;
			rstream >> data;
			node.data = data;
			break;
		}

		case DeviceType::LOAD:
		{
			key parentName = updateStackAndCalcParentName({ node.name, nestingLevel });

			ReadLoad data;

			if (nestingLevel > 1)
				data.commonSinkData.parentName = parentName;
			rstream >> data;
			node.data = data;
			break;
		}
		}


		return *this;
	}




	key FileReader::updateStackAndCalcParentName (pair<key, unsigned> nameAndNestingLevel)
	{
		auto [name, nestingLevel] = nameAndNestingLevel;

		size_t stackSize = hierarchyStack.size();
		if (nestingLevel > (stackSize + 1))
			throw exception("Invalid nesting level of subsequent node");


		key parentName;
		if (nestingLevel == (stackSize + 1))
		{
			if (stackSize > 0)
				parentName = hierarchyStack.back();
			hierarchyStack.push_back(name);
			return parentName;
		}

		if (nestingLevel == stackSize)
		{
			hierarchyStack.back() = name;
			if (stackSize > 1)
				parentName = hierarchyStack[stackSize - 2];
			return parentName;
		}

		size_t deletedNodes_qty = stackSize + 1 - nestingLevel;
		pop_back(hierarchyStack, deletedNodes_qty);
		hierarchyStack.push_back(name);
		if (hierarchyStack.size() > 1)
			parentName = hierarchyStack[nestingLevel - 2];
		return parentName;
	}










	istream & operator >> (istream & is, ReadInput & data)
	{
		string cvKind_tag;
		is >> cvKind_tag;
		data.cvKind = parseVarKind(cvKind_tag);

		double value;
		is >> value;
		data.value = value;

		return is;
	}


	istream & operator >> (istream & is, ReadConverter & data)
	{
		string cvKind_tag;
		is >> cvKind_tag;
		data.cvKind = parseVarKind(cvKind_tag);

		double value;
		is >> value;
		data.value = value;

		string type_tag;
		is >> type_tag;
		data.type = parseConverterType(type_tag);

		if (data.type == ConverterType::PULSE)
		{
			double efficiency;
			is >> efficiency;
			data.efficiency = efficiency;
		}

		return is;
	}


	istream & operator >> (istream & is, ReadLoad & data)
	{
		string type_tag;
		is >> type_tag;
		data.type = parseLoadType(type_tag);

		double mainParam;
		is >> mainParam;
		data.mainParam = mainParam;

		if (data.type == LoadType::DIODE)
		{
			double additionalParam;
			is >> additionalParam;
			data.additionalParam = additionalParam;
		}

		return is;
	}

}