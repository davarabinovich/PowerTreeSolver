
#pragma once


#include "electric_net/electric_net.h"


#include "config.h"



using namespace electric_net;
//static ElectricNet en();
enum class DeletingMode { WITH_DESCES, HANG_DESCES, RECONNECT_DESCES, NONE };


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




namespace electric_net
{

	class ElectricNet_If
	{
		
		public:

			using key = string;

			
			
			virtual void addInput (key name, CvType type = CvType::VOLTAGE, double cvValue = 0.0);
			virtual void addConverter (key name, key sourceName, ConverterType type = ConverterType::PULSE, CvType cvType = CvType::VOLTAGE, 
							           double cvValue = 0.0, double efficiency = 100.0);
			virtual void addConverter (key name, ConverterType type = ConverterType::PULSE, CvType cvType = CvType::VOLTAGE, 
									   double cvValue = 0.0, double efficiency = 100.0);
			virtual void insertConverter (key name, key sourceName, ConverterType type = ConverterType::PULSE, 
										  CvType cvType = CvType::VOLTAGE, double cvValue = 0.0, double efficiency = 100.0);
			virtual void insertConverter (key name, key sourceName, key sinkName, ConverterType type = ConverterType::PULSE, 
								          CvType cvType = CvType::VOLTAGE, double cvValue = 0.0, double efficiency = 100.0);
			

			template <typename type, class Other>
			void addLoad (key name, key sourceName, Other loadParams);

			template <typename type, class Other>
			void addLoad (key name, Other loadParams);


			virtual void deleteInput (key name, key newSourceName);
			virtual void deleteInput (key name);
			virtual void deleteConverter (key name, key newSourceName);
			virtual void deleteConverter (key name);
			virtual void deleteLoad (key name);
			virtual void deleteNode (key name, key newSourceName);
			virtual void deleteNode (key name);
			virtual void deleteSubnet (key headerName);
			virtual void deleteAllSinks (key sourceName);

			virtual void moveConverter (key name, key newSourceName);
			virtual void moveConverter (key name, key newSourceName, key newSinksSourceName);
			virtual void freeConverter (key name);
			virtual void freeConverter (key name, key newSinksSourceName);
			virtual void moveLoad (key name, key newSourceName);
			virtual void freeLoad (key name);
			virtual void moveSubnet (key headerName, key newSourceName);
			virtual void freeSubnet (key headerName);
			virtual void moveNode (key name, key newSourceName);
			virtual void moveNode (key name, key newSourceName, key newSinksSourceName);
			virtual void freeNode (key name);
			virtual void freeNode (key name, key newSinksSourceName);

			virtual void renameNode (key name, key newName);
			virtual void setSourceCvType (key name, CvType newType);
			virtual void setSourceCvValue (key name, double value);
			virtual void setConverterType (key name, ConverterType type);
			virtual void setConverterEfficienct (key name, double efficiency);
			virtual void setLoadType (key name, LoadType type);
			virtual void setLoadResistance (key name, double resistance);
			virtual void setLoadCurrent (key name, double current);
			virtual void setLoadForawrdVoltage (key name, double forwardVoltage);
			virtual void setLoadForwardCurrent (key name, double forwardCurrent);
			virtual void setLoadNomPower (key name, double nomPower);
			virtual void setLoadNomVoltage (key name, double nomVoltage);

			virtual void calculte ();

		protected:


	};

	template<typename type, class Other>
	inline void ElectricNet_If::addLoad (key name, key sourceName, Other loadParams)
	{
		auto newLoad_ptr = make_shared<type>(loadParams);
		//net.pushBackLeaf(name, sourceName, newLoad_ptr);
	}


	template<typename type, class Other>
	inline void ElectricNet_If::addLoad (key name, Other loadParams)
	{
		auto newLoad_ptr = make_shared<type>(loadParams);
		//net.addRoot(name, newLoad_ptr);
	}
}
