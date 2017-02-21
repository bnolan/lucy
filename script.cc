#include <stdio.h>
#include <string.h>
#include <iostream>

#include "script.h"

extern "C" {
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}

lua_State *L;
// Node *element;

typedef std::weak_ptr<Node> NodeWeakPtr;

#define METATABLE "NodeMetaTable"

Script::Script () : Node("script") {
}

void registerNode (lua_State *lua, Node::ptr node);

void Script::tick () {
  lua_getglobal(L, "tick");  /* function to be called */
  lua_pushnumber(L, 200); // milliseconds

  if (lua_pcall(L, 1, 1, 0) != 0) {
    std::cout << "error running function `tick': " << lua_tostring(L, -1) << "\n";
  }
}

#define getWeakPtr auto weakPtr = *static_cast<NodeWeakPtr*>(luaL_checkudata(L, 1, METATABLE))

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
      lua_pushstring(L, node->getAttribute(key).c_str());
    } else {
      // todo
    }

    return 1;
}


int nodeName (lua_State *lua) {
    getWeakPtr;

    if (auto node = weakPtr.lock()) {
      lua_pushstring(L, node->nodeName.c_str());
    } else {
      // todo
    }

    return 1;
}

int childNodes (lua_State *L) {
    getWeakPtr;

    if (auto node = weakPtr.lock()) {
        /* We will pass a table */
        lua_newtable(L);    

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

            lua_pushnumber(L, i);
            registerNode(L, n);
            lua_rawset(L, -3);
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

    lua_pushvalue(L, -1); 
    lua_setfield(L, -2, "__index");
    
    lua_pushcfunction(L, setAttribute); 
    lua_setfield(L, -2, "setAttribute");

    lua_pushcfunction(L, getAttribute); 
    lua_setfield(L, -2, "getAttribute");

    lua_pushcfunction(L, childNodes); 
    lua_setfield(L, -2, "childNodes");

    lua_pushcfunction(L, nodeName); 
    lua_setfield(L, -2, "nodeName");

    lua_setmetatable(lua, -2);
}

int startScript (Node::ptr box, std::string source) {
    int status, result;

    // element = box;

    if (L) {
        lua_close(L);   /* Cya, Lua */
    }

    /*
     * All Lua contexts are held in this structure. We work with it almost
     * all the time.
     */
    L = luaL_newstate();

    luaL_openlibs(L); /* Load Lua libraries */

    // Register node
    registerNode(L, box);
    lua_setglobal(L, "Element");

    /* Load the file containing the script we are going to run */
    if (source.empty()) {
        status = luaL_loadfile(L, "script.lua");
    } else {
        status = luaL_loadstring(L, source.c_str());
    }

    if (status) {
        /* If something went wrong, error message is at the top of */
        /* the stack */
        fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
        // exit(1);
    }

    /* Ask Lua to run our little script */
    result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (result) {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        // exit(1);
    }

    /* Get the returned value at the top of the stack (index -1) */

    // printf("Script returned: %.0f\n", sum);

    lua_pop(L, 1);  /* Take the returned value out of the stack */

    // tickScript();

    return 0;
}
