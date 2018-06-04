#include "PlistNode.h"

namespace Plist
{

INLINE bool NullNode::Equals(const Node* node) const
{ 
	const NullNode* cvnode = dynamic_cast<const NullNode*>(node); 
	return cvnode != NULL;
}

template <class PRIM, NodeType TYPE>
INLINE bool PrimNode<PRIM, TYPE>::Equals(const Node* node) const
{ 
	const PrimNode<PRIM, TYPE>* cvnode = dynamic_cast<const PrimNode<PRIM, TYPE> *>(node); 
	return cvnode && (cvnode->Value()) == (this->Value());
}

template <NodeType TYPE>
INLINE bool VectorNode<TYPE>::Equals(const Node* node) const
{ 
	const VectorNode<TYPE>* cvnode = dynamic_cast<const VectorNode<TYPE> *>(node); 
	if (!cvnode) return false;

	for (u32 i = 0; i < m_vec->size(); i++)
		if (!(cvnode->Value()[i])->Equals(this->Value()[i]))
			return false;
	return true;
}

INLINE bool DataNode::Equals(const Node* node) const
{ 
	const DataNode* cvnode = dynamic_cast<const DataNode *>(node); 
	if (!cvnode || cvnode->Size() != this->Size()) return false;

	return memcmp(cvnode->Value(), this->Value(), this->Size()) == 0;
}



template <typename T, typename NT>
INLINE T Node::GetValue()
{
	NT* cvnode = dynamic_cast<NT *>(this);
	if (!cvnode) Fail(0);
	return cvnode->Value();
}
INLINE int Node::GetArraySize()
{
	ArrayNode* cvnode = dynamic_cast<ArrayNode *>(this);
	if (!cvnode) Fail(0);
	return cvnode->Value().size();
}
INLINE Node* Node::GetArrayNode(int i)
{
	ArrayNode* cvnode = dynamic_cast<ArrayNode *>(this);
	if (!cvnode) Fail(0);
	return cvnode->Value()[i];
}
INLINE bool Node::IsSetContains(Node& node)
{
	SetNode* cvnode = dynamic_cast<SetNode *>(this);
	if (!cvnode) Fail(0);
	std::vector<Node*> vec = cvnode->Value();

	for (u32 i = 0; i < vec.size(); i++)
		if (vec[i]->Equals(&node))
			return true;
	return false;
}
INLINE Node* Node::GetByDict(Node& node)
{
	DictNode* cvnode = dynamic_cast<DictNode *>(this);
	if (!cvnode) Fail(0);
	std::vector<Node*> vec = cvnode->Value();

	for (u32 i = 0; i < vec.size()/2; i++)
		if (vec[i*2]->Equals(&node)) //key
			return vec[i*2 + 1]; //value
	return NULL;
}

INLINE bool Node::GetBool() { return GetValue<bool, BoolNode>(); }
INLINE int64 Node::GetInt() { return GetValue<int64, IntNode>(); }
INLINE int64 Node::GetUid() { return GetValue<int64, UidNode>(); }
INLINE double Node::GetReal() { return GetValue<double, RealNode>(); }
INLINE double Node::GetDate() { return GetValue<double, DateNode>(); }
INLINE std::wstring Node::GetString() { return GetValue<std::wstring, StringNode>(); }
INLINE const ptr Node::GetData() { return GetValue<ptr, DataNode>(); }

}
