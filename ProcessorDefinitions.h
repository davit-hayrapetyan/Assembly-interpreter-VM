#ifndef ProcessorDefinitions_H
#define ProcessorDefinitions_H

#include <QtCore/qglobal.h>

/////unsigned
using t_Byte  = quint8;
using t_Word  = quint16;
using t_DWord = quint32;
using t_QWord = quint64;
/////signed
using t_SByte = qint8;
using t_SWord = qint16;
using t_SDWord = qint32;
using t_SQWord = qint64;

using t_GPReg = quint8; // General Purpose Register
using t_AReg  = quint32; // Adrress Register
using t_Index = quint8; // Adress Index

/////unsigned limits
const quint8 t_ByteMax = 255;
const quint16 t_WordMax = 65535;
const quint32 t_DWordMax = 4294967295;
const quint64 t_QWordMax = 18446744073709551615;

/////signed limits
const qint8 t_SByteMax = 127;
const qint16 t_SWordMax = 32767;
const qint32 t_SDWordMax = 2147483647;
const qint64 t_SQWordMax = 9223372036854775807;

const qint8 t_SByteMin = -128;
const qint16 t_SWordMin = -32768;
const qint32 t_SDWordMin = -2147483648;
const qint64 t_SQWordMin = -9223372036854775808;



enum class EOpCode : quint8
{
	nop    =   0b00000000,
    jump   =   0b00000001,
    call   =   0b00000010,
    ret    =   0b00000011,
    exit   =   0b00000100,
    test   =   0b00000101,
    cmp    =   0b00000110,
    shr_r  =   0b00000111,
    shl_r  =   0b00001000,
    ror_r  =   0b00001001,
    rol_r  =   0b00001010,
    shr_n  =   0b00001011,
    shl_n  =   0b00001100,
    ror_n  =   0b00001101,
    rol_n  =   0b00001110,
    push   =   0b00001111,
    pop    =   0b00010000,
    pushf  =   0b00010001,
    popf   =   0b00010010,
    pushsf =   0b00010011,
    popsf  =   0b00010100,
    assign =   0b00010101,
    move   =   0b00010110,
    swap   =   0b00010111,
    load   =   0b00011000,
    store  =   0b00011001,
    and    =   0b00011010,
    or     =   0b00011011,
    xor    =   0b00011100,
    not    =   0b00011101,
    nand   =   0b00011110,
    nor    =   0b00011111,
    add    =   0b00100000,
    sub    =   0b00100001,
    mul    =   0b00100010,
    div    =   0b00100011,
    inc    =   0b00100100,
    dec    =   0b00100101,
    in_r   =   0b00100110,
    out_r  =   0b00100111,
    in_n   =   0b00101000,
    out_n  =   0b00101001,
    adc    =   0b00101010,
    sbb    =   0b00101011,
    imul   =   0b00101100,
    idiv   =   0b00101101,
    neg    =   0b00101110,
    sar_r  =   0b00101111,
    sal_r  =   0b00110000,
    rcr_r  =   0b00110001,
    rcl_r  =   0b00110010,
    sar_n  =   0b00110011,
    sal_n  =   0b00110100,
    rcr_n  =   0b00110101,
    rcl_n  =   0b00110110,
};

enum class EConditionalOpCode : qint8
{
	je	 =	0b0000,
	jz	 =	0b0000,
	jne  =	0b0001,
	jnz  =	0b0001,
	ja	 =	0b0010,
	jnbe =	0b0010,
	jae  =	0b0011,
	jnb  =	0b0011,
	jb   =	0b0100,
	jnae =	0b0100,
	jbe  =	0b0101,
	jna  =	0b0101,
	jg   =	0b0110,
	jnle =	0b0110,
	jge  =	0b0111,
	jnl  =	0b0111,
	jl   =	0b1000,
	jnge =	0b1000,
	jle  =	0b1001,
	jng  =	0b1001,
	jo   =	0b1010,
	jno  =	0b1011,
	js   =	0b1100,
	jns  =	0b1101,
	jp   =	0b1110,
	jnp  =	0b1111
};

enum class ERegisterCount
{
	ARegister  = 8,
	GPRegister = 64
};

enum class ERegsType
{
	AARegister  =  0b00,   //  Address Address Register
	AGRegister  =  0b01,   //  Address Genereal Register
	GARegister  =  0b10,   //  Genereal Address Register
	GGRegister  =  0b11,   //  Genereal General Register
};

enum class EOpSize
{
	Byte  = 0b00,
	Word  = 0b01,
	DWord = 0b10,
	QWord = 0b11
};

#endif