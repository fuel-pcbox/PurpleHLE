#pragma once
#include "types.h"

template <typename T> class Ref  //T : public Reffable
{
public:
	Ref() 
	{ 
		m_ptr = NULL; 
	}
	Ref(T* ptr) 
	{ 
		m_ptr = ptr;
		PtrAcquire();
	}
	Ref(const Ref<T>& ref) 
	{
		m_ptr = ref.m_ptr;
		PtrAcquire();
	}
	~Ref()
	{
		PtrDiscard();
	}

	Ref<T>& operator =(const Ref<T>& ref)
	{
		if (this != &ref)
		{
			PtrDiscard();
			m_ptr = ref.m_ptr;
			PtrAcquire();
		}
		return *this;
	}

	bool operator ==(const Ref<T>& ref) const
	{
		return m_ptr == ref.m_ptr;
	}
	bool operator !=(const Ref<T>& ref) const
	{
		return m_ptr != ref.m_ptr;
	}

	bool IsNull() const { return m_ptr == NULL; }
	bool IsValid() const { return m_ptr != NULL; }

	template <typename CT> operator Ref<CT>() // CT : public Reffable
	{
		CT* m_cast = m_ptr;
		return Ref<CT>(m_cast);
	}

	template <typename CT> Ref<CT> Cast() // CT : public Reffable
	{
		CT* m_cast = dynamic_cast<CT*>(m_ptr);
		return Ref<CT>(m_cast);
	}

	T& operator *() const { return *m_ptr; }
	T* operator ->() const { return m_ptr; }

private:

	void PtrAcquire() 
	{
		if (m_ptr)
			m_ptr->m_ref++;
	}
	void PtrDiscard()
	{
		if (m_ptr)
		{
			m_ptr->m_ref--;
			if (m_ptr->m_ref == 0)
				delete m_ptr;
		}
	}

	T* m_ptr;
};

class Reffable
{
public:
	Reffable() : m_ref(0) {}
	virtual ~Reffable() {}

	Reffable(const Reffable& reffable) : m_ref(0) {}

	template <typename T> friend class Ref;

private:
	int m_ref;
};

template <typename T>
class Array : public Reffable
{
public:
	Array() 
	{
		m_array = NULL;
	}
	Array(u32 size)
	{
		m_size = size;
		m_array = new T[m_size];
	}
	Array(u32 size, T value)
	{
		m_size = size;
		m_array = new T[m_size];
		for (u32 i = 0; i < m_size; i++)
			m_array[i] = value;
	}
	Array(T* arr, u32 size)
	{
		m_size = size;
		m_array = arr;
	}
	~Array()
	{
		delete[] m_array;
	}

	Array(const Array<T>& arr) { Fail(-1); }
	Array<T>& operator =(const Array<T>& arr) { Fail(-1); }
	
	Array<T> Copy()
	{
		Array<T> copy(m_size);
		for (u32 i = 0; i < m_size; i++)
			copy.m_array[i] = m_array[i];
		return copy;
	}

	template <typename RT> u32 SizeAs() const 
	{ 
		if (sizeof(RT) > sizeof(T))
			return m_size / (sizeof(RT) / sizeof(T));
		else
			return m_size * (sizeof(T) / sizeof(RT));
	}
	template <typename RT> RT& AtAs(int index) 
	{ 
		return *(RT *)(m_array + index);
	}
	template <typename RT> const RT& AtAs(int index) const
	{
		return *(RT *)(m_array + index);
	}

	u32 Size() const
	{ 
		return SizeAs<T>(); 
	}
	T& At(int index) 
	{ 
		return AtAs<T>(index); 
	}
	const T& At(int index) const
	{ 
		return AtAs<T>(index); 
	}

	T& operator [](int index)
	{
		return At(index);
	}
	const T& operator [](int index) const
	{
		return At(index);
	}

	T* RawPtr(int index) { return m_array + index; }
	const T* RawPtr(int index) const { return m_array + index; }

	template <typename RT> RT* RawPtrAs(int index) { return (RT *)(m_array + index); }
	template <typename RT> const RT* RawPtrAs(int index) const { return (RT *)(m_array + index); }

private:
	T* m_array;
	u32 m_size;
};