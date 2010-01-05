#include "ConfigurationService.h"

#include <iostream>

extern "C" 
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

// If your system does not support `stdout', you can just remove this function.
// If you need, you can define your own `print' function, following this
// model but changing `fputs' to put the strings at a proper place
// (a console window or a log file, for instance).

/*
static int luaB_print (lua_State *L) {
  int n = lua_gettop(L);  // number of arguments
  int i;
  lua_getglobal(L, "tostring");
  for (i=1; i<=n; i++) {
    const char *s;
    lua_pushvalue(L, -1);  // function to be called 
    lua_pushvalue(L, i);   // value to print 
    lua_call(L, 1, 1);
    s = lua_tostring(L, -1);  // get result
    if (s == NULL)
      return luaL_error(L, LUA_QL("tostring") " must return a string to "
      LUA_QL("print"));
    if (i>1) fputs("\t", stdout);
    fputs(s, stdout);
    lua_pop(L, 1);  // pop result 
  }
  fputs("\n", stdout);
  return 0;
}/**/

ConfigurationService::ConfigurationService(void)
{
}

ConfigurationService::~ConfigurationService(void)
{
}

bool ConfigurationService::loadConfiguration( std::string fileName )
{
  lua_State *L = lua_open();

  if (luaL_loadfile(L, fileName.c_str()) || lua_pcall(L, 0, 0, 0))
  {
    //error(L, "cannot run configuration file: %s", lua_tostring(L, -1));
  }

  //luaL_dostring(L, "print(\"Hello!!!\")"); 

  // outer loop 
  const char *key = 0; 
  int count = 0; 
  lua_pushvalue(L, LUA_GLOBALSINDEX); 
  lua_pushnil(L);  // first key 
  while (lua_next(L, LUA_GLOBALSINDEX) != 0) 
  { 
    int ktype = lua_type(L, -2); 
    int vtype = lua_type(L, -1); 
    const char* ktname = lua_typename(L, ktype); 
    const char* vtname = lua_typename(L, vtype); 
    const char* key = lua_tostring(L, -2); 
    const char* value = lua_tostring(L, -1);

    if(vtype == LUA_TTABLE)
    {
      std::cout << "Table: " << (key ? key : "null") << ": " << std::endl; 
      parseTable(L, lua_gettop(L)); 
    }
    else
    {
      std::cout << "Key: " << (key ? key : "null") << " (" << (ktname ? ktname : "null") << "), Value: "
        << (value ? value : "null") << " (" << (vtname ? vtname : "null") << ")" << std::endl; 
    }
    
    lua_pop(L, 1); 
  }

  lua_close(L);
  return true; 
}

void ConfigurationService::parseTable(lua_State *L, int idx)
{
  lua_pushnil(L);  // first key 
  while (lua_next(L, idx) != 0) 
  { 
    int ktype = lua_type(L, -2); 
    int vtype = lua_type(L, -1); 
    const char* ktname = lua_typename(L, ktype); 
    const char* vtname = lua_typename(L, vtype); 
    const char* key = lua_tostring(L, -2); 
    const char* value = lua_tostring(L, -1);

    std::cout << "  Key: " << (key ? key : "null") << " (" << (ktname ? ktname : "null") << "), Value: "
      << (value ? value : "null") << " (" << (vtname ? vtname : "null") << ")" << std::endl; 

    lua_pop(L, 1); 
  }
}