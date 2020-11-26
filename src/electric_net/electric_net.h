
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
			
			using key = string;

			
			
			ElectricNet (string name);



			virtual void addInput (key name, CvType type = CvType::VOLTAGE, double cvValue = 0.0) override;
			virtual void addConverter (key name, key sourceName, ConverterType type = ConverterType::PULSE, CvType cvType = CvType::VOLTAGE, 
							          double cvValue = 0.0, double efficiency = 100.0) override;
			virtual void addConverter (key name, ConverterType type = ConverterType::PULSE, CvType cvType = CvType::VOLTAGE, double cvValue = 0.0,
							           double efficiency = 100.0) override;
			virtual void insertConverter (key name, key sourceName, ConverterType type = ConverterType::PULSE, CvType cvType = CvType::VOLTAGE, 
							              double cvValue = 0.0, double efficiency = 100.0) override;
			virtual void insertConverter (key name, key sourceName, key sinkName, ConverterType type = ConverterType::PULSE, 
								          CvType cvType = CvType::VOLTAGE, double cvValue = 0.0, double efficiency = 100.0) override;
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
			virtual void setSourceCvType (key name, CvType newType) override;
			virtual void setSourceCvValue (key name, double value) override;
			virtual void setConverterType (key name, ConverterType type) override;
			virtual void setConverterEfficiency (key name, double efficiency) override;
			virtual void setLoadType (key name, LoadType type) override;
			virtual void setLoadResistance (key name, double resistance) override;
			virtual void setLoadCurrent (key name, double current) override;
			virtual void setLoadForawrdVoltage (key name, double forwardVoltage) override;
			virtual void setLoadForwardCurrent (key name, double forwardCurrent) override;
			virtual void setLoadNomPower (key name, double nomPower) override;
			virtual void setLoadNomVoltage (key name, double nomVoltage) override;

			virtual DeviceType getNodeType (key name) override;
			virtual bool isLoadExsist (key name) override;
			virtual LoadType getLoadType (key name) override;

			virtual string getTitle () override;
			virtual void rename (string newTitle) override;

			virtual void calculte () const override;
			virtual TreeStructure getStructure () const override;




		private:
				
			struct ElectricNode
			{
				ElectricNode (DeviceType type);
				virtual ~ElectricNode ()    {;}

				const DeviceType type;
			};

			struct Source : ElectricNode
			{
				Source (DeviceType devType, CvType type, double value);

				CvType type;
				double cvValue;
			};

			struct Input : Source
			{
				Input (CvType type, double value);
			};

			struct Converter : Source
			{
				Converter (CvType cvType, double value, ConverterType type, double efficiency);

				ConverterType type;
				double efficiency;
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
			};
			
			struct TwoParamLoad : Load
			{
				TwoParamLoad (LoadType type, double firstParam, double secondParam);

				double mainParam;
				double secondaryParam;
			};


			
			string title;

			Forest< string, shared_ptr<ElectricNode> > net;

	};

}
