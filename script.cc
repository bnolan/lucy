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

#define METATABLE "NodeMetaTable"

int tickScript () {
    lua_getglobal(L, "tick");  /* function to be called */
    lua_pushnumber(L, 200); // milliseconds

    if (lua_pcall(L, 1, 1, 0) != 0) {
        std::cout << "error running function `tick': " << lua_tostring(L, -1) << "\n";
    }

    return 0;
}

int setAttribute (lua_State *lua) {
    Node *node = (*reinterpret_cast<Node**>(luaL_checkudata(L, 1, METATABLE)));

    std::string key = luaL_checkstring(lua, 2);
    std::string value = luaL_checkstring(lua, 3);

    node->setAttribute(key, value);

    return 0;
}

int childNodes (lua_State *L) {
    Node *node = (*reinterpret_cast<Node**>(luaL_checkudata(L, 1, METATABLE)));

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
        lua_pushnumber(L, i * 2); // this will be the child
        lua_rawset(L, -3);
    }

    return 1;
}

// Register MyObject to Lua
// void register_myobject(lua_State* L){
//     lua_register(L, METATABLE, myobject_new);
//     luaL_newmetatable(L, METATABLE);
//     lua_pushcfunction(L, myobject_delete); lua_setfield(L, -2, "__gc");
//     lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
//     lua_pushcfunction(L, setAttribute); lua_setfield(L, -2, "setAttribute");
//     lua_pushcfunction(L, myobject_get); lua_setfield(L, -2, "get");
//     lua_pop(L, 1);
// }

void registerNode (lua_State *lua, Node *node) {
    // We assume that the person is a valid pointer
    Node **nptr = (Node**)lua_newuserdata(lua, sizeof(Node*));

    // Store the pointer in userdata. You must take care to ensure 
    // the pointer is valid entire time Lua has access to it.
    *nptr = node; 

    // This is important. Since you 
    // may invoke it many times, you should check, whether the table is newly 
    // created or it already exists
    luaL_newmetatable(lua, METATABLE);

        // lua_pushcfunction(L, myobject_delete); lua_setfield(L, -2, "__gc");

    lua_pushvalue(L, -1); 
    lua_setfield(L, -2, "__index");
    
    lua_pushcfunction(L, setAttribute); 
    lua_setfield(L, -2, "setAttribute");

    lua_pushcfunction(L, childNodes); 
    lua_setfield(L, -2, "childNodes");

        // lua_pushcfunction(L, myobject_get); lua_setfield(L, -2, "get");


        //The table is newly created, so we register its functions
        // lua_pushvalue(lua, -1);
        // lua_setfield(lua, -2, "__index");

        // luaL_Reg nodeFunctions[] = {
        //     "setAttribute", lua_Node_setAttribute,
        //     nullptr, nullptr
        // };

        // luaL_register(lua, 0, nodeFunctions);
    // }

    lua_setmetatable(lua, -2);

    lua_setglobal(lua, "Element");
}

int startScript (Node *box) {
    int status, result, i;
    double sum;

    // element = box;

    /*
     * All Lua contexts are held in this structure. We work with it almost
     * all the time.
     */
    L = luaL_newstate();

    luaL_openlibs(L); /* Load Lua libraries */

    // Register node
    registerNode(L, box);

    /* Load the file containing the script we are going to run */
    status = luaL_loadfile(L, "script.lua");

    if (status) {
        /* If something went wrong, error message is at the top of */
        /* the stack */
        fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
        // exit(1);
    }

    /*
     * Ok, now here we go: We pass data to the lua script on the stack.
     * That is, we first have to prepare Lua's virtual stack the way we
     * want the script to receive it, then ask Lua to run it.
     */
    lua_newtable(L);    /* We will pass a table */

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
    for (i = 1; i <= 5; i++) {
        lua_pushnumber(L, i);   /* Push the table index */
        lua_pushnumber(L, i*2); /* Push the cell value */
        lua_rawset(L, -3);      /* Stores the pair in the table */
    }

    /* By what name is the script going to reference our table? */
    lua_setglobal(L, "foo");

    /* Ask Lua to run our little script */
    result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (result) {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        // exit(1);
    }

    /* Get the returned value at the top of the stack (index -1) */
    sum = lua_tonumber(L, -1);

    printf("Script returned: %.0f\n", sum);

    lua_pop(L, 1);  /* Take the returned value out of the stack */
    // lua_close(L);   /* Cya, Lua */

    tickScript();

    return 0;
}
