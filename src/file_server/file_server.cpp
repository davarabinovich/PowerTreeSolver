
#pragma once


#include <cctype>
#include <utility>
#include <filesystem>


#include "lib/ciflib.h"


#include "file_server/file_server.h"




using namespace std::filesystem;

using std::to_string;





const string FileWriter::file_extension = ".pts";

const string FileWriter::node_tag_template = "f";
const string FileWriter::node_input_tag = "in";
const string FileWriter::node_converter_tag = "con";
const string FileWriter::node_load_tag = "l";

const string FileWriter::var_voltage_tag = "vol";
const string FileWriter::var_current_tag = "cur";

const string FileWriter::converter_pulse_tag = "p";
const string FileWriter::converter_linear_tag = "l";
const string FileWriter::load_resistance_tag = "res";
const string FileWriter::load_constant_current_tag = "cur";
const string FileWriter::load_diode_tag = "d";




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
