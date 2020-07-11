
#include "commands.h"




using namespace deleting_modes_keys;
using namespace inputs_parameters_keys;
using namespace converters_types_keys;
using namespace loads_types_keys;




void Solve ();
void ShowResults ();
void ShowTree ();
void CreateTree (string name);
void CreateInput (string name, string voltage_str, string maxCurrent_str);
void DeleteInput (string name);
void DeleteInput (string name, string newParentName);
void SetInputVoltage (string name, string voltage_str);
void SetInputMaxCurrent (string name, string maxCurrent_str);
void CreateConverter (string name, string type_key, string outVoltage_str, string maxCurrent_str);
void CreateConverter (string name, string type_key, string outVoltage_str, string maxCurrent_str, string efficiency_str);
void CreateLoad (string name, string type_key, string resistance_str, string maxPowerDissipation_str);
void CreateLoad (string name, string type_key, string current_str);
void CreateLoad (string name, string type_key, string power_str, string nomVoltage_str);




bool isCommandToQuit(string command)
{
	if (command == commands::quit_large_str || command == commands::quit_small_str)    return true;
	return false;
}

void executeCommand(string command)
{
	using namespace commands;


	if (command == quit_large_str || command == quit_small_str)    return;


	if (command == load_large_str || command == load_small_str)
	{
		// Что делать с текущим?
		return;
	}
	if (command == save_large_str || command == save_small_str)
	{
		return;
	}

	if (command == solve_large_str || command == solve_small_str)
	{
		Solve();
		ShowResults();
		return;
	}
	if (command == show_results_large_str || command == show_results_small_str)
	{
		ShowResults();
		return;
	}

	if (command == show_tree_large_str || command == show_tree_small_str)
	{
		ShowTree();
		return;
	}

	if (command == create_large_str || command == create_small_str)
	{
#pragma todo what to do whit the current?

		string name;
		cin >> name;
		CreateTree(name);
		return;
	}

	if (command == create_input_large_str || command == create_input_small_str)
	{
		string name, voltage_str, maxCurrent_str;
		cin >> name >> voltage_str >> maxCurrent_str;
		CreateInput(name, voltage_str, maxCurrent_str);
		return;
	}

	if (command == delete_input_large_str || command == delete_input_small_str)
	{
		string name, deletingMode_str;
		cin >> name >> deletingMode_str;
		
		if (deletingMode_str == delete_descendants_str)
			DeleteInput(name);
		else if (deletingMode_str == reassign_descendants_str)
		{
			string newParentName;
			cin >> newParentName;
			DeleteInput(name, newParentName);
		}
		else throw exception("Unrecognized deleting mode");

		return;
	}

	if (command == set_input_params_large_str || command == set_input_params_small_str)
	{
		string name, parameter_str;
		cin >> name, parameter_str;
		
		if (parameter_str == voltage_key)
		{
			string voltage_str;
			cin >> voltage_str;
			SetInputVoltage(name, voltage_str);
		}
		else if (parameter_str == max_current_key)
		{
			string maxCurrent_str;
			cin >> maxCurrent_str;
			SetInputMaxCurrent(name, maxCurrent_str);
		}
		else throw exception("Unrecognized inputs parameter");

		return;
	}

	if (command == create_converter_large_str || command == create_converter_small_str)
	{
		string name, type_str, outVoltage_str, maxCurrent_str, efficiency_str;
		cin >> name >> type_str >> outVoltage_str >> maxCurrent_str >> efficiency_str;
		CreateConverter(name, pulse_key, outVoltage_str, maxCurrent_str, efficiency_str);

		return;
	}

	if (command == delete_converter_large_str || command == delete_converter_small_str)
	{
		string name, deletingMode_str;
		cin >> name >> deletingMode_str;

		if (deletingMode_str == delete_descendants_str)
			DeleteInput(name);
		else if (deletingMode_str == reassign_descendants_str)
		{
			string newParentName;
			cin >> newParentName;
			DeleteInput(name, newParentName);
		}
		else throw exception("Unrecognized deleting mode");

		return;
	}

	if (command == set_converter_params_large_str || command == set_converter_params_small_str)
	{
		string name, parameter_str;
		cin >> name, parameter_str;

		if (parameter_str == outVoltage_key)
		{
			string outVoltage_str;
			cin >> outVoltage_str;
			SetConverterOutVoltage(name, outVoltage_str);
		}
		else if (parameter_str == max_current_key)
		{
			string maxCurrent_str;
			cin >> maxCurrent_str;
			SetConverterMaxCurrent(name, maxCurrent_str);
		}
		else if (parameter_str == efficiency_key)
		{
			string efficiency_str;
			cin >> efficiency_str;
			SetConverterEfficiency(name, efficiency_str);
		}
		else throw exception("Unrecognized converters parameter");

		return;
	}

	if (command == create_load_large_str || command == create_load_small_str)
	{
		string name, type_str;
		cin >> name >> type_str;

		if (type_str == resistive_key)
		{
			string resistance_str, maxPowerDissipation_str;
			cin >> resistance_str, maxPowerDissipation_str;
			CreateLoad(name, resistive_key, resistance_str, maxPowerDissipation_str);
		}
		else if (type_str == current_key)
		{
			string current_str;
			cin >> current_str;
			CreateLoad(name, current_key, current_str);
		}
		else if (type_str == power_key)
		{
			string power_str, nomVoltage;
			cin >> power_str, nomVoltage;
			CreateLoad(name, power_key, power_str, nomVoltage);
		}
		else throw exception("Invalid type of load");

		return;
	}

	if (command == delete_load_large_str || command == delete_load_small_str)
	{
		string name;
		cin >> name;
		DeleteLoad(name);
		return;
	}

	if (command == set_load_params_large_str || command == set_load_params_small_str)
	{

		return;
	}

	if (command == move_large_str || command == move_small_str)
	{

		return;
	}


	throw exception("Unrecognized command");
}




void Solve ()
{

}

void ShowResults ()
{

}

void ShowTree ()
{

}

void CreateTree(string name)
{

}

void CreateInput(string name, string voltage_str, string maxCurrent_str)
{

}

void DeleteInput(string name)
{

}

void DeleteInput(string name, string newParentName)
{

}

void SetInputVoltage(string name, string voltage_str)
{

}

void SetInputMaxCurrent(string name, string maxCurrent_str)
{

}

void CreateConverter(string name, string type_key, string outVoltage_str, string maxCurrent_str)
{

}

void CreateConverter(string name, string type_key, string outVoltage_str, string maxCurrent_str, string efficiency_str)
{

}

void CreateLoad(string name, string type_key, string resistance_str, string maxPowerDissipation_str)
{

}

void CreateLoad(string name, string type_key, string current_str)
{

}
