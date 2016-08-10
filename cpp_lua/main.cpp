#include<iostream>
using namespace std;

extern "C"{
	#include<lua.h>
	#include<lualib.h>
	#include<lua.hpp>
};

#pragma  comment(lib,"lua5.1.lib")
void test1(lua_State* L)
{
	luaL_dostring(L,"function showstr(str) print(str) end");
	luaL_dostring(L,"function add(x,y) return x+y end");
	const char *pstr = "hello edward";
	lua_getglobal(L,"showstr");
	lua_pushstring(L,pstr);
	lua_pcall(L,1,0,0);

	lua_getglobal(L,"add");
	lua_pushinteger(L,2);
	lua_pushinteger(L,3);
	lua_pcall(L,2,1,0);

	printf("lua add function return val is %d\n",lua_tointeger(L,-1));

}
void luaParser2(lua_State* L)
{
	char buff[256];
	int error;
	while(fgets(buff,256,stdin) != NULL)
	{
		error = luaL_loadbuffer(L,buff,strlen(buff),"line") || lua_pcall(L,0,0,0);
		if(error)
		{
			fprintf(stderr,"%s",lua_tostring(L,-1));
			lua_pop(L,1);
		}
	}
}

void luaParser(lua_State* L)
{
	char buff[256];
	int error;
	while(fgets(buff,sizeof(buff),stdin) != NULL)
	{
		error = luaL_loadbuffer(L,buff,strlen(buff),"line") || lua_pcall(L,0,0,0);
		if(error)
		{
			fprintf(stderr,"%s",lua_tostring(L,-1));
			lua_pop(L,1);
		}
	}
}
/// 从栈底到栈顶依次遍历整个堆栈
static void stackDump(lua_State *L)
{
	int i;
	int top = lua_gettop(L);
	for(i = 1; i <= top; i++)
	{
		int t = lua_type(L,i);
		switch(t)
		{
		case LUA_TSTRING:
			printf("%s",lua_tostring(L,i));
			break;
		case LUA_TBOOLEAN:
			printf(" %g", lua_toboolean(L,i)? "true":"false");
			break;
		case LUA_TNUMBER:
			printf(" %g",lua_tonumber(L,i));
			break;
		default:
			printf(" %s",lua_typename(L,t));
			break;
		}
		printf("   ");
	}
	printf("\n");
}

void testDumpStack(lua_State *L)
{
	lua_pushboolean(L,1);
	lua_pushnumber(L,10);
	lua_pushstring(L,"hello");
	stackDump(L);

	lua_pushvalue(L,-4);
	stackDump(L);

	lua_replace(L,3);
	stackDump(L);

	lua_settop(L,6);
	stackDump(L);

	lua_remove(L,-3);
	stackDump(L);	
}

void oper_table(lua_State* L)
{

}

void error(lua_State *L,const char* fmt, ...)
{
	va_list argp;
	va_start(argp,fmt);
	vfprintf(stderr,fmt,argp);
	va_end(argp);
	lua_close(L);
	exit(EXIT_FAILURE);
}

void load(lua_State *L,char* filename, int* width, int* height)
{
	if(luaL_loadfile(L,filename) || lua_pcall(L,0,0,0))
	{
		error(L,"cannot run configure file %s",lua_tostring(L,-1));
	}

	lua_getglobal(L,"width");
	lua_getglobal(L,"height");

	if(!lua_isnumber(L,-2))
		error(L,"width should be a number");
	if(!lua_isnumber(L,-1))
		error(L,"height should be a number");

	*width =(int)lua_tonumber(L,-2);
	*height =(int)lua_tonumber(L,-1);
}

double cppCallLuaFunc(lua_State*L, double x, double y)
{
	double z;
	lua_getglobal(L,"f");
	lua_pushnumber(L,x);
	lua_pushnumber(L,y);

	if(lua_pcall(L,2,1,0) != 0)
		error(L,"error running func f ： %s", lua_tostring(L,-1));
	z = lua_tonumber(L,-1);
	lua_pop(L,1);
	return z;
}

int cppCallLuaFuncAndGetValueX(lua_State* L)
{
	int x ;
	lua_getglobal(L,"printMsg");
	if(lua_pcall(L,0,0,0) != 0)
		error(L,"error runing func f: %s",lua_tostring(L,-1));
	
	lua_getglobal(L,"x");
	x = lua_tointeger(L,-1);
	lua_pop(L,1);
	return x;
}

void access_lua_table(lua_State* L)
{
	printf("%d",lua_gettop(L));
	lua_getglobal(L,"me");
	if(!lua_istable(L,-1))
	{
		printf("error in table ");
		return;
	}
	/// 往栈里面压入一个key:name
	lua_pushstring(L,"name");
	lua_gettable(L,-2);
	printf("name = %s",lua_tostring(L,-1));
}



int main()
{
	lua_State *L = NULL;
	L = lua_open();
	luaL_openlibs(L);

	///test1(L);
	///luaParser(L);
	////testDumpStack(L);
	int width, height;
	load(L,"cfg.lua",&width,&height);
	printf("width = %d height = %d \n",width,height);
	double z = cppCallLuaFunc(L,1.0,2.0);
	int x = cppCallLuaFuncAndGetValueX(L);
	printf("cpp call lua function return value :%f\n",z);
	printf("cpp call lua function return value :%d\n",x);
	printf("---------------------------------\n");
	access_lua_table(L);
	///oper_table(L);
	lua_close(L);
	return 0;
}