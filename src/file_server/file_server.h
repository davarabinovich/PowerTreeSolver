
#pragma once


#include <fstream>
#include <variant>


#include "config.h"


#include "electric_net/electric_net_if.h"




using namespace electric_net;

using std::ofstream;
using std::vector;





class FileHandler
{

	protected:

		FileHandler ();



#pragma todo to use
		using tag = string;



#pragma make a common for that and reader
		static const string file_extension;

#pragma todo to other file
		static const string node_tag_template;
		static const string node_input_tag;
		static const string node_converter_tag;
		static const string node_load_tag;

		static const string var_voltage_tag;
		static const string var_current_tag;

		static const string converter_pulse_tag;
		static const string converter_linear_tag;
		static const string load_resistance_tag;
		static const string load_constant_current_tag;
		static const string load_diode_tag;

};





class FileWriter : public FileHandler
{

	public:

		FileWriter (string treeName, string fileName);
		FileWriter (string treeName, string fileName, string path);



		FileWriter & operator << (InputData & data);
		FileWriter & operator << (ConverterData & data);
		FileWriter & operator << (ResistiveLoadData & data);
		FileWriter & operator << (ConstantCurrentLoadData & data);
		FileWriter & operator << (DiodeLoadData & data);




	private:

		ofstream wstream;



		static string getNodeTagByNestingLevel (unsigned level = 1);
		static string getVarTagByVarKind (VarKind kind);
		static string getConverterTypeTagByType (ConverterType type);

};










struct ReadInput
{
	VarKind cvKind;
	double value;
};


struct ReadConvertert
{
	key parentName;

	ConverterType type;
	VarKind cvKind;
	double value;
	double efficiency;
};


struct ReadLoad
{
	key parentName;

	LoadType type;
	double mainParam;
	double additionalParam;
};


struct ReadNode
{
	DeviceType type;
	key name;
	variant<ReadInput, ReadConvertert, ReadLoad> data;
};




class FileReader : public FileHandler
{

	public:

		FileReader (string fileName, string path);



		string getTitle ();
		bool hasUnreadNode () const;
		FileReader & operator >> (ReadNode & node);




	private:
	
		ifstream rstream;

		vector<key> hierarchyStack;



		friend istream & operator >> (istream & is, ReadInput & data);
		friend istream & operator >> (istream & is, ReadConvertert & data);
		friend istream & operator >> (istream & is, ReadLoad & data);
	
};
