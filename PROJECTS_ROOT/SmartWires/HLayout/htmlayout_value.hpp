/*
 * Terra Informatica Lightweight Embeddable HTMLayout control
 * http://terrainformatica.com/htmlayout
 * 
 * HTMLayout value class. 
 * 
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 * 
 * (C) 2003-2004, Andrew Fedoniouk (andrew@terrainformatica.com)
 */

/**\file
 * \brief value, aka variant, aka discriminated union
 **/

#ifndef __htmlayout_value_hpp__
#define __htmlayout_value_hpp__

#pragma warning(disable:4786) //identifier was truncated...

#include <assert.h>

#include "json-value.h"

#include <map>
#include <string>
#include <vector>

// DISCLAIMER: this code is using primitives from std:: namespace probably 
// in the not efficient manner. (I am not using std:: anywhere beside of this file)
// Feel free to change it. Would be nice if you will drop me your updates.

#pragma once

/**HTMLayout namespace.*/
namespace htmlayout
{

  /// \class value_t
  /// \brief The value type, can hold values of bool, int, double, string(wchar_t) and array(value_t)
  /// value_t is also known as JSON value, see: http://json.org/

  typedef json::named_value named_value_t;
  typedef json::value value_t;
}

#endif