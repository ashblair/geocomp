#pragma once
#pragma pack(push, 1)


class vector_cexception: public std::exception
{ 
public:
    const char * v_Exception[2] = {
        "Error in vector_c - index out of range"
		"Error in vector_c - overflow"
    };
    
};

class vector_c_idx_out: public vector_cexception
{
public:
    virtual const char* what() const throw() {return v_Exception[0];};
};

class vector_c_overflow: public vector_cexception
{
public:
	virtual const char* what() const throw() {return v_Exception[1];};
};

// the _c is for compact
// S s/b BYTE or UINT depending on how many nodes need storing
// for the tree path BYTE will more than suffice
template<typename T, typename S>
class vector_c
{ // one-at-a-time vector
public:
	vector_c(void): elements(NULL), sz(static_cast<S>(0)), cap(static_cast<S>(0)) {}
	vector_c(const vector_c & a): elements(new T[a.cap]), sz(a.sz), cap(a.cap) 
	{
		for(S i = 0; i < a.sz; ++i)
		{
			elements[i] = a.elements[i];
		}
		//memcpy(elements, a.elements, a.sz * sizeof(T));
	}
	vector_c & operator = (const vector_c & a)
	{
		delete[] elements;
		elements = new T[a.cap];
		for (S i = 0; i < a.sz; ++i)
		{
			elements[i] = a.elements[i];
		}
		//memcpy(elements, a.elements, a.sz * sizeof(T));
		sz = a.sz;
		cap = a.cap;
		return *this;
	}
	~vector_c(void)
	{
		if (elements != NULL) delete[] elements; 
		sz = 0;
		cap = 0;
		elements = NULL;
	}
	S push(const T & a) 
	{
		if (sz == cap)
		{
			if (cap == static_cast<S>(-1)) throw vector_c_overflow();
			++cap;
			T * elm = new T[cap];
			for (S i = 0; i < sz; ++i)
			{
				elm[i] = elements[i];
			}
			//memcpy(elm, elements, sz * sizeof(T));
			delete[] elements;
			elements = elm;
		}
		elements[sz] = a;
		++sz;
		return (sz - 1);
	}

	T & operator[](const S & idx) 
	{
		if (idx >= sz) throw vector_c_idx_out();
		return elements[idx];
	}	

	void clear(void) {sz = 0;}
	S size(void) {return sz;}
	void pop(void) {if (sz > 0) --sz;}
	S capacity(void) {return cap;}

	T * elements;
	S sz, cap; 

};

#pragma pack(pop)
