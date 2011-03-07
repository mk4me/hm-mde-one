#include <SDL.h>
#include <SDL_thread.h>
#include <Windows.h>
#include <iostream>
#include <time.h>
#include <vidlib/FFmpegVideoStream.h>
#include <utils/LeakDetection.h>

#ifdef WIN32
#undef main /* We don't want SDL to override our main() */
#endif

int main(int argc, char **argv)
{
    using namespace vidlib;
    using namespace std;

    // czy mamy parametr?
    if ( argc == 1 ) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    // otwieramy Ÿród³o
    VideoStream * input = NULL;
    SDL_Surface * screen = NULL;
    SDL_Surface * bmp = NULL;

    try {
        input = new FFmpegVideoStream( argv[1] );

        // inicjalizacja SDL'a
        SDL_Init( SDL_INIT_VIDEO );
        SDL_EnableKeyRepeat(100, 10);
        SDL_WM_SetCaption("SDL/ffmpeg player", "SDL/ffmpeg player");
        SDL_Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = input->getWidth();
        rect.h = input->getHeight();
        screen = SDL_SetVideoMode( rect.w, rect.h, 24, SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL ); 
        bmp = SDL_CreateRGBSurface(SDL_SWSURFACE, rect.w, rect.h, 24, 0x000000FF, 0x0000ff00, 0x00ff0000, 0 );

        // czas
        double time = 0;
        double timeScale = 1;

        // event
        SDL_Event event;
        bool quit = false;

        // czas rozpoczêcia ramki
        clock_t begin = clock();
        double bufferedTimeScale = 0.0f;

        // pêtla
        while (!quit && !input->isEndOfStream()) {

            // próba ustawienia czasu
            time = min(time, input->getDuration());
            input->setTime(time);

            // kopiujemy i wyœwietlamy ramke
            SDL_LockSurface(bmp);
            Picture picture;
            picture.format = PixelFormatRGB24;
            picture.data = reinterpret_cast<unsigned char*>(bmp->pixels);
            picture.dataWidth = bmp->pitch;
            picture.width = bmp->w;
            picture.height = picture.dataHeight = bmp->h;
            input->getFrame(picture);
            SDL_UnlockSurface(bmp);
            SDL_BlitSurface( bmp, NULL, screen, NULL );
            SDL_Flip( screen );

            // zajmujemy siê eventami
            while (SDL_PollEvent(&event)) {
                switch ( event.type ) {
          case SDL_QUIT:
              quit = true;
              break; 
          case SDL_KEYDOWN:
              switch ( event.key.keysym.sym ) {
          case SDLK_ESCAPE:
              quit = true;
              break;
          case SDLK_PLUS:
          case SDLK_EQUALS:
              timeScale *= 2;
              break;
          case SDLK_MINUS:
              timeScale /= 2;
              break;
          case SDLK_LEFT:
              if ( event.key.keysym.mod & KMOD_LSHIFT ) {
                  time = max(time - input->getFrameDuration(), 0);
              } else {
                  time = max(time - 1, 0);
              }
              break;
          case SDLK_RIGHT:
              if ( event.key.keysym.mod & KMOD_LSHIFT ) {
                  time = min(time + input->getFrameDuration(), input->getDuration());
              } else {
                  time = min(time + 1, input->getDuration());
              }
              break;
          case SDLK_SPACE:
              if ( timeScale == 0.0 ) {
                  timeScale = bufferedTimeScale;
              } else {
                  bufferedTimeScale = timeScale;
                  timeScale = 0.0;
              }
              break;
          default:
              break;
              }
              break;
          default:
              break;
                }
            }

            // drukujemy spacje aby zamazaæ rzeczy z poprzedniego odpisania
            cout << "time: " << time << "\t timestamp: " << input->getFrameTimestamp() << "\t timescale: " << timeScale << "              \r";
            cout.flush();

            // ile czasu minê³o?
            clock_t delta = clock() - begin;
            time += delta / double(CLOCKS_PER_SEC) * timeScale;
            begin = clock();
        }
    } catch ( VideoError & error ) {
        cerr << "ERROR: " << error.what() << endl;
    }

    if ( input ) {
        delete input;
    }
    if ( bmp ) {
        SDL_FreeSurface(bmp);
    }
    if ( screen ) { 
        SDL_FreeSurface(screen);
    }
    SDL_Quit();
    return 0; 
}