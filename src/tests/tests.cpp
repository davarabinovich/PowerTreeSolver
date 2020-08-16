
#include "test_runner/test_runner.h"


#include "tests/tests.h"




void TestCommandDialogs ();

void TestCreate ();
void TestCreateArgumentsParsing ();




void TestAll ()
{
	TestRunner tr;
}




void TestCommandDialogs ()
{
	TestCreate();
}

void TestCreate ()
{
	TestCreateArgumentsParsing();
}
