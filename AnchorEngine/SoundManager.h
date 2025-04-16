#include "soloud.h"
#include "soloud_wav.h"
#include <unordered_map>
#include "Types.h"

#include <thread>
#include <chrono>
#include <filesystem>
#include <algorithm>

namespace engineSpace {

	struct EngineSound {
		SoLoud::Wav sound;
		real soundLength;
		real secSincePlay = -1.0;
	};

	class SoundManager {
	public:
		SoLoud::Soloud soundLib;

		std::unordered_map<string, EngineSound> soundMap;
		
		bool LoadSound(const string& name, const string& path);
		void PlaySound(const string& name);
		void StopSound(const string& name);

		void UpdateSounds(real deltaSeconds);
			
		void Startup();
		void Shutdown();

	};

}