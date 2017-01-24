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
Token::Token(const std::string &tokenID, const std::string tokenStringData) {
	type = STRING_VAL;

	id = tokenID;

	stringData = tokenStringData;
}

Token::Token(const std::string &tokenID, const double tokenDoubleData) {
	type = DOUBLE_VAL;

	id = tokenID;

	doubleData = tokenDoubleData;
}

Token::Token(const std::string &tokenID, const int tokenIntegerData) {
	type = INT_VAL;

	id = tokenID;

	integerData = tokenIntegerData;
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

/*********************
 * SECTION FUNCTIONS *
 * *******************/

CONFIGELEMENT Section::getConfigLineType(const std::string &line) {
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


std::string Section::getKeyName (const std::string &line) {
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
TOKENTYPE Section::getKeyType(const std::string &line) {
	TOKENTYPE keyValueType = INVALID_TYPE;
	std::string keyValue;

	size_t equalSignPos = line.find('=');
	/* AAA = BBB */
	/*    ^ ^    */
	if(std::isspace(line[equalSignPos + 1])) {
		equalSignPos++;
	}

	keyValue = line.substr(equalSignPos + 1);

	/* Check if there are double quotes present */
	if((keyValue.find('"') != std::string::npos) && (keyValue.rfind('"') != std::string::npos)) {
		keyValueType = STRING_VAL;
	} else {

		/* If there are no quotes -- continue checkings */
		bool containsNums
			= std::find_if(keyValue.begin(), keyValue.end(), num_test) != keyValue.end();
		bool containsDot = false;
		if(keyValue.find('.') != std::string::npos) {
			containsDot = true;
		}

		if(containsNums && containsDot) {
			keyValueType = DOUBLE_VAL;
		} else if(containsNums && !containsDot) {
			keyValueType = INT_VAL;
		}
	}

	return keyValueType;
}

std::string Section::getValueLine(const std::string &line) {
	std::string keyValue;

	size_t equalSignPos = line.find('=');
	/* AAA = BBB */
	/*    ^ ^    */
	if(std::isspace(line[equalSignPos + 1])) {
		equalSignPos++;
	}

	keyValue = line.substr(equalSignPos + 1);

	size_t firstQuote = keyValue.find('"');
	size_t lastQuote = keyValue.rfind('"');
	if((firstQuote != std::string::npos) && (lastQuote != std::string::npos)) {
		keyValue = keyValue.substr(firstQuote + 1, (lastQuote - firstQuote - 1));
	}

	return keyValue;
}

void Section::getKeyValue(const std::string &line, std::string &readData) {
	std::string keyValue = getValueLine(line);

	readData = keyValue;
}
void Section::getKeyValue(const std::string &line, int &readData) {
	std::string keyValue = getValueLine(line);

	int val = std::stoi(keyValue);
	readData = val;
	
}
void Section::getKeyValue(const std::string &line, double &readData) {
	std::string keyValue = getValueLine(line);

	double val = std::atof(keyValue.c_str());
	readData = val;
}

void Section::addToken(const std::string &line) {
	std::string keyName = getKeyName(line);
	TOKENTYPE type = getKeyType(line);

	switch(type) {
		case INT_VAL: 
			{
				int val;
				getKeyValue(line, val);

				sectionTokens.push_back(Token(keyName, val));
				break;
			}
		case STRING_VAL:
			{
				std::string val;
				getKeyValue(line, val);

				sectionTokens.push_back(Token(keyName, val));
				break;
			}
		case DOUBLE_VAL:
			{
				double val;
				getKeyValue(line, val);

				sectionTokens.push_back(Token(keyName, val));
				break;
			}
	}
	
}

std::string Section::getSectionName() {
	return sectionName;
}

void Section::readSection(const std::string& sectionName, std::stringstream& sstr) {
	this->sectionName = sectionName;

	std::string line;
	while(sstr.good()) {
		std::getline(sstr, line, '\n'); // Get the stringstream contents line-by-line

		CONFIGELEMENT lineType = getConfigLineType(line);
		if(lineType == KEY) {
			addToken(line);
		}
	}
}

bool Section::getToken(const std::string& tokenName, Token& readToken) {
	for(size_t i = 0; i < sectionTokens.size(); ++i) {
		if(sectionTokens[i].getTokenID() == tokenName) {
			readToken = sectionTokens[i];
			return true;
		}
	}

	return false;
}

/***************************
 * ConfigManager funcitons *
 * *************************/

bool ConfigManager::is_section(const std::string& line) {
	if(line.find(']') != std::string::npos && line.find('[') != std::string::npos){
		return true;
	}
	return false;
}
std::string ConfigManager::getSectionName(const std::string &line) {
	std::string sectionName = "";

	size_t sectionEndIndex = line.find(']');
	sectionName = line.substr(1, (sectionEndIndex - 1));

	return sectionName;
}

void ConfigManager::readConfigFile(const std::string &fileName) {
	configFile.open(fileName, std::ios::in);

	/* Read the whole file and get section blocks */
	std::string line; // Line of config file
	std::string sectionName; //Where to store the section name
	bool inside_section = false; //Are we inside the section?
	while(std::getline(configFile, line)) {
		std::stringstream sstr;

		if(is_section(line)) {
			inside_section = true;
			sectionName = getSectionName(line);

			/* Get the section block to the stringstream */
			while(inside_section && configFile.good()) {
				int pos = configFile.tellg();
				std::getline(configFile, line);

				if(!is_section(line)) {
					sstr << line << '\n'; // Append `\n' separator to the stringstream
				} else {
					inside_section = false;
					configFile.seekg(pos, std::ios_base::beg); //Return one line before
				}
			}

			Section tmpSect;
			tmpSect.readSection(sectionName, sstr);
			configFileSections.push_back(tmpSect);
		}

	} //readLine
	
}

size_t ConfigManager::getSectionIndex(std::string sectionName) {
	for(size_t i = 0; i < configFileSections.size(); ++i) {
		if(configFileSections[i].getSectionName() == sectionName) {
			return i;
		}
	}

	return -1;
}

bool ConfigManager::searchToken(const std::string &section, const std::string &tokenName, Token& readToken) {
	size_t findSectionIndex = getSectionIndex(section);

	/* Not found */
	if(findSectionIndex == -1) {
		return false;
	}

	Token foundToken;
	configFileSections[findSectionIndex].getToken(tokenName, foundToken);

	readToken = foundToken;
	return true;


}

void ConfigManager::getVal(const std::string &section, const std::string &id, std::string &readData) {
	Token foundToken;
	bool found = searchToken(section, id, foundToken);

	if(!found) {
		//exception here
	} else {
		foundToken.getTokenData(readData);
	}
}

void ConfigManager::getVal(const std::string &section, const std::string &id, double &readData) {
	Token foundToken;
	bool found = searchToken(section, id, foundToken);

	if(!found) {
		//exception here
	} else {
		foundToken.getTokenData(readData);
	}
}

void ConfigManager::getVal(const std::string &section, const std::string &id, int &readData) {
	Token foundToken;
	bool found = searchToken(section, id, foundToken);

	if(!found) {
		//exception here
	} else {
		foundToken.getTokenData(readData);
	}
}
