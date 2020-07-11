
#pragma once


#include "config.h"




namespace commands
{
	const string quit_large_str = "quit";
	const string quit_small_str = "qt";



	const string create_large_str = "create";
	const string create_small_str = "c";

	const string load_large_str = "load";
	const string load_small_str = "ld";

	const string save_large_str = "save";
	const string save_small_str = "sv";



	const string solve_large_str = "solve";
	const string solve_small_str = "slv";

	const string show_results_large_str = "show_results";
	const string show_results_small_str = "sr";

	const string show_tree_large_str = "show_tree";
	const string show_tree_small_str = "st";



	const string create_input_large_str = "create_input";
	const string create_input_small_str = "ci";

	const string delete_input_large_str = "delete_input";
	const string delete_input_small_str = "di";

	const string set_input_params_large_str = "set_input_params";
	const string set_input_params_small_str = "si";

	const string create_converter_large_str = "create_converter";
	const string create_converter_small_str = "cc";

	const string delete_converter_large_str = "delete_converter";
	const string delete_converter_small_str = "dc";

	const string set_converter_params_large_str = "set_converter_params";
	const string set_converter_params_small_str = "sc";

	const string create_load_large_str = "create_load";
	const string create_load_small_str = "cl";

	const string delete_load_large_str = "delete_load";
	const string delete_load_small_str = "dl";

	const string set_load_params_large_str = "set_load_params";
	const string set_load_params_small_str = "sl";

	const string move_large_str = "move";
	const string move_small_str = "mv";
};

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
