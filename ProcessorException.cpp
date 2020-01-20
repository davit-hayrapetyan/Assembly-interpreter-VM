#include "ProcessorException.h"

ProcessorException::ProcessorException(std::string const sErrorMsg)
{
	m_sErrorMsg = sErrorMsg;
}

ProcessorException::~ProcessorException()
{
}

const char* ProcessorException::what() const
{
	return m_sErrorMsg.c_str();
}
