#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

enum TOKENTYPE {INT_VAL, STRING_VAL, DOUBLE_VAL, INVALID_TYPE};
enum CONFIGELEMENT {BLANK_LINE, SECTION, KEY, COMMENT, UNKNOWN};
	
class Token {
private:
	TOKENTYPE type;
	std::string id;
	
	std::string stringData;
	double doubleData;
	int integerData;

public:
	Token();
	Token(const std::string &tokenID, const std::string tokenStringData);
	Token(const std::string &tokenID, const double tokenDoubleData);
	Token(const std::string &tokenID, const int tokenIntegerData);

	std::string getTokenID();
	TOKENTYPE getTokenType();

	void getTokenData(std::string &readData);
	void getTokenData(double &readData);
	void getTokenData(int &readData);

};

/*********************
 * Main data section *
 *********************/
class Section {
private:
	std::string sectionName;
	std::vector<Token> sectionTokens;

	CONFIGELEMENT getConfigLineType(const std::string &line);


	std::string getKeyName (const std::string &line);
	TOKENTYPE getKeyType(const std::string &line);

	std::string getValueLine(const std::string &line);
	void getKeyValue(const std::string &line, std::string &readData);
	void getKeyValue(const std::string &line, double &readData);
	void getKeyValue(const std::string &line, int &readData);

	void addToken(const std::string &line);

public:
	std::string getSectionName();
	void readSection(const std::string& sectionName, std::stringstream& sstr);

	bool isTokenPresent(const std::string& tokenName);
	bool getToken(const std::string& tokenName, Token& readToken);
	
};

/************************
 * Special data section *
 ************************/
class BasicDataSection : public Section {
private:
	std::vector<int> neuronsInLayers;
	std::vector<std::pair<int,int> > mapDims;

	/* Misc functions */
	std::string getSubstringBetweenBraces(std::string line);
	std::vector<int> getNumbersInCurlyBraces(std::string line); //{1, 2, 3, 4} --> vector<int> vec = {1, 2, ...}

	/* Main init funcs */
	bool init_neuronsInLayers(std::string line);
	bool init_mapDims(std::string line);

public:
	bool section_full_init = false;

	void readBasicDataSection(std::stringstream& sstr);

	std::vector<int>& getNeuronsInLayers();
	std::vector<std::pair<int,int>>& getMapDimensions();
	
	
};

class ConfigManager {
private:
	std::fstream configFile;
	std::vector<Section> configFileSections;
	BasicDataSection basicSection;

	bool is_section(const std::string& line);
	std::string getSectionName(const std::string &line);

	size_t getSectionIndex(std::string sectionName);
	bool searchToken(const std::string &section, const std::string &tokenName, Token& readToken);

public:

	bool isTokenPresent(const std::string &section, const std::string &tokenName);

	void readConfigFile(const std::string &fileName);

	void getVal(const std::string &section, const std::string &id, std::string &readData);
	void getVal(const std::string &section, const std::string &id, double &readData);
	void getVal(const std::string &section, const std::string &id, int &readData);

	bool getBasicData(std::vector<int> &readNeuronsInLayers, std::vector<std::pair<int,int> > &readMapDims );
};
