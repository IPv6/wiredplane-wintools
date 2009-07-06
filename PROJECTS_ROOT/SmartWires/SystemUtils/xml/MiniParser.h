/********************************************************************
  created:   2005/05/27
  created:   27:5:2005   11:31
  filename:  C:\tool\MiniXML\MiniParser.h
  file path: C:\tool\MiniXML
  file base: MiniParser
  file ext:  h
  author:    Richard Lin
  
  purpose:  
  This file declare and (or) define all the parsing solution for the XML.
  It starts from the tokenize solution and i_panCharEnd up with the Parser for
  attributes, Content.

  User should not access the functions or classes defined in this file.
*********************************************************************/

#ifndef _MiniParser_h
#define _MiniParser_h

/////////////////////////////////////////////////////////////////////////////
// MiniParser

// g_Global      : g_ = Global
// i_Input       : i_ = Input
// no z...
// m_Member      : m_ = Member
// o_Output      : o_ = Output
// l_Local       : l_ = Local
// ...           : use your imagination

// aArray        : a  = Array    (array)
// bBool         : b  = Boolean  (boolean, TRUE/FALSE)
// cConstant     : c  = Constant (constant, whatever...)
// dDefine       : d  = Define   (simple define or defined value)
// eEnum         : e  = Enum     (enum list element)
// iIterator     : i  = Iterator (STL iterator)
// nNumber       : n  = Number   (char, long, int, whatever...)
// oObject       : o  = Object   (C++ class)
// pPointer      : p  = Pointer  (typicaly a 32 bits ulong address)
// sStruct       : s  = Struct   (structure)
// tTemplate     : t  = Template (template)
// uUnion        : u  = Union    (join two or more values of the same size under a common name)
// vVector       : v  = Vector   (STL vector, mostly like an array)

// poRemoteClass : po = Pointer on Object
// cdConstantDef : cd = Constant Define, typicaly a constant defined value
// usUnionStruct : us = Union of Structures
// ...           : use your imagination

// o_psBitmap    : CBitmap::GetBitmap((BITMAP*) o_psBitmap); // i_roString = BITMAP, p = *, o_ means it's an output

// <Value>
// [Function]

/*** g_eCharType **************************************************************/
/* Purpose : Used in the CharTypeTable ASCII table to parse XML elements      */
/* Unit    : None                                                             */
/* Range   : enum LIST                                                        */
/* List    :  0 = NormalChar   : image for the start                          */
/*            1 = ErrorChar    : image for before <mp_nLeft>                  */
/*            2 = Eof          : image for <mp_nLeft>/<mp_nLower>             */
/*            3 = LeftArrow    : image for the center of the bar              */
/*            4 = RightArrow   : image for the bar body                       */
/*            5 = Quote        : image for <mp_nRight>                        */
/*            6 = ForwardSlash : image for after <mp_nRight>                  */
/*            7 = Dash         : image for the bar background                 */
/*            8 = WhiteSpace   : image for the end                            */
/*            9 = Equal        : current number of elements                   */
/* Example : None                                                             */
/******************************************************************************/
enum g_eCharType
{ eCT_NormalChar = 0
, eCT_ErrorChar
, eCT_Eof
, eCT_LeftArrow
, eCT_RightArrow
, eCT_QuestionMark
, eCT_Quote
, eCT_ForwardSlash
, eCT_Dash
, eCT_WhiteSpace
, eCT_Exclaimation
, eCT_Equal
};

/*** g_sCharTypeTable *********************************************************/
/* Purpose : ASCII table for the parser turing machine                        */
/* Unit    : identified as 'CharTypeTable'                                    */
/* Range   : structure list                                                   */
/* List    :  0 - g_eCharType (int) CharTable[256] : empty ASCII table        */
/*            1 - void InitCharType()              : table initializer        */
/* Example : None                                                             */
/* WARNING : Call at least ONCE CharTypeTable::InitCharType()                 */
/******************************************************************************/
struct g_sCharTypeTable
{
  static g_eCharType anCharTable[256];
  static void        InitCharType();
};


class CStringValue
{ // Define structure of CStringValue
  public:
    char* m_panCharBegin;
    char* m_panCharEnd;

    // The default constructor is only used for the parsing stage, which assumes that the external buffer is provided.(for efficiency purpose)
    CStringValue()
    : m_panCharBegin(NULL)
    , m_panCharEnd(NULL)
    , mp_bUseMyBuffer(false)
    , mp_panBuffer(NULL){}

    ~CStringValue();

    CStringValue
    ( char* i_panCharBegin
    , char* i_panCharEnd
    , bool  i_bUseInternalBuffer);

    CStringValue // For NULL terminated string
    ( char* i_panCharBegin
    , bool  i_bUseInternalBuffer
    );

    CStringValue
    ( CStringValue const&
    );

    CStringValue& operator=
    ( CStringValue const&
    );

    inline operator bool () const {return (m_panCharBegin != NULL) && (m_panCharEnd != NULL);}
    inline int GetSize() const {return (*this) ? (m_panCharEnd - m_panCharBegin) : 0;}
    bool Equal(const char* str);
   
    // Warning : This function is only used in the parsing stage.
    friend bool operator == (CStringValue const& i_roStrFrom, CStringValue const& i_roStrTo);
    friend bool operator != (CStringValue const& i_roStrFrom, CStringValue const& i_roStrTo);
    friend std::ostream& operator << (std::ostream& o_rStdStreamOut, CStringValue const& i_roString);

  private:
    bool  mp_bUseMyBuffer;
    char* mp_panBuffer;
};

class CToken  
{
  public:
    enum eTokenCode
    { eTC_Word
    , eTC_Error
    , eTC_Eof
    , eTC_LeftArrow
    , eTC_RightArrow
    , eTC_QuestionMark
    , eTC_Quote
    , eTC_ForwardSlash
    , eTC_Dash
    , eTC_WhiteSpace
    , eTC_Exclaimation
    , eTC_Equal
    };

    CToken()
    : mp_panTokenChar(NULL){}

    virtual ~CToken(){};

    virtual bool       AcquireToken( const char* pCurrent, const char* pEndPos, char* &postPostition);
    inline const char* GetTokenString() const {return mp_panTokenChar;}
    inline eTokenCode  GetTokenCode() const{return mp_eTokenCode;}

  protected:
    const char* mp_panTokenChar;
    eTokenCode  mp_eTokenCode;

  private:
    // Hide the operator= and copy constuctor to prevent the subclass from copying.
    CToken& operator = (CToken const&);
    CToken(CToken const&);
};

class CLeftArrowToken : public CToken
{ // Token of "<"
  public:
    CLeftArrowToken()
    {
      mp_eTokenCode = eTC_LeftArrow;
    }
};

class CRightArrowToken : public CToken
{ // Token of ">"
  public:
    CRightArrowToken() 
    {
      mp_eTokenCode = eTC_RightArrow;
    }
};

class CForwardSlashToken : public CToken
{ // Token of "\"
  public:
    CForwardSlashToken() 
    {
      mp_eTokenCode = eTC_ForwardSlash;
    }
};

class CQuestionMarkToken : public CToken
{ // Token of "?"
  public:
    CQuestionMarkToken() 
    {
      mp_eTokenCode = eTC_QuestionMark;
    }
};

class CQuoteToken : public CToken
{ // Token of "\""
  public:
    CQuoteToken()
    {
      mp_eTokenCode = eTC_Quote;
    }

    virtual bool AcquireToken
    ( const char* pCurrent
    , const char* pEndPos  // The i_panCharEnd postion is the last legal position plus 1
    , char* &postPostition
    );
};

class CDashToken : public CToken
{ // Token of "Dash"
  public:
    CDashToken()
    {
      mp_eTokenCode = eTC_Dash;
    }
};

class CExclaimationToken : public CToken
{ // Token of "Exclaimation"
  public:
    CExclaimationToken()
    {
      mp_eTokenCode = eTC_Exclaimation;
    }
};

class CEofToken : public CToken
{ // Eof Token
  public:
    CEofToken()
    {
      mp_eTokenCode = eTC_Eof;
    }
};

class CEqualToken : public CToken
{ // Token of "CEqualToken"
  public:
    CEqualToken()
    {
      mp_eTokenCode = eTC_Equal;
    }
};

class CErrorToken : public CToken
{ // Error Token
  public:
    CErrorToken()
    {
      mp_eTokenCode = eTC_Error;
    }
};


class CWordToken : public CToken
{ // Words Token
  public:
    CWordToken()
    {
      mp_eTokenCode = eTC_Word;
    }

    virtual bool AcquireToken
    ( const char* pCurrent
    , const char* pEndPos  // The i_panCharEnd postion is the last legal position plus 1
    , char* &postPostition
    );
};


class CWhiteSpace:public CToken
{ // WhiteSpace Token
  public:
    CWhiteSpace()
    {
      mp_eTokenCode = eTC_WhiteSpace;
    }

    virtual bool AcquireToken
    ( const char* i_panTokenPosCurrent
    , const char* i_panTokenPosEnd  // The i_panCharEnd postion is the last legal position plus 1
    , char*      &o_rpanTokenPosNext
    );
};

class CScanner
{ // The CScanner runs in the server concept. The GetToken function is its center of support.
  public:
    CScanner
    ( char* i_panCharBegin
    , char* i_panCharEnd
    )
    : mp_panPosStart(i_panCharBegin)
    , mp_panPosEnd(i_panCharEnd)
    , mp_panPosCurrent(i_panCharBegin)
    {}

    bool GetNextStringMatch(const char* pattern);                

    CToken* GetNextToken();
    void SkipWhiteSpace();
    inline const char* GetCurrentPos() const {return mp_panPosCurrent;}
    inline const char* GetEndPos()     const {return mp_panPosEnd;}

    inline void SetCurrentPos(char* p)      {mp_panPosCurrent = p;}
    inline void SetCurrentPos(const char* p){mp_panPosCurrent = const_cast<char*>(p);}

    inline void BackOneStep()    {mp_panPosCurrent--;}
    inline void ForwardOneStep() {mp_panPosCurrent++;}
  
  private:
    CScanner();
    CScanner(CScanner const&);
    CScanner& operator= (CScanner const&);

    CLeftArrowToken    mp_oTokenLeftArrow;
    CRightArrowToken   mp_oTokenRightArrow;
    CForwardSlashToken mp_oTokenForwardSlash;
    CQuestionMarkToken mp_oTokenQuestionMark;
    CDashToken         mp_oTokenDash;
    CExclaimationToken mp_oTokenExclaimation;
    CQuoteToken        mp_oTokenQuote;
    CWordToken         mp_oTokenWord;
    CWhiteSpace        mp_oTokenWhiteSpace;
    CEofToken          mp_oTokenEof;
    CErrorToken        mp_oTokenError;
    CEqualToken        mp_oTokenEqual;

    char* mp_panPosStart;
    char* mp_panPosCurrent;
    char* mp_panPosEnd;
};

class CElement;

class CBaseParser
{
  public:
    explicit CBaseParser(CElement* parent)
    : mp_poElementParent(parent)
    , mp_bValid(true){}

    virtual bool      Parse(CScanner* i_poScanner)=0;
    inline  bool      IsValid() const{return mp_bValid;}
    inline  CElement* GetParent() const{return mp_poElementParent;}
    inline  void      SetParent(CElement* pParent){mp_poElementParent=pParent;}
    inline  void      SetValid(bool b=true){mp_bValid=b;}

  protected:
    CElement* mp_poElementParent;
    bool      mp_bValid;
};

class CCommentParser:public CBaseParser
{ // The class that parse the XML comments tags
  public:
    explicit CCommentParser(CElement*Parent):CBaseParser(Parent){}
    virtual bool Parse( CScanner* i_poScanner);

  private:
    CCommentParser();
};

class CPIParser:public CBaseParser
{ // parse the "Process Instruction" part of the XML elements
  public:
    explicit CPIParser(CElement*Parent):CBaseParser(Parent){}
    virtual bool Parse(CScanner* i_poScanner);
};

class CChardata:public CBaseParser
{ // Parse the Chardata in between the STag and ETag
  public:
    explicit CChardata(CElement*Parent):CBaseParser(Parent){}

    CChardata(CChardata const&i_roObject)
    : mp_oStrValue(i_roObject.mp_oStrValue)
    , CBaseParser(i_roObject.mp_poElementParent){}
        
    CChardata& operator=(CChardata const&i_roObject)
    {
      if(this!=&i_roObject) mp_oStrValue=i_roObject.mp_oStrValue;
      return *this;
    }

    virtual bool Parse( CScanner* i_poScanner);
    inline operator bool() const{return (mp_oStrValue.m_panCharBegin != NULL) && (mp_oStrValue.m_panCharEnd != NULL);}
  

    CStringValue & GetStringObj() const 
    {return const_cast<CStringValue &>(mp_oStrValue);}

    void SetStringObj(CStringValue& i_roString){ mp_oStrValue=i_roString;}  


    friend std::ostream& operator<<(std::ostream&o_rStdStreamOut, CChardata const& i_roObject)
    {
      if(i_roObject)
      {
        o_rStdStreamOut << i_roObject.GetStringObj();
      }

      return o_rStdStreamOut;
    }

  private:
    CStringValue mp_oStrValue;
};

class CAttribute:public CBaseParser
{ // Parse the Chardata in between the STag and ETag
  public:
    explicit CAttribute(CElement*Parent):CBaseParser(Parent){}

    CAttribute(CStringValue const&Name,CStringValue const&Value,CElement*Parent)
    : CBaseParser(Parent)
    , mp_oStrAttributeName(Name)
    , mp_oStrAttributeValue(Value){}

    virtual bool Parse( CScanner* i_poScanner);

    const CStringValue& GetAttrNameObj()  const {return mp_oStrAttributeName;}
    const CStringValue& GetAttrValueObj() const {return mp_oStrAttributeValue;}

    void SetValue(const char* Value);

    friend std::ostream& operator<<(std::ostream& o_rStdStreamOut, CAttribute const& i_roObject)
    {
      return o_rStdStreamOut << i_roObject.GetAttrNameObj() << "=\"" << i_roObject.GetAttrValueObj() << "\"";
    }

    CAttribute(CAttribute  const &i_roObject)
    : CBaseParser(i_roObject.mp_poElementParent)
    , mp_oStrAttributeName(i_roObject.mp_oStrAttributeName)
    , mp_oStrAttributeValue(i_roObject.mp_oStrAttributeValue)
    {}

  private:
    CAttribute();

    CStringValue mp_oStrAttributeName;
    CStringValue mp_oStrAttributeValue;
};

class CStagParser:public CBaseParser
{ // Parse the information inside start tag
  public:
    explicit CStagParser(CElement*Parent)
    : CBaseParser(Parent)
    , m_bIsEmptyElementTag(false)
    , m_bIsComment(false)
    , m_bIsPI(false)
    {}

    virtual bool Parse( CScanner* i_poScanner);
 
    inline const CStringValue& GetNameObj() const {return mp_oStrValue;}

    inline bool IsEmptyElementTag() const {return m_bIsEmptyElementTag;}
    inline bool IsCommentTag() const {return m_bIsComment;}
    inline bool IsPITag() const {return m_bIsPI;}

  private:
    CStagParser();
    CStringValue mp_oStrValue;
    bool         m_bIsEmptyElementTag;
    bool         m_bIsComment;
    bool         m_bIsPI;
};

class CEtagParser:public CBaseParser
{ // Parse the i_panCharEnd of tag information (</' Name S? '>')
  public:
    CEtagParser(CElement*p):CBaseParser(p){}
    virtual bool Parse(CScanner* i_poScanner);
    inline const CStringValue& GetNameObj() const {return mp_oStrValue;}

  private:
    CEtagParser();
    CStringValue mp_oStrValue;
};

class CContent:public CBaseParser
{ // Parse the string whose format is (element | PCData | Reference | CDSect | PI | Comment)
  public:
    explicit CContent(CElement*Parent):CBaseParser(Parent){}
    virtual bool Parse( CScanner* i_poScanner);

  private:
    CContent();
};

#endif
