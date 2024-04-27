#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "AVL.hpp"


template<typename T>
class debugAVL: public AVL<T>
{
public:
    // members:
    int count, // debug check loop check
        hCheck;

    // constructors / destructor:
    debugAVL(void);
    ~debugAVL(void);
    void DebugPrintPath(unsigned int);
    void TreeWrite(const ::std::string, const ::std::string);

private:
    void nodeToElement(unsigned int, unsigned int, pAVLNode<T>);
    
};

template<typename T>
debugAVL<T>::debugAVL(void):AVL<T>(AVL<T>()), count(0){}

template<typename T>
debugAVL<T>::~debugAVL(void)
{
	AVL<T>::~AVL<T>();
	count = 0;
}

template<typename T>
void debugAVL<T>::DebugPrintPath(unsigned int vCount)
{
	
	std::cerr << "Event Queue path for point " + std::to_string(vCount) + "\n";
	BYTE pathTop = sPath.size() - 1;
	for(BYTE i = 0; i < pathTop; ++i)
	{
		std::cerr << std::to_string(i) << ") h:" +
			std::to_string(sPath[i].node->getBalance()) +
			", d:" + sPath[i].direction;
	}
	std::cerr << "\n";
	
}

template<typename T>
void debugAVL<T>::TreeWrite(const std::string Desc, const std::string fN)
{ // just heights for now:
	if (pRoot == NULL) return;
	BYTE h = GetRealHeight(pRoot) - 1, n = h - 1;
	if (h > 13)
	{
		std::cout << "Tree Too Large For Writing, Error in TreeWrite \n";
		return;
	}

	int TwoPow[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384,
	32768, 65536}; //, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216,
	//33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648, 4294967296}

	double dtCount = TwoPow[h] - 1;
	int tCount = (int)dtCount;
	levels = new pAVLNode<T>[tCount];
	for (int i = 0; i < tCount; ++i) levels[i] = NULL;
	nodeToElement(0, 0, pRoot->right);

	int w1 = 4, // ' #### ' to output the height
		w2 = TwoPow[n] * w1, // line length
		w2_ = w2 + 1;
	char * line = new char[w2_], * ptrs = new char[w2_], * blanks = new char[w2_];
	char sChars[] = {" /_\\\0"};
	char blank = sChars[0], fslash = sChars[1], u_s = sChars[2], bslash = sChars[3], nchr = sChars[4];
	for (int i = 0; i < w2; ++i) blanks[i] = blank;
	blanks[w2] = nchr;
	std::ofstream hf;
	hf.open(fN);

	std::string txtBuff, buff;
	hf << Desc;

	hf << "Tree is at root->right. Root is a buffer node with root->height = " <<
		std::to_string(pRoot->height) << "\n";

	for (int i = 0; i < h; ++i)
	{
		int w3 = w2 / TwoPow[i], d2 = w3 / 2, d1 = d2 - w1 / 2,
			wL = d1 - w2 / TwoPow[i + 2], wR = 3 * w2 / TwoPow[i + 2] - d1 - w1;
		if (wL < 0) wL = 0;
		if (wR < 0) wR = 0;
		memcpy(line, blanks, w2_ * sizeof(char));
		memcpy(ptrs, blanks, w2_ * sizeof(char));
		for (int j = 0; j < (int)pow(2.0, i); ++j)
		{
			AVLNode<T> * p = levels[TwoPow[i] - 1 + j], * q = NULL;
			if (p != NULL)
			{
				
				txtBuff = "(";
				txtBuff += std::setw(2) +  std::to_string(p->height) +
					std::setw(0);
				txtBuff += ")";
				memcpy(line + j * w3 + d1, txtBuff.c_str(), w1 * sizeof(char));
				if (i < n)
				{
					int lftS = j * w3 + w2 / TwoPow[i + 2], rghS = j * w3 + d1 + w1;
					if (p->left != NULL)
					{
						ptrs[lftS] = fslash;
						for (int k = 1; k < wL; ++k) line[lftS + k] = u_s;
					}
					if (p->right != NULL)
					{
						for (int k = 0; k < wR; ++k) line[rghS + k] = u_s;
						ptrs[rghS + wR] = bslash;
					}
				}
			}
		}
		buff = (std::string)line;
		buff += "\n";
		hf << buff;

		if (i < n)
		{
			buff = ptrs;
			buff += "\n";
			hf << buff;
		}
	}
	buff = (std::string)blanks; buff += "\n";
	buff = (std::string)blanks; buff += "\n";
	hf << buff;
	hf.close();
	delete [] levels;
	delete [] line;
	delete [] ptrs;
	delete [] blanks;
}

template<typename T>
void debugAVL<T>::nodeToElement(unsigned int level, unsigned int column, pAVLNode<T> node)
{
	if (node == NULL) return;
	levels[(int)pow(2.0, (int)level) - 1 + column] = node;
	nodeToElement(level + 1, 2 * column, node->left);
	nodeToElement(level + 1, 2 * column + 1, node->right);
}

		


