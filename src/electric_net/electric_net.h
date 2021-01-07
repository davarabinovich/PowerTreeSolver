
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



			virtual void addInput (Key name, VarKind type = VarKind::VOLTAGE, double cvValue = 0.0) override;
			virtual void addConverter (Key name, Key sourceName, ConverterType type = ConverterType::PULSE, VarKind cvType = VarKind::VOLTAGE, 
							          double cvValue = 0.0, double efficiency = 100.0) override;
			virtual void addConverter (Key name, ConverterType type = ConverterType::PULSE, VarKind cvType = VarKind::VOLTAGE, double cvValue = 0.0,
							           double efficiencyParam = 100.0) override;
			virtual void insertConverter (Key name, Key sourceName, ConverterType type = ConverterType::PULSE, VarKind cvType = VarKind::VOLTAGE, 
							              double cvValue = 0.0, double efficiency = 100.0) override;
			virtual void insertConverter (Key name, Key sourceName, Key sinkName, ConverterType type = ConverterType::PULSE, 
								          VarKind cvType = VarKind::VOLTAGE, double cvValue = 0.0, double efficiency = 100.0) override;
			virtual void addLoad (Key name, Key sourceName, LoadType type, double param) override;
			virtual void addLoad (Key name, LoadType type, double param) override;
			virtual void addLoad (Key name, Key sourceName, LoadType type, double mainParam, double secondaryParam) override;
			virtual void addLoad (Key name, LoadType type, double mainParam, double secondaryParam) override;

			virtual void deleteInput (Key name, Key newSourceName) override;
			virtual void deleteInput (Key name) override;
			virtual void deleteConverter (Key name, Key newSourceName) override;
			virtual void deleteConverter (Key name) override;
			virtual void deleteLoad (Key name) override;
			virtual void deleteNode (Key name, Key newSourceName) override;
			virtual void deleteNode (Key name) override;
			virtual void deleteSubnet (Key headerName) override;
			virtual void deleteAllSinks (Key sourceName) override;

			virtual void moveConverter (Key name, Key newSourceName) override;
			virtual void moveConverter (Key name, Key newSourceName, Key newSinksSourceName) override;
			virtual void freeConverter (Key name) override;
			virtual void freeConverter (Key name, Key newSinksSourceName) override;
			virtual void moveLoad (Key name, Key newSourceName) override;
			virtual void freeLoad (Key name) override;
			virtual void moveSubnet (Key headerName, Key newSourceName) override;
			virtual void freeSubnet (Key headerName) override;
			virtual void moveNode (Key name, Key newSourceName) override;
			virtual void moveNode (Key name, Key newSourceName, Key newSinksSourceName) override;
			virtual void freeNode (Key name) override;
			virtual void freeNode (Key name, Key newSinksSourceName) override;

			virtual void renameNode (Key name, Key newName) override;
			virtual void setSourceCvType (Key name, VarKind newType) override;
			virtual void setSourceCvValue (Key name, double value) override;
			virtual void setConverterType (Key name, ConverterType type) override;
			virtual void setConverterEfficiencyParam (Key name, double efficiencyParam) override;
			virtual void setLoadType (Key name, LoadType type) override;
			virtual void setLoadResistance (Key name, double resistance) override;
			virtual void setLoadCurrent (Key name, double current) override;
			virtual void setLoadForawrdVoltage (Key name, double forwardVoltage) override;
			virtual void setLoadForwardCurrent (Key name, double forwardCurrent) override;

			virtual DeviceType getNodeType (Key name) const override;
			virtual InputData getInputData (Key inputName) const override;
			virtual ConverterData getConverterData (Key converterName) const override;
			virtual ResistiveLoadData getResistiveLoadData (Key loadName) const override;
			virtual ConstantCurrentLoadData getConstantCurrentLoadData (Key loadName) const override;
			virtual DiodeLoadData getDiodeLoadData (Key loadName) const override;
			virtual VarKind getConverterVarKind (Key converterName) const override;
			virtual bool isLoadExsist (Key name) const override;
			virtual LoadType getLoadType (Key name) const override;

			virtual string getTitle () const override;
			virtual void rename (string newTitle) override;

			virtual void calculte () const override;
			virtual InputResults getInputResults (Key inputName) const override;
			virtual ConverterResults getConverterResults (Key convertertName) const override;
			virtual ResistiveLoadResults getResistiveLoadResults (Key loadName) const override;
			virtual ConstantCurrentLoadResults getConstantCurrentLoadResults (Key loadName) const override;
			virtual DiodeLoadResults getDiodeLoadResults (Key loadName) const override;

			virtual void iterateAndExecuteForEach (function<void (Key)> functor) const override;

			virtual Key getDefaultNodeName (DeviceType type) const override;

#ifdef DEBUG
			bool operator != (const ElectricNet & other) const;
#endif




		private:

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
				double efficiencyParam;

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
			using ConstDesc_it = ElectricForest::const_desces_group_iterator;


			
			string title;

			Forest< string, Node_ptr > net;


			mutable bool isStoragedResultsActual = false;

			mutable unsigned actualDefaultInputNumber = 1;
			mutable unsigned actualDefaultConverterNumber = 1;
			mutable unsigned actualDefaultLoadNumber = 1;



			void updateCalculations () const;
			double calculateAndUpdateGivenParams (ConstDesc_it source_it) const;
			void writeAvValueToSource (double newAvValue, Key sourceName) const;
			void writeInputValueToConverter (double newInputValue, Key converterName) const;
			void writeInputValueToResistiveLoad (double newInputValue, Key loadName) const;
			double calculateConsumption (ConstDesc_it sink_it, ConstDesc_it source_it) const;
			double reduceOutputToInput (ConstDesc_it sink_it, ConstDesc_it source_it) const;
			double calculateLoadConsumptionDrivenByVoltageSource (ConstDesc_it load_it, ConstDesc_it source_it) const;

			VarKind calcInputVarKindByParent (Key parentName) const;

#ifdef DEBUG
			static bool isNodesEqualByContent (Node_ptr first_ptr, Node_ptr second_ptr);
#endif

	};

}
