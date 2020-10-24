
#include <fstream>
#include <iostream>
#include <string>


#include "test_runner/test_runner.h"


#include "forest/forest.h"


#include "tests/tests.h"




void TestAll ()
{
	TestRunner tr;
	tr.RunTest(TestForest);
}




void TestForest ()
{

}
