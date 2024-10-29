#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_mixer.h>
#include <thread>

namespace tzeva_adom {
    inline std::pmr::string filename;
    // Асинхронная функция для воспроизведения аудио
    inline void playAudio() {
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            std::cerr << "Ошибка инициализации SDL: " << SDL_GetError() << std::endl;
            return;
        }

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            std::cerr << "Ошибка открытия аудио: " << Mix_GetError() << std::endl;
            SDL_Quit();
            return;
        }

        Mix_Music* music = Mix_LoadMUS(filename.c_str());
        if (!music) {
            std::cerr << "Ошибка загрузки MP3: " << Mix_GetError() << std::endl;
        } else {
            Mix_PlayMusic(music, 1);

            // Ожидание завершения воспроизведения
            while (Mix_PlayingMusic() != 0) {
                SDL_Delay(100);  // Пауза для проверки состояния воспроизведения
            }

            Mix_FreeMusic(music);
        }

        Mix_CloseAudio();
        SDL_Quit();
    }

    // Функция для запуска воспроизведения аудио в отдельном потоке
    inline void playAudioAsync(std::pmr::string file) {
        filename = file;
        std::thread audioThread(playAudio);
        audioThread.detach();  // Отделяем поток, чтобы он работал асинхронно
    }
}
