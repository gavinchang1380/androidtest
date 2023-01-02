#include <stdio.h>
#include <media/AudioTrack.h>
#include <math.h>

void callback(int event, void* user, void *info)
{
    (void)user; (void)info;
    printf("callback: event = %d\n", event);
}

void usage(const char *cmd)
{
    printf("Usage: %s freq streamtype flag\n", cmd);
}

int main(int argc, char *argv[])
{
    int freq = 1000;
    audio_stream_type_t stream_type = AUDIO_STREAM_MUSIC;
    audio_output_flags_t flag = AUDIO_OUTPUT_FLAG_NONE;

    if (argc == 2 && (strcmp("-h", argv[1]) == 0)) {
        usage(argv[0]);
        return 0;
    }

    if (argc >= 2) {
        freq = (int)strtol(argv[1], NULL, 0);
    }

    if (argc >= 3) {
        stream_type = (audio_stream_type_t)strtol(argv[2], NULL, 0);
    }

    if (argc >= 4) {
        flag = (audio_output_flags_t)strtol(argv[3], NULL, 0);
    }

    int16_t buffer[960 * 2];
    android::AudioTrack *at = new android::AudioTrack();
    android::status_t status = at->set(stream_type,
                                       48000,
                                       AUDIO_FORMAT_PCM_16_BIT,
                                       AUDIO_CHANNEL_OUT_STEREO,
                                       960,
                                       flag,
                                       nullptr,
                                       0,
                                       0,
                                       true,
                                       AUDIO_SESSION_ALLOCATE,
                                       android::AudioTrack::TRANSFER_SYNC,
                                       nullptr,
                                       android::content::AttributionSourceState(),
                                       nullptr,
                                       false,
                                       1.0f,
                                       AUDIO_PORT_HANDLE_NONE
    );

    for (int i = 0; i < 960; i++) {
        buffer[2 * i + 1] = static_cast<int16_t>(sin(2 * 3.1415926 * i * freq / 48000) * 32767 * 1);
        buffer[2 * i] = buffer[2 * i + 1];
    }
    printf("AudioTrack create: %d\n", status);
    status = at->start();
    printf("AudioTrack start: %d\n", status);
    at->dump(STDOUT_FILENO, android::Vector<android::String16>());
    while (1) {
        at->write(buffer, sizeof(buffer));
    }

	return 0;
}
