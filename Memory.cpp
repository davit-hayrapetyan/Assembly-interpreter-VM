#include "Memory.h"




bool CMemory::loadFromFile(QFile& file)
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
		memory.append(a);
	}
	return true;
}
