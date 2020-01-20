#include "Processor.h"
#include <QtMath>
////////////////////////////////////////////////////////////////////////////////
//
//	CProcessor Implementation
//
////////////////////////////////////////////////////////////////////////////////
CProcessor::CProcessor(CMemory* pMemory)	:m_pMemory(pMemory)
{
	m_pState = new SState();
	m_pExecute = new CExecute(m_pState, m_pState->pFlags,m_pMemory);

	if(!pMemory->isEmpty())
		m_pState->nIP = pMemory->operator[]<t_Index>(0);
}

CProcessor::~CProcessor()
{
}

quint64 CProcessor::fetch()
{
	return m_pMemory->operator[]<quint64>(m_pState->nIP);
}

CProcessor::SInstructionSet CProcessor::decode(quint64 instruction)
{
								 //1b	 //2b    //3b   //4b    //5     //6     //7     //8 
	//quint64 instruction = 0b001000000000101100000011000010100000000000000000000000000000000;
	t_Byte *pOffset = (quint8*)(&instruction);
	t_Byte *pStartPoint = pOffset;
	t_Byte opSize_Mask = 0b00000011;
	t_Byte regType_Mask = 0b11000000;
	
	SInstructionSet oInstruction;
	oInstruction.eOpCode = static_cast<EOpCode>(*pOffset);
	pOffset++;
	oInstruction.eOpSize = static_cast<EOpSize>(*pOffset & opSize_Mask);
	oInstruction.eRegsType = static_cast<ERegsType>( (*pOffset & regType_Mask) >> 6 );

	switch (oInstruction.eOpCode)
    {
	// commands with 0 args
    case EOpCode::nop:
	case EOpCode::ret:
	case EOpCode::exit:
	case EOpCode::pushf:
    case EOpCode::popf:
    case EOpCode::pushsf:
    case EOpCode::popsf:
		break;
	///////////////////

	// commands with 1 args(reg)
	case EOpCode::jump:
    break;
	case EOpCode::not:
	case EOpCode::inc:
	case EOpCode::dec:
	case EOpCode::push:
    case EOpCode::pop:
	case EOpCode::neg:
	case EOpCode::in_r:
    case EOpCode::out_r:
	case EOpCode::load:
    case EOpCode::store:
		pOffset++;
		oInstruction.nArg1 = *pOffset;
		break;
	///////////////////////

	// commands wth 1 args(num)
	case EOpCode::call:
	case EOpCode::in_n:
    case EOpCode::out_n:
		pOffset++;
		oInstruction.nNumber = byteReverse<t_DWord>(pOffset);
			pOffset+=4;
        break;

	// commands with 2 args(reg,reg)
	case EOpCode::move:
    case EOpCode::swap:
	case EOpCode::and:
    case EOpCode::or:
    case EOpCode::xor:
    case EOpCode::nand:
    case EOpCode::nor:
    case EOpCode::add:
    case EOpCode::sub:
    case EOpCode::mul:
    case EOpCode::div:
	case EOpCode::test:
    case EOpCode::cmp:
    case EOpCode::shr_r:
    case EOpCode::shl_r:
    case EOpCode::ror_r:
    case EOpCode::rol_r:
	case EOpCode::adc:
    case EOpCode::sbb:
    case EOpCode::imul:
    case EOpCode::idiv:
    case EOpCode::sar_r:
    case EOpCode::sal_r:
    case EOpCode::rcr_r:
    case EOpCode::rcl_r:
		pOffset++;
		oInstruction.nArg1 = *pOffset;
		pOffset++;
		oInstruction.nArg2 = *pOffset;
		break;
	/////////////////////////

	// commands with 2 args(reg,num)
	case EOpCode::shr_n:
    case EOpCode::shl_n:
    case EOpCode::ror_n:
    case EOpCode::rol_n:
    case EOpCode::assign:
	case EOpCode::sar_n:
    case EOpCode::sal_n:
    case EOpCode::rcr_n:
    case EOpCode::rcl_n:
			pOffset++;
			oInstruction.nArg1 = *pOffset;
			pOffset++;

			oInstruction.nNumber = byteReverse<t_DWord>(pOffset);
			pOffset+=4;
            break;
	////////////////////////////////

    default:
            break;
    }
	
	t_Index nCommandSize = pOffset - pStartPoint;
	if (nCommandSize <= 4)
		nCommandSize = 4;
	else if (nCommandSize <= 8)
		nCommandSize=8;
	
	m_pState->nIP += nCommandSize;
	return oInstruction;
}

void CProcessor::execute(SInstructionSet& oInstruction)
{
	m_pExecute->execute(oInstruction);
}
CProcessor::SState* CProcessor::getState()
{
	return m_pState;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CProcessor::CFlags Implementation
//
////////////////////////////////////////////////////////////////////////////////
CProcessor::CFlags::CFlags()
{
	m_aFlags.resize(4);
	m_aFlags.fill(false);
}

void CProcessor::CFlags::setCarryFlag(bool bCarry)
{
	bCarry ? m_aFlags.setBit(0) : m_aFlags.clearBit(0);
}

void CProcessor::CFlags::setZeroFlag(bool bZero)
{
	bZero ? m_aFlags.setBit(1) : m_aFlags.clearBit(1);
}

void CProcessor::CFlags::setSignFlag(bool bSign)
{
	bSign ? m_aFlags.setBit(2) : m_aFlags.clearBit(2);
}

void CProcessor::CFlags::setOverflowFlag(bool bOverflow)
{
	bOverflow ? m_aFlags.setBit(3) : m_aFlags.clearBit(3);
}

bool CProcessor::CFlags::getCarryFlag() const
{
	return m_aFlags.testBit(0);
}

bool CProcessor::CFlags::getZeroFlag() const
{
	return m_aFlags.testBit(1);
}

bool CProcessor::CFlags::getSignFlag() const
{	
	return m_aFlags.testBit(2);
}

bool CProcessor::CFlags::getOverflowFlag() const
{	
	return m_aFlags.testBit(3);
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//	CProcessor::CExecute Implementation
//
////////////////////////////////////////////////////////////////////////////////


CProcessor::CExecute::CExecute(SState * pState, CFlags &oFlags, CMemory* pMemory)
	:m_pCurrState(pState),m_pMemory(pMemory)
{
}

CProcessor::CExecute::~CExecute()
{
}

void CProcessor::CExecute::execute(SInstructionSet &oInstruction)
{
    switch (oInstruction.eOpCode)
    {
    case EOpCode::nop:
            Nop(oInstruction);
            break;
    case EOpCode::jump:
            Jump(oInstruction);
            break;
    case EOpCode::call:
            Call(oInstruction);
            break;
    case EOpCode::ret:
            Ret(oInstruction);
            break;
    case EOpCode::exit:
            Exit(oInstruction);
            break;
    case EOpCode::test:
            Test(oInstruction);
            break;
    case EOpCode::cmp:
            Cmp(oInstruction);
            break;
    case EOpCode::shr_r:
            ShR_R(oInstruction);
            break;
    case EOpCode::shl_r:
            ShL_R(oInstruction);
            break;
    case EOpCode::ror_r:
            RoR_R(oInstruction);
            break;
    case EOpCode::rol_r:
            RoL_R(oInstruction);
            break;
    case EOpCode::shr_n:
            ShR_N(oInstruction);
            break;
    case EOpCode::shl_n:
            ShL_N(oInstruction);
            break;
    case EOpCode::ror_n:
            RoR_N(oInstruction);
            break;
    case EOpCode::rol_n:
            RoL_N(oInstruction);
            break;
    case EOpCode::push:
            Push(oInstruction);
            break;
    case EOpCode::pop:
            Pop(oInstruction);
            break;
    case EOpCode::pushf:
            PushF(oInstruction);
            break;
    case EOpCode::popf:
            PopF(oInstruction);
            break;
    case EOpCode::pushsf:
            PushSF(oInstruction);
            break;
    case EOpCode::popsf:
            PopSF(oInstruction);
            break;
    case EOpCode::assign:
            Assign(oInstruction);
            break;
    case EOpCode::move:
            Move(oInstruction);
            break;
    case EOpCode::swap:
            Swap(oInstruction);
            break;
    case EOpCode::load:
            Load(oInstruction);
            break;
    case EOpCode::store:
            Store(oInstruction);
            break;
    case EOpCode::and:
            And(oInstruction);
            break;
    case EOpCode::or:
            Or(oInstruction);
            break;
    case EOpCode::xor:
            Xor(oInstruction);
            break;
    case EOpCode::not:
            Not(oInstruction);
            break;
    case EOpCode::nand:
            Nand(oInstruction);
            break;
    case EOpCode::nor:
            Nor(oInstruction);
            break;
    case EOpCode::add:
            Add(oInstruction);
            break;
    case EOpCode::sub:
            Sub(oInstruction);
            break;
    case EOpCode::mul:
            Mul(oInstruction);
            break;
    case EOpCode::div:
            Div(oInstruction);
            break;
    case EOpCode::inc:
            Inc(oInstruction);
            break;
    case EOpCode::dec:
            Dec(oInstruction);
            break;
    case EOpCode::in_r:
            In_R(oInstruction);
            break;
    case EOpCode::out_r:
            Out_R(oInstruction);
            break;
    case EOpCode::in_n:
            In_N(oInstruction);
            break;
    case EOpCode::out_n:
            Out_N(oInstruction);
            break;
    case EOpCode::adc:
            Adc(oInstruction);
            break;
    case EOpCode::sbb:
            Sbb(oInstruction);
            break;
    case EOpCode::imul:
            Imul(oInstruction);
            break;
    case EOpCode::idiv:
            IDiv(oInstruction);
            break;
    case EOpCode::neg:
            Neg(oInstruction);
            break;
    case EOpCode::sar_r:
            SaR_R(oInstruction);
            break;
    case EOpCode::sal_r:
            SaL_R(oInstruction);
            break;
    case EOpCode::rcr_r:
            RcR_R(oInstruction);
            break;
    case EOpCode::rcl_r:
            RcL_R(oInstruction);
            break;
    case EOpCode::sar_n:
            SaR_N(oInstruction);
            break;
    case EOpCode::sal_n:
            SaL_L(oInstruction);
            break;
    case EOpCode::rcr_n:
            RcR_N(oInstruction);
            break;
    case EOpCode::rcl_n:
            RcL_N(oInstruction);
            break;
    default:
            break;
    }
}

void CProcessor::CExecute::Nop(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::Jump(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::Call(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::Ret(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::Exit(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::Test(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::Cmp(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
	{
		t_Byte val1 = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_Byte val2 = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (val1 = val2)
			m_pCurrState->pFlags.setZeroFlag(true);		
		break;
	}
	case EOpSize::Word:
	{
		t_Word val1 = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_Word val2 = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (val1 = val2)
			m_pCurrState->pFlags.setZeroFlag(true);
		break;
	}
	case EOpSize::DWord:
	{
		t_DWord val1 = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_DWord val2 = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (val1 = val2)
			m_pCurrState->pFlags.setZeroFlag(true);
		break;
	}
	case EOpSize::QWord:
	{
		t_QWord val1 = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_QWord val2 = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (val1 = val2)
			m_pCurrState->pFlags.setZeroFlag(true);
	}
	default:
		break;
	}
}

void CProcessor::CExecute::ShR_R(SInstructionSet& oInstruction)
{

	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
		(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::Word:		
		(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::DWord:		
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::QWord:		
		(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];		
	default:
		break;
	}
}

void CProcessor::CExecute::ShL_R(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
		(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::Word:
		(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::DWord:
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::QWord:
		(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
	default:
		break;
	}
}
////////////////////////////////////////////
void CProcessor::CExecute::RoR_R(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
	{
		t_Byte shiftCount = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		for (int i = 0; i < shiftCount; ++i)
		{
			t_Byte maskEnd = 1;
			t_Byte bitToShift = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskEnd;
			t_Byte maskBegin = bitToShift * qPow(2, 7);
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= 1;
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskBegin;
		}
		break;
	}
	case EOpSize::Word:
	{
		t_Word shiftCount = (t_Word&)(m_pCurrState->aGPRegisters[oInstruction.nArg2]);
		for (int i = 0; i < shiftCount; ++i)
		{
			t_Word maskEnd = 1;
			t_Word bitToShift = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskEnd;
			t_Word maskBegin = bitToShift * qPow(2, 15);
			(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= 1;
			(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskBegin;
		}
		break;
	}
	case EOpSize::DWord:
	{
		t_DWord shiftCount = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		for (int i = 0; i < shiftCount; ++i)
		{
			t_DWord maskEnd = 1;
			t_DWord bitToShift = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskEnd;
			t_DWord maskBegin = bitToShift * qPow(2, 31);
			(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= 1;
			(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskBegin;
		}
		break;
	}
	case EOpSize::QWord:
	{
		t_QWord shiftCount = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		for (int i = 0; i < shiftCount; ++i)
		{
			t_QWord maskEnd = 1;
			t_QWord bitToShift = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskEnd;
			t_QWord maskBegin = bitToShift * qPow(2, 63);
			(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= 1;
			(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskBegin;
		}
	}
	default:
		break;
	}
}

void CProcessor::CExecute::RoL_R(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
	{
		t_Byte shiftCount = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		for (int i = 0; i < shiftCount; ++i)
		{
			t_Byte maskBegin = qPow(2, 7);			
			t_Byte bitToShift = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskBegin;
			t_Byte maskEnd = bitToShift >> 7;
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= 1;
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskEnd;
		}
		break;
	}
	case EOpSize::Word:
	{
		t_Word shiftCount = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		for (int i = 0; i < shiftCount; ++i)
		{
			t_Word maskBegin = qPow(2, 15);
			t_Word bitToShift = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskBegin;
			t_Word maskEnd = bitToShift >> 15;
			(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= 1;
			(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskEnd;
		}
		break;
	}
	case EOpSize::DWord:
	{
	t_DWord shiftCount = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
	for (int i = 0; i < shiftCount; ++i)
	{
		t_DWord maskBegin = qPow(2, 31);
		t_DWord bitToShift = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskBegin;
		t_DWord maskEnd = bitToShift >> 31;
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= 1;
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskEnd;
	}
	break;
	}
	case EOpSize::QWord:
	{	
		t_QWord shiftCount = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		for (int i = 0; i < shiftCount; ++i)
		{
			t_QWord maskBegin = qPow(2, 63);
			t_QWord bitToShift = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskBegin;
			t_QWord maskEnd = bitToShift >> 63;
			(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= 1;
			(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskEnd;
		}	
	}
	default:
		break;
	}
}

void CProcessor::CExecute::ShR_N(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
		(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= oInstruction.nNumber;
		break;
	case EOpSize::Word:
		(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= oInstruction.nNumber;
		break;
	case EOpSize::DWord:
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= oInstruction.nNumber;
		break;
	case EOpSize::QWord:
		(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= oInstruction.nNumber;
	default:
		break;
	}
}

void CProcessor::CExecute::ShL_N(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
		(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= oInstruction.nNumber;
		break;
	case EOpSize::Word:
		(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= oInstruction.nNumber;
		break;
	case EOpSize::DWord:
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= oInstruction.nNumber;
		break;
	case EOpSize::QWord:
		(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= oInstruction.nNumber;
	default:
		break;
	}
}

void CProcessor::CExecute::RoR_N(SInstructionSet& oInstruction)
{
	t_DWord shiftCount = oInstruction.nNumber;
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
		for (int i = 0; i < shiftCount; ++i)
		{
			t_Byte maskEnd = 1;
			t_Byte bitToShift = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskEnd;
			t_Byte maskBegin = bitToShift * qPow(2, 7);
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= 1;
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskBegin;
		}
		break;
	case EOpSize::Word:
		for (int i = 0; i < shiftCount; ++i)
		{
			t_Word maskEnd = 1;
			t_Word bitToShift = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskEnd;
			t_Word maskBegin = bitToShift * qPow(2, 15);
			(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= 1;
			(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskBegin;
		}
		break;
	case EOpSize::DWord:
		for (int i = 0; i < shiftCount; ++i)
		{
			t_DWord maskEnd = 1;
			t_DWord bitToShift = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskEnd;
			t_DWord maskBegin = bitToShift * qPow(2, 31);
			(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= 1;
			(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskBegin;
		}
		break;
	case EOpSize::QWord:
		for (int i = 0; i < shiftCount; ++i)
		{
			t_QWord maskEnd = 1;
			t_QWord bitToShift = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskEnd;
			t_QWord maskBegin = bitToShift * qPow(2, 63);
			(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] >>= 1;
			(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskBegin;
		}
	default:
		break;
	}
}

void CProcessor::CExecute::RoL_N(SInstructionSet& oInstruction)
{
	t_DWord shiftCount = oInstruction.nNumber;
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
		for (int i = 0; i < shiftCount; ++i)
		{
			t_Byte maskBegin = qPow(2, 7);
			t_Byte bitToShift = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskBegin;
			t_Byte maskEnd = bitToShift >> 7;
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= 1;
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskEnd;
		}
		break;
	case EOpSize::Word:
		for (int i = 0; i < shiftCount; ++i)
		{
			t_Word maskBegin = qPow(2, 15);
			t_Word bitToShift = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskBegin;
			t_Word maskEnd = bitToShift >> 15;
			(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= 1;
			(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskEnd;
		}
		break;
	case EOpSize::DWord:
		for (int i = 0; i < shiftCount; ++i)
		{
			t_DWord maskBegin = qPow(2, 31);
			t_DWord bitToShift = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskBegin;
			t_DWord maskEnd = bitToShift >> 31;
			(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= 1;
			(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskEnd;
		}
		break;
	case EOpSize::QWord:
		for (int i = 0; i < shiftCount; ++i)
		{
			t_QWord maskBegin = qPow(2, 63);
			t_QWord bitToShift = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & maskBegin;
			t_QWord maskEnd = bitToShift >> 63;
			(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] <<= 1;
			(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= maskEnd;
		}
	default:
		break;
	}
}

void CProcessor::CExecute::Push(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::Pop(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::PushF(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::PopF(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::PushSF(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::PopSF(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::Assign(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
		(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = oInstruction.nNumber;
		break;
	case EOpSize::Word:
		(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = oInstruction.nNumber;
		break;
	case EOpSize::DWord:
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = oInstruction.nNumber;
		break;
	case EOpSize::QWord:
		(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = oInstruction.nNumber;
	default:
		break;
	}
}

void CProcessor::CExecute::Move(SInstructionSet& oInstruction)
{
	switch (oInstruction.eRegsType)
	{
	case ERegsType::AARegister:
		m_pCurrState->aARegisters[oInstruction.nArg2] = m_pCurrState->aARegisters[oInstruction.nArg1];
		break;
	case ERegsType::GGRegister:
		for (int ofs = 0; ofs < (int)oInstruction.eOpSize; ofs++)
		{
			m_pCurrState->aGPRegisters[oInstruction.nArg2 + ofs] = m_pCurrState->aGPRegisters[oInstruction.nArg1 + ofs];
			(t_GPReg&)m_pCurrState->aGPRegisters[oInstruction.nArg1 + ofs] = 0;
		}
	default:
		break;
	}	
}

void CProcessor::CExecute::Swap(SInstructionSet& oInstruction)
{
	switch (oInstruction.eRegsType)
	{
	case ERegsType::AARegister:
	{
		t_AReg tmpReg = (t_AReg&) m_pCurrState->aARegisters[oInstruction.nArg2];
		m_pCurrState->aARegisters[oInstruction.nArg2] = m_pCurrState->aARegisters[oInstruction.nArg1];
		(t_AReg&) m_pCurrState->aARegisters[oInstruction.nArg1] = tmpReg;
		break;
	}
	case ERegsType::GGRegister:
	{
		for (int ofs = 0; ofs < (int)oInstruction.eOpSize; ofs++)
		{
			t_GPReg tmpReg = (t_GPReg&) m_pCurrState->aGPRegisters[oInstruction.nArg2 + ofs];
			m_pCurrState->aGPRegisters[oInstruction.nArg2 + ofs] = m_pCurrState->aGPRegisters[oInstruction.nArg1 + ofs];
			(t_GPReg&) m_pCurrState->aGPRegisters[oInstruction.nArg1 + ofs] = tmpReg;
		}		
	}
	default:
		break;
	}
}

void CProcessor::CExecute::Load(SInstructionSet& oInstruction)
{
		if (oInstruction.eRegsType == ERegsType::AARegister)
	{
		t_AReg memIndex = (t_AReg&)m_pCurrState->aARegisters[oInstruction.nArg2];
		(t_AReg&)m_pCurrState->aARegisters[oInstruction.nArg1] = m_pMemory->operator[]<t_AReg>(memIndex);
	}
	else
	{
		if (oInstruction.eRegsType == ERegsType::GARegister)
		{
			t_AReg memIndex = (t_AReg&)m_pCurrState->aARegisters[oInstruction.nArg2];
			switch (oInstruction.eOpSize)
			{
			case EOpSize::Byte:
				(t_Byte&)m_pCurrState->aARegisters[oInstruction.nArg1] = m_pMemory->operator[]<t_AReg>(memIndex);
				break;
			case EOpSize::Word:
				(t_Word&)m_pCurrState->aARegisters[oInstruction.nArg1] = m_pMemory->operator[]<t_Word>(memIndex);
				break;
			case EOpSize::DWord:
				(t_DWord&)m_pCurrState->aARegisters[oInstruction.nArg1] = m_pMemory->operator[]<t_DWord>(memIndex);
				break;
			case EOpSize::QWord:
				(t_QWord&)m_pCurrState->aARegisters[oInstruction.nArg1] = m_pMemory->operator[]<t_QWord>(memIndex);
			default:
				break;
			}
		}
	}
}

void CProcessor::CExecute::Store(SInstructionSet& oInstruction)
{
	if (oInstruction.eRegsType == ERegsType::AARegister)
	{
		t_AReg memIndex = (t_AReg&)m_pCurrState->aARegisters[oInstruction.nArg2];
		m_pMemory->operator[]<t_AReg>(memIndex) = (t_AReg&)m_pCurrState->aARegisters[oInstruction.nArg1];
	}
	else
	{
		if (oInstruction.eRegsType == ERegsType::GARegister)
		{
			t_AReg memIndex = (t_AReg&)m_pCurrState->aARegisters[oInstruction.nArg2];
			switch (oInstruction.eOpSize)
			{
			case EOpSize::Byte:
				m_pMemory->operator[]<t_AReg>(memIndex) = (t_Byte&)m_pCurrState->aARegisters[oInstruction.nArg1];
				break;
			case EOpSize::Word:
				m_pMemory->operator[]<t_Word>(memIndex) = (t_Word&)m_pCurrState->aARegisters[oInstruction.nArg1];
				break;
			case EOpSize::DWord:
				m_pMemory->operator[]<t_DWord>(memIndex) = (t_DWord&)m_pCurrState->aARegisters[oInstruction.nArg1];
				break;
			case EOpSize::QWord:
				m_pMemory->operator[]<t_QWord>(memIndex) = (t_QWord&)m_pCurrState->aARegisters[oInstruction.nArg1];
			default:
				break;
			}
		}
	}
}

void CProcessor::CExecute::And(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
		(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] &= (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::Word:
		(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] &= (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::DWord:
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] &= (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::QWord:
		(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] &= (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
	default:
		break;
	}
}

void CProcessor::CExecute::Or(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
		(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::Word:
		(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::DWord:
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::QWord:
		(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] |= (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
	default:
		break;
	}
}

void CProcessor::CExecute::Xor(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
		(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] ^= (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::Word:
		(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] ^= (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::DWord:
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] ^= (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		break;
	case EOpSize::QWord:
		(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] ^= (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
	default:
		break;
	}
}

void CProcessor::CExecute::Not(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
		(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = ~(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		break;
	case EOpSize::Word:
		(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = ~(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		break;
	case EOpSize::DWord:
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = ~(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		break;
	case EOpSize::QWord:
		(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = ~(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
	default:
		break;
	}
}

void CProcessor::CExecute::Nand(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
		(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = 
			~((t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2]);
		break;
	case EOpSize::Word:
		(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = 
			~((t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg2]);
		break;
	case EOpSize::DWord:
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = 
			~((t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2]);
		break;
	case EOpSize::QWord:
		(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = 
			~((t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] & (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2]);
	default:
		break;
	}
}

void CProcessor::CExecute::Nor(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
		(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = 
			~((t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] | (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2]);
		break;
	case EOpSize::Word:
		(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = 
			~((t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] | (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg2]);
		break;
	case EOpSize::DWord:
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = 
			~((t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] | (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2]);
		break;
	case EOpSize::QWord:
		(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = 
			~((t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] | (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2]);
	default:
		break;
	}
}

void CProcessor::CExecute::Add(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
	{
		t_Byte val1 = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_Byte val2 = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (t_ByteMax - val1 < val2)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 + val2;
		break;
	}
	case EOpSize::Word:
	{
		t_Word val1 = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_Word val2 = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (t_WordMax - val1 < val2)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 + val2;
		break;
	}
	case EOpSize::DWord:
	{
		t_DWord val1 = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_DWord val2 = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (t_DWordMax - val1 < val2)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 + val2;
		break;
	}
	case EOpSize::QWord:
	{
		t_QWord val1 = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_QWord val2 = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (t_QWordMax - val1 < val2)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 + val2;
		break;
	}
	default:
		break;
	}
}

void CProcessor::CExecute::Sub(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
	{
		t_Byte val1 = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_Byte val2 = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (val1 < val2)
			m_pCurrState->pFlags.setSignFlag(true);
		else
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 - val2;
		break;
	}
	case EOpSize::Word:
	{
		t_Word val1 = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_Word val2 = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (val1 < val2)
			m_pCurrState->pFlags.setSignFlag(true);
		else
			(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 - val2;
		break;
	}
	case EOpSize::DWord:
	{
		t_DWord val1 = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_DWord val2 = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (val1 < val2)
			m_pCurrState->pFlags.setSignFlag(true);
		else
			(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 - val2;
		break;
	}
	case EOpSize::QWord:
	{
		t_QWord val1 = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_QWord val2 = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (val1 < val2)
			m_pCurrState->pFlags.setSignFlag(true);
		else
			(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 - val2;
	}
	default:
		break;
	}
}

void CProcessor::CExecute::Mul(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
	{
		t_Byte val1 = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_Byte val2 = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (t_ByteMax / val1 < val2)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 * val2;
		break;
	}
	case EOpSize::Word:
	{
		t_Word val1 = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_Word val2 = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (t_WordMax / val1 < val2)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 * val2;
		break;
	}
	case EOpSize::DWord:
	{
		t_DWord val1 = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_DWord val2 = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (t_DWordMax / val1 < val2)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 * val2;
		break;
	}
	case EOpSize::QWord:
	{
		t_QWord val1 = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_QWord val2 = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (t_QWordMax / val1 < val2)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 * val2;
		break;
	}
	default:
		break;
	}
}

void CProcessor::CExecute::Div(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
	{
		t_Byte val1 = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_Byte val2 = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		//if (val2 == 0)
			//TODO throw exeption
		//else
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 / val2;
		break;
	}
	case EOpSize::Word:
	{
		t_Word val1 = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_Word val2 = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		//if (val2 == 0)
			//TODO throw exeption
		//else
		(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 / val2;
		break;
	}
	case EOpSize::DWord:
	{
		t_DWord val1 = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_DWord val2 = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		//if (val2 == 0)
			//TODO throw exeption
		//else
		(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 / val2;
		break;
	}
	case EOpSize::QWord:
	{
		t_QWord val1 = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_QWord val2 = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		//if (val2 == 0)
			//TODO throw exeption
		//else
		(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 / val2;
	}
	default:
		break;
	}
}

void CProcessor::CExecute::Inc(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
	{
		t_Byte val1 = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
				if (t_ByteMax - val1 < 1)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 + 1;
		break;
	}
	case EOpSize::Word:
	{
		t_Word val1 = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		if (t_WordMax - val1 < 1)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 + 1;
		break;
	}
	case EOpSize::DWord:
	{
		t_DWord val1 = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		if (t_DWordMax - val1 < 1)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 + 1;
		break;
	}
	case EOpSize::QWord:
	{
		t_QWord val1 = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		if (t_QWordMax - val1 < 1)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 + 1;
	}
	default:
		break;
	}
}

void CProcessor::CExecute::Dec(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
	{
		t_Byte val1 = (t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		if (val1 < 1)
			m_pCurrState->pFlags.setSignFlag(true);
		else
			(t_Byte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 - 1;
		break;
	}
	case EOpSize::Word:
	{
		t_Word val1 = (t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		if (val1 < 1)
			m_pCurrState->pFlags.setSignFlag(true);
		else
			(t_Word&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 - 1;
		break;
	}
	case EOpSize::DWord:
	{
		t_DWord val1 = (t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		if (val1 < 1)
			m_pCurrState->pFlags.setSignFlag(true);
		else
			(t_DWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 - 1;
		break;
	}
	case EOpSize::QWord:
	{
		t_QWord val1 = (t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		if (val1 < 1)
			m_pCurrState->pFlags.setSignFlag(true);
		else
			(t_QWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 - 1;
	}
	default:
		break;
	}
}

void CProcessor::CExecute::In_R(SInstructionSet& oInstruction)
{
	//TODO
}

void CProcessor::CExecute::Out_R(SInstructionSet& oInstruction)
{
	//TODO
}

void CProcessor::CExecute::In_N(SInstructionSet& oInstruction)
{
	//TODO
}

void CProcessor::CExecute::Out_N(SInstructionSet& oInstruction)
{
	//TODO
}

void CProcessor::CExecute::Adc(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::Sbb(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::Imul(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
	{
		t_SByte val1 = (t_SByte&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_SByte val2 = (t_SByte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (t_SByteMax / val1 < val2)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_SByte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 * val2;
		break;
	}
	case EOpSize::Word:
	{
		t_SWord val1 = (t_SWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_SWord val2 = (t_SWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (t_SWordMax / val1 < val2)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_SWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 * val2;
		break;
	}
	case EOpSize::DWord:
	{
		t_SDWord val1 = (t_SDWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_SDWord val2 = (t_SDWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (t_SDWordMax / val1 < val2)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_SDWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 * val2;
		break;
	}
	case EOpSize::QWord:
	{
		t_SQWord val1 = (t_SQWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_SQWord val2 = (t_SQWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		if (t_SQWordMax / val1 < val2)
			m_pCurrState->pFlags.setOverflowFlag(true);
		else
			(t_SQWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 * val2;
		break;
	}
	default:
		break;
	}
}

void CProcessor::CExecute::IDiv(SInstructionSet& oInstruction)
{
	switch (oInstruction.eOpSize)
	{
	case EOpSize::Byte:
	{
		t_SByte val1 = (t_SByte&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_SByte val2 = (t_SByte&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		//if (val2 == 0)
			//TODO throw exeption
		//else
		(t_SByte&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 / val2;
		break;
	}
	case EOpSize::Word:
	{
		t_SWord val1 = (t_SWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_SWord val2 = (t_SWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		//if (val2 == 0)
			//TODO throw exeption
		//else
		(t_SWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 / val2;
		break;
	}
	case EOpSize::DWord:
	{
		t_SDWord val1 = (t_SDWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_SDWord val2 = (t_SDWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		//if (val2 == 0)
			//TODO throw exeption
		//else
		(t_SDWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 / val2;
		break;
	}
	case EOpSize::QWord:
	{
		t_SQWord val1 = (t_SQWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1];
		t_SQWord val2 = (t_SQWord&)m_pCurrState->aGPRegisters[oInstruction.nArg2];
		//if (val2 == 0)
			//TODO throw exeption
		//else
		(t_SQWord&)m_pCurrState->aGPRegisters[oInstruction.nArg1] = val1 / val2;
	}
	default:
		break;
	}
}

void CProcessor::CExecute::Neg(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::SaR_R(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::SaL_R(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::RcR_R(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::RcL_R(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::SaR_N(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::SaL_L(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::RcR_N(SInstructionSet& oInstruction)
{
}

void CProcessor::CExecute::RcL_N(SInstructionSet& oInstruction)
{
}

////////////////////////////////////////////////////////////////////////////////

//QString CProcessor::SState::toString() const
//{
//	QString sData;
//
//	sData.append("IP: ");
//	sData.append(binarToString(nIP) + "\n\n Address Registers: \n");
//	
//	for (int i = 0; i < aARegisters.count(); ++i)
//	{
//		if (i == 0)
//			sData.append("SP =" +  binarToString(aARegisters[i]) + "\n");
//		else if (i == 1)
//			sData.append("SF =" +  binarToString(aARegisters[i]) + "\n");
//		else
//		{
//			sData.append("AReg_"+ QString::number(i)+ " = " + binarToString(aARegisters[i]) + "\n");
//		}
//	}
//	
//	sData.append("\n\n GPRegisters: ");
//
//	for (int i = 0; i < aGPRegisters.count(); ++i)
//	{
//		sData.append("GPReg[" + QString::number(i) + "] " + binarToString(aGPRegisters[i]) + " ");
//		if (i%10==0) sData.append("\n");
//	}
//
//	sData.append("\n\n Flags: ");
//
//	(pFlags->getCarryFlag()) ? sData.append("CF 1 ") : sData.append("CF 0 "); 
//	(pFlags->getZeroFlag()) ? sData.append("ZF 1 ") : sData.append("ZF 0 "); 
//	(pFlags->getOverflowFlag()) ? sData.append("OF 1 ") : sData.append("OF 0 "); 
//	(pFlags->getSignFlag()) ? sData.append("SF 1 ") : sData.append("SF 0 "); 
//
//	sData.append("\n\n Flags: ");
//
//	return sData;
//}

QString binarToString(quint8 nSource)
{
	QString sData;
	
	quint8 mask = 1;
    int nCount = 0;
	while (nSource)
	{
		nSource & mask ? sData.prepend("1") : sData.prepend("0");
		nSource >>= 1;
		nCount++;
	}
	if(sData.isEmpty())
		return QString("00000000");

	while (sData.size() != 8)
	{
		sData.prepend("0");
	}
	return sData;
}
