
#include <fstream>
#include <iostream>
#include <string>


#include "test_runner/test_runner.h"


#include "forest/forest.h"
#include "electric_net/electric_net_if.h"
#include "lib/ciflib.h"


#include "tests/tests.h"




void TestAll ()
{
	TestRunner tr;
	tr.RunTest(TestForest);
	tr.RunTest(TestApiCommonFunctions);
	tr.RunTest(TestLibrary);
}


void TestForest ()
{

}




void TestIsConverterTypeChecker ()
{
	using namespace electric_net;

	{
		string testingValue = "l";
		bool result = isConverterTypeString(testingValue);
		Assert(result, "l");
	}

	{
		string testingValue = "L";
		bool result = isConverterTypeString(testingValue);
		Assert(result, "L");
	}

	{
		string testingValue = "lin";
		bool result = isConverterTypeString(testingValue);
		Assert(result, "lin");
	}

	{
		string testingValue = "Lin";
		bool result = isConverterTypeString(testingValue);
		Assert(result, "Lin");
	}

	{
		string testingValue = "linear";
		bool result = isConverterTypeString(testingValue);
		Assert(result, "linear");
	}

	{
		string testingValue = "Linear";
		bool result = isConverterTypeString(testingValue);
		Assert(result, "Linear");
	}

	{
		string testingValue = "p";
		bool result = isConverterTypeString(testingValue);
		Assert(result, "p");
	}

	{
		string testingValue = "P";
		bool result = isConverterTypeString(testingValue);
		Assert(result, "P");
	}

	{
		string testingValue = "pul";
		bool result = isConverterTypeString(testingValue);
		Assert(result, "pul");
	}

	{
		string testingValue = "Pul";
		bool result = isConverterTypeString(testingValue);
		Assert(result, "Pul");
	}

	{
		string testingValue = "pulse";
		bool result = isConverterTypeString(testingValue);
		Assert(result, "pulse");
	}

	{
		string testingValue = "Pulse";
		bool result = isConverterTypeString(testingValue);
		Assert(result, "Pulse");
	}

	{
		string testingValue = "inc";
		bool result = isConverterTypeString(testingValue);
		Assert(!result, "not converter type");
	}

	{
		string testingValue = "li";
		bool result = isConverterTypeString(testingValue);
		Assert(!result, "not converter type");
	}

	{
		string testingValue = "Li";
		bool result = isConverterTypeString(testingValue);
		Assert(!result, "not converter type");
	}

	{
		string testingValue = "linu";
		bool result = isConverterTypeString(testingValue);
		Assert(!result, "not converter type");
	}

	{
		string testingValue = "Linu";
		bool result = isConverterTypeString(testingValue);
		Assert(!result, "not converter type");
	}

	{
		string testingValue = "lineari";
		bool result = isConverterTypeString(testingValue);
		Assert(!result, "not converter type");
	}

	{
		string testingValue = "Lineari";
		bool result = isConverterTypeString(testingValue);
		Assert(!result, "not converter type");
	}

	{
		string testingValue = "pa";
		bool result = isConverterTypeString(testingValue);
		Assert(!result, "not converter type");
	}

	{
		string testingValue = "Pa";
		bool result = isConverterTypeString(testingValue);
		Assert(!result, "not converter type");
	}

	{
		string testingValue = "pull";
		bool result = isConverterTypeString(testingValue);
		Assert(!result, "not converter type");
	}

	{
		string testingValue = "Pull";
		bool result = isConverterTypeString(testingValue);
		Assert(!result, "not converter type");
	}

	{
		string testingValue = "pulsee";
		bool result = isConverterTypeString(testingValue);
		Assert(!result, "not converter type");
	}

	{
		string testingValue = "Pulsee";
		bool result = isConverterTypeString(testingValue);
		Assert(!result, "not converter type");
	}
}


void TestConverterTypeFunctions ()
{
	TestIsConverterTypeChecker();
}


void TestApiCommonFunctions ()
{
	TestConverterTypeFunctions();
}




void TestStrToDouble ()
{
	{
		string input("0.0");
		double result = strToDouble(input);
		AssertEqual(result, 0, input);
	}

	{
		string input("0");
		double result = strToDouble(input);
		AssertEqual(result, 0, input);
	}

	{
		string input("1");
		double result = strToDouble(input);
		AssertEqual(result, 1, input);
	}

	{
		string input("19028");
		double result = strToDouble(input);
		AssertEqual(result, 19028, input);
	}

	{
		string input("1.0");
		double result = strToDouble(input);
		AssertEqual(result, 1, input);
	}

	{
		string input("19028.0");
		double result = strToDouble(input);
		AssertEqual(result, 19028, input);
	}

	{
		string input("-1");
		double result = strToDouble(input);
		AssertEqual(result, -1, input);
	}

	{
		string input("-19028");
		double result = strToDouble(input);
		AssertEqual(result, -19028, input);
	}

	{
		string input("-1.0");
		double result = strToDouble(input);
		AssertEqual(result, -1, input);
	}

	{
		string input("-19028.0");
		double result = strToDouble(input);
		AssertEqual(result, -19028, input);
	}

	{
		string input("245.873");
		double result = strToDouble(input);
		AssertEqual(result, 245.873, input);
	}

	{
		string input("-245.873");
		double result = strToDouble(input);
		AssertEqual(result, -245.873, input);
	}
}


void TestCapitalize ()
{
	{
		string input("");
		string result = capitalize(input);
		AssertEqual(string(""), result, input);
	}

	{
		string input(" ");
		string result = capitalize(input);
		AssertEqual(string(" "), result, input);
	}

	{
		string input("a e");
		string result = capitalize(input);
		AssertEqual(string("A e"), result, input);
	}

	{
		string input("A e");
		string result = capitalize(input);
		AssertEqual(string("A e"), result, input);
	}

	{
		string input("a");
		string result = capitalize(input);
		AssertEqual(string("A"), result, input);
	}

	{
		string input("A");
		string result = capitalize(input);
		AssertEqual(string("A"), result, input);
	}

	{
		string input("abs");
		string result = capitalize(input);
		AssertEqual(string("Abs"), result, input);
	}

	{
		string input("Abs");
		string result = capitalize(input);
		AssertEqual(string("Abs"), result, input);
	}

	{
		string input("abs ne");
		string result = capitalize(input);
		AssertEqual(string("Abs ne"), result, input);
	}

	{
		string input("Abs ne");
		string result = capitalize(input);
		AssertEqual(string("Abs ne"), result, input);
	}

	{
		string input("Abs Ne");
		string result = capitalize(input);
		AssertEqual(string("Abs Ne"), result, input);
	}




	{
		string input("");
		string result = capitalize(input, false);
		AssertEqual(input, result, input);
	}

	{
		string input(" ");
		string result = capitalize(input, false);
		AssertEqual(input, result, input);
	}

	{
		string input("a e");
		string result = capitalize(input, false);
		AssertEqual(input, result, input);
	}

	{
		string input("A e");
		string result = capitalize(input, false);
		AssertEqual(input, result, input);
	}

	{
		string input("a");
		string result = capitalize(input, false);
		AssertEqual(input, result, input);
	}

	{
		string input("A");
		string result = capitalize(input, false);
		AssertEqual(input, result, input);
	}

	{
		string input("abs");
		string result = capitalize(input, false);
		AssertEqual(input, result, input);
	}

	{
		string input("Abs");
		string result = capitalize(input, false);
		AssertEqual(input, result, input);
	}

	{
		string input("abs ne");
		string result = capitalize(input, false);
		AssertEqual(input, result, input);
	}

	{
		string input("Abs ne");
		string result = capitalize(input, false);
		AssertEqual(input, result, input);
	}

	{
		string input("Abs Ne");
		string result = capitalize(input, false);
		AssertEqual(input, result, input);
	}
}


void TestLibrary ()
{
	TestStrToDouble();
	TestCapitalize();
}
