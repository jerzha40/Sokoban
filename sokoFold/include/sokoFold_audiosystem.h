#include <entt/entt.hpp>
#include <miniaudio.h>
#include <components.h>
class AudioSystem
{
public:
    AudioSystem(entt::registry &registry) : m_Registry(registry)
    {
        ma_engine_config config = ma_engine_config_init();
        ma_engine_init(&config, &m_Engine);
    }

    ~AudioSystem()
    {
        ma_engine_uninit(&m_Engine);
    }

    void Update()
    {
        std::vector<entt::entity> to_destroy;
        to_destroy.clear();
        auto view = m_Registry.view<components::AudioSource>();
        for (auto entity : view)
        {
            auto &audio = view.get<components::AudioSource>(entity);

            // 处理播放请求
            if (audio.shouldPlay && !audio.isPlaying)
            {
                StartSound(entity);
                audio.shouldPlay = false;
                audio.isPlaying = true;
            }

            // 处理自动停止
            if (!audio.loop && audio.isPlaying)
            {
                if (ma_sound_at_end(&audio.handle))
                {
                    StopSound(entity);
                    // to_destroy.push_back(entity);
                }
            }
        }
        // // 清理完成播放的临时音效
        // auto destroyView = m_Registry.view<components::AudioSource>();
        // for (auto entity : destroyView)
        // {
        //     auto &audio = destroyView.get<components::AudioSource>(entity);
        //     if (!audio.isPlaying)
        //     {
        //         std::cout << std::format("{} {} {}\n", audio.filePath, audio.isMusic, audio.isPlaying);
        //         to_destroy.push_back(entity);
        //     }
        // }
        // std::cout << "\n";
        // for (auto entity : to_destroy)
        // {
        //     m_Registry.destroy(entity);
        // }
    }

private:
    ma_engine m_Engine;
    entt::registry &m_Registry;

    void StartSound(entt::entity entity)
    {
        auto &audio = m_Registry.get<components::AudioSource>(entity);

        ma_sound_init_from_file(
            &m_Engine,
            audio.filePath.c_str(),
            MA_SOUND_FLAG_DECODE | (audio.isMusic ? MA_SOUND_FLAG_STREAM : 0),
            nullptr,
            nullptr,
            &audio.handle);

        ma_sound_set_volume(&audio.handle, audio.volume);
        ma_sound_set_looping(&audio.handle, audio.loop);
        ma_sound_start(&audio.handle);
    }

    void StopSound(entt::entity entity)
    {
        auto &audio = m_Registry.get<components::AudioSource>(entity);
        if (ma_sound_is_playing(&audio.handle))
        {
            ma_sound_stop(&audio.handle);
            ma_sound_uninit(&audio.handle);
        }
        audio.isPlaying = false;
        // m_Registry.destroy(entity);
    }
};