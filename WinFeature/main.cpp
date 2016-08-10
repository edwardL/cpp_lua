extern "C"  
{  
#include <lua.h>  
#include <lualib.h>  
#include <lauxlib.h>  
#pragma comment(lib, "lua5.1.lib")  
};

#include <Windows.h>  
#include <iostream>  
using namespace std;

static const char* const ERROR_ARGUMENT_COUNT = "参数数目错误！";  
static const char* const ERROR_ARGUMENT_TYPE  = "参数类型错误！";  

// 发生错误,报告错误
void ErrorMsg(lua_State* luaEnv, const char* const pszErrorInfo)  
{  
	lua_pushstring(luaEnv, pszErrorInfo);  
	lua_error(luaEnv);  
}  

// 检测函数调用参数个数是否正常
void CheckParamCount(lua_State* luaEnv, int paramCount)  
{  
	// lua_gettop获取栈中元素个数.  
	if (lua_gettop(luaEnv) != paramCount)  
	{  
		ErrorMsg(luaEnv, ERROR_ARGUMENT_COUNT);  
	}  
} 


// 显示Windows对话框.  
// @param [in] pszMessage string 1  
// @param [in] pszCaption string 2  
extern "C" int ShowMsgBox(lua_State* luaEnv)  
{  
	const char* pszMessage = 0;  
	const char* pszCaption = 0;  

	// 检测参数个数是否正确.  
	CheckParamCount(luaEnv, 2);  

	// 提取参数.  
	pszMessage = luaL_checkstring(luaEnv, 1);    /// 检测一个值是否为 string , 并返回。
	pszCaption = luaL_checkstring(luaEnv, 2);  

	if (pszCaption && pszMessage)  
	{  
		::MessageBoxA(  
			NULL,  
			pszMessage,  
			pszCaption,  
			MB_OK | MB_ICONINFORMATION  
			);  
	}  
	else  
	{  
		ErrorMsg(luaEnv, ERROR_ARGUMENT_TYPE);  
	}  

	// 返回值个数为0个.  
	return 0;  
}  
extern "C" int Add(lua_State* L)
{
	CheckParamCount(L,2);

	double left = luaL_checknumber(L,1);
	double right = luaL_checknumber(L,2);

	double result = left + right;
	lua_pushnumber(L,result);
	return 1;    /// 返回 1 代表返回参数的个数。。
}

/// test userdata  for lua
typedef struct NumArray
{
	int size;
	double values[1];
}NumberArray;



///  辅助函数，  检查数组的meta table 是否为meta
NumberArray* checkarray(lua_State* L)
{
	void* ud = luaL_checkudata(L,1,"meta");
	luaL_argcheck(L,ud != NULL,1,"array except");
	return (NumberArray*)ud;
}

//// lua 语句： new array
extern "C" int newarray(lua_State* L)
{
	int n = luaL_checkint(L,1);
	size_t nbytes = sizeof(NumberArray) + (n-1)*sizeof(double);
	NumberArray* a = (NumberArray*)lua_newuserdata(L,nbytes);

	/// 获取预先创建好的meta table , 并设置给新建的userdata
	luaL_getmetatable(L,"meta");
	lua_setmetatable(L,-2);

	a->size = n;
	return 1;		//// 新建的userdata 会压栈
}
/// lua 语句: setarray(userdata,index,value)
extern "C" int setarray(lua_State* L)
{
	NumberArray* a = (NumberArray*)lua_touserdata(L,1);
	int index = luaL_checkint(L,2);
	double value = luaL_checknumber(L,3);

	luaL_argcheck(L, a != NULL, 1,"array except");
	luaL_argcheck(L,index >= 1  && index <= a->size , 2 ,"index out of range");
	a->values[index-1] = value;
	return 0;
}
/// lua 语句: getarray(userdata,index)
extern "C" int getarray(lua_State* L)
{
///	NumberArray* a = (NumberArray*)lua_touserdata(L,1);
	NumberArray* a = checkarray(L);
	
	int index  = luaL_checkint(L,2);
	luaL_argcheck(L,a != NULL, 1,"array except");
	luaL_argcheck(L,index >= 1 && index <= a->size, 2, "index out of range");

	lua_pushnumber(L,a->values[index-1]);
	return 1;
}


/// lua 语句: getsize(userdata)
extern "C" int getsize(lua_State* L)
{
	NumberArray* a =(NumberArray*)lua_touserdata(L,1);
	luaL_argcheck(L, a!=NULL, 1, "array except");
	lua_pushnumber(L,a->size);
	return 1;
}


// 导出函数列表.  
static luaL_Reg luaLibs[] =  
{  
	{"ShowMsgBox", ShowMsgBox},
	{"Add",Add},
	{"new", newarray},
	{"set", setarray},
	{"get", getarray},
	{"size", getsize},
	{NULL, NULL}  
};  

// Dll入口函数，Lua调用此Dll的入口函数.  
extern "C" __declspec(dllexport)  
	int luaopen_WinFeature(lua_State* L)  
{  
	/// 创建数组userdata 将要用到的metatable
	luaL_newmetatable(L,"meta");

	const char* const LIBRARY_NAME = "WinFeature";  
	luaL_register(L, LIBRARY_NAME, luaLibs);  

	return 1;  
}  