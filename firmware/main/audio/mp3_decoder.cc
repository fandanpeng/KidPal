#include "mp3_decoder.h"

#include <esp_log.h>

#include "mp3dec.h"

#define TAG "Mp3Decoder"

// Helix fixed-point mp3 decoder emits at most MAX_NCHAN(2) * MAX_NSAMP(576) *
// MAX_NGRAN(2) = 2304 interleaved int16 samples per decoded frame.
static constexpr int kMp3MaxFrameSamples = 1152 * 2;

Mp3Decoder::Mp3Decoder() {
    decoder_ = MP3InitDecoder();
    if (decoder_ == nullptr) {
        ESP_LOGE(TAG, "MP3InitDecoder failed (out of memory)");
    }
}

Mp3Decoder::~Mp3Decoder() {
    if (decoder_ != nullptr) {
        MP3FreeDecoder(static_cast<HMP3Decoder>(decoder_));
    }
}

bool Mp3Decoder::Decode(const std::vector<uint8_t>& mp3, std::vector<int16_t>& pcm_out,
                        int& sample_rate, int& channels) {
    auto* h = static_cast<HMP3Decoder>(decoder_);
    if (h == nullptr || mp3.empty()) {
        return false;
    }

    unsigned char* inbuf = const_cast<unsigned char*>(mp3.data());
    int bytes_left = static_cast<int>(mp3.size());

    // Skip any leading non-audio bytes (e.g. an ID3 tag) up to the first frame
    // sync word.
    int sync = MP3FindSyncWord(inbuf, bytes_left);
    if (sync < 0) {
        ESP_LOGW(TAG, "no mp3 sync word found");
        return false;
    }
    inbuf += sync;
    bytes_left -= sync;

    pcm_out.clear();
    sample_rate = 0;
    channels = 0;

    int16_t outbuf[kMp3MaxFrameSamples];

    while (bytes_left > 0) {
        int err = MP3Decode(h, &inbuf, &bytes_left, outbuf, 0);
        if (err != ERR_MP3_NONE) {
            // Underflow on a trailing partial frame is a clean stop, not an error.
            if (err == ERR_MP3_MAINDATA_UNDERFLOW || err == ERR_MP3_INDATA_UNDERFLOW) {
                break;
            }
            // Corrupt frame header: advance one byte and re-synchronize.
            if (bytes_left <= 0) {
                break;
            }
            inbuf++;
            bytes_left--;
            sync = MP3FindSyncWord(inbuf, bytes_left);
            if (sync < 0) {
                break;
            }
            inbuf += sync;
            bytes_left -= sync;
            continue;
        }

        MP3FrameInfo info;
        MP3GetLastFrameInfo(h, &info);
        if (sample_rate == 0) {
            sample_rate = info.samprate;
            channels = info.nChans;
        }
        if (info.outputSamps <= 0 || info.outputSamps > kMp3MaxFrameSamples) {
            continue;
        }
        pcm_out.insert(pcm_out.end(), outbuf, outbuf + info.outputSamps);
    }

    return sample_rate > 0 && !pcm_out.empty();
}