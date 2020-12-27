
#pragma once


#include <fstream>


#include "config.h"


#include "electric_net/electric_net_if.h"




using namespace electric_net;

using std::ofstream;





class FileWriter
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

#pragma todo to other file
		static const string file_extension;

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



		ofstream wstream;



		static string getNodeTagByNestingLevel (unsigned level = 1);
		static string getVarTagByVarKind (VarKind kind);
		static string getConverterTypeTagByType (ConverterType type);

};
