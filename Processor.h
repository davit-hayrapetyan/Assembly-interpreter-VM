#ifndef Processor_H
#define Processor_H

#include <QtCore/qglobal.h>
#include <QVector>
#include <QBitArray>

#include "ProcessorDefinitions.h"
#include "Memory.h"

QString binarToString(quint8 nSource);

class CProcessor
{
public:

	struct SState;
	CProcessor(CMemory* pMemory);
	CProcessor() {};
	~CProcessor();

	struct SInstructionSet;

	//void init(CMemoryPtr pMemory, IPortsPtr pPorts, t_size nStackSize);

	quint64 fetch();
	SInstructionSet decode(quint64 instruction);
	void execute(SInstructionSet& oInstruction);

	SState* getState();

	struct SInstructionSet
	{
		EOpCode eOpCode;
		EOpSize eOpSize;
		ERegsType eRegsType;

		t_Index nArg1;
		t_Index nArg2;
		t_DWord nNumber;
	};

	class CFlags
	{
	public:
		CFlags();

		void setCarryFlag(bool bCarry);
		void setZeroFlag(bool bZero);
		void setSignFlag(bool bSign);
		void setOverflowFlag(bool bOverflow);

		bool getCarryFlag() const;
		bool getZeroFlag() const;
		bool getSignFlag() const;
		bool getOverflowFlag() const;
	private:
		QBitArray m_aFlags; // CF, ZF, SF, OF
	};

	struct SState
	{
		SState() : nSP((t_AReg&)aARegisters[0]), nSF((t_AReg&)aARegisters[1])
		{
  		}

		SState(SState const& rhs)
		{
			pFlags = rhs.pFlags;
			aARegisters = rhs.aARegisters;
			aGPRegisters = rhs.aGPRegisters;
			nSP = rhs.nSP;
			nSF = rhs.nSF;
			nIP = rhs.nIP;
			bIsActive = rhs.bIsActive;
		}

		SState& operator=(SState const& rhs)
		{
			if (this == &rhs)
				return *this;

			pFlags = rhs.pFlags;
			aARegisters = rhs.aARegisters;
			aGPRegisters = rhs.aGPRegisters;
			nSP = rhs.nSP;
			nSF = rhs.nSF;
			nIP = rhs.nIP;
			bIsActive = rhs.bIsActive;

			return *this;
		}

		QVector<t_AReg>  aARegisters = QVector<t_AReg>((int)ERegisterCount::ARegister);
		QVector<t_GPReg>  aGPRegisters = QVector<t_GPReg>((int)ERegisterCount::GPRegister);
	
		CFlags  pFlags;
		t_AReg& nSP = aARegisters[0];
		t_AReg& nSF = aARegisters[1];
		t_Index nIP = 0;

		bool bIsActive = true;
	};

private:
	class CExecute
    {
		SState* m_pCurrState;
		CFlags m_oCurrFlags;
		CMemory* m_pMemory;
	public:
        CExecute(SState* pState,CFlags &oFlags, CMemory* pMemory);
        ~CExecute();
        
		void execute(SInstructionSet &oInstruction);

        void Nop(SInstructionSet	&oInstruction);
        void Jump(SInstructionSet	&oInstruction);
        void Call(SInstructionSet	&oInstruction);
        void Ret(SInstructionSet	&oInstruction);
        void Exit(SInstructionSet	&oInstruction);
        void Test(SInstructionSet	&oInstruction);
        void Cmp(SInstructionSet	&oInstruction);
        void ShR_R(SInstructionSet	&oInstruction);
        void ShL_R(SInstructionSet	&oInstruction);
        void RoR_R(SInstructionSet	&oInstruction);
        void RoL_R(SInstructionSet	&oInstruction);
        void ShR_N(SInstructionSet	&oInstruction);
        void ShL_N(SInstructionSet	&oInstruction);
        void RoR_N(SInstructionSet	&oInstruction);
        void RoL_N(SInstructionSet	&oInstruction);
        void Push(SInstructionSet	&oInstruction);
        void Pop(SInstructionSet	&oInstruction);
        void PushF(SInstructionSet	&oInstruction);
        void PopF(SInstructionSet	&oInstruction);
        void PushSF(SInstructionSet &oInstruction);
        void PopSF(SInstructionSet	&oInstruction);
        void Assign(SInstructionSet &oInstruction);
        void Move(SInstructionSet	&oInstruction);
        void Swap(SInstructionSet	&oInstruction);
        void Load(SInstructionSet	&oInstruction);
        void Store(SInstructionSet	&oInstruction);
        void And(SInstructionSet	&oInstruction);
        void Or(SInstructionSet		&oInstruction);
        void Xor(SInstructionSet	&oInstruction);
        void Not(SInstructionSet	&oInstruction);
        void Nand(SInstructionSet	&oInstruction);
        void Nor(SInstructionSet	&oInstruction);
        void Add(SInstructionSet	&oInstruction);
        void Sub(SInstructionSet	&oInstruction);
        void Mul(SInstructionSet	&oInstruction);
        void Div(SInstructionSet	&oInstruction);
        void Inc(SInstructionSet	&oInstruction);
        void Dec(SInstructionSet	&oInstruction);
        void In_R(SInstructionSet	&oInstruction);
        void Out_R(SInstructionSet	&oInstruction);
        void In_N(SInstructionSet	&oInstruction);
        void Out_N(SInstructionSet	&oInstruction);
        void Adc(SInstructionSet	&oInstruction);
        void Sbb(SInstructionSet	&oInstruction);
        void Imul(SInstructionSet	&oInstruction);
        void IDiv(SInstructionSet	&oInstruction);
        void Neg(SInstructionSet	&oInstruction);
        void SaR_R(SInstructionSet	&oInstruction);
        void SaL_R(SInstructionSet	&oInstruction);
        void RcR_R(SInstructionSet	&oInstruction);
        void RcL_R(SInstructionSet	&oInstruction);
        void SaR_N(SInstructionSet	&oInstruction);
        void SaL_L(SInstructionSet	&oInstruction);
        void RcR_N(SInstructionSet	&oInstruction);
        void RcL_N(SInstructionSet	&oInstruction);
    };

	CMemory*  m_pMemory = nullptr;
	SState*   m_pState = nullptr;
	CExecute* m_pExecute = nullptr;


	//helper functions
	template <typename T>
	T byteReverse(t_Byte* pStart)
	{
		QVector<t_Byte> tmpVec;
		int nCount = sizeof(T)/sizeof(t_Byte);
		for(int i = 0; i < nCount; i++)
		{
			tmpVec.push_front(*pStart++);
		}
		return (T&)tmpVec[0];
	}
};

#endif // Processor_H
