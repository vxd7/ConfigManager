#include "ConfigMgr.h"

#include <fstream>
#include <string>
#include <vector>

#include <algorithm>
#include <cctype>

/*******************
 * TOKEN FUNCTIONS *
 *******************/

Token::Token() { }
Token::Token(const std::string &tokenSection, const std::string &tokenID, const std::string tokenStringData) {
	type = STRING_VAL;

	section = tokenSection;
	id = tokenID;

	stringData = tokenStringData;
}

Token::Token(const std::string &tokenSection, const std::string &tokenID, const double tokenDoubleData) {
	type = DOUBLE_VAL;

	section = tokenSection;
	id = tokenID;

	doubleData = tokenDoubleData;
}

Token::Token(const std::string &tokenSection, const std::string &tokenID, const int tokenIntegerData) {
	type = INT_VAL;

	section = tokenSection;
	id = tokenID;

	integerData = tokenIntegerData;
}

std::string Token::getTokenSection() {
	return section;
}

std::string Token::getTokenID() {
	return id;
}

TOKENTYPE Token::getTokenType() {
	return type;
}

void Token::getTokenData(std::string &readData) {
	readData = stringData;
}
void Token::getTokenData(double &readData) {
	readData = doubleData;
}
void Token::getTokenData(int &readData) {
	readData = integerData;
}

/****************************
 * CONFIG MANAGER FUNCTIONS *
 ****************************/

CONFIGELEMENT ConfigManager::getConfigLineType(const std::string &line) {
	CONFIGELEMENT elementType = UNKNOWN;

	/* If string has only spaces */
	if(line.find_first_not_of(' ') == std::string::npos) {
		elementType = BLANK_LINE;

		/* if the line starts with `;' or #, then it is a comment */
	} else if(line[0] == ';' || line[0] == '#') {
		elementType = COMMENT;

		/* If the line starts with `[', then it is probably a section name */
	} else if(line[0] == '[') {

		size_t sectionEndIndex = -1;
		sectionEndIndex = line.find(']');

		if(sectionEndIndex != -1) {
			elementType = SECTION;
		}

		/* Or it can be a key or garbage */
	} else {
		size_t equalSignPos = -1;
		equalSignPos = line.find('=');

		if(equalSignPos != -1) {
			elementType = KEY;
		}
	}
		
	return elementType;
		
}

std::string ConfigManager::getSectionName(const std::string &line) {
	std::string sectionName = "";

	size_t sectionEndIndex = line.find(']');
	sectionName = line.substr(1, (sectionEndIndex - 1));

	return sectionName;
}

std::string ConfigManager::getKeyName (const std::string &line) {
	std::string keyName = "";

	size_t keyNameEndIndex = line.find('=') - 1;
	if(std::isspace(line[keyNameEndIndex])) {
		keyNameEndIndex -= 1;
	}

	/* (+1) because second param is len */
	keyName = line.substr(0, keyNameEndIndex + 1);

	return keyName;
}

static bool alpha_test(char c) {
	return std::isalpha(c);
}
static bool num_test(char c) {
	return std::isdigit(c);
}
TOKENTYPE ConfigManager::getKeyType(const std::string &line) {
	TOKENTYPE keyValueType = INVALID_TYPE;
	std::string keyValue;

	size_t equalSignPos = line.find('=');
	/* AAA = BBB */
	/*    ^ ^    */
	if(std::isspace(line[equalSignPos + 1])) {
		equalSignPos++;
	}

	keyValue = line.substr(equalSignPos + 1);

	bool containsLetters
		= std::find_if(keyValue.begin(), keyValue.end(), alpha_test) != keyValue.end();
	bool containsNums
		= std::find_if(keyValue.begin(), keyValue.end(), num_test) != keyValue.end();
	bool containsDot = false;
	if(keyValue.find('.') != std::string::npos) {
		containsDot = true;
	}

	if(containsLetters) {
		keyValueType = STRING_VAL;
	} else if(containsNums && containsDot) {
		keyValueType = DOUBLE_VAL;
	} else if(containsNums && !containsDot) {
		keyValueType = INT_VAL;
	}

	return keyValueType;
}

std::string getValueLine(const std::string &line) {
	std::string keyValue;

	size_t equalSignPos = line.find('=');
	/* AAA = BBB */
	/*    ^ ^    */
	if(std::isspace(line[equalSignPos + 1])) {
		equalSignPos++;
	}

	keyValue = line.substr(equalSignPos + 1);

	return keyValue;
}

void ConfigManager::getKeyValue(const std::string &line, std::string &readData) {
	std::string keyValue = getValueLine(line);

	readData = keyValue;
}
void ConfigManager::getKeyValue(const std::string &line, int &readData) {
	std::string keyValue = getValueLine(line);

	int val = std::stoi(keyValue);
	readData = val;
	
}
void ConfigManager::getKeyValue(const std::string &line, double &readData) {
	std::string keyValue = getValueLine(line);

	double val = std::atof(keyValue.c_str());
	readData = val;
}

void ConfigManager::initKey(const std::string &line, const std::string &section) {
	std::string keyName = getKeyName(line);
	TOKENTYPE type = getKeyType(line);

	switch(type) {
		case INT_VAL: 
			{
				int val;
				getKeyValue(line, val);

				configFileTokens.push_back(Token(section, keyName, val));
				break;
			}
		case STRING_VAL:
			{
				std::string val;
				getKeyValue(line, val);

				configFileTokens.push_back(Token(section, keyName, val));
				break;
			}
		case DOUBLE_VAL:
			{
				double val;
				getKeyValue(line, val);

				configFileTokens.push_back(Token(section, keyName, val));
				break;
			}
	}

}

void ConfigManager::readConfigFile(const std::string &fileName) {
	configFile.open(fileName, std::ios::in);

	std::string line;
	std::string curSection = "";
	while(std::getline(configFile, line)) {
		CONFIGELEMENT lineType = getConfigLineType(line);

		switch(lineType) {
			case SECTION: curSection = getSectionName(line); break;
			case KEY: initKey(line, curSection); break;
		}

	} //readLine
	
}


size_t ConfigManager::searchTokens(const TOKENTYPE &type, const std::string &section, const std::string &id) {
	for(size_t i = 0; i < configFileTokens.size(); ++i) {
		if(configFileTokens[i].getTokenType() != type) {
			continue;
		} else if((configFileTokens[i].getTokenSection() == section) && (configFileTokens[i].getTokenID() == id)) {
			return i;
		}
	}

	return -1;
}

void ConfigManager::getVal(const TOKENTYPE &type, const std::string &section, const std::string &id, std::string &readData) {
	size_t index = searchTokens(type, section, id);
	configFileTokens[index].getTokenData(readData);
}

void ConfigManager::getVal(const TOKENTYPE &type, const std::string &section, const std::string &id, double &readData) {
	size_t index = searchTokens(type, section, id);
	configFileTokens[index].getTokenData(readData);
}

void ConfigManager::getVal(const TOKENTYPE &type, const std::string &section, const std::string &id, int &readData) {
	size_t index = searchTokens(type, section, id);
	configFileTokens[index].getTokenData(readData);
}
