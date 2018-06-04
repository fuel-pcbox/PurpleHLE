#include "types.h"
#include <vector>

template <typename T>
class List //wrapper for std::list
{
public:
	void Add(T value) { m_list.push_back(value); }

	bool Contains(T value)
	{
		for (Iter = m_list.begin(); Iter != m_list.end(); Iter++)
			if (*Iter == value)
				return true;
		return false;
	}

	void Remove(T value) { m_list.remove(value); }

	u32 Size() { return m_vec.size(); }

private:
	typedef typename std::list<T>::iterator Iter;
	std::vector<T> m_list;
};