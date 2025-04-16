#include "SoundManager.h"
#include <iostream>

namespace engineSpace {

	void SoundManager::Startup() {
		this->soundLib.init();
	}

	void SoundManager::Shutdown() {
		this->soundLib.deinit();
	}

	bool SoundManager::LoadSound(const string& name, const string& path) {
		EngineSound newSound = {};
		bool error = soundMap[name].sound.load(path.c_str());
		if (!error) {
			soundMap[name].soundLength = soundMap[name].sound.getLength();
			soundMap[name].secSincePlay = -1.0;
		}
		return error;
	}

	void SoundManager::PlaySound(const string& name) {
		if (soundMap[name].secSincePlay == -1.0) {
			soundLib.play(soundMap[name].sound);
			soundMap[name].secSincePlay = 0;
		}
	}

	void SoundManager::StopSound(const string& name) {
		soundMap[name].sound.stop();
		soundMap[name].secSincePlay = -1.0;
	}

	void SoundManager::UpdateSounds(real deltaSeconds) {
		int iterations = 0;
		for ( auto& [name, sound] : soundMap) {
			bool goodSound = sound.secSincePlay == -1.0;
			if (!goodSound) {
				sound.secSincePlay += deltaSeconds;
				if (sound.soundLength < sound.secSincePlay) {
					sound.secSincePlay = -1.0;
					sound.sound.stop();
				}
			}
		}
	}
}