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
	bool getToken(const std::string& tokenName, Token& readToken);
	
};

class SpecialSection : public Section {
private:
	
};

class ConfigManager {
private:
	std::fstream configFile;
	std::vector<Section> configFileSections;

	bool is_section(const std::string& line);
	std::string getSectionName(const std::string &line);

	size_t getSectionIndex(std::string sectionName);
	bool searchToken(const std::string &section, const std::string &tokenName, Token& readToken);

public:

	void readConfigFile(const std::string &fileName);

	void getVal(const std::string &section, const std::string &id, std::string &readData);
	void getVal(const std::string &section, const std::string &id, double &readData);
	void getVal(const std::string &section, const std::string &id, int &readData);
};
