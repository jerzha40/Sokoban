#include <miniaudio.h>
#include <stdio.h>

int main()
{
    ma_result result;
    ma_engine engine;

    // 初始化音频引擎
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS)
    {
        return -1;
    }

    // 创建两个独立的解码器
    ma_decoder decoder1, decoder2;
    result = ma_decoder_init_file("box.mp3", NULL, &decoder1);
    if (result != MA_SUCCESS)
    {
        printf("Failed to load audio file\n");
        ma_engine_uninit(&engine);
        return result;
    }

    result = ma_decoder_init_file("box.mp3", NULL, &decoder2);
    if (result != MA_SUCCESS)
    {
        printf("Failed to load second decoder\n");
        ma_decoder_uninit(&decoder1);
        ma_engine_uninit(&engine);
        return result;
    }

    // 创建 `ma_sound` 并分别绑定两个解码器
    ma_sound sound1, sound2;
    result = ma_sound_init_from_data_source(&engine, &decoder1, 0, NULL, &sound1);
    if (result != MA_SUCCESS)
    {
        ma_decoder_uninit(&decoder1);
        ma_decoder_uninit(&decoder2);
        ma_engine_uninit(&engine);
        return result;
    }

    result = ma_sound_init_from_data_source(&engine, &decoder2, 0, NULL, &sound2);
    if (result != MA_SUCCESS)
    {
        ma_sound_uninit(&sound1);
        ma_decoder_uninit(&decoder1);
        ma_decoder_uninit(&decoder2);
        ma_engine_uninit(&engine);
        return result;
    }

    // 播放第一个声音
    ma_sound_start(&sound1);

    printf("Press Enter to start second voice...\n");
    getchar(); // 等待用户按下 Enter

    // 播放第二个声音（实现轮唱效果）
    ma_sound_start(&sound2);

    printf("Press Enter to quit...\n");
    getchar(); // 等待退出

    ma_decoder decoder3;
    result = ma_decoder_init_file("box.mp3", NULL, &decoder3);
    if (result != MA_SUCCESS)
    {
        printf("Failed to load second decoder\n");
        return result;
    }
    ma_sound sound3;
    result = ma_sound_init_from_data_source(&engine, &decoder3, 0, NULL, &sound3);
    if (result != MA_SUCCESS)
    {
        return result;
    }
    ma_sound_start(&sound3);

    printf("Press Enter to quit...\n");
    getchar(); // 等待退出
    ma_decoder decoder4;
    result = ma_decoder_init_file("box.mp3", NULL, &decoder4);
    if (result != MA_SUCCESS)
    {
        printf("Failed to load second decoder\n");
        return result;
    }
    ma_sound sound4;
    result = ma_sound_init_from_data_source(&engine, &decoder4, 0, NULL, &sound4);
    if (result != MA_SUCCESS)
    {
        return result;
    }
    ma_sound_start(&sound4);

    printf("Press Enter to quit...\n");
    getchar(); // 等待退出

    // 释放资源
    ma_sound_uninit(&sound2);
    ma_sound_uninit(&sound1);
    ma_decoder_uninit(&decoder2);
    ma_decoder_uninit(&decoder1);
    ma_engine_uninit(&engine);

    return 0;
}
