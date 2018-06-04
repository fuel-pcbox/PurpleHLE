#pragma once
#include "types.h"
#include <map>
#include <sstream>
#include "Ref.h"

#include "3rdParty\tinyxml\tinyxml.h"

class PlistInt;
class PlistStr;
class PlistBool;
class PlistDict;

class Plist : public Reffable
{
public:
	static Ref<Plist> Load(TiXmlElement* element);
	static Ref<Plist> Load(const std::string& propstr);

	virtual TiXmlElement* Store(int* id) = 0;
	std::string StoreStr();
	
	virtual bool Equals(Ref<Plist> other) = 0;

	virtual bool IsInt() { return false; }
	virtual bool IsStr() { return false; }
	virtual bool IsBool() { return false; }
	virtual bool IsDict() { return false; }
	virtual bool IsData() { return false; }

	virtual s64 GetInt() { return 0; }
	virtual std::string GetStr() { return std::string(); }
	virtual std::string GetData() { return std::string(); }
	virtual bool GetBool() { return false; }
	virtual Ref<PlistDict> GetDict() { return NULL; } //or empty dict?
};

class PlistInt : public Plist
{
public:
	PlistInt(TiXmlElement* element);
	PlistInt(s64 value) : m_value(value) {}
	TiXmlElement* Store(int* id);

	bool IsInt() { return true; }
	s64 GetInt() { return m_value; }

	bool Equals(Ref<Plist> other) { return other->IsInt() && other->GetInt() == GetInt(); }

private:
	s64 m_value;
};

class PlistStr : public Plist
{
public:
	PlistStr(TiXmlElement* element);
	PlistStr(std::string value) : m_value(value) {}
	TiXmlElement* Store(int* id);

	bool IsStr() { return true; }
	std::string GetStr() { return m_value; }

	bool Equals(Ref<Plist> other) { return other->IsStr() && other->GetStr() == GetStr(); }

private:
	std::string m_value;
};

class PlistData : public Plist
{
public:
	PlistData(TiXmlElement* element);
	PlistData(std::string value) : m_value(value) {}
	TiXmlElement* Store(int* id);

	bool IsData() { return true; }
	std::string GetStr() { return m_value; }

	bool Equals(Ref<Plist> other) { return other->IsStr() && other->GetStr() == GetStr(); }

private:
	std::string m_value;
};

class PlistBool : public Plist
{
public:
	PlistBool(TiXmlElement* element);
	PlistBool(bool value) : m_value(value) {}
	TiXmlElement* Store(int* id);

	bool IsBool() { return true; }
	bool GetBool() { return m_value; }

	bool Equals(Ref<Plist> other) { return other->IsBool() && other->GetBool() == GetBool(); }

private:
	bool m_value;
};

class PlistDict : public Plist
{
public:
	PlistDict(TiXmlElement* element);
	PlistDict() {}
	TiXmlElement* Store(int* id);

	void AddValue(const std::string& name, Ref<Plist> value)
	{
		m_valueMap[name] = value;
	}
	void MergeDict(Ref<PlistDict> dict)
	{
		m_valueMap.insert(dict->m_valueMap.begin(), dict->m_valueMap.end());
	}
	bool IsSubsetOf(Ref<PlistDict> dict);
	bool Equals(Ref<Plist> other);

	Ref<Plist> GetValue(const std::string& name) const; //add GetInt, etc.

	bool IsDict() { return true; }
	Ref<PlistDict> GetDict() { return this; }

private:
	typedef std::map<std::string, Ref<Plist> > ValueMap;
	typedef std::map<std::string, Ref<Plist> >::iterator ValueIter;
	typedef std::map<std::string, Ref<Plist> >::const_iterator ValueConstIter;

	ValueMap m_valueMap;
	
public:
	class Iter
	{
	public:
		Iter(ValueConstIter iter) : m_iter(iter) {}
		Iter operator ++(int) { return Iter(m_iter++); }
		Iter operator ++() { return Iter(++m_iter); }	
		bool operator ==(Iter other) const { return m_iter == other.m_iter; }
		bool operator !=(Iter other) const { return m_iter != other.m_iter; }
		const std::string& operator *() const { return m_iter->first; }
		const std::string* operator ->() const { return &(m_iter->first); } //?
	private:
		ValueConstIter m_iter;
	};

	Iter BeginKeys() const { return Iter(m_valueMap.begin()); }
	Iter EndKeys() const { return Iter(m_valueMap.end()); }
};
