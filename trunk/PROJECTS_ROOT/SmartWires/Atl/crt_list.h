// list standard header

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _COMLIST
#define _COMLIST

template<class _Ty>
class CCOMListItem
{
	typedef CCOMListItem<_Ty> _Myt;
public:
	CCOMListItem(_Ty in)
	{
		_next=0;
		value=in;
	}
	_Myt* _next;
	_Ty value;
};

template<class _Ty>
class CCOMList{
	typedef CCOMList<_Ty> _Myt;
	typedef CCOMListItem<_Ty> _Myit;
	typedef bool (*_sort)(const _Ty &a, const _Ty &b);

	int iSize;
	_Myit* _begin;
public:

	CCOMList()
	{
		iSize=0;
		_begin=0;
	}
	
	~CCOMList()
	{
		for(int i=0;i<iSize;i++){
			RemoveAt(i);
		}
		_begin=0;
	}
	
	int size()
	{
		return iSize;
	};
	
	_Ty& front()
	{
		return GetAt(0).value;
	}
	_Ty& back()
	{
		return GetAt(size()-1).value;
	}
	
	int push_back(_Ty obj)
	{
		_Myit* pNew=new _Myit(obj);
		pNew->_next=0;
		if(iSize>0){
			_Myit& pLast=GetAt(iSize-1);
			pLast._next=pNew;
		}else{
			_begin=pNew;
		}
		iSize++;
		return iSize;
	};
	
	int clear()
	{
		return RemoveAll();
	};
	
	void merge(_Myt& X)
	{
		for(int i=0;i<X.size();i++){
			push_back(X[i]);
		}
	}
	
	_Ty& operator[] (int nIndex)
	{
		return GetAt(nIndex).value;
	}
	
	_Myit& GetAt(int iIndex)
	{
		_Myit* p=_begin;
		for(int i=0;i<iIndex;i++){
			p=p->_next;
		}
		return *p;
	}
	
	int RemoveAt(int iIndex)
	{
		_Myit* p=_begin;
		if(iIndex==0){
			_begin=_begin->_next;
			delete p;
			iSize--;
			return 1;
		}
		for(int i=0;i<iIndex-1;i++){
			p=p->_next;
		}
		if(p->_next){
			_Myit* pAft=p->_next;
			p->_next=p->_next->_next;
			delete pAft;
			iSize--;
			return 1;
		}
		return 0;
	}
	
	int RemoveAll()
	{
		while(size()){
			RemoveAt(0);
		}
		return 1;
	}
	
	int sort(_sort fp)
	{
		for(int i=0;i<size()-1;i++){
			_Myit& i1=GetAt(i);
			for(int j=i+1;j<size();j++){
				_Myit& i2=GetAt(j);
				if(!fp(i1.value,i2.value)){
					_Ty p=i1.value;
					i1.value=i2.value;
					i2.value=p;
				}
			}
		}
		return 1;
	}
	#ifdef _COMSTR
	// Для этого метода нужно включить crt_cstring.h
	CCOMString toString(CCOMString sDelim=" ")
	{
		CCOMString sRes="";
		for(int i=0;i<size();i++){
			if(i>0){
				sRes+=sDelim;
			}
			sRes+=CCOMString(GetAt(i).value);
		}
		return sRes;
	}
	#endif
};

#endif