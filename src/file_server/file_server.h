
#pragma once


#include <fstream>


#include "config.h"


#include "electric_net/electric_net_if.h"




using namespace electric_net;

using std::ofstream;





class FileServer
{

	public:
	
		class writing_stream
		{
			public:
				writing_stream operator << (InputData & data);
				writing_stream operator << (ConverterData & data);
				writing_stream operator << (ResistiveLoadData & data);
				writing_stream operator << (ConstantCurrentLoadData & data);
				writing_stream operator << (DiodeLoadData & data);

			private:
				ofstream wstream;
		};



		FileServer (string treeName, string fileName);



	    writing_stream & getWritingStream ();



		~FileServer ();




	private:
		
		static const string file_extension;

		static const string node_tag_template;
		static const string input_tag;



		writing_stream wstream;



		static string getNodeTagByNestingLevel (unsigned level = 1);
		static string getVarTagByVarKind (VarKind kind);

};
