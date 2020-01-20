#ifndef VirtualMachine_H
#define VirtualMachine_H

#include <QMainWindow>
#include <QWidget>
#include <QPlainTextEdit>
#include <QAction>
#include <QTabWidget>

#include "Processor.h"
#include "Memory.h"
#include "CallStackWidget.h"

class CVirtualMachine : public QMainWindow
{
	Q_OBJECT
public:
	CVirtualMachine(QWidget* pParent = 0);
	~CVirtualMachine();
	
	void init(QFile& oInputFile);
	static	CMemory* m_pMemory;
	
protected:
	void run();
	void setupUi();
	void showState(CProcessor::SInstructionSet& oInstructionSet);

protected slots:
	void onRun();
	void onNext();

private:
	CProcessor*		m_pProcessor   = nullptr;
	QPlainTextEdit*	m_pwGPRegList  = nullptr;
	QPlainTextEdit* m_pwStateInfo  = nullptr;
	QPlainTextEdit*	m_pwAdrRegList = nullptr;

	QTabWidget*		m_pwTab;
	QAction*		m_pActRun  = nullptr;
	QAction*		m_pActNext = nullptr;
	CCallStackWidget* m_pwCallStack = nullptr;

	QVector <CProcessor::SState> arrStates;

};

#endif // VirtualMachine_H