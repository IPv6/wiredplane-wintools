#ifndef __HTMENGINE_AUX_H__
#define __HTMENGINE_AUX_H__

#include <stdio.h>
#include <stdlib.h>

#include "json-aux.h"

//
// This file is a part of 
// Terra Informatica Lightweight Embeddable HTMEngine control SDK
// Created by Andrew Fedoniouk @ TerraInformatica.com
//

//
// Auxiliary classes and functions
//

#define SIZED_STRUCT( type_name, var_name ) type_name var_name; memset(&var_name,0,sizeof(var_name)); var_name.cbSize = sizeof(var_name)

/* other stuff moved to json-aux.h */

#endif