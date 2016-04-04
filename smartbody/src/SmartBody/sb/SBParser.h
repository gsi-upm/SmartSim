#ifndef _SBPARSER_H_
#define _SBPARSER_H_

#include <sb/SBTypes.h>
#include <vhcl.h>
#include <external/parser/Params.h>

class InputTree;

namespace SmartBody {

class SBParseNode;

class SBParserListener
{
	public:
		SBParserListener();
		SBAPI virtual void onWord(std::string timeMarker, std::string word, std::string emphasis);
		SBAPI virtual void onPartOfSpeech(std::string timeMarker, std::string partOfSpeech);
		SBAPI std::string getCurrentBML();
		SBAPI void addBML(std::string bml);
		SBAPI void resetBML();

	protected:
		std::string _bml;
};


class SBParser
{
	public:
		SBAPI SBParser();
		SBAPI ~SBParser();

		SBAPI void initialize(const std::string& arg1, const std::string& arg2);
		SBAPI SmartBody::SBParseNode* parse(const std::string& input);
		SBAPI void cleanUp(SmartBody::SBParseNode* node);
		SBAPI bool isInitialized();

		SBAPI std::string parseUtterance(SBParserListener* listener, std::string utterance);

	protected:
		void createParseTree(InputTree* inputTree, SBParseNode* node);
		void parseTraverse(SBParserListener* listener, SmartBody::SBParseNode* node, int& curWord);

		bool _initialized;
		Params params;
		bool release;
		string agent_id;
};

}

#endif