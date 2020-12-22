
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
				writing_stream operator << (InputData & stream) const;
				writing_stream operator << (ConverterData & stream) const;
				writing_stream operator << (ResistiveLoadData & stream) const;
				writing_stream operator << (ConstantCurrentLoadData & stream) const;
				writing_stream operator << (DiodeLoadData & stream) const;
		};



		void createOrOpenFile (string fileName) const;
		void printHeader () const;
		const writing_stream & getWritingStream () const;
		void printTail () const;
		void saveAndCloseFile () const;




	private:
		
		ofstream wstream;

};
