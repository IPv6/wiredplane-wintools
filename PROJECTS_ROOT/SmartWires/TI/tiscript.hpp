#ifndef __tiscript_hpp__
#define __tiscript_hpp__

/*
 * Terra Informatica Script Engine.
 * Copyright 2005, Andrew Fedoniouk 
 * 
 * TISCRIPT is a variation of ECMAScript.
 * Compiler, bytecoded VM, copying GC
 *
 * C++ API.
 *
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>

#include "tiscript.h"

namespace tiscript 
{
  typedef wchar_t wchar;
  typedef json::value  value;
  typedef json::string string;

  class engine;

  class stream 
  {
    friend class engine;
  public:
    stream() {}

    virtual const wchar_t* stream_name() { return 0; }
    
    // these two needs to be overrided
    virtual int  get() { return -1; }
    virtual bool put(int v) { v; return false; }

    void *tag() { return this; }
    static int CALLBACK input(void *tag)          {  return ((stream*)tag)->get(); }
    static int CALLBACK output(void *tag, int v)  {  return ((stream*)tag)->put(v); }
  };

  class engine
  {
  
public:	  
	  HSCRIPT _hengine;
    bool    _owner;
    stream *_console;

  
    engine(stream *console = 0): _console(console), _owner(true)
    {
      _hengine = ::TIS_create_engine( 0xFFFFFFFF, this, 0 );
      if( _console )
        ::TIS_set_std_streams(_hengine, 
        _console->tag(), &stream::input, // stdin in script
        _console->tag(), &stream::output, // stdout in script
        _console->tag(), &stream::output); // stderr in script
    }

    explicit engine(HSCRIPT hengine): _hengine(hengine), _owner(false) { }

    ~engine()
    {
      if(_owner)
        ::TIS_destroy_engine( _hengine );
    }

    bool load(stream* in, bool hypertext_mode)
    {
      return 0 != ::TIS_load(_hengine, in->tag(), &stream::input, in->stream_name(), hypertext_mode? TRUE:FALSE);
    }

    bool compile(stream* in, stream* out, bool hypertext_mode)
    {
      return 0 != ::TIS_compile(_hengine, in->tag(), &stream::input, in->stream_name(), out->tag(), &stream::output, hypertext_mode? TRUE:FALSE);
    }

    bool loadbc( stream* bytecodes )
    {
      return 0 != ::TIS_loadbc(_hengine, bytecodes->tag(), &stream::input);
    }

    bool call(const char* funcpath, JSON_VALUE* argv, int argc, JSON_VALUE& r)
    {
      return ::TIS_call(_hengine, funcpath, argv, argc, &r) != 0;
    }

    // add native class to script namespace
    bool add(TIS_class_def* pcd)
    {
        return ::TIS_define_class( _hengine, pcd) != 0;
    }

    void throw_error(const wchar_t* message)
    {
      ::TIS_throw(_hengine, message);
    }

  };

  // various stream implementations
  class string_in_stream: public stream
  {
    const wchar* _str;  
  public:
    string_in_stream(const wchar* str): _str(str) {}
    virtual int get() 
    { 
      if(*_str) return *_str++;
      return -1; 
    }
  };
  class string_out_stream: public stream
  {
    wchar *_str, *_p, *_end;
  public:
    string_out_stream() { _p = _str = (wchar*)malloc( 128 * sizeof(wchar) ); _end = _str + 128; }
    virtual bool put(int v) 
    {
      if( _p >= _end )
      {
        size_t sz = _end - _str; size_t nsz = (sz * 2) / 3;
        wchar *nstr = (wchar*)realloc(_str, nsz * sizeof(wchar));
        if(!nstr) return false;
        _str = nstr; _p = _str + sz; _end = _str + nsz;
      }
      *_p++ = v;
      return true; 
    }
  };

  // simple file stream. 
  class file_in_stream: public stream
  {
    FILE * _file;  
    string _name;
  public:
    file_in_stream(const char* filename) {  _file = fopen(filename,"rb"); _name = aux::a2w(filename); }
    ~file_in_stream() { if(_file) fclose(_file); }

    virtual const wchar_t* stream_name() { return _name; }

    virtual int get() 
    { 
      if(!_file || feof(_file)) return -1;
      return fgetc(_file);
    }
    bool is_valid() const { return _file != 0; }
  };

  class file_in_stream_w: public stream
  {
    FILE * _file;
    string _name;
  public:
    file_in_stream_w(const char* filename) {  _file = fopen(filename,"rb"); _name = aux::a2w(filename); }
    ~file_in_stream_w() { if(_file) fclose(_file); }

    virtual const wchar_t* stream_name() { return _name; }

    virtual int get() 
    { 
      if(!_file || feof(_file)) return -1;
      wchar_t wc;
      if(fread(&wc,sizeof(wchar_t),1,_file)) return wc;
      return -1;
    }
    bool is_valid() const { return _file != 0; }
  };

  inline wchar oem2wchar(char c)
  {
    wchar wc = '?';
    MultiByteToWideChar(CP_OEMCP,0,&c,1,&wc,1);
    return wc;
  }
  inline char wchar2oem(wchar wc)
  {
    char c = '?';
    WideCharToMultiByte(CP_OEMCP,0,&wc,1,&c,1,0,0);
    return c;
  }

  class console: public stream 
  {
  public:
    virtual int  get() { int c = getchar(); return c != EOF? oem2wchar(c) : -1; }      
    virtual bool put(int v) { return putchar( wchar2oem(v) ) != EOF; }
  };


}

#endif 

