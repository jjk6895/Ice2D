#include "pch.h"

#include "Sound.h"
#include "SafeRelease.h"
#include "HRException.h"

namespace Ice2D
{
    Sound::Sound() : m_buffer({}), m_wfx({})
    {
    }

    Sound::Sound(ResourceManager* pManager, const wchar_t* filePath) : IBasicResource(pManager)
    {
        HRESULT hr = LoadWav(filePath);
        CheckHR(hr);
        OnLoad();
    }

    Sound::Sound(Sound&& other) noexcept : IBasicResource(other), m_buffer(other.m_buffer), m_wfx(other.m_wfx)
    {
        other.m_buffer.pAudioData = nullptr;
        OnLoad();
    }

    Sound& Sound::operator=(Sound&& other) noexcept
    {
        if (this == &other) return *this;
        m_pManager = other.m_pManager;
        Release();
        m_buffer = other.m_buffer;
        m_wfx = other.m_wfx;
        other.m_buffer.pAudioData = nullptr;

        OnLoad();
        return *this;
    }

    Sound::~Sound()
    {
        Release();
    }

    void Sound::Release()
    {
        if (m_buffer.pAudioData)
        {
            delete[] m_buffer.pAudioData;
            m_buffer.pAudioData = nullptr;
        }

        OnUnload();
    }

    XAUDIO2_BUFFER* Sound::GetBuffer()
    {
        if (!m_buffer.pAudioData) throw std::runtime_error("Sound buffer contains no audio data.");
        return &m_buffer;
    }

    WAVEFORMATEX* Sound::GetFormat()
    {
        return (WAVEFORMATEX*)&m_wfx;
    }

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif
#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif
    HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
    {
        HRESULT hr = S_OK;
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());

        DWORD dwChunkType;
        DWORD dwChunkDataSize;
        DWORD dwRIFFDataSize = 0;
        DWORD dwFileType;
        DWORD bytesRead = 0;
        DWORD dwOffset = 0;

        while (hr == S_OK)
        {
            DWORD dwRead;
            if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());

            if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());

            switch (dwChunkType)
            {
            case fourccRIFF:
                dwRIFFDataSize = dwChunkDataSize;
                dwChunkDataSize = 4;
                if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                    hr = HRESULT_FROM_WIN32(GetLastError());
                break;

            default:
                if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                    return HRESULT_FROM_WIN32(GetLastError());
            }

            dwOffset += sizeof(DWORD) * 2;

            if (dwChunkType == fourcc)
            {
                dwChunkSize = dwChunkDataSize;
                dwChunkDataPosition = dwOffset;
                return S_OK;
            }

            dwOffset += dwChunkDataSize;

            if (bytesRead >= dwRIFFDataSize) return S_FALSE;
        }

        return hr;
    }

    HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
    {
        HRESULT hr = S_OK;
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());
        DWORD dwRead;
        if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    HRESULT Sound::LoadWav(const wchar_t* filePath)
    {
        m_buffer = { 0 };
        m_wfx = { 0 };

        // Open the file
        HANDLE hFile = CreateFile(
            filePath,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (INVALID_HANDLE_VALUE == hFile)
            return HRESULT_FROM_WIN32(GetLastError());

        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());

        DWORD dwChunkSize;
        DWORD dwChunkPosition;
        // check the file type, should be fourccWAVE or 'XWMA'
        FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
        DWORD filetype;
        ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
        if (filetype != fourccWAVE)
            return S_FALSE;

        FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
        ReadChunkData(hFile, &m_wfx, dwChunkSize, dwChunkPosition);

        // fill out the audio data buffer with the contents of the fourccDATA chunk
        FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
        BYTE* pDataBuffer = new BYTE[dwChunkSize];
        ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

        m_buffer.AudioBytes = dwChunkSize;  // size of the audio buffer in bytes
        m_buffer.pAudioData = pDataBuffer;  // buffer containing audio data
        m_buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

        return S_OK;
    }

    Voice::Voice() : m_pVoice(nullptr)
    {
    }

    Voice::Voice(ResourceManager* pManager, const WAVEFORMATEX* pFormat) : IBasicResource(pManager)
    {
        HRESULT hr = pManager->GetXAudio()->CreateSourceVoice(&m_pVoice, pFormat);
        CheckHR(hr);
        OnLoad();
    }

    Voice::Voice(Voice&& other) noexcept : IBasicResource(other), m_pVoice(other.m_pVoice)
    {
        other.m_pVoice = nullptr;
        OnLoad();
    }

    Voice& Voice::operator=(Voice&& other) noexcept
    {
        if (this == &other) return *this;
        m_pManager = other.m_pManager;
        Release();
        m_pVoice = other.m_pVoice;
        other.m_pVoice = nullptr;

        OnLoad();
        return *this;
    }

    Voice::~Voice()
    {
        Release();
    }

    void Voice::Release()
    {
        if (m_pVoice)
        {
            m_pVoice->DestroyVoice();
            m_pVoice = nullptr;
        }

        OnUnload();
    }

    void Voice::SubmitBuffer(const XAUDIO2_BUFFER* buffer)
    {
        if (!m_pVoice) throw std::runtime_error("Voice is null.");
        m_pVoice->SubmitSourceBuffer(buffer);
    }

    void Voice::Play()
    {
        if (!m_pVoice) throw std::runtime_error("Voice is null.");
        HRESULT hr = m_pVoice->Start();
        CheckHR(hr);
    }

    void Voice::Stop()
    {
        if (!m_pVoice) throw std::runtime_error("Voice is null.");
        HRESULT hr = m_pVoice->Stop();
        CheckHR(hr);
    }

    IXAudio2SourceVoice* Voice::Get() const
    {
        if (!m_pVoice) throw std::runtime_error("Voice is null.");
        return m_pVoice;
    }
}