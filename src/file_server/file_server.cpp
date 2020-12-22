
#pragma once


#include "file_server/file_server.h"




FileServer::writing_stream FileServer::writing_stream::operator<<(InputData& stream) const
{
	return writing_stream();
}

FileServer::writing_stream FileServer::writing_stream::operator << (ConverterData& stream) const
{
	return writing_stream();
}
FileServer::writing_stream FileServer::writing_stream::operator << (ResistiveLoadData& stream) const
{
	return writing_stream();
}
FileServer::writing_stream FileServer::writing_stream::operator << (ConstantCurrentLoadData& stream) const
{
	return writing_stream();
}
FileServer::writing_stream FileServer::writing_stream::operator << (DiodeLoadData& stream) const
{
	return writing_stream();
}

void FileServer::createOrOpenFile(string fileName) const
{
}

void FileServer::printHeader () const
{

}

const FileServer::writing_stream & FileServer::getWritingStream() const
{
	return writing_stream();
}

void FileServer::printTail() const
{
}

void FileServer::saveAndCloseFile() const
{
}
