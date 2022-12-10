#pragma once

class Error
{
public:
	Error(void);
	~Error(void);

	void addError(const char *, const size_t);
	void displayError(void);

	bool triggered;
	char * msg;

};