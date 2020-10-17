
#pragma once


#include "power_tree/power_tree_core.h"

#include "config.h"




struct Results
{
	struct Load
	{
		string name;
		LoadType type;
		double value;
		double power;
		double secondaryParam;
	};

	struct Source
	{
		string name;
		CvType cvType;
		double cvValue;
		double avValue;
		double power;

		vector<Source> converterSinks;
		vector<Load>   loadSinks;
	};


	vector<Source> inputs;
};

struct TreeStructure
{
	struct Load
	{
		string name;
		LoadType type;
		double value;
	};

	struct Converter
	{
		string name;
		CvType cvType;
		double cvValue;
		ConverterType type;

		vector<Converter> converterSinks;
		vector<Load>      loadSinks;
	};

	struct Input
	{
		string name;
		CvType cvType;
		double cvValue;

		vector<Converter> converterSinks;
		vector<Load>      loadSinks;
	};




	vector<Input> inputs;
	vector<Converter> flyingConverters;
	vector<Load> flyingLoads;
};




bool IsThereSomeTree() { return true; }

void CreateTree() {}
void CreateTree(string name) {}

void RenameTree(string name) {}

void Solve() {}
Results GetResults() { return Results(); }

string GetNameOfTree() { return string(); }
TreeStructure GetTreeStructure() { return TreeStructure(); }



bool IsNodeExsist(string name) { return true; }

string GetTypeOfNode_str(string name) { return string("Converter"); }



bool IsSourceExsist(string name) { return true; }
string GetSourceType(string name) { return string(); }

string GetTypeOfSource_str(string name) { return string(); }

void DeleteSourceWithDescendants(string name) {}
void DeleteSourceAndMoveDescendants(string name, string newParentName = "") {}



bool IsSinkExsist(string name) { return true; }

void ConnectSinkTo(string sinkName, string parentName) {}
void DisconnectSink(string name) {}

string GetTypeOfSink_str(string name) { return string(); }



string CreateInput() { return string(); }
void CreateInput(string name) {}

void RenameInput(string oldName, string newName) {}
void SetCvTypeForInput(string name, CvType type) {}
void SetCvValueForInput(string name, double cvValue) {}

bool IsInputExsist(string name) { return true; }



string CreateConverter() { return string(); }
void CreateConverter(string name) {}

void RenameConverter(string oldName, string newName) {}
void SetTypeForConverter(string name, ConverterType type) {}
void SetCvTypeForConverter(string name, CvType type) {}
void SetCvValueForConverter(string name, double value) {}
void SetEfficiencyForConverter(string name, double efficiency) {}

bool IsConverterExsist(string name) { return true; }



string CreateLoad() { return string(); }
void CreateLoad(string name) {}

void RenameLoad(string oldName, string newName) {}
void SetTypeForLoad(string name, LoadType type) {}
void SetValueForLoad(string name, double value) {}
void SetNomVoltageForPowerLoad(string name, double nomVoltage) {}

void DeleteLoad(string name) {}

bool IsLoadExsist(string name) { return true; }



bool AreParentAndDescendant(string assumedParent, string assumedDescendant) { return false; }
