#include "NibReader.h"
#include <vector>

namespace Nib
{

Reader::Reader(const std::vector<u8>& data)
{
	m_plistReader = new Plist::BReader(data);

	BuildNibTreeTop();
}

void Reader::BuildNibTreeTop()
{
	Plist::Node* top = m_plistReader->GetTop();

	Plist::StringNode objectsKey(L"$objects");
	m_objectsNode = top->GetByDict(objectsKey);
	
	Plist::StringNode topKey(L"$top");
	m_topNode = top->GetByDict(topKey);

	BuildNibTree(m_topNode);
}

Plist::Node* Reader::BuildNibTree(Plist::Node* node)
{
	if (node->Type() == Plist::Uid)
	{
		int64 uid = node->GetUid();
		Plist::Node* newNode = m_objectsNode->GetArrayNode((int)uid);
		return BuildNibTree(newNode);
	}
	else
	{
		std::vector<Plist::Node*>* nodeVec = node->GetSubNodes();
		if (nodeVec)
		{
			for (u32 i = 0; i < nodeVec->size(); i++)
				nodeVec->at(i) = BuildNibTree(nodeVec->at(i));
		}
		return node;
	}
}

}