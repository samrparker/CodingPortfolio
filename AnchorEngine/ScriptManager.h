#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <unordered_map>

#include "Types.h"

namespace engineSpace {
	class ScriptManager {
	public:
		sol::state lua;

		std::unordered_map<string, sol::protected_function> solmap;

		void Startup();

		bool LoadScript(const string& scriptName, const string& path);
		void RunScript(const string& scriptName);

		sol::state& GetLua();
	private:
	};
}