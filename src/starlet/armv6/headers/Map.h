#include "types.h"
#include <map>

template <typename KT, typename VT>
struct Pair 
{
	Pair(std::pair<KT, VT> p) : key(p.first), value(p.second) {}

	KT key;
	VT value;
};


template <typename KT, typename VT>
class Map //wrapper for std::map
{
public:
	void Add(KT key, VT value) { m_map.insert( Pair(key, value) ); }

	bool ContainsKey(KT key) 
	{
		return m_map.find(key) != m_map.end();
	}

	void Remove(KT key) { m_map.erase(key); }

	void DeleteAll(bool keys, bool values) //for maps of pointers
	{
		for (Iter i = m_map.begin(); i != m_map.end(); i++)
		{
			if (keys) delete i->key;
			if (values) delete i->value;
		}
	}

	VT& operator [](KT key) { return m_map[key]; }

	Pair<KT,VT> First() { return *m_map.begin(); }
	Pair<KT,VT> Last() { return *m_map.rbegin(); }

	typename std::map<KT, VT>::iterator Begin() { return m_map.begin(); }
	typename std::map<KT, VT>::iterator End() { return m_map.end(); }
	typename std::map<KT, VT>::reverse_iterator RBegin() { return m_map.rbegin(); }
	typename std::map<KT, VT>::reverse_iterator REnd() { return m_map.rend(); }

private:
	typedef typename std::map<KT, VT>::iterator Iter;
	typedef typename std::map<KT, VT>::reverse_iterator RIter;
	typedef std::pair<KT, VT> Pair;
	std::map<KT, VT> m_map;
};