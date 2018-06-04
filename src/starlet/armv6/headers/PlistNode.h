#pragma once
#include "types.h"

namespace Plist
{

typedef enum {Null, Bool, Int, Real, Date, Data, String, Uid, Array, Set, Dict} NodeType;

class Node
{
public:
	typedef std::vector<Node*> NodeVec;

	virtual NodeType Type() const = 0;
	virtual NodeVec* GetSubNodes() { return NULL; }
	virtual bool Equals(const Node* node) const = 0;

	bool GetBool();
	int64 GetInt();
	int64 GetUid();
	double GetReal();
	double GetDate();
	std::wstring GetString();
	const ptr GetData();
	int GetArraySize();
	Node* GetArrayNode(int i);
	bool IsSetContains(Node& node);
	Node* GetByDict(Node& node);

protected:
	template <typename T, typename NT>
	T GetValue();
};

class NullNode : public Node
{
public:
	NodeType Type() const { return Null; }
	bool Equals(const Node* node) const;
};

template <class PRIM, NodeType TYPE>
class PrimNode : public Node
{
public:
	PrimNode(PRIM value) : m_value(value) {}
	NodeType Type() const { return TYPE; }
	PRIM Value() const { return m_value; }
	
	bool Equals(const Node* node) const;

protected:
	PRIM m_value;
};

typedef PrimNode<bool, Bool> BoolNode;
typedef PrimNode<int64, Int> IntNode;
typedef PrimNode<int64, Uid> UidNode;
typedef PrimNode<double, Real> RealNode;
typedef PrimNode<double, Date> DateNode;
typedef PrimNode<std::wstring, String> StringNode;

template <NodeType TYPE>
class VectorNode : public Node
{
public:
	VectorNode(NodeVec* vec) : m_vec(vec) {}
	~VectorNode() { delete m_vec; }
	NodeType Type() const { return TYPE; }
	const NodeVec& Value() const { return *m_vec; }
	NodeVec* GetSubNodes() { return m_vec; }
	
	bool Equals(const Node* node) const;

protected:
	NodeVec* m_vec;
};

typedef VectorNode<Array> ArrayNode;
typedef VectorNode<Set> SetNode;
typedef VectorNode<Dict> DictNode;

class DataNode : public Node
{
public:
	DataNode(ptr pointer, int size) : m_size(size), m_pointer(pointer) {}
	~DataNode() { delete[] m_pointer; }
	NodeType Type() const { return Data; }
	const ptr Value() const { return m_pointer; }
	int Size() const { return m_size; }

	bool Equals(const Node* node) const;
protected:
	const ptr m_pointer;
	int m_size;
};

}

#include "PlistNode.inl"