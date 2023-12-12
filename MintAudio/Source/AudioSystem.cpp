#include <MintAudio/Include/AudioSystem.h>

#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/Vector.hpp>
#include <MintPlatform/Include/BinaryFile.hpp>

#include <xaudio2.h>


namespace mint
{
#define fourccRIFF	'FFIR'
#define fourccDATA	'atad'
#define fourccFMT	' tmf'
#define fourccWAVE	'EVAW'


	AudioItem::AudioItem()
		: _bitsPerSample{ 0 }
		, _samplesPerSec{ 0 }
		, _channelCount{ 0 }
		, _byteLength{ 0 }
		, _lengthSec{ 0.0f }
		, _sourceVoice{ nullptr }
	{
		__noop;
	}
	AudioItem::~AudioItem()
	{
		if (_sourceVoice != nullptr)
		{
			Stop();
			_sourceVoice->DestroyVoice();
		}
	}
	bool AudioItem::Play()
	{
		if (_sourceVoice == nullptr)
			return false;

		_sourceVoice->Start();
		return true;
	}
	void AudioItem::Stop()
	{
		if (_sourceVoice == nullptr)
			return;

		_sourceVoice->Stop();
	}


	AudioSystem::AudioSystem()
		: _xAudio2{ nullptr }
		, _masteringVoice{ nullptr }
	{
		if (FAILED(::CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		{
			MINT_NEVER;
		}

		if (FAILED(::XAudio2Create(&_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
		{
			MINT_NEVER;
		}

		if (FAILED(_xAudio2->CreateMasteringVoice(&_masteringVoice)))
		{
			MINT_NEVER;
		}
	}

	AudioSystem::~AudioSystem()
	{
		if (_masteringVoice != nullptr)
		{
			_masteringVoice->DestroyVoice();
		}

		if (_xAudio2 != nullptr)
		{
			_xAudio2->Release();
			_xAudio2 = nullptr;
		}

		::CoUninitialize();
	}

	bool AudioSystem::LoadAudio(const StringA& fileName, AudioItem& audioItem)
	{
		audioItem._fileName.Clear();

		BinaryFileReader fileReader;
		if (fileReader.Open(fileName.CString()) == false)
		{
			return false;
		}

		DWORD chunkSize = 0;
		DWORD chunkPosition = 0;
		DWORD fileType = 0;
		FindChunk(fileReader, fourccRIFF, chunkSize, chunkPosition);
		ReadChunkData(fileReader, &fileType, sizeof(DWORD), chunkPosition);
		if (fileType != fourccWAVE)
			return false;

		FindChunk(fileReader, fourccFMT, chunkSize, chunkPosition);
		WAVEFORMATEXTENSIBLE waveFormatExt{};
		ReadChunkData(fileReader, &waveFormatExt, chunkSize, chunkPosition);

		FindChunk(fileReader, fourccDATA, chunkSize, chunkPosition);
		audioItem._audioDataBuffer.Resize(chunkSize);
		ReadChunkData(fileReader, &audioItem._audioDataBuffer[0], chunkSize, chunkPosition);

		audioItem._bitsPerSample = waveFormatExt.Format.wBitsPerSample;
		audioItem._samplesPerSec = waveFormatExt.Format.nSamplesPerSec;
		audioItem._channelCount = waveFormatExt.Format.nChannels;
		audioItem._byteLength = chunkSize;
		const uint32 bytesPerSample = audioItem._bitsPerSample / 8;
		const uint32 bytesPerSec = audioItem._samplesPerSec * bytesPerSample;
		audioItem._lengthSec = static_cast<float>(audioItem._byteLength) / (bytesPerSec * audioItem._channelCount);

		XAUDIO2_BUFFER buffer{};
		buffer.AudioBytes = audioItem._byteLength;
		buffer.pAudioData = &audioItem._audioDataBuffer[0];
		buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

		if (FAILED(_xAudio2->CreateSourceVoice(&audioItem._sourceVoice, (WAVEFORMATEX*)&waveFormatExt)))
			return false;

		if (FAILED(audioItem._sourceVoice->SubmitSourceBuffer(&buffer)))
			return false;

		audioItem._fileName = fileName;
		return true;
	}

	bool AudioSystem::FindChunk(BinaryFileReader& fileReader, DWORD fourCC, DWORD& outChunkSize, DWORD& outChunkDataPosition)
	{
		fileReader.GoTo(0);

		DWORD chunkType = 0;
		DWORD chunkDataSize = 0;
		DWORD RIFFDataSize = 0;
		DWORD fileType = 0;
		DWORD bytesRead = 0;
		DWORD offset = 0;
		while (true)
		{
			if (fileReader.CanRead<DWORD>() == false)
			{
				return false;
			}

			chunkType = *fileReader.Read<DWORD>();

			if (fileReader.CanRead<DWORD>() == false)
			{
				return false;
			}

			chunkDataSize = *fileReader.Read<DWORD>();

			switch (chunkType)
			{
			case fourccRIFF:
				RIFFDataSize = chunkDataSize;
				chunkDataSize = 4;

				if (fileReader.CanRead<DWORD>() == false)
				{
					return false;
				}

				fileType = *fileReader.Read<DWORD>();
				break;
			default:
				fileReader.Skip(chunkDataSize);
				break;
			}

			offset += sizeof(DWORD) * 2;

			if (chunkType == fourCC)
			{
				outChunkSize = chunkDataSize;
				outChunkDataPosition = offset;
				return true;
			}

			offset += chunkDataSize;

			if (bytesRead >= RIFFDataSize)
			{
				return false;
			}
		}
		return true;
	}

	void AudioSystem::ReadChunkData(BinaryFileReader& fileReader, void* outBuffer, DWORD bufferSize, DWORD bufferOffset)
	{
		fileReader.GoTo(bufferOffset);

		::memcpy(outBuffer, fileReader.Read<byte>(bufferSize), bufferSize);
	}
}
