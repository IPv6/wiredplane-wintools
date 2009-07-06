//|
//|
//| Copyright (c) 2001-2006
//| Andrew Fedoniouk - andrew@terrainformatica.com
//|
//| slices, array fragments
//|
//|
/*
  slice token;
  tokens toks( "Hello world", ' ');
  while(toks.next(token))
  {
    // делаем чего-нибудь полезное с token
  }
*/

#ifndef __tl_slice_h__
#define __tl_slice_h__

//#include "tl_basic.h"

namespace tool
{

 template <typename T >
 struct slice
 {
    const T* start;
    size_t   length;

    slice(): start(0), length(0) {}
    slice(const T* start_, size_t length_) { start = start_; length = length_; }


    slice(const slice& src): start(src.start), length(src.length) {}
    slice(const T* start_, const T* end_): start(start_), length( max(end_-start_,0)) {}

    slice& operator = (const slice& src) { start = src.start; length = src.length; return *this; }

    const T*      end() const { return start + length; } 

    bool operator == ( const slice& r ) const 
    {  
      if( length != r.length )
        return false;
      for( uint i = 0; i < length; ++i )
        if( start[i] != r.start[i] )
          return false;
      return true;
    }

    bool operator != ( const slice& r ) const { return !operator==(r); }
           
    T operator[] ( uint idx ) const 
    {  
      assert( idx < length );
      if(idx < length)
        return start[idx];
      return 0;
    }

    // [idx1..length)
    slice operator() ( uint idx1 ) const 
    {  
      assert( idx1 < length );
      if ( idx1 < length ) 
          return slice( start + idx1, length - idx1 );
      return slice();
    }
    // [idx1..idx2)
    slice operator() ( uint idx1, uint idx2 ) const 
    {  
      assert( idx1 < length );
      assert( idx2 <= length );
      assert( idx1 <= idx2 );
      if ( idx1 < idx2 ) 
          return slice( start + idx1, idx2 - idx1 );
      return slice();
    }

    int index_of( T e ) const
    {  
      for( uint i = 0; i < length; ++i ) if( start[i] == e ) return i;
      return -1;
    }

    int last_index_of( T e ) const
    {  
      for( uint i = length; i > 0 ;) if( start[--i] == e ) return i;
      return -1;
    }

    int index_of( const slice& s ) const
    {  
      if( s.length > length ) return -1;
      if( s.length == 0 ) return -1;
      uint l = length - s.length;
      for( uint i = 0; i < l ; ++i) 
        if( start[i] == *s.start ) 
        {
          const T* p = s.start;
          uint last = i + s.length;
          for( uint j = i + 1; j < last; ++j ) 
            if( *(++p) != start[j]) 
              goto next_i;
          return i;
          next_i: continue;
        }
      return -1;
    }

    int last_index_of( const slice& s ) const
    {  
      if( s.length > length ) return -1;
      if( s.length == 0 ) return -1;
      const T* ps = s.end() - 1;
      for( uint i = length; i > 0 ; ) 
        if( start[--i] == *ps ) 
        {
          const T* p = ps;
          uint j, first = i - s.length + 1; 
          for( j = i; j > first; ) 
            if( *(--p) != start[--j]) 
              goto next_i;
          return j;
          next_i: continue;
        }
      return -1;
    }


 };
  
#define MAKE_SLICE( T, D ) slice<T>(D, sizeof(D) / sizeof(D[0]))

#ifdef _DEBUG

/*
inline void slice_unittest() 
{
  int v1[] = { 0,1,2,3,4,5,6,7,8,9 };
  int v2[] = { 3,4,5 };
  int v3[] = { 0,1,2 };
  int v4[] = { 0,1,2,4 };
  int v5[] = { 1,1,2,3 };

  slice<int> s1 = MAKE_SLICE( int, v1 );
  slice<int> s2 = MAKE_SLICE( int, v2 );
  slice<int> s3 = MAKE_SLICE( int, v3 );
  slice<int> s4 = MAKE_SLICE( int, v4 );
  slice<int> s5 = MAKE_SLICE( int, v5 );

  assert( s1 != s2 );
  assert( s1(3,6) == s2 );
  assert( s1.index_of(3) == 3 );
  assert( s1.index_of(s2) == 3 );
  assert( s1.last_index_of(3) == 3 );
  assert( s1.last_index_of(s2) == 3 );

  assert( s1.index_of(s3) == 0 );
  assert( s1.last_index_of(s3) == 0 );

  assert( s1.index_of(s4) == -1 );
  assert( s1.last_index_of(s4) == -1 );

  assert( s1.index_of(s5) == -1 );
  assert( s1.last_index_of(s5) == -1 );

}

#endif
*/

 template <typename T >
    class tokens
    {
      T   delimeter;
      const T* p;
      const T* start;
      const T* end;
      const T* tok()
      {
        for(;p && *p; ++p)
          if(*p == delimeter) return p++;
        return p;
      }
    public:

      tokens(const T *text, T separator = ' '): delimeter(separator)
      {
        start = p = text;
        end = tok();
      }
      bool next(slice<T>& v) 
      {  
        if(start && *start)
        {
          v.set(start,end-start);
          start = p; 
          end = tok();
          return true;
        }
        return false;
      }
    };

 template <typename T >
    class tokensz
    {
      T   delimeter;
      const T* p;
      const T* tail;
      const T* start;
      const T* end;
      const T* tok()
      {
        for(;p < tail; ++p)
          if(*p == delimeter) return p++;
        return p;
      }
    public:

      tokensz(const T *text, size_t text_length, T separator = ' '): delimeter(separator)
      {
        start = p = text;
        tail = p + text_length;
        end = tok();
      }
      bool next(slice<T>& v) 
      {  
        if(start < tail)
        {
          v.set(start,end-start);
          start = p; 
          end = tok();
          return true;
        }
        return false;
      }
    };

typedef slice<char> chars;
typedef slice<wchar_t> wchars;

}

#endif