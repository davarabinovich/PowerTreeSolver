
#pragma once


#include <cctype>
#include <utility>
#include <filesystem>


#include "lib/ciflib.h"


#include "file_server/file_server.h"




using namespace std::filesystem;

using std::to_string;





istream & operator >> (istream & is, ReadInput & data);
istream & operator >> (istream & is, ReadConvertert & data);
istream & operator >> (istream & is, ReadLoad & data);










const string FileHandler::file_extension = ".pts";

const string FileHandler::node_tag_template = "f";
const string FileHandler::node_input_tag = "in";
const string FileHandler::node_converter_tag = "con";
const string FileHandler::node_load_tag = "l";

const string FileHandler::var_voltage_tag = "vol";
const string FileHandler::var_current_tag = "cur";

const string FileHandler::converter_pulse_tag = "p";
const string FileHandler::converter_linear_tag = "l";
const string FileHandler::load_resistance_tag = "res";
const string FileHandler::load_constant_current_tag = "cur";
const string FileHandler::load_diode_tag = "d";




FileHandler::FileHandler () { ; }










FileWriter::FileWriter (string treeName, string fileName)
	: wstream(fileName + file_extension)
{
	wstream << treeName << endl << endl;
}


FileWriter::FileWriter (string treeName, string fileName, string path)
{
	create_directory(path);

	size_t pathSize = path.size();
	wchar_t lastChar = path[pathSize-1];
	if (lastChar != '\\')
		path += '\\';

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
	wstream << getVarTagByVarKind(cvKind) << " " << to_string(value) << " " << getConverterTypeTagByType(type) << " " 
		    << to_string(efficiency) << endl;
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
	wchar_t lastChar = path[pathSize - 1];
	if (lastChar != '\\')
		path += '\\';

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
	bool result = rstream.eof();
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

	else    throw exception ("Invalid type of electric node");


	rstream >> node.name;


	switch (node.type)
	{
		case DeviceType::INPUT:
		{
			ReadInput data;
			rstream >> data;
			node.data = data;
			break;
		}

		case DeviceType::CONVERTER:
		{
			ReadConvertert data;

			rstream >> data;
			node.data = data;
			break;
		}

		case DeviceType::LOAD:
		{
			ReadLoad data;
			rstream >> data;
			node.data = data;
			break;
		}
	}


	return *this;
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


istream & operator >> (istream & is, ReadConvertert & data)
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

	double efficiency;
	is >> efficiency;
	data.value = efficiency;

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
