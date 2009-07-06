#if !defined(_7134_AVERAPI)
#define _7134_AVERAPI

extern int RC_LowGpioPin;
extern int RC_HighGpioPin;
extern int RC_ResetGpioPin;
extern int RC_StatusGpioPin;
bool InitAPI(void);
bool DeinitAPI(void);
int ReadGPIO(int iPin);
void WriteGPIO(int iPin, int iVal);
int ReadGpioRange(int iLow, int iHigh);

#endif