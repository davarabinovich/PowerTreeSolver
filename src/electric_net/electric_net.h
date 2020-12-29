
#pragma once


#include <map>
#include <memory>


#include "config.h"


#include "lib/ciflib.h"
#include "forest/forest.h"


#include "electric_net/electric_net_if.h"




using namespace std;




namespace electric_net
{

	class ElectricNet : public ElectricNet_If
	{
	
		public:
			
			ElectricNet (string name);



			virtual void addInput (key name, VarKind type = VarKind::VOLTAGE, double cvValue = 0.0) override;
			virtual void addConverter (key name, key sourceName, ConverterType type = ConverterType::PULSE, VarKind cvType = VarKind::VOLTAGE, 
							          double cvValue = 0.0, double efficiency = 100.0) override;
			virtual void addConverter (key name, ConverterType type = ConverterType::PULSE, VarKind cvType = VarKind::VOLTAGE, double cvValue = 0.0,
							           double efficiency = 100.0) override;
			virtual void insertConverter (key name, key sourceName, ConverterType type = ConverterType::PULSE, VarKind cvType = VarKind::VOLTAGE, 
							              double cvValue = 0.0, double efficiency = 100.0) override;
			virtual void insertConverter (key name, key sourceName, key sinkName, ConverterType type = ConverterType::PULSE, 
								          VarKind cvType = VarKind::VOLTAGE, double cvValue = 0.0, double efficiency = 100.0) override;
			virtual void addLoad (key name, key sourceName, LoadType type, double param) override;
			virtual void addLoad (key name, LoadType type, double param) override;
			virtual void addLoad (key name, key sourceName, LoadType type, double mainParam, double secondaryParam) override;
			virtual void addLoad (key name, LoadType type, double mainParam, double secondaryParam) override;

			virtual void deleteInput (key name, key newSourceName) override;
			virtual void deleteInput (key name) override;
			virtual void deleteConverter (key name, key newSourceName) override;
			virtual void deleteConverter (key name) override;
			virtual void deleteLoad (key name) override;
			virtual void deleteNode (key name, key newSourceName) override;
			virtual void deleteNode (key name) override;
			virtual void deleteSubnet (key headerName) override;
			virtual void deleteAllSinks (key sourceName) override;

			virtual void moveConverter (key name, key newSourceName) override;
			virtual void moveConverter (key name, key newSourceName, key newSinksSourceName) override;
			virtual void freeConverter (key name) override;
			virtual void freeConverter (key name, key newSinksSourceName) override;
			virtual void moveLoad (key name, key newSourceName) override;
			virtual void freeLoad (key name) override;
			virtual void moveSubnet (key headerName, key newSourceName) override;
			virtual void freeSubnet (key headerName) override;
			virtual void moveNode (key name, key newSourceName) override;
			virtual void moveNode (key name, key newSourceName, key newSinksSourceName) override;
			virtual void freeNode (key name) override;
			virtual void freeNode (key name, key newSinksSourceName) override;

			virtual void renameNode (key name, key newName) override;
			virtual void setSourceCvType (key name, VarKind newType) override;
			virtual void setSourceCvValue (key name, double value) override;
			virtual void setConverterType (key name, ConverterType type) override;
			virtual void setConverterEfficiency (key name, double efficiency) override;
			virtual void setLoadType (key name, LoadType type) override;
			virtual void setLoadResistance (key name, double resistance) override;
			virtual void setLoadCurrent (key name, double current) override;
			virtual void setLoadForawrdVoltage (key name, double forwardVoltage) override;
			virtual void setLoadForwardCurrent (key name, double forwardCurrent) override;

			virtual DeviceType getNodeType (key name) override;
			virtual InputData getInputData (key inputName) override;
			virtual ConverterData getConverterData (key converterName) override;
			virtual ResistiveLoadData getResistiveLoadData (key loadName) override;
			virtual ConstantCurrentLoadData getConstantCurrentLoadData (key loadName) override;
			virtual DiodeLoadData getDiodeLoadData (key loadName) override;
			virtual bool isLoadExsist (key name) override;
			virtual LoadType getLoadType (key name) override;

			virtual string getTitle () override;
			virtual void rename (string newTitle) override;

			virtual void calculte () override;
			virtual InputResults getInputResults (key inputName) const override;
			virtual ConverterResults getConverterResults (key convertertName) const override;
			virtual ResistiveLoadResults getResistiveLoadResults (key loadName) const override;
			virtual ConstantCurrentLoadResults getConstantCurrentLoadResults (key loadName) const override;
			virtual DiodeLoadResults getDiodeLoadResults (key loadName) const override;

			virtual void iterateAndExecuteForEach (function<void (key)> functor) override;




#pragma todo only debug
			bool operator != (const ElectricNet& second) const;



		private:

#pragma todo there is probably doubling with the data structures of specified nodes in the interface. It needs to eliminate that.
			struct ElectricNode
			{
				ElectricNode (DeviceType type);
				virtual ~ElectricNode ()    {;}

				const DeviceType type;
			};

			struct Source : ElectricNode
			{
				Source (DeviceType devType, VarKind type, double value);

				VarKind cvKind;
				double cvValue;

				double avValue = NAN;
			};

			struct Input : Source
			{
				Input (VarKind type, double value);
			};

			struct Converter : Source
			{
				Converter (VarKind cvType, double value, ConverterType type, double efficiency);

				ConverterType type;
				double efficiency;

				double inputValue = NAN;
			};

			struct Load : ElectricNode
			{
				Load (LoadType type);

				LoadType type;
			};

			struct OneParamLoad : Load
			{
				OneParamLoad (LoadType type, double param);

				double param;

				double inputValue = NAN;
			};
			
			struct TwoParamsLoad : Load
			{
				TwoParamsLoad (LoadType type, double firstParam, double secondParam);

				double mainParam;
				double secondaryParam;
			};


			using Node_ptr = shared_ptr<ElectricNode>;
			using ElectricForest = Forest< string, Node_ptr >;
			using Desc_it = ElectricForest::desces_group_iterator;


			
			string title;

			Forest< string, Node_ptr > net;


			bool isStoragedResultsActual = false;



			#pragma todo make const
			void updateCalculations ();
			double calculateAndUpdateGivenParams (Desc_it source_it);
			void writeAvValueToSource (double newAvValue, key sourceName);
			void writeInputValueToConverter (double newInputValue, key converterName);
			void writeInputValueToResistiveLoad (double newInputValue, key loadName);
			double calculateConsumption (Desc_it sink_it, Desc_it source_it);
			double reduceOutputToInput (Desc_it sink_it, Desc_it source_it);
			double calculateLoadConsumptionDrivenByVoltageSource (Desc_it load_it, Desc_it source_it);

			VarKind calcInputVarTypeByParent (key parentName) const;

			static bool isNodesEqual (Node_ptr first_ptr, Node_ptr second_ptr);

	};

}
