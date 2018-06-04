#pragma once
#include "PlistBReader.h"

namespace Nib
{

class Reader
{
public:
	Reader(const std::vector<u8>& data);
	Plist::Node* GetTop() { return m_topNode; }

protected:
	void BuildNibTreeTop();
	Plist::Node* BuildNibTree(Plist::Node* node);

	Plist::Node* m_topNode;
	Plist::Node* m_objectsNode;
	Plist::BReader* m_plistReader;
};

}