/********************************************************************
  created:   2005/05/27
  created:   27:5:2005   10:11
  filename:  C:\tool\MiniXML\MiniXML.h
  file path: C:\tool\MiniXML
  file ext:  h
  author:    Richard Lin
  
  purpose:
  Define class CElement, class CElementIterator, and class CXmlConf.
  Basically, users use those three classes to access the XML data.

  CXmlConf will acquire XML data from the given file or char* buffer.
  The CElement::Parse will parse the data stored in the internal buffer
  and create a DOM tree with root of CElement*  mp_poElementRoot, a member of 
  CXmlConf.

  CElement is the class that users can access Attributes, Chardata,
  element name and subElements.

  CElementIterator is a helper class to access the Children elements of
  a CElement.

  This file also define a global function called CElement* Clone (CElement* p);
  This function is majorily for demo purpose which shows how to use the public
functions provided by CElement and CElementiter.
 
*********************************************************************/

#ifndef _MINI_XML_H
#define _MINI_XML_H
#include <string>
#include <vector>
#include "new_stl_algorithm.h"
using namespace std;

/*
		// TEMPLATE FUNCTION find
template<class _InIt,
	class _Ty> inline
	_InIt find(_InIt _First, _InIt _Last, const _Ty& _Val)
	{	// find first matching _Val
	for (; _First != _Last; ++_First)
		if (*_First == _Val)
			break;
	return (_First);
	}

inline const char *find(const char *_First, const char *_Last, int _Val)
	{	// find first char that matches _Val
	_First = (const char *)::memchr(_First, _Val, _Last - _First);
	return (_First == 0 ? _Last : _First);
	}

inline const signed char *find(const signed char *_First,
	const signed char *_Last, int _Val)
	{	// find first signed char that matches _Val
	_First = (const signed char *)::memchr(_First, _Val,
		_Last - _First);
	return (_First == 0 ? _Last : _First);
	}

inline const unsigned char *find(const unsigned char *_First,
	const unsigned char *_Last, int _Val)
	{	// find first unsigned char that matches _Val
	_First = (const unsigned char *)::memchr(_First, _Val,
		_Last - _First);
	return (_First == 0 ? _Last : _First);
	}

		// TEMPLATE FUNCTION _Dist_type
template<class _Iter> inline
	typename iterator_traits<_Iter>::difference_type
		*_Dist_type(_Iter)
	{	// return distance type from arbitrary argument
	return (0);
	}

		// TEMPLATE FUNCTION search
template<class _FwdIt1,
	class _FwdIt2,
	class _Diff1,
	class _Diff2> inline
	_FwdIt1 _Search(_FwdIt1 _First1, _FwdIt1 _Last1,
		_FwdIt2 _First2, _FwdIt2 _Last2, _Diff1 *, _Diff2 *)
	{	// find first [_First2, _Last2) match
	_Diff1 _Count1 = 0;
	_Distance(_First1, _Last1, _Count1);
	_Diff2 _Count2 = 0;
	_Distance(_First2, _Last2, _Count2);

	for (; _Count2 <= _Count1; ++_First1, --_Count1)
		{	// room for match, try it
		_FwdIt1 _Mid1 = _First1;
		for (_FwdIt2 _Mid2 = _First2; ; ++_Mid1, ++_Mid2)
			if (_Mid2 == _Last2)
				return (_First1);
			else if (!(*_Mid1 == *_Mid2))
				break;
		}
	return (_Last1);
	}

template<class _FwdIt1,
	class _FwdIt2> inline
	_FwdIt1 search(_FwdIt1 _First1, _FwdIt1 _Last1,
		_FwdIt2 _First2, _FwdIt2 _Last2)
	{	// find first [_First2, _Last2) match
	return (_Search(_First1, _Last1, _First2, _Last2,
		_Dist_type(_First1), _Dist_type(_First2)));
	}

		// TEMPLATE FUNCTION search WITH PRED
template<class _FwdIt1,
	class _FwdIt2,
	class _Diff1,
	class _Diff2,
	class _Pr> inline
	_FwdIt1 _Search(_FwdIt1 _First1, _FwdIt1 _Last1,
		_FwdIt2 _First2, _FwdIt2 _Last2, _Pr _Pred, _Diff1 *, _Diff2 *)
	{	// find first [_First2, _Last2) satisfying _Pred
	_Diff1 _Count1 = 0;
	_Distance(_First1, _Last1, _Count1);
	_Diff2 _Count2 = 0;
	_Distance(_First2, _Last2, _Count2);

	for (; _Count2 <= _Count1; ++_First1, --_Count1)
		{	// room for match, try it
		_FwdIt1 _Mid1 = _First1;
		for (_FwdIt2 _Mid2 = _First2; ; ++_Mid1, ++_Mid2)
			if (_Mid2 == _Last2)
				return (_First1);
			else if (!_Pred(*_Mid1, *_Mid2))
				break;
		}
	return (_Last1);
	}

template<class _FwdIt1,
	class _FwdIt2,
	class _Pr> inline
	_FwdIt1 search(_FwdIt1 _First1, _FwdIt1 _Last1,
		_FwdIt2 _First2, _FwdIt2 _Last2, _Pr _Pred)
	{	// find first [_First2, _Last2) satisfying _Pred
	return (_Search(_First1, _Last1, _First2, _Last2, _Pred,
		_Dist_type(_First1), _Dist_type(_First2)));
	} 
*/
#include "MiniParser.h"
/*
  vector<char>& _VectorFromCString
  ( CString& i_oString
  );
  
  CString& _CStringFromVector
  ( vector<char>& i_vString
  );

  int _CompareVectorFromCString
  ( vector<char>& i_vString
  , CString&      i_oString
  );
  
  int _CompareVectorFromChar
  ( vector<char>& i_vString
  , char*         i_panString
  );
*/

// class CElement

// The class whose instance is going to parse the entire element.
class CElementIterator;
class CXmlConf;

class CElement : public CBaseParser
{
  public:
    static CElement* CreateNewElement(const char* Name, CElement* parent = NULL);

    void Delete()
    {
      delete this;
    }

    // Accessor for attributes
    bool GetAttribute(const char* i_panAttributeName, vector<char>& o_rvAttributeValue);
    int  GetAttributeCount();
    bool GetAttributePairByIndex(int i_nAttributeIndex, vector<char>& o_rvAttributeName, vector<char>& o_rvAttributeValue);

    void AddAttributePair(const char* i_panAttributeName, const char* i_panAttributeValue);

    bool ModifyAttribute(const char* i_panAttributeName, const char* i_panAttributeValue);
    bool ModifyAttributeByIndex(int i_nAttributeIndex, const char* i_panAttributeValue);

    bool DeleteAttributeByName(const char* i_panAttributeName);
    bool DeleteAttributeByIndex(int i_nAttributeIndex);

    // Accessors for the Chile Elements 
    // Please use the CElementIterator to access the child Elements
    bool DeleteChild(CElement* i_poElementChild);

    CElement* GetFirstChild()
    {
      return mp_poElementFirstChild;
    }

    CElement* GetFirstChild(const char* i_panAttributeName);

    // Accessors for the chardata and Element Name
    bool GetCharData(vector<char>& o_rvStringData);
    void SetCharData(vector<char>& i_rvStringData);

    bool GetElementName(vector<char> & ret);

    void AddChildElement(CElement* l_poAttribute);

  private:
    CElement();

    explicit CElement(CElement* i_poElementParent):

    CBaseParser(i_poElementParent)
    , mp_poElementPrevSibling(NULL)
    , mp_poElementNextSibling(NULL)
    , mp_poElementFirstChild(NULL)
    , mp_bValid(false)
    {
    }

    ~CElement()
    {
      Clean();
    }

    void ParserAcquireCharData(CStringValue& i_roString);
    void ParserAddAttribute(CAttribute* i_poAttribute)
    {
      mp_vpoAttributeList.push_back(i_poAttribute);
    }

    inline CStringValue const& GetName() const
    {
      return mp_oStringValue;
    }

    void SetElementName(CStringValue& i_roString)
    {
      mp_oStringValue = i_roString;
    }

    virtual bool Parse(CScanner* i_poScanner);

    void Clean();

    CStringValue             mp_oStringValue;
    std::vector<CAttribute*> mp_vpoAttributeList;

    CStringValue mp_poStringData;
    CElement*    mp_poElementPrevSibling;
    CElement*    mp_poElementNextSibling;
    CElement*    mp_poElementFirstChild;
    bool         mp_bValid;

    friend std::ostream& operator << (std::ostream& o_rStdStreamOut, CElement& i_roElementParser);
    friend class CElementIterator;
    friend class CContent;
    friend class CXmlConf;
    friend class CAttribute;
};

// Class CElementIterator

// This is a friend class of the class CElement
class CElementIterator
{
  public:
    // The constructor that accept the CElement parameter.
    explicit CElementIterator(CElement* i_poObject);

    CElementIterator& GetNext();
    CElementIterator& operator++();

    inline bool IsValid() const
    {
      return (mp_bValid == true) && (mp_poElementCurrent != NULL);
    }

    inline void Invalidate()
    {
      mp_bValid           = false;
      mp_poElementCurrent = NULL;
    }

    CElementIterator(CElementIterator const & );
    CElementIterator & operator = (CElementIterator const & );

    CElement* operator *()
    {
      return GetElementPtr();
    }

    CElement* GetElementPtr()
    {
      return mp_bValid ? mp_poElementCurrent : NULL;
    }

    void Delete();

  private:
    CElementIterator();

    bool      mp_bValid;
    CElement* mp_poElementCurrent;
};

//
// class name CXmlConf
//
// CXmlConf encapsulated the XML data acquired from a file or a string buffer.
// Users can call 'GetRootElement()' to get the pointer of the root DOM elements after
// the XML data is successfully built through the constructor. Users should call
//
//    if (myXmlConf)
//    {
//      // access XML data
//    }
//...
//    or call
//    if (myXmlConf.IsValid())
//    {
//      // access XML data
//    }
//
// The copy constructor and operator= was not provided in order to avoid concept confusion.
// However, a member function Clone() is provided to generate a new root CElement.
class CXmlConf
{
  public:
    // Constructor that gets the XML strings from the given file with the name of 'i_panFileName'.
    CXmlConf(const char* i_panFileName);
    CXmlConf(char* begin, char* i_panCharEnd);

    inline operator bool() const
    {
      return mp_bValid;
    }

    inline bool IsValid() const
    {
      return mp_bValid;
    }

    inline CElement* GetRootElement() const
    {
      return IsValid() ? mp_poElementRoot : NULL;
    }

    CElement* GetRootElement(const char* );
//    CElement* Clone();
    virtual ~CXmlConf();

  private:
    // Hide the default constuctor. Copy constructor and operator=
    CXmlConf();
    bool Parse(char* i_panBuffer, int i_nBufferSize);

    bool      mp_bValid;
    CElement* mp_poElementRoot;
    char*     mp_panBuffer;
    CScanner* mp_poScanner;

    friend ostream & operator << (ostream& o_rStdStreamOut, CXmlConf const& i_roObject);
};


//CElement* Clone(CElement* p);

#endif

