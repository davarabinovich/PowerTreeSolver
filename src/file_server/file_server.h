
#pragma once


#include <fstream>
#include <variant>


#include "config.h"


#include "electric_net/electric_net_if.h"
#include "file_server/file_server_language_description.h"




namespace file_server
{

	using namespace electric_net;

	using std::ofstream;
	using std::vector;





	class FileWriter
	{
	
		public:
	
			FileWriter (string treeName, string fileName, string path = "");
	
	
	
			FileWriter & operator << (InputData & data);
			FileWriter & operator << (ConverterData & data);
			FileWriter & operator << (ResistiveLoadData & data);
			FileWriter & operator << (ConstantCurrentLoadData & data);
			FileWriter & operator << (DiodeLoadData & data);
	
	
	
	
		private:
	
			ofstream wstream;
	
	
	
			static Tag getNodeTagByNestingLevel (unsigned level = 1);
			static Tag getVarTagByVarKind (VarKind kind);
			static Tag getConverterTypeTagByType (ConverterType type);
	
	};










	struct ReadInput
	{
		VarKind cvKind;
		double value;
	};


	struct ReadSink
	{
		Key parentName;
	};


	struct ReadConverter
	{
		ReadSink commonSinkData;

		ConverterType type;
		VarKind cvKind;
		double value;
		double efficiencyParam;
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
		Key name;
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

			vector<Key> hierarchyStack;



			Key updateStackAndCalcParentName (pair<Key, unsigned> nameAndNestingLevel);

	};

}