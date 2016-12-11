#include <iostream>
#include <string>
#include "ConfigMgr.h"

int main() {
	ConfigManager mgr;
	mgr.readConfigFile("my.cfg");

	std::string getStringVal;
	int getIntVal;
	double getDoubleVal;

	mgr.getVal(STRING_VAL, "section1", "stringVal", getStringVal);
	std::cout << getStringVal << std::endl;

	mgr.getVal(INT_VAL, "section1", "intVal", getIntVal);
	std::cout << getIntVal << std::endl;

	mgr.getVal(DOUBLE_VAL, "section1", "dVal", getDoubleVal);
	std::cout << getDoubleVal << std::endl;

	mgr.getVal(STRING_VAL, "section2", "anotherVal", getStringVal);
	std::cout << getStringVal << std::endl;

	return 0;
}
