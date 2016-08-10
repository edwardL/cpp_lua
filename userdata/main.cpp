#include<stdio.h>
#include<string.h>

extern "C" 
{  
#include<lua.h>
#include <lauxlib.h>  
#include <lualib.h>  
} 

#pragma  comment(lib,"lua5.1.lib")

typedef struct NumArray
{
	int size;
	double values[1];
}NumArray;

// lua��䣺newarray(size)
extern "C" int newarray(lua_State* L)
{
	int n = luaL_checkint(L, 1);
	size_t nbytes = sizeof(NumArray) + (n - 1)*sizeof(double);
	NumArray* a = (NumArray*)lua_newuserdata(L, nbytes);
	a->size = n;
	return 1;    // �½���userdata��ѹջ
}

// lua��䣺setarray(userdata, index, value)
extern "C" int setarray(lua_State* L)
{
	NumArray* a = (NumArray*)lua_touserdata(L, 1);
	int index = luaL_checkint(L, 2);
	double value = luaL_checknumber(L, 3);

	luaL_argcheck(L, a != NULL, 1, "array excepted");
	luaL_argcheck(L, index >= 1 && index <= a->size, 2, "index out of range");

	a->values[index - 1] = value;
	return 0;
}

// lua��䣺getarray(userdata, index)
extern "C" int getarray(lua_State* L)
{
	NumArray* a = (NumArray*)lua_touserdata(L, 1);
	int index = luaL_checkint(L, 2);

	luaL_argcheck(L, a != NULL, 1, "array excepted");
	luaL_argcheck(L, index >= 1 && index <= a->size, 2, "index out of range");

	lua_pushnumber(L, a->values[index - 1]);
	return 1;
}

// lua��䣺getsize(userdata)
extern "C" int getsize(lua_State* L)
{
	NumArray* a = (NumArray*)lua_touserdata(L, 1);
	luaL_argcheck(L, a != NULL, 1, "array excepted");
	lua_pushnumber(L, a->size);
	return 1;
}

static const struct luaL_reg arraylib[] = 
{
	{"new", newarray},
	{"set", setarray},
	{"get", getarray},
	{"size", getsize},
	{NULL, NULL}
};

extern "C" __declspec(dllexport)
	int luaopen_array(lua_State* L)
{
	const char* libName = "userdata";
	luaL_register(L, libName, arraylib);
	return 1;
}

