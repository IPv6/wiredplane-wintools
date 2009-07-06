/********************************************************************
  created:   2005/05/27
  created:   27:5:2005   10:11
  filename:  C:\tool\MiniXML\MiniXML.cpp
  file path: C:\tool\MiniXML
  file ext:  cpp
  author:    Richard Lin
  
  Purpose : 
  Define class CElement, class CElementIterator, and class CXmlConf.
  Basically, users use those three classes to access the XML data.

  CXmlConf will acquire XML data from the given file or char* i_vStringData.
  The CElement::Parse will parse the data stored in the internal i_vStringData
  and create a DOM tree with root of CElement*  mp_poElementRoot, a member of 
  CXmlConf.

  CElement is the class that users can access Attributes, Chardata,
  element i_panAttributeName and subElements.

  CElementIterator is a helper class to access the Children elements of
  a CElement.

  This file also define a global function called CElement* Clone (CElement* l_poAttribute);
  This function is majorily for demo purpose which shows how to use the public
  functions provided by CElement and CElementiter.
 
*********************************************************************/

#include "StdAfx.h"
#include "MiniXML.h"



using namespace std;

g_eCharType g_sCharTypeTable::anCharTable[256];

void g_sCharTypeTable::InitCharType()
{ // Fill character type table
  for
  ( int i = 0
  ; i < 256
  ; i += 1
  )
  {
    char ch = static_cast<char> (i);

    if
    (
         ch >= 'a' && ch <= 'z'
      || ch >= 'A' && ch <= 'Z'
      || ch >= '0' && ch <= '9'
      || ch == '@'
      || ch == '$'
      || ch == '^'
      || ch == '&'
      || ch == '*'
      || ch == '('
      || ch == ')'
      || ch == '_'
      || ch == '+'
      || ch == ':'
      || ch == '{'
      || ch == '}'
      || ch == '['
      || ch == ']'
      || ch == '.'
      || ch == '~'
      || ch == '\\'
    )
    {
      g_sCharTypeTable::anCharTable[i] = eCT_NormalChar;
    }
    else
    {
      g_sCharTypeTable::anCharTable[i] = eCT_ErrorChar;
    }
  }

  g_sCharTypeTable::anCharTable[' ']  = eCT_WhiteSpace;
  g_sCharTypeTable::anCharTable['\t'] = eCT_WhiteSpace;
  g_sCharTypeTable::anCharTable['\n'] = eCT_WhiteSpace;
  g_sCharTypeTable::anCharTable['\r'] = eCT_WhiteSpace;
  g_sCharTypeTable::anCharTable['<']  = eCT_LeftArrow;
  g_sCharTypeTable::anCharTable['>']  = eCT_RightArrow;
  g_sCharTypeTable::anCharTable['?']  = eCT_QuestionMark;
  g_sCharTypeTable::anCharTable['\"'] = eCT_Quote;
  g_sCharTypeTable::anCharTable['/']  = eCT_ForwardSlash;
  g_sCharTypeTable::anCharTable['-']  = eCT_Dash;
  g_sCharTypeTable::anCharTable['!']  = eCT_Exclaimation;
  g_sCharTypeTable::anCharTable['=']  = eCT_Equal;
}

/*
vector<char>& _VectorFromCString
( CString& i_oString
)
{
  int   l_nLoop;
  int   l_nSize     = i_oString.GetLength();
  char* l_panString = i_oString.GetBuffer(0);

  // On efface la chaine précédente
  mp_vStrTempo.erase
  ( mp_vStrTempo.begin()
  , mp_vStrTempo.end()
  );

  for
  ( l_nLoop = 0
  ; l_nLoop < l_nSize
  ; l_nLoop += 1
  )
  { // On copie la chaine
    mp_vStrTempo.push_back(l_panString[l_nLoop]);
  }

//  mp_vStrTempo.push_back('\0'); // NULL à la fin
//  mp_vStrTempo.pop_back(); // Ne compte pas NULL

  return mp_vStrTempo;
}

CString& _CStringFromVector
( vector<char>& i_vString
)
{
  mp_oStrTempo = &i_vString[0];
  
  return mp_oStrTempo;
}

int _CompareVectorFromCString
( vector<char>& i_vString
, CString&      i_oString
)
{
  return _stricmp
  ( &i_vString[0]
  , i_oString.GetBuffer(0)
  );
}

int _CompareVectorFromChar
( vector<char>& i_vString
, char*         i_panString
)
{
  return _stricmp
  ( &i_vString[0]
  , i_panString
  );
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////

CStringValue::~CStringValue()
{
  if(mp_bUseMyBuffer)
  {
    delete [] mp_panBuffer;
  }
}

CStringValue::CStringValue
( char* i_panCharBegin
, char* i_panCharEnd
, bool  i_bUseInternalBuffer
)
{
  mp_bUseMyBuffer = i_bUseInternalBuffer;

  if(mp_bUseMyBuffer == true)
  {
    mp_panBuffer = new char[i_panCharEnd - i_panCharBegin];

    memcpy
    ( mp_panBuffer
    , i_panCharBegin
    , i_panCharEnd - i_panCharBegin
    );
    
    m_panCharBegin = mp_panBuffer;
    m_panCharEnd   = mp_panBuffer + (i_panCharEnd - i_panCharBegin);
  }
  else
  {
    m_panCharBegin = i_panCharBegin;
    m_panCharEnd   = i_panCharEnd;
  }
}

CStringValue::CStringValue
( char* i_panCharBegin
, bool  i_bUseInternalBuffer
)
{
  int stringSize = strlen(i_panCharBegin);

  mp_bUseMyBuffer = i_bUseInternalBuffer;

  if(mp_bUseMyBuffer == true)
  {
    mp_panBuffer = new char[stringSize];

    std::copy
    ( i_panCharBegin
    , i_panCharBegin+stringSize
    , mp_panBuffer
    );
    
    m_panCharBegin = mp_panBuffer;
    m_panCharEnd   = mp_panBuffer+stringSize;
  }
  else
  {
    m_panCharBegin = i_panCharBegin;
    m_panCharEnd   = i_panCharBegin+stringSize;
  }
}

CStringValue::CStringValue
( CStringValue const& i_roObject
)
{
  if(i_roObject.mp_bUseMyBuffer == true)
  {
    mp_panBuffer = new char[i_roObject.m_panCharEnd-i_roObject.m_panCharBegin];

    memcpy
    ( mp_panBuffer
    , i_roObject.mp_panBuffer
    , i_roObject.m_panCharEnd - i_roObject.m_panCharBegin
    );
    
    m_panCharBegin = mp_panBuffer;
    m_panCharEnd   = mp_panBuffer + (i_roObject.m_panCharEnd - i_roObject.m_panCharBegin);

    mp_bUseMyBuffer = true;
  }
  else
  {
    m_panCharBegin = i_roObject.m_panCharBegin;
    m_panCharEnd   = i_roObject.m_panCharEnd;

    mp_bUseMyBuffer = false;
  }
}

CStringValue& CStringValue::operator =
( CStringValue const& i_roObject
)
{
  if(this != &i_roObject)
  {
    if(mp_bUseMyBuffer)
    {
      delete [] mp_panBuffer;
    }

    if(i_roObject.mp_bUseMyBuffer == true)
    {
      mp_panBuffer = new char[i_roObject.m_panCharEnd-i_roObject.m_panCharBegin];

      memcpy
      ( mp_panBuffer
      , i_roObject.mp_panBuffer
      , i_roObject.m_panCharEnd - i_roObject.m_panCharBegin
      );
      
      m_panCharBegin = mp_panBuffer;
      m_panCharEnd   = mp_panBuffer + (i_roObject.m_panCharEnd - i_roObject.m_panCharBegin);

      mp_bUseMyBuffer = true;
    }
    else
    {
      m_panCharBegin = i_roObject.m_panCharBegin;
      m_panCharEnd   = i_roObject.m_panCharEnd;

      mp_bUseMyBuffer = false;
    }
  }
  return *this;
}

bool /*CStringValue::*/operator ==
( CStringValue const& i_roStrFrom
, CStringValue const& i_roStrTo
)
{ // Compare the two strings values
  if
  (
       (i_roStrFrom.m_panCharEnd - i_roStrFrom.m_panCharBegin)
    == (i_roStrTo.m_panCharEnd   - i_roStrTo.m_panCharBegin)
  )
  {
    return std::equal
    ( i_roStrFrom.m_panCharBegin
    , i_roStrFrom.m_panCharEnd
    , i_roStrTo.m_panCharBegin
    );
  }
  else
  {
    return false;
  }
}


bool /*CStringValue::*/operator !=
( CStringValue const& i_roStrFrom
, CStringValue const& i_roStrTo
)
{
  return !(i_roStrFrom == i_roStrTo);
}

std::ostream& /*CStringValue::*/operator <<
( std::ostream&       o_rStdStreamOut
, CStringValue const& i_roString
)
{
  char* l_poAttribute = i_roString.m_panCharBegin;
  while(l_poAttribute < i_roString.m_panCharEnd)
  {
    o_rStdStreamOut << *l_poAttribute;

    l_poAttribute += 1;
  }

  return o_rStdStreamOut;
}

bool CStringValue::Equal
( const char* i_panString
)
{
  if(GetSize() != strlen(i_panString))
  {
    return false;
  }
  else
  {
    return std::equal
    ( m_panCharBegin
    , m_panCharEnd
    , i_panString
    );
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CToken::AcquireToken
( const char* i_panTokenPosCurrent
, const char* i_panTokenPosEnd
, char*      &o_rpanTokenPosNext
)
{ // The basic acquiretoken action
  mp_panTokenChar    = i_panTokenPosCurrent;
  o_rpanTokenPosNext = const_cast<char*> (mp_panTokenChar) + 1;

  return true;
}

bool CQuoteToken::AcquireToken
( const char* i_panTokenPosCurrent
, const char* i_panTokenPosEnd
, char*      &o_rpanTokenPosNext
)
{
  char* l_panTokenPos  = const_cast<char*> (i_panTokenPosCurrent);
  char* l_panTokenFind = find(const_cast<char*> (i_panTokenPosCurrent), const_cast<char*> (i_panTokenPosEnd), '\"');

  if(l_panTokenFind == i_panTokenPosEnd)
  {
    return false;
  }
  else
  {
    o_rpanTokenPosNext = l_panTokenFind;
    mp_panTokenChar    = i_panTokenPosCurrent;

    return true;
  }
}

bool CWordToken::AcquireToken
( const char* i_panTokenPosCurrent
, const char* i_panTokenPosEnd
, char*      &o_rpanTokenPosNext
)
{
  char* l_panTokenPos = const_cast<char*> (i_panTokenPosCurrent);
  while(l_panTokenPos < i_panTokenPosEnd)
  {
    if
    (
         g_sCharTypeTable::anCharTable[(unsigned char) *l_panTokenPos]
      == eCT_NormalChar
    )
    {
      l_panTokenPos += 1;
    }
    else
    {
      mp_panTokenChar = i_panTokenPosCurrent;

      break;
    }
  }

  o_rpanTokenPosNext = l_panTokenPos;

  return true;
}

bool CWhiteSpace::AcquireToken
( const char* i_panTokenPosCurrent
, const char* i_panTokenPosEnd
, char*      &o_rpanTokenPosNext
)
{
  char* l_panTokenPos = const_cast<char*> (i_panTokenPosCurrent);
  while(l_panTokenPos < i_panTokenPosEnd)
  {
    if
    (
         g_sCharTypeTable::anCharTable[(unsigned char) *l_panTokenPos]
      == eCT_WhiteSpace
    )
    {
      l_panTokenPos += 1;
    }
    else
    {
      mp_panTokenChar = i_panTokenPosCurrent;

      break;
    }
  }

  o_rpanTokenPosNext = l_panTokenPos;

  return true;
}

// i_panAttributeName :  const CToken* CScanner::GetNextToken()
// Purpose : 
//
//  Scan the char stream referenced by the internal pointer and
//  return a token. The function will mark the i_panCharEnd of token position
//  with null terminator.
//
// Return:
// The function return its internal token object (so don't remove scanner objcet)
// when using the the returned token object.).
//
// The function returns NULL when NoToken was found in the string stream.
CToken* CScanner::GetNextToken()
{
  if(mp_panPosCurrent >= mp_panPosEnd)
  {
    return & mp_oTokenEof;
  }

  char* l_panPosCurrent = NULL;

  while(mp_panPosCurrent < mp_panPosEnd)
  {
    switch(g_sCharTypeTable::anCharTable[(unsigned char) *mp_panPosCurrent])
    {
      case eCT_ErrorChar:
        return & mp_oTokenError;

      case eCT_NormalChar:
        mp_oTokenWord.AcquireToken(mp_panPosCurrent, mp_panPosEnd, l_panPosCurrent);
        mp_panPosCurrent = l_panPosCurrent;
        return & mp_oTokenWord;

      case eCT_LeftArrow:
        mp_oTokenLeftArrow.AcquireToken(mp_panPosCurrent, mp_panPosEnd, l_panPosCurrent);
        mp_panPosCurrent = l_panPosCurrent;
        return & mp_oTokenLeftArrow;

      case eCT_RightArrow:
        mp_oTokenRightArrow.AcquireToken(mp_panPosCurrent, mp_panPosEnd, l_panPosCurrent);
        mp_panPosCurrent = l_panPosCurrent;
        return & mp_oTokenRightArrow;

      case eCT_QuestionMark:
        mp_oTokenQuestionMark.AcquireToken(mp_panPosCurrent, mp_panPosEnd, l_panPosCurrent);
        mp_panPosCurrent = l_panPosCurrent;
        return & mp_oTokenQuestionMark;

      case eCT_ForwardSlash:
        mp_oTokenForwardSlash.AcquireToken(mp_panPosCurrent, mp_panPosEnd, l_panPosCurrent);
        mp_panPosCurrent = l_panPosCurrent;
        return & mp_oTokenForwardSlash;

      case eCT_Dash:
        mp_oTokenDash.AcquireToken(mp_panPosCurrent, mp_panPosEnd, l_panPosCurrent);
        mp_panPosCurrent = l_panPosCurrent;
        return & mp_oTokenDash;

      case eCT_Exclaimation:
        mp_oTokenExclaimation.AcquireToken(mp_panPosCurrent, mp_panPosEnd, l_panPosCurrent);
        mp_panPosCurrent = l_panPosCurrent;
        return & mp_oTokenExclaimation;

      case eCT_Quote:
        if(mp_oTokenQuote.AcquireToken(mp_panPosCurrent+1, mp_panPosEnd, l_panPosCurrent))
        {
          mp_panPosCurrent = l_panPosCurrent;
          return & mp_oTokenQuote;
        }
        else
        {
          return & mp_oTokenError;
        }

      case eCT_WhiteSpace:
        mp_oTokenWhiteSpace.AcquireToken(mp_panPosCurrent, mp_panPosEnd, l_panPosCurrent);
        mp_panPosCurrent = l_panPosCurrent;
        return & mp_oTokenWhiteSpace;

      case eCT_Equal:
        mp_oTokenEqual.AcquireToken(mp_panPosCurrent, mp_panPosEnd, l_panPosCurrent);
        mp_panPosCurrent = l_panPosCurrent;
        return & mp_oTokenEqual;

      default:
        return & mp_oTokenError;
        break;
    };
  }

  return & mp_oTokenEof;
}

void CScanner::SkipWhiteSpace()
{
  while
  (
       (mp_panPosCurrent < mp_panPosEnd)
    && (
            g_sCharTypeTable::anCharTable[(unsigned char) *mp_panPosCurrent]
         == eCT_WhiteSpace
       )
  )
  {
    mp_panPosCurrent += 1;
  }
}

bool CScanner::GetNextStringMatch(const char* i_panString)
{ // This function is used to quickly parse the comments and PI only
  if(mp_panPosCurrent >= mp_panPosEnd)
  {
    return false;
  }
  else
  {
    int   l_nStringLenght = strlen(i_panString);
    char* l_panTokenPos   = search(mp_panPosCurrent, mp_panPosEnd, i_panString, i_panString + l_nStringLenght);

    if(l_panTokenPos == mp_panPosEnd)
    {
      return false;
    }
    else
    {
      mp_panPosCurrent = l_panTokenPos+l_nStringLenght;

      return true;
    }
  }
}

// Parser.cpp: implementation of the CParser class.
//
//////////////////////////////////////////////////////////////////////

bool CCommentParser::Parse(CScanner* i_poScanner)
{
  return i_poScanner->GetNextStringMatch("-->"); // <![CDATA[5067]]> from GPX files will not be considered as a comment
}

bool CPIParser::Parse(CScanner* i_poScanner)
{ // Parse the processing instruction
  return i_poScanner->GetNextStringMatch((const char* ) "?>");
}


bool CChardata::Parse(CScanner* i_poScanner)
{ // Parse the remained part of Chardata
  if(!i_poScanner->GetNextStringMatch((const char* ) "<"))
  {
    return false;
  }
  return true;
}

void CAttribute::SetValue(const char* i_panAttributeValue)
{ // Set the value of the attributes
  CStringValue l_oString(const_cast<char*> (i_panAttributeValue), true);

  mp_oStrAttributeValue = l_oString;
}

bool CAttribute::Parse(CScanner* i_poScanner)
{
  i_poScanner->SkipWhiteSpace();

  CToken*            l_oToken     = i_poScanner->GetNextToken();
  CToken::eTokenCode l_eTokenCode = l_oToken->GetTokenCode();

  if
  (
       (l_eTokenCode == CToken::eTC_LeftArrow)
    || (l_eTokenCode == CToken::eTC_ForwardSlash)
  )
  {
    i_poScanner->BackOneStep();

    return true;
  }
  else
  {
    if(l_eTokenCode != CToken::eTC_Word)
    {
      return false;
    }
    else
    {
      CWordToken* l_oTokenWord = static_cast <CWordToken*> (l_oToken);

      // Get the attribute i_panAttributeName
      mp_oStrAttributeName.m_panCharBegin = const_cast<char*> (l_oTokenWord->GetTokenString());
      mp_oStrAttributeName.m_panCharEnd   = const_cast<char*> (i_poScanner->GetCurrentPos());

      CToken* l_oTokenEqual = i_poScanner->GetNextToken();

      if(l_oTokenEqual->GetTokenCode() != CToken::eTC_Equal)
      {
        return false;
      }
      else
      {
        CToken* l_oTokenQuote = i_poScanner->GetNextToken();

        if(l_oTokenQuote->GetTokenCode() != CToken::eTC_Quote)
        {
          return false;
        }
        else
        { // Get the attribute values.
          mp_oStrAttributeValue.m_panCharBegin = const_cast<char*> (static_cast < CQuoteToken * > (l_oTokenQuote)->GetTokenString());
          mp_oStrAttributeValue.m_panCharEnd   = const_cast<char*> (i_poScanner->GetCurrentPos());

          CAttribute* l_poAttribute = new CAttribute(*this);

          l_poAttribute->GetParent()->ParserAddAttribute(l_poAttribute);

          i_poScanner->SetCurrentPos(mp_oStrAttributeValue.m_panCharEnd+1);

          return true;
        }
      }
    }
  }
}

bool CStagParser::Parse(CScanner* i_poScanner)
{ // Parse the XML STag ('<' i_panAttributeName (S Attribute)* S? '>')
  CToken* l_oTokenLeftArrow = i_poScanner->GetNextToken();

  if(l_oTokenLeftArrow->GetTokenCode() != CToken::eTC_LeftArrow)
  {
    return false;
  }
  else
  {

    CToken*            l_oTokenWord = i_poScanner->GetNextToken();
    CToken::eTokenCode l_eTokenCode      = l_oTokenWord->GetTokenCode();

    if(l_eTokenCode != CToken::eTC_Word)
    { // Check if it is a PI or COMMENT
      if(l_eTokenCode == CToken::eTC_Exclaimation)
      {
        l_eTokenCode = i_poScanner->GetNextToken()->GetTokenCode();
        if(l_eTokenCode == CToken::eTC_Dash)
        {
          l_eTokenCode = i_poScanner->GetNextToken()->GetTokenCode();
          if(l_eTokenCode == CToken::eTC_Dash)
          { // It is a comments elements.
            CCommentParser l_oParserComment(this->GetParent());
            if(l_oParserComment.Parse(i_poScanner))
            {
              m_bIsComment = true;

              return true;
            }
          }
        }
      }
      else if(l_eTokenCode == CToken::eTC_QuestionMark)
      { // Parse it as PI
        CPIParser l_oParserPI(this->GetParent());
        if(l_oParserPI.Parse(i_poScanner))
        {
          m_bIsPI = true;

          return true;
        }
      }

      return false;
    }
    else
    { // Remember the start address and the i_panCharEnd address of the <i_panAttributeName>
      mp_oStrValue.m_panCharBegin = const_cast<char*> (l_oTokenWord->GetTokenString());
      mp_oStrValue.m_panCharEnd   = const_cast<char*> (i_poScanner->GetCurrentPos());

      // Parse the attribute list
      bool bParseAttrList = true;
      CAttribute AttrParser(this->GetParent());

      CToken*            l_oTokenWhiteOrRightArrow     = i_poScanner->GetNextToken();
      CToken::eTokenCode l_eTokenCodeWhiteOrRightArrow = l_oTokenWhiteOrRightArrow->GetTokenCode();

      while(bParseAttrList)
      {
        if(l_eTokenCodeWhiteOrRightArrow == CToken::eTC_WhiteSpace)
        {
          if(!AttrParser.Parse(i_poScanner))
          {
            return false;
          }

          l_eTokenCodeWhiteOrRightArrow = i_poScanner->GetNextToken()->GetTokenCode();
        }
        else
        {
          bParseAttrList = false;
        }
      }

      // Parse the '>'
      if(l_eTokenCodeWhiteOrRightArrow == CToken::eTC_RightArrow)
      {
        m_bIsEmptyElementTag = false;

        return true;
      }
      else
      {
        if(l_eTokenCodeWhiteOrRightArrow == CToken::eTC_ForwardSlash)
        {
          CToken::eTokenCode l_eTokenCode = i_poScanner->GetNextToken()->GetTokenCode();
          if(l_eTokenCode == CToken::eTC_RightArrow)
          {
            m_bIsEmptyElementTag = true;
            return true;
          }
        }

        return false;
      }
    }
  }
}

bool CEtagParser::Parse(CScanner* i_poScanner)
{ // Parse the Etag element
  CToken* l_oTokenLeftArrow = i_poScanner->GetNextToken();
  if(l_oTokenLeftArrow->GetTokenCode() != CToken::eTC_LeftArrow)
  {
    return false;
  }
  else
  {
    CToken* l_oTokenForward = i_poScanner->GetNextToken();
    if(l_oTokenForward->GetTokenCode() != CToken::eTC_ForwardSlash)
    {
      return false;
    }
    else
    {
      CToken* l_oTokenWord = i_poScanner->GetNextToken();
      if(l_oTokenWord->GetTokenCode() != CToken::eTC_Word)
      {
        return false;
      }
      else
      {
        mp_oStrValue.m_panCharBegin = const_cast<char*> (l_oTokenWord->GetTokenString());
        mp_oStrValue.m_panCharEnd = const_cast<char*> (i_poScanner->GetCurrentPos());

        CToken* l_oTokenRightArrow = i_poScanner->GetNextToken();
        if(l_oTokenRightArrow->GetTokenCode() != CToken::eTC_RightArrow)
        {
          return false;
        }
        else
        {
          return true;
        }
      }
    }
  }
}

bool CElement::Parse(CScanner* i_poScanner)
{
  CStagParser l_oParserStag(this);
  CEtagParser l_oParserEtag(this);

  // Parse the begin tag
  if(!l_oParserStag.Parse(i_poScanner))
  {
    return false;
  }
  else
  {
    if
    (
         l_oParserStag.IsEmptyElementTag()
      || l_oParserStag.IsPITag()
      || l_oParserStag.IsCommentTag()
    )
    {
      mp_oStringValue = l_oParserStag.GetNameObj();

      if(mp_poElementParent)
      {
        mp_poElementParent->AddChildElement(this);
      }

      mp_bValid = true;

      return true;
    }
    else
    { // Parse the content
      CContent l_oParserContent(this);
      if(!l_oParserContent.Parse(i_poScanner))
      {
        return false;
      }
      else
      { // Parse the i_panCharEnd tag
        if(!l_oParserEtag.Parse(i_poScanner))
        {
          return false;
        }
        else
        {
          if(l_oParserStag.GetNameObj() == l_oParserEtag.GetNameObj())
          {
            mp_oStringValue = l_oParserStag.GetNameObj();

            // Parse success, add the element
            if(mp_poElementParent)
            {
              mp_poElementParent->AddChildElement(this);
            }

            mp_bValid = true;

            return true;
          }
          else
          {
            return false;
          }
        }
      }
    }
  }
}

bool CContent::Parse(CScanner* i_poScanner)
{ // Check the leading contents and decide if we need to 
  const char*        l_panPosCurrent = i_poScanner->GetCurrentPos();
  CToken*            l_poToken       = i_poScanner->GetNextToken();
  CToken::eTokenCode l_eTokenCode    = l_poToken->GetTokenCode();
  bool               l_bCharParsed   = false;

  while
  (
       (l_eTokenCode != CToken::eTC_Eof)
    && (l_eTokenCode != CToken::eTC_Error)
  )
  {
    switch(l_eTokenCode)
    {
      case CToken::eTC_WhiteSpace:
        break; // Get the next token and check

      case CToken::eTC_Word:
      {
        if(l_bCharParsed)
        {
          return false;
        }
        else
        { // Keep the start pointer of the CData
          const char* l_panPosCurrent = l_poToken->GetTokenString();

          // CData part
          CChardata cdataParser(this->GetParent());
          cdataParser.GetStringObj().m_panCharBegin = const_cast<char*> (l_panPosCurrent);

          if(!cdataParser.Parse(i_poScanner))
          {
            return false;
          }
          else
          {
            // Keep the i_panCharEnd pointer of the CData
            i_poScanner->BackOneStep();
            cdataParser.GetStringObj().m_panCharEnd = const_cast<char*> (i_poScanner->GetCurrentPos());
            mp_poElementParent->ParserAcquireCharData(cdataParser.GetStringObj());

            l_bCharParsed = true;
            break;
          }
        }
      }
      case CToken::eTC_LeftArrow:
      { // Check the following three conditions : <!, </, <abc
        char* i_panTokenPosCurrent = (char* ) i_poScanner->GetCurrentPos();

        if(g_sCharTypeTable::anCharTable[(unsigned char) *i_panTokenPosCurrent] == eCT_Exclaimation)
        { // <! : comment
          i_poScanner->SetCurrentPos(l_panPosCurrent);

          CCommentParser l_oParserComment(this->GetParent());
          if(!l_oParserComment.Parse(i_poScanner))
          { // Doesn't ends with "-->", let'i_roString catch the string (e.g. <![CDATA[5067]]> from GPX files)
            vector<char> l_vStrComment;

            while(*(l_panPosCurrent) != '<')
            { // Copy the string
              l_vStrComment.push_back(*l_panPosCurrent);
              l_panPosCurrent += 1;
            }

            mp_poElementParent->SetCharData(l_vStrComment);
            i_poScanner->SetCurrentPos(l_panPosCurrent);

            return true;
          }
        }
        else if(g_sCharTypeTable::anCharTable[(unsigned char) *i_panTokenPosCurrent] == eCT_ForwardSlash)
        { // </ : etag, job done.
          i_poScanner->SetCurrentPos(l_panPosCurrent);

          return true;
        }
        else if(g_sCharTypeTable::anCharTable[(unsigned char) *i_panTokenPosCurrent] == eCT_NormalChar)
        { // <abc : new tag, parse it
          i_poScanner->SetCurrentPos(l_panPosCurrent);

          CElement* pElementParser = new CElement(this->GetParent());
          if(!pElementParser->Parse(i_poScanner))
          {
            delete pElementParser;
            return false;
          }
        }

        // Otherwise, return false
        break;
      }

      default:
        return false;
    };

    l_panPosCurrent = i_poScanner->GetCurrentPos();
    l_poToken       = i_poScanner->GetNextToken();
    l_eTokenCode    = l_poToken->GetTokenCode();
  }

  return false;
}

//////////////////////////////////////////////////////////////////////////////////////////

void CElement::Clean()
{ // Clean up all the subelements and sibling elements
  CElement* l_poAttribute = mp_poElementFirstChild;
  while(l_poAttribute)
  {
    CElement* l_poElement = l_poAttribute;
    l_poAttribute = l_poAttribute->mp_poElementNextSibling;

    delete l_poElement;

    if(l_poAttribute == mp_poElementFirstChild)
    {
      break;
    }
  }

  // Cleanup the attribute list
  vector<CAttribute*>::iterator l_ivpoAttributeBegin = mp_vpoAttributeList.begin();
  vector<CAttribute*>::iterator l_ivpoAttributeEnd   = mp_vpoAttributeList.end();

  while(l_ivpoAttributeBegin != l_ivpoAttributeEnd)
  {
    delete *l_ivpoAttributeBegin;

    l_ivpoAttributeBegin++; // iterator, use only '++'
  }

  mp_vpoAttributeList.clear();
}

void CElement::AddChildElement
( CElement* i_poElementChild
)
{ // Add the child elements of the current elements
  if(mp_poElementFirstChild != NULL)
  { // Configure the sibling chain
    i_poElementChild->mp_poElementNextSibling = mp_poElementFirstChild;
    i_poElementChild->mp_poElementPrevSibling = mp_poElementFirstChild->mp_poElementPrevSibling;

    mp_poElementFirstChild->mp_poElementPrevSibling->mp_poElementNextSibling = i_poElementChild;
    mp_poElementFirstChild->mp_poElementPrevSibling                          = i_poElementChild;
  }
  else
  {
    mp_poElementFirstChild                          = i_poElementChild;
    mp_poElementFirstChild->mp_poElementPrevSibling = mp_poElementFirstChild;
    mp_poElementFirstChild->mp_poElementNextSibling = mp_poElementFirstChild;
  }

  i_poElementChild->SetParent(this);
}

bool CElement::DeleteChild
( CElement* i_poElementChild
)
{ // Delete the child referenced by CElement*
  if(!mp_poElementFirstChild)
  {
    return false;
  }
  else
  {
    CElement* l_poAttribute = mp_poElementFirstChild;
    bool      l_bLoopEnd    = false;

    while(l_poAttribute != i_poElementChild && ! l_bLoopEnd)
    {
      l_poAttribute = l_poAttribute->mp_poElementNextSibling;
      if(l_poAttribute == mp_poElementFirstChild)
      {
        l_bLoopEnd = true;
      }
    }

    if(l_poAttribute == i_poElementChild)
    {
      CElement* l_poElementFirst = mp_poElementFirstChild;

      if(i_poElementChild == mp_poElementFirstChild)
      {
        l_poElementFirst = mp_poElementFirstChild->mp_poElementNextSibling;
      }

      bool l_bChildSingle = (mp_poElementFirstChild->mp_poElementPrevSibling == mp_poElementFirstChild);

      i_poElementChild->mp_poElementPrevSibling->mp_poElementNextSibling = i_poElementChild->mp_poElementNextSibling;
      i_poElementChild->mp_poElementNextSibling->mp_poElementPrevSibling = i_poElementChild->mp_poElementPrevSibling;

      delete i_poElementChild;

      if(l_bChildSingle)
      {
        l_poElementFirst = NULL;
      }

      mp_poElementFirstChild = l_poElementFirst;

      return true;
    }
    else
    {
      return false;
    }
  }
}

std::ostream & operator <<
( std::ostream& o_rStdStreamOut
, CElement&     i_roElementParser
)
{ // Output the XML stream from the current element
  int l_nLoop;
  int l_nIndent = 0;// Tag indentation counter

  CElement* l_poElementParent = i_roElementParser.GetParent();
  while(l_poElementParent != NULL)
  {
    l_nIndent += 1;
    l_poElementParent = l_poElementParent->GetParent();
  }

  o_rStdStreamOut << endl;
  
  for
  ( l_nLoop = 0
  ; l_nLoop < l_nIndent
  ; l_nLoop += 1
  )
  {
    o_rStdStreamOut << "  "; // one l_nIndent = two space char
  }

  // Begin Tag
  o_rStdStreamOut << '<' << i_roElementParser.GetName();

  // Cleanup the attribute list
  vector<CAttribute*>::iterator l_ivpoAttrBegin = i_roElementParser.mp_vpoAttributeList.begin();
  vector<CAttribute*>::iterator l_ivpoAttrEnd   = i_roElementParser.mp_vpoAttributeList.end();

  if(i_roElementParser.GetAttributeCount() > 2)
  { // line feeded
    while(l_ivpoAttrBegin != l_ivpoAttrEnd)
    {
      o_rStdStreamOut << ' ' << **l_ivpoAttrBegin;
      l_ivpoAttrBegin++; // iterator, use only '++'

      if(l_ivpoAttrBegin != l_ivpoAttrEnd)
      {
        o_rStdStreamOut << ' ' << **l_ivpoAttrBegin;
        l_ivpoAttrBegin++; // iterator, use only '++'

        if(l_ivpoAttrBegin != l_ivpoAttrEnd)
        {
          o_rStdStreamOut << endl;

          for
          ( l_nLoop  = 0
          ; l_nLoop  < l_nIndent + 1
          ; l_nLoop += 1
          )
          {
            o_rStdStreamOut << "  "; // one l_nIndent = two space char
          }
        }
      }
    }
/*
    // Put the '>' char on the next line
    o_rStdStreamOut << endl;

    for
    ( l_nLoop  = 0
    ; l_nLoop  < l_nIndent
    ; l_nLoop += 1
    )
    {
      o_rStdStreamOut << "  "; // one indent = two space char
    }
*/
  }
  else
  { // same line
    while(l_ivpoAttrBegin != l_ivpoAttrEnd)
    {
      o_rStdStreamOut << ' ' << **l_ivpoAttrBegin;
      l_ivpoAttrBegin++; // iterator, use only '++'
    }
  }

  // close tag
  o_rStdStreamOut << '>';

  // Walk through the children chains and output each child
  CElement* l_poElement = i_roElementParser.mp_poElementFirstChild;

  if
  (
       (l_poElement != NULL)
    && (i_roElementParser.mp_poStringData.GetSize() > 0)
  )
  { // If child, write data part on the next line and close tag after child
    o_rStdStreamOut << endl;

    for
    ( l_nLoop  = 0
    ; l_nLoop  < l_nIndent + 1
    ; l_nLoop += 1
    )
    {
      o_rStdStreamOut << "  "; // one indent = two space char
    }
  }

  // Output the data part
  o_rStdStreamOut << i_roElementParser.mp_poStringData;

  bool l_bLastItem = false;

  while
  (
       (l_bLastItem == false)
    && (l_poElement != NULL)
  )
  {
    o_rStdStreamOut << *l_poElement;
    l_poElement = l_poElement->mp_poElementNextSibling;

    if
    (
         (l_poElement == NULL)
      || (l_poElement == i_roElementParser.mp_poElementFirstChild)
    )
    {
      l_bLastItem = true;
    }
  }

  if
  (
       (i_roElementParser.GetAttributeCount() > 2)
    || (l_bLastItem == true)
  )
  { // line feeded
    o_rStdStreamOut << endl;

    for
    ( l_nLoop  = 0
    ; l_nLoop  < l_nIndent
    ; l_nLoop += 1
    )
    {
      o_rStdStreamOut << "  "; // one indent = two space char
    }
  }
  
  // End tag
  o_rStdStreamOut << "</" << i_roElementParser.GetName() << '>';

  return o_rStdStreamOut;
}

// ================================================================================
//
// Begin the attributes functions of the CElement
//

int CElement::GetAttributeCount
(
)
{ // Get the number of the attributes of the element represented by the iterator
  if(IsValid() == false)
  {
    return 0;
  }
  else
  {
    return mp_vpoAttributeList.size();
  }
}

bool CElement::GetAttributePairByIndex
( int           i_nAttributeIndex
, vector<char>& o_rvAttributeName
, vector<char>& o_rvAttributeValue
)
{
  if
  (
       (i_nAttributeIndex >= 0)
    && (i_nAttributeIndex < GetAttributeCount())
  )
  {
    CAttribute* l_poAttribute = mp_vpoAttributeList[i_nAttributeIndex];

    o_rvAttributeName.resize(l_poAttribute->GetAttrNameObj().GetSize());

    std::copy
    ( l_poAttribute->GetAttrNameObj().m_panCharBegin
    , l_poAttribute->GetAttrNameObj().m_panCharEnd
    , o_rvAttributeName.begin()
    );
    
    o_rvAttributeName.push_back('\0');

    o_rvAttributeValue.resize(l_poAttribute->GetAttrValueObj().GetSize());

    std::copy
    ( l_poAttribute->GetAttrValueObj().m_panCharBegin
    , l_poAttribute->GetAttrValueObj().m_panCharEnd
    , o_rvAttributeValue.begin()
    );
    
    o_rvAttributeValue.push_back('\0');

    return true;
  }
  else
  {
    return false;
  }
}

bool CElement::GetAttribute
( const char*   i_panAttributeName
, vector<char>& o_rvAttributeValue
)
{
  int l_nAttributeCount = GetAttributeCount();
  int l_nIndex;

  for
  ( l_nIndex  = 0
  ; l_nIndex  < l_nAttributeCount
  ; l_nIndex += 1
  )
  {
    CAttribute* l_poAttribute = mp_vpoAttributeList[l_nIndex];
    if(const_cast <CStringValue&> (l_poAttribute->GetAttrNameObj()).Equal(i_panAttributeName))
    {
      o_rvAttributeValue.resize(l_poAttribute->GetAttrValueObj().GetSize());

      std::copy
      ( l_poAttribute->GetAttrValueObj().m_panCharBegin
      , l_poAttribute->GetAttrValueObj().m_panCharEnd
      , o_rvAttributeValue.begin()
      );
      
      o_rvAttributeValue.push_back('\0');
      
      return true;
    }
  }

  return false;
}

void CElement::AddAttributePair
( const char* i_panAttributeName
, const char* i_panAttributeValue
)
{ // Add the attributes value pair to the Element
  CStringValue l_oStrName(const_cast<char*>  (i_panAttributeName), true);
  CStringValue l_oStrValue(const_cast<char*> (i_panAttributeValue), true);

  CAttribute* l_poAttribute = new CAttribute(l_oStrName, l_oStrValue, this);
  ParserAddAttribute(l_poAttribute);
}

bool CElement::ModifyAttribute
( const char* i_panAttributeName
, const char* i_panAttributeValue
)
{ // Update the attribute value represented by 'i_panAttributeName', return false if the attribute was not found
  int l_nAttributeCount = GetAttributeCount();
  int l_nIndex;

  for
  ( l_nIndex  = 0
  ; l_nIndex  < l_nAttributeCount
  ; l_nIndex += 1
  )
  {
    CAttribute* l_poAttribute = mp_vpoAttributeList[l_nIndex];

    if(const_cast <CStringValue&> (l_poAttribute->GetAttrNameObj()).Equal(i_panAttributeName))
    {
      l_poAttribute->SetValue(i_panAttributeValue);
      return true;
    }
  }

  return false;
}

bool CElement::ModifyAttributeByIndex
( int         i_nAttributeIndex
, const char* i_panAttributeValue
)
{ // Update the attribute value represented by 'i_nAttributeIndex', return false if the attribute was not found
  if(i_nAttributeIndex >= GetAttributeCount())
  {
    return false;
  }
  else
  {
    mp_vpoAttributeList[i_nAttributeIndex]->SetValue(i_panAttributeValue);

    return true;
  }
}

bool CElement::DeleteAttributeByName
( const char* i_panAttributeName
)
{
  int l_nAttributeCount = GetAttributeCount();

  vector<CAttribute*>::iterator l_ivpoAttrBegin = mp_vpoAttributeList.begin();
  vector<CAttribute*>::iterator l_ivpoAttrEnd   = mp_vpoAttributeList.end();

  while(l_ivpoAttrBegin != l_ivpoAttrEnd)
  {
    CAttribute* l_poAttribute = (*l_ivpoAttrBegin);
    
    if(const_cast <CStringValue&> (l_poAttribute->GetAttrNameObj()).Equal(i_panAttributeName))
    {
      delete l_poAttribute;

      mp_vpoAttributeList.erase(l_ivpoAttrBegin);

      return true;
    }

    l_ivpoAttrBegin++; // iterator, use only '++'
  }

  return false;
}

bool CElement::DeleteAttributeByIndex
( int i_nAttributeIndex
)
{
  int l_nAttributeCount = GetAttributeCount();
  int l_nAttributeIndex = 0;

  vector<CAttribute*>::iterator l_ivpoAttrBegin = mp_vpoAttributeList.begin();
  vector<CAttribute*>::iterator l_ivpoAttrEnd   = mp_vpoAttributeList.end();

  while(l_ivpoAttrBegin != l_ivpoAttrEnd)
  {
    if(l_nAttributeIndex == i_nAttributeIndex)
    {
      delete (*l_ivpoAttrBegin);

      mp_vpoAttributeList.erase(l_ivpoAttrBegin);

      return true;
    }

    l_nAttributeIndex += 1;
    l_ivpoAttrBegin++; // iterator, use only '++'
  }

  return false;
}

// ================================================================================

bool CElement::GetElementName
( vector<char>& o_rvStringName
)
{
  o_rvStringName.resize(mp_oStringValue.GetSize());

  std::copy
  ( mp_oStringValue.m_panCharBegin
  , mp_oStringValue.m_panCharEnd
  , o_rvStringName.begin()
  );

  o_rvStringName.push_back('\0'); // Make sure it is NULL terminated
  o_rvStringName.pop_back();      // Don't count the NULL character in l_nAttributeCount()

  return o_rvStringName.size() > 0;
}

bool CElement::GetCharData
( vector<char>& o_rvStringData
)
{ // Return the string value to the 'o_rvStringData', false if the Chardata is not available
  if(mp_poStringData)
  {
    o_rvStringData.resize(mp_poStringData.m_panCharEnd - mp_poStringData.m_panCharBegin);

    std::copy
    ( mp_poStringData.m_panCharBegin
    , mp_poStringData.m_panCharEnd
    , o_rvStringData.begin()
    );

    o_rvStringData.push_back('\0'); // Make sure it is NULL terminated
    o_rvStringData.pop_back();      // Don't count the NULL character in l_nAttributeCount()

    return true;
  }
  else
  {
    return false;
  }
}

void CElement::SetCharData
( vector<char>& i_rvStringData
)
{
  if(i_rvStringData.size() > 0)
  {
    CStringValue l_oString
    ( i_rvStringData.begin()
    , i_rvStringData.end()
    , true
    );

    mp_poStringData = l_oString;
  }
}

void CElement::ParserAcquireCharData
( CStringValue& i_roString
)
{
  mp_poStringData = (i_roString);
}

std::ostream & operator <<
( std::ostream&   o_rStdStreamOut
, CElement const& i_roElementParser
)
{
  return o_rStdStreamOut << const_cast<CElement&> (i_roElementParser);
}

CElement* CElement::CreateNewElement
( const char* i_panAttributeName
, CElement*   i_poElementParent // = NULL
)
{
  CStringValue l_oStringName(const_cast<char*> (i_panAttributeName), true);

  CElement* l_poAttribute = new CElement(i_poElementParent);
  l_poAttribute->SetElementName(l_oStringName);

  return l_poAttribute;
}

CElement* CElement::GetFirstChild
( const char* i_panAttributeName
)
{ // The input string is as i_panAttributeName.name2.name3, the function will return the pointer of the firstChild that match with the string
  CElement* l_poAttribute = GetFirstChild();
  string    l_oStdString  = (i_panAttributeName);
  int       l_nPos        = l_oStdString.find_first_of(".", 0);
  string    l_oStdTempo   = l_oStdString;

  if(l_nPos != -1)
  {
    l_oStdTempo = l_oStdString.substr(0, l_nPos);
  }

  CElementIterator l_ioElementIterator(l_poAttribute);

  while(l_ioElementIterator.IsValid())
  {
    bool l_bFind = true;

    if(const_cast<CStringValue&> ((*l_ioElementIterator)->GetName()).Equal(const_cast<char*> (l_oStdTempo.c_str())))
    {
      if(l_nPos == -1)
      {
        return l_poAttribute;
      }
      else
      {
        string l_oStdStringSub = l_oStdString.substr(l_nPos + 1, l_oStdString.size());
        CElement* l_poElementFound = (*l_ioElementIterator)->GetFirstChild(l_oStdStringSub.c_str());

        if(l_poElementFound != NULL)
        {
          return l_poElementFound;
        }
      }
    }

    ++l_ioElementIterator; // iterator, use only '++'
  }

  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// class CElementIterator

CElementIterator::CElementIterator
( CElement* i_poObject
)
: mp_bValid(false)
{ // Copy constructors and operator =
  if(i_poObject)
  {
    mp_poElementCurrent = i_poObject;
    mp_bValid           = true;
  }
}

CElementIterator::CElementIterator
( CElementIterator const& i_roObject
)
: mp_bValid(i_roObject.mp_bValid)
, mp_poElementCurrent(i_roObject.mp_poElementCurrent)
{
}

CElementIterator & CElementIterator::operator = 
( CElementIterator const& i_roObject
)
{
  if(this != &i_roObject)
  {
    mp_bValid           = i_roObject.mp_bValid;
    mp_poElementCurrent = i_roObject.mp_poElementCurrent;
  }

  return *this;
}

CElementIterator& CElementIterator::GetNext
(
)
{
  if(!IsValid())
  {
    return *this;
  }

  if
  (
       (mp_poElementCurrent->mp_poElementNextSibling != NULL)
    && (mp_poElementCurrent->mp_poElementNextSibling != mp_poElementCurrent->GetParent()->mp_poElementFirstChild)
  )
  {
    mp_poElementCurrent = mp_poElementCurrent->mp_poElementNextSibling;
  }
  else
  {
    mp_bValid = false;
  }

  return *this;
}

CElementIterator& CElementIterator::operator ++
(
)
{ // Move the CElementIterator to the next Elements that share the same root
  return GetNext();
}

void CElementIterator::Delete
(
)
{ // Delete the associated element. After deletion, the element iterator will become invalidated.
  if(IsValid())
  {
    CElement* l_poElementParent = mp_poElementCurrent->GetParent();

    if(!l_poElementParent)
    {
      // This is the root, just delete it
      delete mp_poElementCurrent;
    }
    else
    {
      // Should call something like l_poElementParent->DeleteChild(mp_poElementCurrent);
      l_poElementParent->DeleteChild(mp_poElementCurrent);
    }

    mp_bValid           = false;
    mp_poElementCurrent = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// class CXmlConf

CXmlConf:: ~CXmlConf
(
)
{
  if(mp_poScanner)
  {
    delete mp_poScanner;
  }

  if(mp_poElementRoot)
  {
    delete mp_poElementRoot;
  }

  if(mp_panBuffer)
  {
    delete mp_panBuffer;
  }
}

CXmlConf::CXmlConf
( const char* i_panFileName
)
: mp_bValid(false)
, mp_poElementRoot(NULL)
, mp_panBuffer(NULL)
, mp_poScanner(NULL)
{ // Constructor by Reading the xml file from 'i_panFileName'
  FILE* l_sFile = fopen(i_panFileName, "r");
  if(!l_sFile)
  {
    return;
  }

  fseek(l_sFile, 0, SEEK_END);
  unsigned int l_nAttributeCount = ftell(l_sFile);
  fseek(l_sFile, 0, SEEK_SET);

  mp_panBuffer = new char[l_nAttributeCount];

  unsigned int l_nByteRead = 0;
  while(l_nByteRead < l_nAttributeCount)
  {
    unsigned int l_nBlockSize = fread(mp_panBuffer+l_nByteRead, 1, l_nAttributeCount-l_nByteRead, l_sFile);

    if(l_nBlockSize < 0)
    {
      fclose(l_sFile);
      delete [] mp_panBuffer;
      return;
    }
    else if(l_nBlockSize == 0)
    {
      break;
    }

    l_nByteRead += l_nBlockSize;
  }

  fclose(l_sFile);

  if(!Parse(mp_panBuffer, l_nByteRead))
  {
    mp_bValid = false;
  }
  else
  {
    mp_bValid = true;
  }
}

CXmlConf::CXmlConf
( char* i_panCharBegin
, char* i_panCharEnd
)
: mp_bValid(false)
, mp_poElementRoot(NULL)
, mp_panBuffer(NULL)
, mp_poScanner(NULL)
{ // Constructor by getting the input 'Buffer'
  int l_nBufferSize = i_panCharEnd - i_panCharBegin;
  mp_panBuffer   = new char[l_nBufferSize];

  std::copy
  ( i_panCharBegin
  , i_panCharEnd
  , mp_panBuffer
  );

  if(!Parse(mp_panBuffer, l_nBufferSize))
  {
    mp_bValid = false;
  }
  else
  {
    mp_bValid = true;
  }
}

bool CXmlConf::Parse
( char* i_panBuffer
, int   i_nBufferSize
)
{
  g_sCharTypeTable::InitCharType();

  mp_poScanner     = new CScanner(i_panBuffer, i_panBuffer + i_nBufferSize);
  mp_poElementRoot = new CElement(NULL);
  mp_poScanner->SkipWhiteSpace();

  CPIParser l_oParserPI(NULL);
  if(!l_oParserPI.Parse(mp_poScanner))
  {
    return false;
  }
  else
  {
    mp_poScanner->SkipWhiteSpace();

    if(!mp_poElementRoot->Parse(mp_poScanner))
    {
      return false;
    }
    else
    {
      return true;
    }
  }
}

/*
CElement* CXmlConf::Clone()
{ // Create a XML tree from the internal XML data and return the pointer of the Root.
  return ::Clone(mp_poElementRoot);
}
*/

CElement* CXmlConf::GetRootElement
( const char* i_panAttributeName
)
{
  string l_oStdString = (i_panAttributeName);
  int    l_nPos       = l_oStdString.find_first_of(".", 0);
  string l_oStdTempo  = l_oStdString;

  if(l_nPos != -1)
  {
    l_oStdTempo = l_oStdString.substr(0, l_nPos);
  }

  if(*this)
  {
    if(!const_cast <CStringValue&> (mp_poElementRoot->GetName()).Equal(const_cast<char*> (l_oStdTempo.c_str())))
    {
      return NULL;
    }

    if(l_nPos == -1)
    {
      return mp_poElementRoot;
    }

    string sRemain = l_oStdString.substr(l_nPos + 1, l_oStdString.size());
    return GetRootElement()->GetFirstChild(sRemain.c_str());
  }

  return NULL;
}

ostream & operator <<
( ostream&        o_rStdStreamOut
, CXmlConf const& i_roObject
)
{
  o_rStdStreamOut << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";

  if
  (
       i_roObject
    && i_roObject.GetRootElement()
  )
  {
    o_rStdStreamOut << *i_roObject.GetRootElement() << endl;
  }

  return o_rStdStreamOut;
}
