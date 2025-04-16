#include "ScriptManager.h"

using namespace engineSpace;

void ScriptManager::Startup() {
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table);
}

bool ScriptManager::LoadScript(const string& scriptName, const string& path) {
	sol::load_result loadedScript = lua.load_file(path);
	if (!loadedScript.valid()) {
		return false;
	}
	sol::protected_function newFunc = loadedScript;
	solmap[scriptName] = newFunc;
	return true;
}

void ScriptManager::RunScript(const string& scriptName) {
	solmap[scriptName]();
}

sol::state& ScriptManager::GetLua() {
	return lua;
}