
#pragma once


#include <map>
#include "config.h"




using command_mnemonic = const string;



enum class CvType { VOLTAGE, CURRENT };

const string default_tree_name = "Tree1";
const string default_input_name = "Input";

const CvType default_input_cv_type = CvType::VOLTAGE;





namespace deleting_modes_keys
{
	const string delete_descendants_str   = "del";
	const string reassign_descendants_str = "asg";
};

namespace inputs_parameters_keys
{
	const string voltage_key = "v";
	const string max_current_key = "mc";
}

namespace converters_types_keys
{
	const string  pulse_key = "p";
	const string linear_key = "p";
};

namespace loads_types_keys
{
	const string resistive_key = "r";
	const string   current_key = "c";
	const string     power_key = "p";
};




bool isCommandToQuit (string );
void executeCommand (string );
