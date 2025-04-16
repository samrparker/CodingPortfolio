#include "Types.h"
#include <filesystem>

namespace engineSpace {
	class ResourceManager {
	public:
		std::filesystem::path resolvePath(std::filesystem::path newPath);
	};
}