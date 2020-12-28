
#pragma once


#include <fstream>
#include <variant>


#include "config.h"


#include "electric_net/electric_net_if.h"




namespace file_server
{

	using namespace electric_net;

	using std::ofstream;
	using std::vector;





	class FileWriter
	{
	
		public:
	
			FileWriter (string treeName, string fileName, string path = "");
	
	
	
			FileWriter& operator << (InputData & data);
			FileWriter& operator << (ConverterData & data);
			FileWriter& operator << (ResistiveLoadData & data);
			FileWriter& operator << (ConstantCurrentLoadData & data);
			FileWriter& operator << (DiodeLoadData & data);
	
	
	
	
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


	struct ReadSink
	{
		key parentName;
	};


	struct ReadConverter
	{
		ReadSink commonSinkData;

		ConverterType type;
		VarKind cvKind;
		double value;
		double efficiency;
	};


	struct ReadLoad
	{
		ReadSink commonSinkData;

		LoadType type;
		double mainParam;
		double additionalParam;
	};


	struct ReadNode
	{
		DeviceType type;
		key name;
		variant<ReadInput, ReadConverter, ReadLoad> data;
	};




	class FileReader
	{

		public:

			FileReader (string fileName, string path);



			string getTitle ();
			bool hasUnreadNode () const;
			FileReader & operator >> (ReadNode & node);




		private:

			ifstream rstream;

			vector<key> hierarchyStack;



			key updateStackAndCalcParentName (pair<key, unsigned> nameAndNestingLevel);

	};

}