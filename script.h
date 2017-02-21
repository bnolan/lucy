#pragma once

#include "node.h"
extern "C" {
  #include <lua.h>
}

int startScript(Node::ptr, std::string);

class Script: public Node {

public:
  lua_State *L;

  Script ();

  void tick ();
};
