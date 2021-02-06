
#pragma once


#include <string>
#include <vector>
#include <map>
#include <set>

#include <iostream>
#include <sstream>




using namespace std;




template <typename Type>
ostream & operator << (ostream & os, pair<Type, Type> p)
{
	os << "[" << p.first << " / " << p.second << "]";
	return os;
}


template <class T>
ostream& operator << (ostream& os, const vector<T>& s) {
  os << "{";
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << x;
  }
  return os << "}";
}


template <class T>
ostream& operator << (ostream& os, const set<T>& s) {
  os << "{";
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << x;
  }
  return os << "}";
}


template <class K, class V>
ostream& operator << (ostream& os, const map<K, V>& m) {
  os << "{";
  bool first = true;
  for (const auto& kv : m) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << kv.first << ": " << kv.second;
  }
  return os << "}";
}




template<class T, class U>
void AssertEqual(const T& t, const U& u, const string& hint = {}) {
  if (t != u) {
    ostringstream os;
    os << "Assertion failed: " << t << " != " << u;
    if (!hint.empty()) {
       os << " hint: " << hint;
    }
    throw runtime_error(os.str());
  }
}


template<class T, class U>
void AssertEqual_not_reporting(const T& t, const U& u, const string& hint = {}) {
	if (t != u) {
		ostringstream os;
		os << "Assertion failed";
		if (!hint.empty()) {
			os << ":  hint: " << hint;
		}
		throw runtime_error(os.str());
	}
}


void Assert(bool b, const string& hint);



class TestRunner 
{
public:
  template <class TestFunc>
  void RunTest(TestFunc func, const string& test_name = "") {
    try {
      func();
      cerr << test_name << " OK" << endl;
    } catch (exception& e) {
      ++fail_count;
      cerr << test_name << " fail: " << e.what() << endl;
    } catch (...) {
      ++fail_count;
      cerr << "Unknown exception caught" << endl;
    }
  }

  ~TestRunner();

private:
  int fail_count = 0;
};




class IoBufsKeeper
{
public:
	static void retargetIoBuffers(string cinContent);
	static void restoreIoBuffers();

	static void retargetCin(string newContent);
	static void restoreCin();

	static void retargetCout();
	static void restoreCout();


private:
	static bool isCinStored;
	static bool isCoutStored;
	
	static stringstream* tempCin;
	static streambuf* cinStorage;

	static stringstream* tempCout;
	static streambuf* coutStorage;
};
