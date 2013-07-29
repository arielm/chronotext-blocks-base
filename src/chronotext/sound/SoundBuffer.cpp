#include "SoundBuffer.h"
#include <limits>

using namespace std;
using namespace ci;

struct WavHeader
{
    char                ChunkID[4];     // Contains the letters "RIFF"
    unsigned long       ChunkSize;      // 36 + SubChunk2Size 
    char                Format[4];      // Contains the letters "WAVE"
    char                Subchunk1ID[4]; // Contains the letters "fmt "
    unsigned long       Subchunk1Size;  // 16 for PCM
    unsigned short      AudioFormat;    // PCM = 1
    unsigned short      NumChannels;    // Mono = 1, Stereo = 2
    unsigned long       SampleRate;     // 8000, 44100, etc.
    unsigned long       ByteRate;       // SampleRate * NumChannels * BitsPerSample / 8
    unsigned short      BlockAlign;     // NumChannels * BitsPerSample / 8
    unsigned short      BitsPerSample;  // 8 bits = 8, 16 bits = 16, etc.
    char                Subchunk2ID[4]; // Contains the letters "data"
    unsigned long       Subchunk2Size;  // NumSamples * NumChannels * BitsPerSample / 8
};

void SoundBuffer::load(DataSourceRef source)
{
    buffer = source->getBuffer();
    WavHeader *header = static_cast<WavHeader*>(buffer.getData());

    if (header->AudioFormat != 1)
    {
        throw runtime_error("WAV READER: EXPECTING PCM FORMAT");
    }
    
//    if (header->NumChannels != 1)
//    {
//        throw runtime_error("WAV READER: EXPECTING MONO");
//    }
    
    if (header->BitsPerSample != 16)
    {
        throw runtime_error("WAV READER: EXPECTING 16-BITS-PER-SAMPLE");
    }
    
//    if (header->SampleRate != 44100)
//    {
//        throw runtime_error("WAV READER: EXPECTING SAMPLE-RATE OF 44100");
//    }
    
    channels = header->NumChannels;
    bitsPerSample = header->BitsPerSample;

    sampleRate = header->SampleRate;
    sampleCount = (header->ChunkSize - 36) >> 1; // USING ChunkSize, WHICH IS MORE RELIABLE THAN Subchunk2Size
}

double SoundBuffer::getDuration()
{
    return sampleCount / (double)sampleRate;
}

int16_t* SoundBuffer::getSamples()
{
    return static_cast<int16_t*>(buffer.getData()) + sizeof(WavHeader);
}

uint64_t SoundBuffer::getSampleRate()
{
    return sampleRate;
}

uint64_t SoundBuffer::getSampleCount()
{
    return sampleCount;
}

unsigned short SoundBuffer::getChannels() {
    return channels;
}

unsigned short SoundBuffer::getBitsPerSample() {
    return bitsPerSample;
}

uint64_t SoundBuffer::getFrameCount() {
    return getSampleCount() / getChannels();
}

float SoundBuffer::getSample(unsigned short chan, uint64_t frame) {
    if (chan > getChannels()) throw runtime_error("WAV READER: CHANNEL DOES NOT EXIST");
    if (frame > getFrameCount()) throw runtime_error("WAV READER: FRAME DOES NOT EXIST");
    return *(getSamples() + (frame * getChannels()) + chan) / (double)numeric_limits<int16_t>::max();
}
