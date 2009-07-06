#include "stdafx.h"
#include "exports.h"

// Класс - одиночное значение
XmlRpcValue::XmlRpcValue(long Data)
{
	InitializeValue(XmlRpcLong, &Data);
};

XmlRpcValue::XmlRpcValue(double Data)
{
	InitializeValue(XmlRpcDouble, &Data);
};

XmlRpcValue::XmlRpcValue(const char* Data)
{
	InitializeValue(XmlRpcString, (LPVOID)Data);
};

XmlRpcValue::XmlRpcValue(XmlRpcValue* value)
{
	InitializeValue(value->m_iType, value->m_pData);
}

BOOL XmlRpcValue::InitializeValue(int iType, void* pData, int iDataLength)
{
	m_iType=iType;
	m_pData=NULL;
	if(pData==NULL){
		return FALSE;
	}
	BOOL bRes=TRUE;
	switch(iType){
	case XmlRpcLong:
		m_pData=new long;
		*((long*)m_pData)=*((long*)pData);
		break;
	case XmlRpcDouble:
		m_pData=new double;
		*((double*)m_pData)=*((double*)pData);
		break;
	case XmlRpcString:
		if(iDataLength==0){
			iDataLength=strlen((char*)pData)+1;
		}
		m_pData=new char[iDataLength];
		memcpy(m_pData,pData,iDataLength);
		break;
	default:
		bRes=FALSE;
		m_iType=-1;
		break;
	}
	return bRes;
};

XmlRpcValue* XmlRpcValue::Copy()
{
	return new XmlRpcValue(this);
}

XmlRpcValue::~XmlRpcValue()
{
	switch(m_iType){
	case XmlRpcLong:
		delete (long*)m_pData;
		break;
	case XmlRpcDouble:
		delete (double*)m_pData;
		break;
	case XmlRpcString:
		delete (char*)m_pData;
		break;
	default:
		break;
	}
	m_pData=NULL;
};

void XmlRpcValues::RemoveAll()
{
	for(int i=0;i<aValues.GetSize();i++){
		delete aValues[i];
	}
	aValues.RemoveAll();
}

XmlRpcValues::~XmlRpcValues()
{
	RemoveAll();
};

BOOL XmlRpcValues::AddValue(XmlRpcValue* pValue)
{
	aValues.Add(pValue);
	return TRUE;
};

int XmlRpcValues::GetSize()
{
	return aValues.GetSize();
};

XmlRpcValue* XmlRpcValues::GetValue(int iIndex, CActionError& error)
{
	error.resetError();
	if(iIndex<0 || iIndex>=aValues.GetSize()){
		error.setError(AE_IndexOutOfBounds);
		return NULL;
	}
	return aValues[iIndex];
};

const char* XmlRpcValues::GetValueType(int iIndex, CActionError& error)
{
	XmlRpcValue* pValue=GetValue(iIndex,error);
	if(pValue==NULL){
		return NULL;
	}
	switch(pValue->m_iType){
	case XmlRpcLong:
		return "int";
	case XmlRpcDouble:
		return "double";
	case XmlRpcString:
		return "string";
	default:
		break;
	}
	return NULL;
};

BOOL XmlRpcValues::ConvertValueToString(int iIndex, CString& sRes, CActionError& error)
{	
	XmlRpcValue* pValue=GetValue(iIndex,error);
	if(pValue==NULL){
		return FALSE;
	}
	switch(pValue->m_iType){
	case XmlRpcLong:
		{
			long val=*((long*)pValue->m_pData);
			sRes.Format("%i",val);
			return TRUE;
		}
	case XmlRpcDouble:
		{
			double val=*((double*)pValue->m_pData);
			sRes.Format("%f",val);
			return TRUE;
		}
	case XmlRpcString:
		{
			sRes=(const char*)pValue->m_pData;
			return TRUE;
		}
	default:
		break;
	}
	return FALSE;
};

long XmlRpcValues::GetAsLong(int iIndex, CActionError& error)
{
	XmlRpcValue* pValue=GetValue(iIndex, error);
	if(pValue==NULL){
		return 0;
	}
	if(pValue->m_iType!=XmlRpcLong){
		error.setError(AE_UnsupportedValueType);
		return 0;
	}
	return *((long*)pValue->m_pData);
};

double XmlRpcValues::GetAsDouble(int iIndex, CActionError& error)
{
	XmlRpcValue* pValue=GetValue(iIndex, error);
	if(pValue==NULL){
		return 0;
	}
	if(pValue->m_iType!=XmlRpcDouble){
		error.setError(AE_UnsupportedValueType);
		return 0;
	}
	return *((double*)pValue->m_pData);
};

const char* XmlRpcValues::GetAsString(int iIndex, CActionError& error)
{	
	XmlRpcValue* pValue=GetValue(iIndex, error);
	if(pValue==NULL){
		return 0;
	}
	if(pValue->m_iType!=XmlRpcString){
		error.setError(AE_UnsupportedValueType);
		return 0;
	}
	return (const char*)pValue->m_pData;
};

XmlRpcValues& XmlRpcValues::operator=(XmlRpcValues& obj)
{
	RemoveAll();
	for(int i=0; i<obj.aValues.GetSize();i++){
		aValues.Add(obj.aValues[i]->Copy());
	}
	return *this;
}

XmlRpcValues& XmlRpcValues::operator<<(XmlRpcValue* value)
{
	AddValue(value);
	return *this;
}

XmlRpcValues& XmlRpcValues::operator<<(const char* value)
{
	AddValue(new XmlRpcValue(value));
	return *this;
}

XmlRpcValues& XmlRpcValues::operator<<(long value)
{
	AddValue(new XmlRpcValue(value));
	return *this;
}

XmlRpcValues& XmlRpcValues::operator<<(double value)
{
	AddValue(new XmlRpcValue(value));
	return *this;
}