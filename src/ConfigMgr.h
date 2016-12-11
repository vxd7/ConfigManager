#pragma once

#include <fstream>
#include <string>
#include <vector>

enum TOKENTYPE {INT_VAL, STRING_VAL, DOUBLE_VAL, INVALID_TYPE};
enum CONFIGELEMENT {BLANK_LINE, SECTION, KEY, COMMENT, UNKNOWN};
	
struct Token {
private:
	TOKENTYPE type;

	std::string section;
	std::string id;
	
	std::string stringData;
	double doubleData;
	int integerData;
public:
	Token();
	Token(const std::string &tokenSection, const std::string &tokenID, const std::string tokenStringData);
	Token(const std::string &tokenSection, const std::string &tokenID, const double tokenDoubleData);
	Token(const std::string &tokenSection, const std::string &tokenID, const int tokenIntegerData);

	// template <class T>
	// void initToken(std::string tokenSection, std::string tokenID, T tokenData);

	std::string getTokenSection();
	std::string getTokenID();
	TOKENTYPE getTokenType();

	void getTokenData(std::string &readData);
	void getTokenData(double &readData);
	void getTokenData(int &readData);

};

class ConfigManager {
private:
	std::fstream configFile;
	std::vector<Token> configFileTokens;

	CONFIGELEMENT getConfigLineType(const std::string &line);

	std::string getSectionName(const std::string &line);
	std::string getKeyName (const std::string &line);
	TOKENTYPE getKeyType(const std::string &line);

	void getKeyValue(const std::string &line, std::string &readData);
	void getKeyValue(const std::string &line, double &readData);
	void getKeyValue(const std::string &line, int &readData);

	void initKey(const std::string &line, const std::string &section);

	size_t searchTokens(const TOKENTYPE &type, const std::string &section, const std::string &id);



public:
	void readConfigFile(const std::string &fileName);

	void getVal(const TOKENTYPE &type, const std::string &section, const std::string &id, std::string &readData);
	void getVal(const TOKENTYPE &type, const std::string &section, const std::string &id, double &readData);
	void getVal(const TOKENTYPE &type, const std::string &section, const std::string &id, int &readData);
};
