#include "stdafx.h"

HMODULE hMod;
typedef void fnHolder(void);
fnHolder * I2C_Create;
fnHolder * I2C_SetSlave;
fnHolder * I2C_ReadSeq;
fnHolder * I2C_ReadFromGPIONr;
fnHolder * I2C_WriteGPIONr;
fnHolder * I2C_DVampGPIO;
fnHolder * I2C_CVampGPIO;
char AudioDecoder_Space[100]={0};
char I2C_Space[100]={0};
char gpio[3000]={0};
int RC_LowGpioPin=0x01;//0x08; 
int RC_HighGpioPin=0x0d;//0x0c;
int RC_ResetGpioPin=0x0f;
int RC_StatusGpioPin=0x0e;

bool IsAverPresent(void)
{
	hMod = LoadLibrary("34api.dll");
    if (!hMod) return false;
    I2C_Create = (fnHolder *) GetProcAddress(hMod,"??0CVampI2c@@QAE@KH@Z");
    I2C_SetSlave = (fnHolder *) GetProcAddress(hMod,"?SetSlave@CVampI2c@@QAEXH@Z");
    I2C_ReadSeq = (fnHolder *) GetProcAddress(hMod,"?ReadSeq@CVampI2c@@QAE?AW4eI2cStatus@@PAEH@Z");
	I2C_ReadFromGPIONr=(fnHolder *) GetProcAddress(hMod, "?ReadFromGPIONr@CVampGPIOStatic@@QAEJEPAE@Z");
	I2C_WriteGPIONr=(fnHolder *) GetProcAddress(hMod, "?WriteToGPIONr@CVampGPIOStatic@@QAEJEE@Z");
	I2C_CVampGPIO=(fnHolder *) GetProcAddress(hMod, "??0CVampGPIOStatic@@QAE@K@Z");
	I2C_DVampGPIO=(fnHolder *) GetProcAddress(hMod, "??1CVampGPIOStatic@@UAE@XZ");
	if(!I2C_CVampGPIO || !I2C_DVampGPIO || !I2C_ReadFromGPIONr || !I2C_WriteGPIONr){
		return false;
	}
	return true;
}

bool InitAPI(void)
{
	if(IsAverPresent()){
		memset(gpio,0,sizeof(gpio));
		_asm
		{
			push 0
				mov ecx,offset gpio
				call I2C_CVampGPIO
		}
		return true;
	}
	return false;
}

bool DeinitAPI(void)
{
	_asm
    {
		mov ecx,offset gpio
		call I2C_DVampGPIO
    }
	FreeLibrary(hMod);
	return true;
}


void WriteGPIO(int iPin, int iVal)
{
	_asm
	{
		mov eax,iVal
		mov ecx,iPin
		push eax
		push ecx
		mov ecx,offset gpio
		call I2C_WriteGPIONr
	}
};

int ReadGPIO(int iPin)
{
	int iRes=0;
	_asm
	{
		lea ecx,iRes
		xor edx,edx
		mov [ecx],edx
		push ecx
		push iPin
		mov ecx,offset gpio
		call I2C_ReadFromGPIONr
	}
	return iRes;
}

int ReadGpioRange(int iLow, int iHigh)
{
	int mask=0,value=0;
	for(int i=iLow;i<iHigh;i++){
		value|=ReadGPIO(i)<<mask;
		mask++;
	}
	return value;
}
