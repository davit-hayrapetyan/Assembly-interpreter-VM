#include <QApplication>
#include "Processor.h"
#include <QDebug>
#include <QVector>
#include "VirtualMachine.h"
 

void Save()
{
       QVector<int> vect;
       QFile file("file2.obj");
       file.open(QIODevice::ReadWrite);
       QDataStream in(&file);
       in << quint8(0b00010101) << quint8(0b11000010) << quint8(3)  << quint32(2) << quint8(0) 
		   << quint8(0b00010101) << quint8(0b11000010) << quint8(7) <<quint32(35648895) << quint8(0) 
		   << quint8(0b00100000) << quint8(0b11000010) << quint8(3) << quint8(7) 
		   << quint8(0b00010101) << quint8(0b11000010) << quint8(11)  << quint32(4294967293) << quint8(0) 
		   << quint8(0b00010101) << quint8(0b11000010) << quint8(15) <<quint32(10) << quint8(0) 
		   << quint8(0b00100000) << quint8(0b11000010) << quint8(11) << quint8(15) ;


	   file.flush();
       file.close();
}

 

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	Save();



	CVirtualMachine* pVM = new CVirtualMachine();
	pVM->init(QFile("file2.obj"));
	pVM->resize(1200, 850);
	pVM->show();

	return a.exec();
}
