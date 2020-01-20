#ifndef  LOADER_H
#define LOADER_H
#include <qfile.h>
#include <qdatastream.h>
#include <VirtualMachine.h>
class CVirtualMachine;
class CLoader
{
public:
	CLoader();
	~CLoader();
	bool loadFromFile(QFile& file);
};

bool CLoader::loadFromFile(QFile& file)
{
	if (!file.open(QIODevice::ReadOnly))
		return false;

	QDataStream in(&file);
	quint8 beginCode, beginSymbol;
	quint8 a;
	/*in >> beginCode;
	in >> beginSymbol;
	in.skipRawData(beginCode)*/

	for (size_t i = 0; !in.atEnd(); i++)
	{
		in >> a;
		CVirtualMachine::m_pMemory->wirteToMemory(a);
	}
	return true;
}




#endif // ! LOADER_H
