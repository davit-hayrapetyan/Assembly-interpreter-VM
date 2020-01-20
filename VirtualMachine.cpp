#include "VirtualMachine.h"
#include <QToolBar>
#include <QTextEdit>
#include <QSplitter>
#include <QBoxLayout>

CMemory *CVirtualMachine::m_pMemory = nullptr;
struct SState;
CVirtualMachine::CVirtualMachine(QWidget *pParent) : QMainWindow(pParent)
{
	setupUi();
}

CVirtualMachine::~CVirtualMachine()
{
	delete m_pMemory;
	delete m_pProcessor;
}

void CVirtualMachine::run()
{
	if (m_pProcessor->getState()->bIsActive)
	{
		quint64 instruction = m_pProcessor->fetch();
		CProcessor::SInstructionSet oInstructionSet = m_pProcessor->decode(instruction);
		m_pProcessor->execute(oInstructionSet);
		
		showState(oInstructionSet);
	}

	m_pProcessor->getState()->bIsActive = false;
}

void CVirtualMachine::showState(CProcessor::SInstructionSet& oInstructionSet)
{
	m_pwAdrRegList->clear();
	m_pwGPRegList->clear();
	m_pwStateInfo->clear();
	m_pwAdrRegList->appendPlainText(" Current State \n");
	CProcessor::SState* pState = m_pProcessor->getState();
	QString name(" R%1 : %2");
	for (int i = 0; i < pState->aARegisters.count(); i++)
	{
		if (i == 0)
			m_pwAdrRegList->appendPlainText( QString(" SP ") + binarToString( pState->aARegisters.value(i)));
		else if (i == 1)
			m_pwAdrRegList->appendPlainText( QString(" SF ") + binarToString(pState->aARegisters.value(i)) );
		else
			m_pwAdrRegList->appendPlainText(name.arg(i+1).arg(binarToString(pState->aARegisters.value(i))) );
		m_pwAdrRegList->moveCursor(QTextCursor::End);
		m_pwAdrRegList->insertPlainText("\n");
	}
	m_pwAdrRegList->moveCursor(QTextCursor::Start);
	
	for (int i = 0; i < pState->aGPRegisters.count(); i++)
	{
		m_pwGPRegList->appendPlainText(" " + name.arg(i).arg(binarToString(pState->aGPRegisters.value(i))));
		m_pwGPRegList->moveCursor(QTextCursor::End);
		m_pwGPRegList->insertPlainText("\n");
	}
	m_pwGPRegList->moveCursor(QTextCursor::Start);
	QString sIP(" IP %1");
	m_pwStateInfo->appendPlainText(sIP.arg(binarToString(pState->nIP)));
	m_pwStateInfo->moveCursor(QTextCursor::End);
	m_pwStateInfo->insertPlainText("\n");

	QString FlagsName (" OF  SF  ZF  CF ");
	QString StateFlasgs (" %1   %2   %3   %4 ");
	m_pwStateInfo->appendPlainText(FlagsName);
	m_pwStateInfo->moveCursor(QTextCursor::End);
	m_pwStateInfo->insertPlainText("\n");
	m_pwStateInfo->appendPlainText(StateFlasgs.arg(pState->pFlags.getOverflowFlag()).arg(pState->pFlags.getSignFlag()).arg(pState->pFlags.getZeroFlag()).arg(pState->pFlags.getCarryFlag()));
	m_pwStateInfo->moveCursor(QTextCursor::Start);
 	arrStates.append(*pState);
	

	switch (oInstructionSet.eOpCode)
	{
	case EOpCode::nop:
		m_pwCallStack->addItem( QString("nop"));
		break;
	case EOpCode::jump:
		m_pwCallStack->addItem( QString("jump"));
		break;
	case EOpCode::call:
		m_pwCallStack->addItem( QString("call"));
		break;
	case EOpCode::ret:
		m_pwCallStack->addItem( QString("ret"));
		break;
	case EOpCode::exit:
		m_pwCallStack->addItem( QString("exit"));
		break;
	case EOpCode::test:
		m_pwCallStack->addItem( QString("test"));
		break;
	case EOpCode::cmp:
		m_pwCallStack->addItem( QString("cmp"));
		break;
	case EOpCode::shr_r:
		m_pwCallStack->addItem( QString("shr_r"));
		break;
	case EOpCode::shl_r:
		m_pwCallStack->addItem( QString("shl_r"));
		break;
	case EOpCode::ror_r:
		m_pwCallStack->addItem( QString("ror_r"));
		break;
	case EOpCode::rol_r:
		m_pwCallStack->addItem( QString("rol_r"));
		break;
	case EOpCode::shr_n:
		m_pwCallStack->addItem( QString("shr_n"));
		break;
	case EOpCode::shl_n:
		m_pwCallStack->addItem( QString("shl_n"));
		break;
	case EOpCode::ror_n:
		m_pwCallStack->addItem( QString("ror_n"));
		break;
	case EOpCode::rol_n:
		m_pwCallStack->addItem( QString("rol_n"));
		break;
	case EOpCode::push:
		m_pwCallStack->addItem( QString("push"));
		break;
	case EOpCode::pop:
		m_pwCallStack->addItem( QString("pop"));
		break;
	case EOpCode::pushf:
		m_pwCallStack->addItem( QString("pushf"));
		break;
	case EOpCode::popf:
		m_pwCallStack->addItem( QString("popf"));
		break;
	case EOpCode::pushsf:
		m_pwCallStack->addItem( QString("pushsf"));
		break;
	case EOpCode::popsf:
		m_pwCallStack->addItem( QString("popsf"));
		break;
	case EOpCode::assign:
		m_pwCallStack->addItem( QString("assign"));
		break;
	case EOpCode::move:
		m_pwCallStack->addItem( QString("move"));
		break;
	case EOpCode::swap:
		m_pwCallStack->addItem( QString("swap"));
		break;
	case EOpCode::load:
		m_pwCallStack->addItem( QString("load"));
		break;
	case EOpCode::store:
		m_pwCallStack->addItem( QString("store"));
		break;
	case EOpCode::and:
		m_pwCallStack->addItem( QString("and"));
		break;
	case EOpCode:: or :
		m_pwCallStack->addItem( QString("or"));
		break;
	case EOpCode::xor:
		m_pwCallStack->addItem( QString("xor"));
		break;
	case EOpCode::not:
		m_pwCallStack->addItem( QString("not"));
		break;
	case EOpCode::nand:
		m_pwCallStack->addItem( QString("nand"));
		break;
	case EOpCode::nor:
		m_pwCallStack->addItem( QString("nor"));
		break;
	case EOpCode::add:
		m_pwCallStack->addItem( QString("add"));
		break;
	case EOpCode::sub:
		m_pwCallStack->addItem( QString("sub"));
		break;
	case EOpCode::mul:
		m_pwCallStack->addItem( QString("mul"));
		break;
	case EOpCode::div:
		m_pwCallStack->addItem( QString("div"));
		break;
	case EOpCode::inc:
		m_pwCallStack->addItem( QString("inc"));
		break;
	case EOpCode::dec:
		m_pwCallStack->addItem( QString("dec"));
		break;
	case EOpCode::in_r:
		m_pwCallStack->addItem( QString("in_r"));
		break;
	case EOpCode::out_r:
		m_pwCallStack->addItem( QString("out_r"));
		break;
	case EOpCode::in_n:
		m_pwCallStack->addItem( QString("in_n"));
		break;
	case EOpCode::out_n:
		m_pwCallStack->addItem( QString("out_n"));
		break;
	case EOpCode::adc:
		m_pwCallStack->addItem( QString("adc"));
		break;
	case EOpCode::sbb:
		m_pwCallStack->addItem( QString("sbb"));
		break;
	case EOpCode::imul:
		m_pwCallStack->addItem( QString("imul"));
		break;
	case EOpCode::idiv:
		m_pwCallStack->addItem( QString("idiv"));
		break;
	case EOpCode::neg:
		m_pwCallStack->addItem( QString("neg"));
		break;
	case EOpCode::sar_r:
		m_pwCallStack->addItem( QString("sar_r"));
		break;
	case EOpCode::sal_r:
		m_pwCallStack->addItem( QString("sal_r"));
		break;
	case EOpCode::rcr_r:
		m_pwCallStack->addItem( QString("rcr_r"));
		break;
	case EOpCode::rcl_r:
		m_pwCallStack->addItem( QString("rcl_r"));
		break;
	case EOpCode::sar_n:
		m_pwCallStack->addItem( QString("sar_n"));
		break;
	case EOpCode::sal_n:
		m_pwCallStack->addItem( QString("sal_n"));
		break;
	case EOpCode::rcr_n:
		m_pwCallStack->addItem( QString("rcr_n"));
		break;
	case EOpCode::rcl_n:
		m_pwCallStack->addItem( QString("rcl_n"));
	default:
		break;
	}


}

void CVirtualMachine::init(QFile& oInputFile)
{
	m_pMemory->loadFromFile(oInputFile);
}

void CVirtualMachine::onRun()
{
	m_pActRun->setEnabled(false);
	run();
}


void CVirtualMachine::onNext()
{
	m_pProcessor->getState()->bIsActive = true;
	run();
}

void CVirtualMachine::setupUi()
{
	setWindowIcon(QIcon(":/courseWork/run.png"));
	setWindowTitle("Virtual Machine");

	m_pMemory = new CMemory();
	m_pProcessor = new CProcessor(m_pMemory);

	QSplitter* pwSplitter = new QSplitter();
	setCentralWidget(pwSplitter);

	m_pwTab = new QTabWidget();
	m_pwTab->setStyleSheet("QTabBar::tab { height: 50px; width: 200px; font-size:15px; }");
	pwSplitter->addWidget(m_pwTab);

	m_pwAdrRegList = new QPlainTextEdit();
	m_pwGPRegList = new QPlainTextEdit();
	m_pwStateInfo = new QPlainTextEdit();

	QWidget* pwMain = new QWidget();
	QVBoxLayout* pMainLayout = new QVBoxLayout();
	pwMain->setLayout(pMainLayout);

	pMainLayout->addWidget(m_pwStateInfo);
	pMainLayout->addWidget(m_pwTab);
	
	pMainLayout->setStretch(0, 30);
	pMainLayout->setStretch(1, 100);

	pwSplitter->addWidget(pwMain);

	m_pwTab->addTab(m_pwAdrRegList, QIcon(""), "Adress Registers");
	m_pwTab->addTab(m_pwGPRegList, QIcon(""), "General Purpose Registers");

	m_pwCallStack = new CCallStackWidget();
	m_pwCallStack->setStyleSheet("background-color:black; color:white; border:2px solid white; font-size:20px");

	pwSplitter->addWidget(m_pwCallStack);

	m_pwAdrRegList->setReadOnly(true);
	m_pwAdrRegList->setStyleSheet("background-color:black; color:white; border:2px solid white; font-size:20px");
	m_pwAdrRegList->setOverwriteMode(false);

	m_pwGPRegList->setReadOnly(true);
	m_pwGPRegList->setStyleSheet("background-color:black; color:white; border:2px solid white; font-size:20px");
	m_pwGPRegList->setOverwriteMode(false);

	m_pwStateInfo->setReadOnly(true);
	m_pwStateInfo->setStyleSheet("background-color:black; color:white; border:2px solid white; font-size:20px");
	m_pwStateInfo->setOverwriteMode(false);

	pwSplitter->setStretchFactor(0, 100);
	pwSplitter->setStretchFactor(1, 20);

	QToolBar* pToolBar = new QToolBar();
	pToolBar->setIconSize(QSize(30, 30));

	m_pActRun = new QAction(QIcon(":/courseWork/Resources/run.png"), "Run");
	m_pActNext = new QAction(QIcon(":/courseWork/Resources/next.png"), "Next");

	connect(m_pActRun, SIGNAL(triggered()), this, SLOT(onRun()));
	connect(m_pActNext, SIGNAL(triggered()), this, SLOT(onNext()));
	pToolBar->addAction(m_pActRun);
	pToolBar->addSeparator();
	pToolBar->addAction(m_pActNext);

	addToolBar(pToolBar);
}
