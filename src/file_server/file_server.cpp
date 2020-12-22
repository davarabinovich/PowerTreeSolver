
#pragma once


#include <cctype>


#include "file_server/file_server.h"




using std::to_string;





FileServer::writing_stream FileServer::writing_stream::operator << (InputData & data)
{
	wstream << getNodeTagByNestingLevel(1) << " ";

	auto [name, type, value] = data;
	wstream << input_tag << " " << name << endl;
	wstream << getVarTagByVarKind(type) << " " << to_string(value) << endl;
	wstream << endl;

	return *this;
}


FileServer::writing_stream FileServer::writing_stream::operator << (ConverterData & data)
{
	return writing_stream();
}


FileServer::writing_stream FileServer::writing_stream::operator << (ResistiveLoadData & data)
{
	return writing_stream();
}


FileServer::writing_stream FileServer::writing_stream::operator << (ConstantCurrentLoadData & data)
{
	return writing_stream();
}


FileServer::writing_stream FileServer::writing_stream::operator << (DiodeLoadData & data)
{
	return writing_stream();
}










const string FileServer::file_extension = ".pts";
const string FileServer::node_tag_template = "f";
const string FileServer::input_tag = "in";




FileServer::FileServer (string treeName, string fileName)
{
	ofstream wstream;

	string fileFullName(fileName);
	fileFullName = fileFullName + file_extension;
	wstream.open(fileFullName);

	wstream << treeName << endl << endl;
}





FileServer::writing_stream & FileServer::getWritingStream ()
{
	return wstream;
}




string FileServer::getNodeTagByNestingLevel (unsigned level)
{
	string tag(node_tag_template);
	tag += to_string(level);
	return tag;
}


string FileServer::getVarTagByVarKind (VarKind kind)
{
	switch (kind)
	{
		case VarKind::VOLTAGE:
			return "vol";
		case VarKind::CURRENT:
			return "cur";
		
		default:
			throw exception("Invalid kind of controlled variable");
	}
}




FileServer::~FileServer ()
{

}
