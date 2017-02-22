#include <stdio.h>
#include <string.h>
#include <iostream>

#include "script.h"

extern "C" {
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}

typedef std::weak_ptr<Node> NodeWeakPtr;

#define METATABLE "NodeMetaTable"

void registerNode (lua_State *lua, Node::ptr node);

Script::Script () : Node("script") {
  initialized = false;
}

void Script::initialize () {
  // Create new state
  L = luaL_newstate();
  luaL_openlibs(L);

  // Register this script node
  registerNode(L, self());
  lua_setglobal(L, "Element");

  // fixme - test status: int status = 
  luaL_loadstring(L, innerText.c_str());

  /* Ask Lua to run our little script */
  int result = lua_pcall(L, 0, LUA_MULTRET, 0);
  if (result) {
    fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
  }

  // Clear stack
  lua_pop(L, 1);

  // Done
  initialized = true;
}

void Script::tick () {
  if (!initialized) {
    throw "Trying to tick uninitialized script";
  }

  lua_getglobal(L, "tick");  /* function to be called */
  lua_pushnumber(L, 200); // milliseconds

  if (lua_pcall(L, 1, 1, 0) != 0) {
    std::cout << "error running function `tick': " << lua_tostring(L, -1) << "\n";
  }
}

#define getWeakPtr auto weakPtr = *static_cast<NodeWeakPtr*>(luaL_checkudata(lua, 1, METATABLE))

int setAttribute (lua_State *lua) {
    getWeakPtr;

    if (auto node = weakPtr.lock()) {
      std::string key = luaL_checkstring(lua, 2);
      std::string value = luaL_checkstring(lua, 3);

      node->setAttribute(key, value);
    } else {
      // todo
    }

    return 0;
}

int getAttribute (lua_State *lua) {
    getWeakPtr;

    if (auto node = weakPtr.lock()) {
      std::string key = luaL_checkstring(lua, 2);
      lua_pushstring(lua, node->getAttribute(key).c_str());
    } else {
      // todo
    }

    return 1;
}

int nodeName (lua_State *lua) {
    getWeakPtr;

    if (auto node = weakPtr.lock()) {
      lua_pushstring(lua, node->nodeName.c_str());
    } else {
      // todo
    }

    return 1;
}

int parentNode (lua_State *lua) {
    getWeakPtr;

    if (auto node = weakPtr.lock()) {
      registerNode(lua, node->parentNode->self());
    } else {
      // todo
    }

    return 1;
}

int childNodes (lua_State *lua) {
    getWeakPtr;

    if (auto node = weakPtr.lock()) {
        /* We will pass a table */
        lua_newtable(lua);    

        /*
        * To put values into the table, we first push the index, then the
        * value, and then call lua_rawset() with the index of the table in the
        * stack. Let's see why it's -3: In Lua, the value -1 always refers to
        * the top of the stack. When you create the table with lua_newtable(),
        * the table gets pushed into the top of the stack. When you push the
        * index and then the cell value, the stack looks like:
        *
        * <- [stack bottom] -- table, index, value [top]
        *
        * So the -1 will refer to the cell value, thus -3 is used to refer to
        * the table itself. Note that lua_rawset() pops the two last elements
        * of the stack, so that after it has been called, the table is at the
        * top of the stack.
        */
        int i = 0;

        for (auto const n : node->childNodes) {
            i++;

            lua_pushnumber(lua, i);
            registerNode(lua, n);
            lua_rawset(lua, -3);
        }

        return 1;
    } else {
        return 0;
    }
}

// From here:
//    http://stackoverflow.com/questions/1047212/detecting-stale-c-references-in-lua

void registerNode (lua_State *lua, Node::ptr node) {
    // We assume that the person is a valid pointer
    auto userdata = static_cast<NodeWeakPtr*>(lua_newuserdata(lua, sizeof(NodeWeakPtr)));

    // Store the pointer in userdata. We use a weak ptr, so
    // we have to check the node is still valid on each access.
    new(userdata) NodeWeakPtr(node);

    luaL_newmetatable(lua, METATABLE);

    lua_pushvalue(lua, -1); 
    lua_setfield(lua, -2, "__index");
    
    lua_pushcfunction(lua, setAttribute); 
    lua_setfield(lua, -2, "setAttribute");

    lua_pushcfunction(lua, getAttribute); 
    lua_setfield(lua, -2, "getAttribute");

    lua_pushcfunction(lua, childNodes); 
    lua_setfield(lua, -2, "childNodes");

    lua_pushcfunction(lua, nodeName); 
    lua_setfield(lua, -2, "nodeName");

    lua_pushcfunction(lua, parentNode); 
    lua_setfield(lua, -2, "parentNode");

    lua_setmetatable(lua, -2);
}
