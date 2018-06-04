#include "Plist.h"

Ref<Plist> Plist::Load(TiXmlElement* element)	
{
	if (element->ValueStr() == _S("integer"))
		return new PlistInt(element);
	else if (element->ValueStr() == _S("string"))
		return new PlistStr(element);
	else if (element->ValueStr() == _S("data"))
		return new PlistData(element);
	else if (element->ValueStr() == _S("true") || element->ValueStr() == _S("false"))
		return new PlistBool(element);
	else if (element->ValueStr() == _S("dict"))
		return new PlistDict(element);
	else
		Fail(-1);
}

Ref<Plist> Plist::Load(const std::string& propstr)
{
	TiXmlDocument doc;
	doc.Parse(propstr.c_str());

	TiXmlElement* element = doc.FirstChildElement();
	Assert(element && !element->NextSiblingElement()); //or what?

	return Load(element);
}

std::string Plist::StoreStr()
{
	int id = 0;
	TiXmlElement* root = Store(&id);

	TiXmlDocument doc;
	doc.LinkEndChild(root);

	std::string docStr;
	docStr << doc;
	return docStr;
}

PlistInt::PlistInt(TiXmlElement* element)
{
	std::string intName = element->GetText();
	if (intName.find(_S("0x")) == 0) //hexadecimal
	{
		std::stringstream stream(intName.substr(2));
		stream >> std::hex >> m_value;
	}
	else  //are there any others?
	{
		std::stringstream stream(intName);
		stream >> m_value;
	}
}

TiXmlElement* PlistInt::Store(int* id)
{
	TiXmlElement* element = new TiXmlElement(_S("integer"));
	if (id)	element->SetAttribute(_S("ID"), (*id)++);

	element->SetAttribute(_S("size"), 64); //okay always?

	std::stringstream stream;
	stream << _S("0x") << std::hex << m_value;
	element->LinkEndChild(new TiXmlText(stream.str()));

	return element;
}

PlistStr::PlistStr(TiXmlElement* element)
{
	m_value = element->GetText();
}

TiXmlElement* PlistStr::Store(int* id)
{
	TiXmlElement* element = new TiXmlElement(_S("string"));
	if (id)	element->SetAttribute(_S("ID"), (*id)++);

	element->LinkEndChild(new TiXmlText(m_value));
	return element;
}

PlistData::PlistData(TiXmlElement* element)
{
	m_value = element->GetText();
}

TiXmlElement* PlistData::Store(int* id)
{
	TiXmlElement* element = new TiXmlElement(_S("data"));
	if (id)	element->SetAttribute(_S("ID"), (*id)++);

	element->LinkEndChild(new TiXmlText(m_value));
	return element;
}

PlistBool::PlistBool(TiXmlElement* element)
{
	m_value = (element->ValueStr() == _S("true"));
}

TiXmlElement* PlistBool::Store(int* id)
{
	TiXmlElement* element = new TiXmlElement( m_value ? _S("true") : _S("false") );
	if (id)	element->SetAttribute(_S("ID"), (*id)++);

	return element;
}

PlistDict::PlistDict(TiXmlElement* element)
{
	for (TiXmlElement* key = element->FirstChildElement(_S("key")); key; key = key->NextSiblingElement(_S("key")))
	{
		TiXmlElement* value = key->NextSiblingElement();
		m_valueMap[key->GetText()] = Plist::Load(value);
	}
}

TiXmlElement* PlistDict::Store(int* id)
{
	TiXmlElement* element = new TiXmlElement(_S("dict"));
	if (id)	element->SetAttribute(_S("ID"), (*id)++);

	for (ValueIter it = m_valueMap.begin(); it != m_valueMap.end(); it++)
	{
		TiXmlElement* key = new TiXmlElement(_S("key"));
		key->LinkEndChild(new TiXmlText(it->first));
		element->LinkEndChild(key);
		
		TiXmlElement* value = it->second->Store(id);
		element->LinkEndChild(value);
	}
	return element;
}
 
Ref<Plist> PlistDict::GetValue(const std::string& name) const
{ 
	ValueConstIter it = m_valueMap.find(name);
	if (it != m_valueMap.end())
		return it->second;
	return NULL;
}

bool PlistDict::IsSubsetOf(Ref<PlistDict> dict)
{
	for (Iter it = BeginKeys(); it != EndKeys(); it++)
	{
		Ref<Plist> thisVal = this->GetValue(*it);
		Ref<Plist> dictVal = dict->GetValue(*it);

		if (thisVal.IsNull() || dictVal.IsNull() || !thisVal->Equals(dictVal))
			return false;
	}
	return true;
}
bool PlistDict::Equals(Ref<Plist> other)
{	
	if (!other->IsDict())
		return false;
	
	Ref<PlistDict> otherDict = other->GetDict();
	return this->IsSubsetOf(otherDict) && otherDict->IsSubsetOf(this); //unoptimal to say the least
}







