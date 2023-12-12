#include <MintAudio/Include/AudioSystem.h>

#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/Vector.hpp>
#include <MintPlatform/Include/BinaryFile.hpp>

#include <xaudio2.h>

#pragma warning( disable : 6262 26451 4267 6386 6011 4244 6385 )
#define MINIMP3_IMPLEMENTATION
#include <Externals/minimp3/minimp3.h>
#include <Externals/minimp3/minimp3_ex.h>
#pragma warning( default : 6262 26451 4267 6386 6011 4244 6385 )


namespace mint
{
#define fourccRIFF	'FFIR'
#define fourccDATA	'atad'
#define fourccFMT	' tmf'
#define fourccWAVE	'EVAW'


#pragma region AudioItem
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
#pragma endregion


#pragma region AudioSystem
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

	bool AudioSystem::LoadAudioMP3(const StringA& fileName, AudioItem& audioItem)
	{
		mp3dec_t mp3d{};
		mp3dec_file_info_t info{};
		const int result = mp3dec_load(&mp3d, fileName.CString(), &info, NULL, NULL);
		if (result)
		{
			/* error */
			return false;
		}

		const uint32 bytesPerSample = static_cast<uint32>(sizeof(mp3d_sample_t));
		audioItem._byteLength = static_cast<uint32>(info.samples * bytesPerSample);
		audioItem._audioDataBuffer.Resize(audioItem._byteLength);
		{
			::memcpy(&audioItem._audioDataBuffer[0], info.buffer, audioItem._byteLength);
			::free(info.buffer);
			info.buffer = nullptr;
		}

		WAVEFORMATEXTENSIBLE waveFormatExt{};
		waveFormatExt.Format.wFormatTag = WAVE_FORMAT_PCM; // PCM (Pulse-Code modulation)
		waveFormatExt.Format.nChannels = info.channels;
		waveFormatExt.Format.nSamplesPerSec = info.hz; // Hertz
		waveFormatExt.Format.wBitsPerSample = bytesPerSample * 8;
		waveFormatExt.Format.nBlockAlign = waveFormatExt.Format.nChannels * bytesPerSample;
		waveFormatExt.Format.nAvgBytesPerSec = waveFormatExt.Format.nSamplesPerSec * bytesPerSample * waveFormatExt.Format.nChannels;

		audioItem._bitsPerSample = waveFormatExt.Format.wBitsPerSample;
		audioItem._samplesPerSec = waveFormatExt.Format.nSamplesPerSec;
		audioItem._channelCount = waveFormatExt.Format.nChannels;
		const uint32 bytesPerSec = audioItem._samplesPerSec * bytesPerSample;
		audioItem._lengthSec = static_cast<float>(audioItem._byteLength) / (bytesPerSec * audioItem._channelCount);

		XAUDIO2_BUFFER buffer{};
		buffer.AudioBytes = audioItem._byteLength;
		buffer.pAudioData = &audioItem._audioDataBuffer[0];
		buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

		if (FAILED(_xAudio2->CreateSourceVoice(&audioItem._sourceVoice, (WAVEFORMATEX*)&waveFormatExt)))
		{
			return false;
		}

		if (FAILED(audioItem._sourceVoice->SubmitSourceBuffer(&buffer)))
		{
			return false;
		}

		audioItem._fileName = fileName;
		return true;
	}

	bool AudioSystem::LoadAudioWAV(const StringA& fileName, AudioItem& audioItem)
	{
		audioItem._fileName.Clear();

		BinaryFileReader binaryFileReader;
		if (binaryFileReader.Open(fileName.CString()) == false)
		{
			return false;
		}

		BinaryPointerReader binaryPointerReader{ binaryFileReader.GetBytes().Data(), binaryFileReader.GetBytes().Size() };
		DWORD chunkSize = 0;
		DWORD chunkPosition = 0;
		DWORD fileType = 0;
		FindChunk(binaryPointerReader, fourccRIFF, chunkSize, chunkPosition);
		ReadChunkData(binaryPointerReader, &fileType, sizeof(DWORD), chunkPosition);
		if (fileType != fourccWAVE)
			return false;

		FindChunk(binaryPointerReader, fourccFMT, chunkSize, chunkPosition);
		WAVEFORMATEXTENSIBLE waveFormatExt{};
		ReadChunkData(binaryPointerReader, &waveFormatExt, chunkSize, chunkPosition);

		FindChunk(binaryPointerReader, fourccDATA, chunkSize, chunkPosition);
		audioItem._byteLength = chunkSize;
		audioItem._audioDataBuffer.Resize(audioItem._byteLength);
		ReadChunkData(binaryPointerReader, &audioItem._audioDataBuffer[0], chunkSize, chunkPosition);

		audioItem._bitsPerSample = waveFormatExt.Format.wBitsPerSample;
		audioItem._samplesPerSec = waveFormatExt.Format.nSamplesPerSec;
		audioItem._channelCount = waveFormatExt.Format.nChannels;
		const uint32 bytesPerSample = audioItem._bitsPerSample / 8;
		const uint32 bytesPerSec = audioItem._samplesPerSec * bytesPerSample;
		audioItem._lengthSec = static_cast<float>(audioItem._byteLength) / (bytesPerSec * audioItem._channelCount);

		XAUDIO2_BUFFER buffer{};
		buffer.AudioBytes = audioItem._byteLength;
		buffer.pAudioData = &audioItem._audioDataBuffer[0];
		buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
		if (FAILED(_xAudio2->CreateSourceVoice(&audioItem._sourceVoice, (WAVEFORMATEX*)&waveFormatExt)))
		{
			return false;
		}

		if (FAILED(audioItem._sourceVoice->SubmitSourceBuffer(&buffer)))
		{
			return false;
		}

		audioItem._fileName = fileName;
		return true;
	}

	bool AudioSystem::FindChunk(BinaryPointerReader& binaryPointerReader, DWORD fourCC, DWORD& outChunkSize, DWORD& outChunkDataPosition)
	{
		binaryPointerReader.GoTo(0);

		DWORD chunkType = 0;
		DWORD chunkDataSize = 0;
		DWORD RIFFDataSize = 0;
		DWORD fileType = 0;
		DWORD bytesRead = 0;
		DWORD offset = 0;
		while (true)
		{
			if (binaryPointerReader.CanRead<DWORD>() == false)
			{
				return false;
			}

			chunkType = *binaryPointerReader.Read<DWORD>();

			if (binaryPointerReader.CanRead<DWORD>() == false)
			{
				return false;
			}

			chunkDataSize = *binaryPointerReader.Read<DWORD>();

			switch (chunkType)
			{
			case fourccRIFF:
				RIFFDataSize = chunkDataSize;
				chunkDataSize = 4;

				if (binaryPointerReader.CanRead<DWORD>() == false)
				{
					return false;
				}

				fileType = *binaryPointerReader.Read<DWORD>();
				break;
			default:
				binaryPointerReader.Skip(chunkDataSize);
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

	void AudioSystem::ReadChunkData(BinaryPointerReader& binaryPointerReader, void* outBuffer, DWORD bufferSize, DWORD bufferOffset)
	{
		binaryPointerReader.GoTo(bufferOffset);

		::memcpy(outBuffer, binaryPointerReader.Read<byte>(bufferSize), bufferSize);
	}
#pragma endregion
}
