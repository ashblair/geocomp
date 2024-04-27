#pragma once

template <class T>
class Sorter
{
public:
    Sorter(void):
        s_array(NULL), tmp_array(NULL), firstIdx(0), lastIdx(0), numElements(0) {}
    Sorter(const Sorter& s): 
        s_array(s.s_array), tmp_array(s.tmp_array), firstIdx(s.firstIdx), lastIdx(s.lastIdx), 
            numElements(s.numElements) {}
    Sorter(T * Table, int first, int last, int length) :
        s_array(Table), tmp_array(NULL), firstIdx(first), lastIdx(last),
            numElements(length)
    {
        T * r = new T[numElements];
        tmp_array = r;
    }
    ~Sorter(void) 
    {
        if (tmp_array != NULL) delete [] tmp_array;
        tmp_array = NULL;
    }
    void AddArray(T*, int, int, int);
    void doSort(void);
    int GetLength(void);

private:
    T * s_array, * tmp_array;
    int firstIdx, lastIdx, numElements;
    void MergeSort(int, int);
    void Merge(int, int, int);
};

template<class T>
void Sorter<T>::AddArray(T* Table, int first, int last, int length)
{

	s_array = Table;
	firstIdx = first;
	lastIdx = last;
	numElements = length;
	if (tmp_array != NULL) delete [] tmp_array;
	T * r = new T[numElements];
	tmp_array = r;
	
}

template<class T>
void Sorter<T>::doSort(void)
{
	bool proceed = s_array != NULL;
	int span = lastIdx - firstIdx + 1;
	proceed = proceed && (span > 1);
	if (proceed) MergeSort(firstIdx, lastIdx);
	//return s_array;

}

template<class T>
int Sorter<T>::GetLength(void)
{
	return numElements;

}

template<class T>
void Sorter<T>::MergeSort(int First, int Last)
{
	if (First < Last)
	{
		int Middle = (First + Last) / 2;
		//MergeSort<T>(First, Middle);
		//MergeSort<T>(Middle + 1, Last);
		//Merge<T>(First, Middle, Last);
		MergeSort(First, Middle);
		MergeSort(Middle + 1, Last);
		Merge(First, Middle, Last);
	}

}

template<class T>
void Sorter<T>::Merge(int First, int Middle, int Last)
{   // part of recursive Merge Sort s/b called log base 2 N times
	// uses temporary storage buffer tmp_array
	//int num2Mrg = Last - First + 1;
	int NextLeft = First, NextRight = Middle + 1, Index = 0;
	while ((NextLeft <= Middle) && (NextRight <= Last))
	{
		if (s_array[NextLeft] < s_array[NextRight])
		{
			tmp_array[Index++] = s_array[NextLeft++];
		}
		else
		{
			tmp_array[Index++] = s_array[NextRight++];
		}
	}
	if (NextLeft <= Middle) for (int i = NextLeft; i <= Middle; ++i) tmp_array[Index++] = s_array[i];
	else /*if (NextRight <= Last)*/ for (int i = NextRight; i <= Last; ++i) tmp_array[Index++] = s_array[i];
	for (int i = First; i <= Last; ++i) s_array[i] = tmp_array[i - First];
}
