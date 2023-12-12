#pragma once


#ifndef _MINT_AUDIO_AUDIO_SYSTEM_H_
#define _MINT_AUDIO_AUDIO_SYSTEM_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/String.h>
#include <MintContainer/Include/Vector.h>


struct IXAudio2;
struct IXAudio2MasteringVoice;
struct IXAudio2SourceVoice;
typedef unsigned long DWORD;
namespace mint
{
	class BinaryFileReader;
	class AudioSystem;
}


namespace mint
{
	class AudioItem
	{
		friend AudioSystem;

	public:
		AudioItem();
		~AudioItem();
		bool Play();
		void Stop();

	private:
		StringA _fileName;
		Vector<byte> _audioDataBuffer;
		
		uint8 _bitsPerSample;
		uint32 _samplesPerSec;
		uint32 _channelCount;

		uint32 _byteLength;
		float _lengthSec;
		IXAudio2SourceVoice* _sourceVoice;
	};

	class AudioSystem
	{
	public:
		AudioSystem();
		~AudioSystem();

		bool LoadAudio(const StringA& fileName, AudioItem& audioItem);

	private:
		bool FindChunk(BinaryFileReader& fileReader, DWORD fourCC, DWORD& outChunkSize, DWORD& outChunkDataPosition);
		void ReadChunkData(BinaryFileReader& fileReader, void* outBuffer, DWORD bufferSize, DWORD bufferOffset);

	private:
		IXAudio2* _xAudio2;
		IXAudio2MasteringVoice* _masteringVoice;
	};
}


#endif // !_MINT_AUDIO_AUDIO_SYSTEM_H_
