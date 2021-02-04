
#include "test_runner/test_runner.h"




void Assert(bool b, const string& hint) {
    AssertEqual(b, true, hint);
}




TestRunner::~TestRunner() {
    if (fail_count > 0) {
        cerr << fail_count << " unit tests failed. Terminate" << endl;
        exit(1);
    }
}








bool StandardIoBuffersKeeper::isCinStored = false;
bool StandardIoBuffersKeeper::isCoutStored = false;

stringstream* StandardIoBuffersKeeper::tempCin = nullptr;
streambuf* StandardIoBuffersKeeper::cinStorage = nullptr;

stringstream* StandardIoBuffersKeeper::tempCout = nullptr;
streambuf* StandardIoBuffersKeeper::coutStorage = nullptr;


void StandardIoBuffersKeeper::retargetIoBuffers(string cinContent)
{
	retargetCin(cinContent);
	retargetCout();
}


void StandardIoBuffersKeeper::restoreIoBuffers()
{
	restoreCin();
	restoreCout();
}


void StandardIoBuffersKeeper::retargetCin(string newContent)
{
	isCinStored = true;
	cinStorage = cin.rdbuf();

	tempCin = new stringstream;
	*tempCin << newContent;
	cin.rdbuf(tempCin->rdbuf());
}

void StandardIoBuffersKeeper::restoreCin()
{
	delete tempCin;
	cin.rdbuf(cinStorage);
	isCinStored = false;
}

void StandardIoBuffersKeeper::retargetCout()
{
	isCoutStored = true;
	coutStorage = cin.rdbuf();

	tempCout = new stringstream;
	cout.rdbuf(tempCout->rdbuf());
}

void StandardIoBuffersKeeper::restoreCout()
{
	delete tempCout;
	cout.rdbuf(coutStorage);
	isCoutStored = false;
}

