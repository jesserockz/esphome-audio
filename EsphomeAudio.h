#pragma once
#include "esphome.h"
#include "Audio.h"

static const char *TAG_AUDIO = "audio";

// Digital I/O used

#define I2S_DOUT 25 // 25 is default pin but can be changed
#define I2S_BCLK 27 // 27 is default pin but can be changed
#define I2S_LRC 26 // 26 is default pin but can be changed

Audio audio;
// to use built-in DAC on ESP32 (low quality), use: 
// Audio audio(true, I2S_DAC_CHANNEL_BOTH_EN);

bool Playing = false;
const char *toPlay = nullptr;

#ifdef ESP_LOGD
IRAM_ATTR void audio_info(const char *info)
{
        ESP_LOGD(TAG_AUDIO, info);
}
void audio_eof_mp3(const char *info)
{
        ESP_LOGD(TAG_AUDIO, "End of file: %s", info);
}
#endif

class EsphomeAudio : public Component, public CustomAPIDevice
{
public:

        void setup() override
        {       
                // setPinout can be leftout when using the builtin DAC on ESP32
                audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
                
                audio.setVolume(12); // TODO: restore previous ?

                register_service(&EsphomeAudio::play_url, "play_url", {"url"});
                register_service(&EsphomeAudio::volume_set, "volume_set", {"volume"});
                register_service(&EsphomeAudio::media_stop, "media_stop");
        }

        float get_setup_priority() const override { return esphome::setup_priority::LATE; }

        void loop() override
        {
                audio.loop();
        }

        void media_stop()
        {
                if (audio.isRunning())
                {
                        audio.stopSong();
                }
        }

        void play_url(std::string url)
        {
                media_stop();
                this->high_freq_.start();
                audio.connecttohost(url.c_str());
        }

        void volume_set(int volume)
        {
                audio.setVolume(volume); // Use value of 0...21
        }

private:
        HighFrequencyLoopRequester high_freq_;
};