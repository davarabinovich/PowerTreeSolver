
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>


#include "test_runner/test_runner.h"


#include "forest/forest.h"
#include "file_server/file_server.h"
#include "electric_net/electric_net_if.h"
#include "electric_net/electric_net.h"
#include "lib/ciflib.h"


#include "tests/tests.h"




void TestAll ()
{
	TestRunner tr;
	tr.RunTest(TestForest);
	tr.RunTest(TestApiCommonFunctions);
	tr.RunTest(TestLibrary);
	tr.RunTest(IntegrationTests);
}


void TestIterating ()
{
	struct TestStr
	{
		TestStr() {;}
		TestStr (int gen_a, int gen_b) : a(gen_a), b(gen_b) {;}

		int a;
		int b;
	};

	using TestForest = Forest<string, TestStr>;

	TestStr a(1, 2);
	TestStr b(1, 3);
	TestStr c(1, 4);
	TestForest forest;
	forest.addRoot("a", a);
	forest.addRoot("b", b);
	forest.addRoot("c", c);

	TestForest::desces_group_iterator it = forest.dgbegin();
	it++; it++;
	auto & ref = (*it).second;
	ref.a = 33;
}


void TestForest ()
{
	TestIterating();
}




void TestIsVarKindChecker ()
{
	using namespace electric_net;

	//// Positive cases ////
	{
		string testingValue = "c";
		bool result = isVarKindString(testingValue);
		Assert(result, testingValue);
	}

	{
		string testingValue = "C";
		bool result = isVarKindString(testingValue);
		Assert(result, testingValue);
	}

	{
		string testingValue = "cur";
		bool result = isVarKindString(testingValue);
		Assert(result, testingValue);
	}

	{
		string testingValue = "Cur";
		bool result = isVarKindString(testingValue);
		Assert(result, testingValue);
	}

	{
		string testingValue = "current";
		bool result = isVarKindString(testingValue);
		Assert(result, testingValue);
	}

	{
		string testingValue = "Current";
		bool result = isVarKindString(testingValue);
		Assert(result, testingValue);
	}

	{
		string testingValue = "v";
		bool result = isVarKindString(testingValue);
		Assert(result, testingValue);
	}

	{
		string testingValue = "V";
		bool result = isVarKindString(testingValue);
		Assert(result, testingValue);
	}

	{
		string testingValue = "vol";
		bool result = isVarKindString(testingValue);
		Assert(result, testingValue);
	}

	{
		string testingValue = "Vol";
		bool result = isVarKindString(testingValue);
		Assert(result, testingValue);
	}

	{
		string testingValue = "voltage";
		bool result = isVarKindString(testingValue);
		Assert(result, testingValue);
	}

	{
		string testingValue = "Voltage";
		bool result = isVarKindString(testingValue);
		Assert(result, testingValue);
	}


	//// Negative cases ////
	{
		string testingValue = "ñ";
		bool result = isVarKindString(testingValue);
		Assert(!result, testingValue);
	}

	{
		string testingValue = "Ñ";
		bool result = isVarKindString(testingValue);
		Assert(!result, testingValue);
	}

	{
		string testingValue = "cu";
		bool result = isVarKindString(testingValue);
		Assert(!result, testingValue);
	}

	{
		string testingValue = "Cu";
		bool result = isVarKindString(testingValue);
		Assert(!result, testingValue);
	}

	{
		string testingValue = "cure";
		bool result = isVarKindString(testingValue);
		Assert(!result, testingValue);
	}

	{
		string testingValue = "Cure";
		bool result = isVarKindString(testingValue);
		Assert(!result, testingValue);
	}

	{
		string testingValue = "vo";
		bool result = isVarKindString(testingValue);
		Assert(!result, testingValue);
	}

	{
		string testingValue = "Vo";
		bool result = isVarKindString(testingValue);
		Assert(!result, testingValue);
	}

	{
		string testingValue = "volt";
		bool result = isVarKindString(testingValue);
		Assert(!result, testingValue);
	}

	{
		string testingValue = "Volt";
		bool result = isVarKindString(testingValue);
		Assert(!result, testingValue);
	}

	{
		string testingValue = "sdf";
		bool result = isVarKindString(testingValue);
		Assert(!result, testingValue);
	}
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


void TestVarKindFunctions ()
{
	TestIsVarKindChecker();
}


void TestConverterTypeFunctions ()
{
	TestIsConverterTypeChecker();
}


void TestApiCommonFunctions ()
{
	TestVarKindFunctions();
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




using namespace electric_net;


extern shared_ptr<ElectricNet> readTreeFromFile (string name, string path);
extern void writeTreeToFile (string name, string path, shared_ptr<ElectricNet> source);
extern void resetTree ();


void IntegrationTestFileServer ()
{
	using namespace file_server;



	IoBufsKeeper::retargetIoBuffers("n");



	try
	{
		string fileToBeRead = "test_storage";
		string pathToFileToBeRead = "src\\tests";
		auto readTree = readTreeFromFile(fileToBeRead, pathToFileToBeRead);

		string buffer = "checking_storage";
		string pathToBuffer = "src\\tests";
		writeTreeToFile(buffer, pathToBuffer, readTree);

		auto writenTree = readTreeFromFile(buffer, pathToBuffer);
	
		
		AssertEqual_not_reporting(*readTree, *writenTree);
	}
	catch (exception & ex) { cerr << ex.what(); }



	resetTree();

	IoBufsKeeper::restoreIoBuffers();

	remove("src\\tests\\intermediary_storage");
	remove("src\\tests\\checking_storage");
}


void IntegrationTests ()
{
	IntegrationTestFileServer();
}
