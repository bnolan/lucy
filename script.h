#pragma once

#include "node.h"

extern "C" {
  #include <lua.h>
}

class Script: public Node {

public:
  lua_State *L;
  bool initialized;
  
  Script ();
  void initialize () ;
  void tick ();
};
