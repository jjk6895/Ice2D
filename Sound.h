#pragma once
#include "ResourceManager.h"
#include <xaudio2.h>
#include <vector>
#include <fstream>

namespace Ice2D
{
	class Sound : private IBasicResource
	{
	public:
		Sound();
		Sound(ResourceManager* pManager, const wchar_t* filePath);
		Sound(const Sound& other) = delete;
		Sound& operator=(const Sound& other) = delete;
		Sound(Sound&& other) noexcept;
		Sound& operator=(Sound&& other) noexcept;
		~Sound();
		void Release() override;
		XAUDIO2_BUFFER* GetBuffer();
		WAVEFORMATEX* GetFormat();
	private:
		XAUDIO2_BUFFER m_buffer;
		WAVEFORMATEXTENSIBLE m_wfx;
		HRESULT LoadWav(const wchar_t* filePath);
	};

	class Voice : private IBasicResource
	{
	public:
		Voice();
		Voice(ResourceManager* pManager, const WAVEFORMATEX* pFormat);
		Voice(const Voice& other) = delete;
		Voice& operator=(const Voice& other) = delete;
		Voice(Voice&& other) noexcept;
		Voice& operator=(Voice&& other) noexcept;
		~Voice();
		void Release() override;
		void SubmitBuffer(const XAUDIO2_BUFFER* buffer);
		void Play();
		void Stop();
		IXAudio2SourceVoice* Get() const;
	private:
		IXAudio2SourceVoice* m_pVoice;
	};
}