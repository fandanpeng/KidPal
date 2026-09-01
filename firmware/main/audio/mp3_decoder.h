#ifndef MP3_DECODER_H
#define MP3_DECODER_H

#include <cstdint>
#include <vector>

// KidPal tracer-bullet downlink mp3 decoder.
//
// Wraps the fixed-point Helix mp3 decoder (chmorgan/esp-libhelix-mp3) so the
// device can decode the edge-tts mp3 returned by the cloud shim (/v1/chat)
// into int16 PCM that the speaker codec can play. The stock xiaozhi audio
// pipeline is Opus-only; this is the minimal additional mp3 play path
// (taskpackage intake note ⑤, acceptance SMK-A3 device leg).
class Mp3Decoder {
public:
    Mp3Decoder();
    ~Mp3Decoder();

    // Decode an in-memory mp3 payload into interleaved int16 PCM.
    // Returns true and populates sample_rate/channels when at least one frame
    // decoded successfully.
    bool Decode(const std::vector<uint8_t>& mp3, std::vector<int16_t>& pcm_out,
                int& sample_rate, int& channels);

private:
    void* decoder_ = nullptr;  // HMP3Decoder from libhelix-mp3
};

#endif // MP3_DECODER_H