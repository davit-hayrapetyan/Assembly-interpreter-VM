#ifndef ProcessorException_H
#define ProcessorException_H

#include <exception>
#include <QString>

class ProcessorException : public std::exception
{
public:
	ProcessorException(std::string const sErrorMsg);
	~ProcessorException();

	const char* what() const override;

private:
	std::string m_sErrorMsg;
};

#endif // ProcessorException_H

