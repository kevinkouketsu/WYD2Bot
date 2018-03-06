#pragma once
#ifndef LUASCRIPT_H
#define LUASCRIPT_H
/*
#include <string>
#include <vector>
#include <iostream>

// Lua is written in C, so compiler needs to know how to link its libraries
extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

class LuaScript {
public:
	LuaScript(const std::string& filename);
	~LuaScript();
	void printError(const std::string& variableName, const std::string& reason);

	template<typename T>
	T get(const std::string& variableName) {
		// will be implemented later in tutorial
	}
	bool lua_gettostack(const std::string& variableName) {
		// will be explained later too
	}

	// Generic get
	template<typename T>
	T lua_get(const std::string& variableName) {
		return 0;
	}

	// Generic default get
	template<typename T>
	T lua_getdefault(const std::string& variableName) {
		return 0;
	}
private:
	lua_State * L;
};

template <>
inline bool LuaScript::lua_get(const std::string& variableName) {
	return (bool)lua_toboolean(L, -1);
}

template <>
inline float LuaScript::lua_get(const std::string& variableName) {
	if (!lua_isnumber(L, -1)) {
		printError(variableName, "Not a number");
	}
	return (float)lua_tonumber(L, -1);
}

template <>
inline int LuaScript::lua_get(const std::string& variableName) {
	if (!lua_isnumber(L, -1)) {
		printError(variableName, "Not a number");
	}
	return (int)lua_tonumber(L, -1);
}

template <>
inline std::string LuaScript::lua_get(const std::string& variableName) {
	std::string s = "null";
	if (lua_isstring(L, -1)) {
		s = std::string(lua_tostring(L, -1));
	}
	else {
		printError(variableName, "Not a string");
	}
	return s;
}
*/
#endif