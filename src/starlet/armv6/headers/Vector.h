#include "types.h"
#include <vector>

template <typename T>
class Vector //wrapper for std::vector
{
public:
	void Add(T value) { m_vec.push_back(value); }

	bool Contains(T value) 
	{
		for (int i = 0; i < Size(); i++)
			if (m_vec[i] == value) 
				return true;
		return false;
	}

	u32 Size() { return m_vec.size(); }

	void DeleteAll() //for vectors of pointers
	{
		for (int i = 0; i < Size(); i++)
			delete m_vec[i];
	}

	T& operator [](int index) { return m_vec[index]; }

private:
	typedef typename std::vector<T>::iterator Iter;
	std::vector<T> m_vec;
};