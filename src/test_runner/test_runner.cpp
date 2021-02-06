
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








bool IoBufsKeeper::isCinStored = false;
bool IoBufsKeeper::isCoutStored = false;

stringstream* IoBufsKeeper::tempCin = nullptr;
streambuf* IoBufsKeeper::cinStorage = nullptr;

stringstream* IoBufsKeeper::tempCout = nullptr;
streambuf* IoBufsKeeper::coutStorage = nullptr;


void IoBufsKeeper::retargetIoBuffers(string cinContent)
{
	retargetCin(cinContent);
	retargetCout();
}


void IoBufsKeeper::restoreIoBuffers()
{
	restoreCin();
	restoreCout();
}


void IoBufsKeeper::retargetCin(string newContent)
{
	isCinStored = true;
	cinStorage = cin.rdbuf();

	tempCin = new stringstream;
	*tempCin << newContent;
	cin.rdbuf(tempCin->rdbuf());
}

void IoBufsKeeper::restoreCin()
{
	delete tempCin;
	cin.rdbuf(cinStorage);
	isCinStored = false;
}

void IoBufsKeeper::retargetCout()
{
	isCoutStored = true;
	coutStorage = cout.rdbuf();

	tempCout = new stringstream;
	cout.rdbuf(tempCout->rdbuf());
}

void IoBufsKeeper::restoreCout()
{
	delete tempCout;
	cout.rdbuf(coutStorage);
	isCoutStored = false;
}
