#ifndef MEMORY_H
#define MEMORY_H

#include <qvector.h>
#include <qbytearray.h>
#include <qfile.h>
#include <qdatastream.h>

typedef unsigned int uint64;
typedef unsigned char uint8;

class CMemory
{
public:
	// explisit Memory(ifstream binery file) ;
	inline CMemory() = default;
	inline ~CMemory() = default;

	template <typename Type>
	Type& operator[] (size_t address);
	template <typename Type>
	Type const& operator[] (size_t address) const;


	template <typename Type>
	bool wirteToMemory(Type size);

	bool isEmpty() { return memory.isEmpty(); }
	bool loadFromFile(QFile&);

private:
	QVector<uint8> memory;
};

template <typename Type>
inline bool CMemory::wirteToMemory (Type size)
{
	memory.append(size);
	return true;
}



template <typename Type>
inline Type& CMemory::operator[] (size_t adress)
{
	return (Type&)(memory[adress]);
}
template <typename Type>
inline const Type& CMemory::operator[](size_t adress) const
{
	return (Type&)(memory[adress]);
}

#endif // !MEMORY_H


 