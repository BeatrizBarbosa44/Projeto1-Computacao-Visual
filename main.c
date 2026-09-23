// Copyright (c) 2026 Andre Kishimoto - https://kishimoto.com.br/
// SPDX-License-Identifier: Apache-2.0
/*Projeto 1 - Computação Visual
 *Processamento de Imagens
 *
 * Integrantes: 
 * Beatriz Aparecida de Mello Barbosa - RA: 10354067
 * Henrique Jeam Ferreira Lima - RA: 10277156
 *
 * Projeto desenvolvido a partir do código base do disponibilizado pelo Prof. Andre Kishimoto
 */
//------------------------------------------------------------------------------
// Exemplo: 05-filter_image
// O programa carrega o arquivo de imagem indicado na constante IMAGE_FILENAME
// e exibe o conteúdo na janela ("kodim23.png" pertence ao "Kodak Image Set").
//
// Caso a imagem seja maior do que WINDOW_WIDTHxWINDOW_HEIGHT, a janela é
// redimensionada logo após a imagem ser carregada.
//
// As teclas '0' e 'R' restauram a imagem original e a exibe na janela.
// As teclas '1' a '9' aplicam um filtro de média na imagem original e exibem
// a imagem filtrada na janela (cada tecla corresponde a um tamanho diferente
// do filtro - veja o código da função loop()).
//
// Observações:
// O código não está focado em performance e filtros grandes (ex. 29x29) levam
// um certo tempo para processar toda a imagem. Para indicar que o programa
// ainda está filtrando a imagem, o cursor do mouse é alterado para um
// SDL_SYSTEM_CURSOR_WAIT e volta para o padrão após a filtragem ser concluída.
//
// Em um projeto mais realista, o código abaixo provavelmente seria refatorado.
// Alguns exemplos de refatoração do projeto:
// - Uso de headers (.h) e outros arquivos .c (ex. estruturas e operações
//   relacionadas à imagens);
// - Remoção de variáveis globais;
// - Redução de logs (ou melhor, seriam desativados na build release);
// - Arquivo de imagem seria um parâmetro do programa (argv), ao invés de ser
//   uma string constante IMAGE_FILENAME.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
//------------------------------------------------------------------------------
// Custom types, structs, constants, etc.
//------------------------------------------------------------------------------
static const char *WINDOW_TITLE = "Filter image";

enum constants
{
    DEFAULT_WINDOW_WIDTH = 1024,
    DEFAULT_WINDOW_HEIGHT = 768,

    SECONDARY_WINDOW_WIDTH = 500,
    SECONDARY_WINDOW_HEIGHT = 650,
};

typedef struct MyWindow MyWindow;
struct MyWindow
{
  SDL_Window *window;
  SDL_Renderer *renderer;
};

typedef struct MyImage MyImage;
struct MyImage
{
  SDL_Surface *surface;
  SDL_Texture *texture;
  SDL_FRect rect;
};

//------------------------------------------------------------------------------
// Globals (argh!)
//------------------------------------------------------------------------------
static MyWindow g_window = { .window = NULL, .renderer = NULL };

static MyWindow g_secondary_window = { .window = NULL, .renderer = NULL };

static TTF_Font *g_font = NULL;

static MyImage g_image = {
  .surface = NULL,
  .texture = NULL,
  .rect = { .x = 0.0f, .y = 0.0f, .w = 0.0f, .h = 0.0f }
};

static int g_histogram[256] = {0};
static double g_mean = 0.0;
static const char *g_brightness = "";
static double g_stddev = 0.0;
static const char *g_contrast = "";
static bool g_original_resolution = false;
static bool g_resolution_hover = false;
static bool g_resolution_pressed = false;

static bool g_equalized = false;
static bool g_equalize_hover = false;
static bool g_equalize_pressed = false;
static SDL_Surface *g_original_grayscale = NULL;

static SDL_FRect g_equalize_button = {
    20.0f,
    440.0f,
    460.0f,
    50.0f
};

static SDL_FRect g_resolution_button = {
    20.0f,
    500.0f,
    460.0f,
    50.0f
};

static SDL_Surface *surfaceFilter = NULL;

static SDL_Cursor *defaultMouseCursor = NULL;
static SDL_Cursor *hourglassMouseCursor = NULL;

//------------------------------------------------------------------------------
// Function declaration
//------------------------------------------------------------------------------
static bool MyWindow_initialize(MyWindow *window, const char *title, int width, int height, SDL_WindowFlags window_flags);
static void MyWindow_destroy(MyWindow *window);
static void MyImage_destroy(MyImage *image);
static bool MyImage_update_texture_with_surface(MyImage* image, SDL_Renderer *renderer, SDL_Surface *surface);


/**
 * Carrega a imagem indicada no parâmetro `filename` e a converte para o formato
 * RGBA32, eliminando dependência do formato original da imagem. A imagem
 * carregada é armazenada em output_image.
 * Caso ocorra algum erro no processo, a função retorna false.
 */
static bool load_rgba32(const char *filename, SDL_Renderer *renderer, MyImage *output_image);

static bool is_grayscale(SDL_Surface *surface);
static bool convert_to_grayscale(SDL_Surface *surface);
static bool calculate_histogram(SDL_Surface *surface, int histogram[256]);
static bool equalize_grayscale(SDL_Surface *surface);
static double calculate_mean(const int histogram[256], int total_pixels);
static const char *classify_brightness(double mean);
static double calculate_stddev(const int histogram[256], int total_pixels, double mean);
static const char *classify_contrast(double stddev);
static void draw_histogram(SDL_Renderer *renderer, const int histogram[256],
                           float x, float y, float width, float height);
static void draw_text(SDL_Renderer *renderer, TTF_Font *font,
                      const char *text, float x, float y);                           
/**
 * Aplica um filtro de média na imagem original, salva o resultado na variável
 * global surfaceFilter e atualiza o conteúdo da janela.
 */


static SDL_AppResult initialize(void);
static void shutdown(void);
static void render(void);
static void loop(void);

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool MyWindow_initialize(MyWindow *window, const char *title, int width, int height, SDL_WindowFlags window_flags)
{
  SDL_Log("\tMyWindow_initialize(%s, %d, %d)", title, width, height);

  if (!window)
  {
    SDL_Log("\t\t*** Erro: Janela/renderizador inválidos (window == NULL).");
    return false;
  }

  if (!SDL_CreateWindowAndRenderer(
        title,
        width,
        height,
        window_flags,
        &window->window,
        &window->renderer))
{
    SDL_Log("*** Erro ao criar janela/renderizador: %s", SDL_GetError());
    return false;
}

if (!SDL_SetWindowPosition(
        window->window,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED))
{
    SDL_Log("*** Erro ao centralizar janela: %s", SDL_GetError());
    return false;
}

return true;
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void MyWindow_destroy(MyWindow *window)
{
  SDL_Log(">>> MyWindow_destroy()");

  if (!window)
  {
    SDL_Log("\t*** Erro: Janela/renderizador inválidos (window == NULL).");
    SDL_Log("<<< MyWindow_destroy()");
    return;
  }

  SDL_Log("\tDestruindo MyWindow->renderer...");
  SDL_DestroyRenderer(window->renderer);
  window->renderer = NULL;

  SDL_Log("\tDestruindo MyWindow->window...");
  SDL_DestroyWindow(window->window);
  window->window = NULL;

  SDL_Log("<<< MyWindow_destroy()");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void MyImage_destroy(MyImage *image)
{
  SDL_Log(">>> MyImage_destroy()");

  if (!image)
  {
    SDL_Log("\t*** Erro: Imagem inválida (image == NULL).");
    SDL_Log("<<< MyImage_destroy()");
    return;
  }

  if (image->texture)
  {
    SDL_Log("\tDestruindo MyImage->texture...");
    SDL_DestroyTexture(image->texture);
    image->texture = NULL;
  }

  if (image->surface)
  {
    SDL_Log("\tDestruindo MyImage->surface...");
    SDL_DestroySurface(image->surface);
    image->surface = NULL;
  }

  SDL_Log("\tRedefinindo MyImage->rect...");
  image->rect.x = image->rect.y = image->rect.w = image->rect.h = 0.0f;

  SDL_Log("<<< MyImage_destroy()");
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool MyImage_update_texture_with_surface(MyImage* image, SDL_Renderer *renderer, SDL_Surface *surface)
{
  SDL_Log(">>> MyImage_update_texture_with_surface()");

  if (!image)
  {
    SDL_Log("\t*** Erro: Imagem inválida (image == NULL).");
    SDL_Log("<<< MyImage_update_texture_with_surface()");
    return false;
  }

  if (!renderer)
  {
    SDL_Log("\t*** Erro: Renderer inválido (renderer == NULL).");
    SDL_Log("<<< MyImage_update_texture_with_surface()");
    return false;
  }

  if (!surface)
  {
    SDL_Log("\t*** Erro: Superfície inválida (surface == NULL).");
    SDL_Log("<<< MyImage_update_texture_with_surface()");
    return false;
  }

  SDL_DestroyTexture(image->texture);

  image->texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!image->texture)
  {
    SDL_Log("\t*** Erro ao criar textura: %s", SDL_GetError());
    SDL_Log("<<< MyImage_update_texture_with_surface()");
    return false;
  }

  SDL_Log("\tObtendo dimensões da textura...");
  SDL_GetTextureSize(image->texture, &image->rect.w, &image->rect.h);

  SDL_Log("<<< MyImage_update_texture_with_surface()");
  return true;
}


//------------------------------------------------------------------------------
bool load_rgba32(const char *filename, SDL_Renderer *renderer, MyImage *output_image)
{
  SDL_Log(">>> load_rgba32(\"%s\")", filename);

  if (!filename)
  {
    SDL_Log("\t*** Erro: Nome do arquivo inválido (filename == NULL).");
    SDL_Log("<<< load_rgba32(\"%s\")", filename);
    return false;
  }

  if (!renderer)
  {
    SDL_Log("\t*** Erro: Renderer inválido (renderer == NULL).");
    SDL_Log("<<< load_rgba32(\"%s\")", filename);
    return false;
  }

  if (!output_image)
  {
    SDL_Log("\t*** Erro: Imagem de saída inválida (output_image == NULL).");
    SDL_Log("<<< load_rgba32(\"%s\")", filename);
    return false;
  }

  MyImage_destroy(output_image);

  SDL_Log("\tCarregando imagem \"%s\" em uma superfície...", filename);
  SDL_Surface *surface = IMG_Load(filename);
  if (!surface)
  {
    SDL_Log("\t*** Erro ao carregar a imagem: %s", SDL_GetError());
    SDL_Log("<<< load_rgba32(\"%s\")", filename);
    return false;
  }

  SDL_Log("\tConvertendo superfície para formato RGBA32...");
  output_image->surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
  SDL_DestroySurface(surface);
  if (!output_image->surface)
  {
    SDL_Log("\t*** Erro ao converter superfície para formato RGBA32: %s", SDL_GetError());
    SDL_Log("<<< load_rgba32(\"%s\")", filename);
    return false;
  }

  SDL_Log("\tCriando textura a partir da superfície...");
  if (!MyImage_update_texture_with_surface(output_image, renderer, output_image->surface))
  {
    SDL_Log("\t*** Erro ao criar textura.");
    SDL_Log("<<< load_rgba32(\"%s\")", filename);
    return false;
  }

  SDL_Log("<<< load_rgba32(\"%s\")", filename);
  return true;
}

static bool is_grayscale(SDL_Surface *surface)
{
    if (!surface)
        return false;

    for (int y = 0; y < surface->h; y++)
    {
        for (int x = 0; x < surface->w; x++)
        {
            Uint8 r, g, b, a;

            if (!SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a))
                return false;

            if (r != g || g != b)
                return false;
        }
    }

    return true;
}

static bool convert_to_grayscale(SDL_Surface *surface)
{
    if (!surface)
        return false;

    for (int y = 0; y < surface->h; y++)
    {
        for (int x = 0; x < surface->w; x++)
        {
            Uint8 r, g, b, a;

            if (!SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a))
                return false;

            Uint8 gray = (Uint8)(
                0.2125 * r +
                0.7154 * g +
                0.0721 * b
            );

            if (!SDL_WriteSurfacePixel(
                    surface, x, y,
                    gray, gray, gray, a))
            {
                return false;
            }
        }
    }

    return true;
}

static bool calculate_histogram(SDL_Surface *surface, int histogram[256])
{
    if (!surface)
        return false;

    for (int i = 0; i < 256; i++)
    {
        histogram[i] = 0;
    }

    for (int y = 0; y < surface->h; y++)
    {
        for (int x = 0; x < surface->w; x++)
        {
            Uint8 r, g, b, a;

            if (!SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a))
                return false;

            histogram[r]++;
        }
    }

    return true;
}

static bool equalize_grayscale(SDL_Surface *surface)
{
    if (!surface)
        return false;

    int histogram[256] = {0};
    int cdf[256] = {0};

    if (!calculate_histogram(surface, histogram))
        return false;

    int total_pixels = surface->w * surface->h;

    cdf[0] = histogram[0];

    for (int i = 1; i < 256; i++)
    {
        cdf[i] = cdf[i - 1] + histogram[i];
    }

    int cdf_min = 0;

    for (int i = 0; i < 256; i++)
    {
        if (cdf[i] > 0)
        {
            cdf_min = cdf[i];
            break;
        }
    }

    if (total_pixels == cdf_min)
        return true;

    for (int y = 0; y < surface->h; y++)
    {
        for (int x = 0; x < surface->w; x++)
        {
            Uint8 r, g, b, a;

            if (!SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a))
                return false;

            int new_value =
                ((cdf[r] - cdf_min) * 255) /
                (total_pixels - cdf_min);

            if (new_value < 0)
                new_value = 0;

            if (new_value > 255)
                new_value = 255;

            Uint8 gray = (Uint8)new_value;

            if (!SDL_WriteSurfacePixel(
                    surface,
                    x,
                    y,
                    gray,
                    gray,
                    gray,
                    a))
            {
                return false;
            }
        }
    }

    return true;
}

static double calculate_mean(const int histogram[256], int total_pixels)
{
    if (total_pixels <= 0)
        return 0.0;

    long long sum = 0;

    for (int i = 0; i < 256; i++)
    {
        sum += (long long)i * histogram[i];
    }

    return (double)sum / total_pixels;
}

static const char *classify_brightness(double mean)
{
    if (mean <= 84.0)
        return "escura";

    if (mean <= 170.0)
        return "media";

    return "clara";
}

static double calculate_stddev(const int histogram[256], int total_pixels, double mean)
{
    if (total_pixels <= 0)
        return 0.0;

    double sum = 0.0;

    for (int i = 0; i < 256; i++)
    {
        double diff = i - mean;
        sum += diff * diff * histogram[i];
    }

    return sqrt(sum / total_pixels);
}

static const char *classify_contrast(double stddev)
{
    if (stddev <= 42.5)
        return "baixo";

    if (stddev <= 85.0)
        return "medio";

    return "alto";
}
static void draw_histogram(SDL_Renderer *renderer, const int histogram[256],
                           float x, float y, float width, float height)
{
    int max_value = 0;

    for (int i = 0; i < 256; i++)
    {
        if (histogram[i] > max_value)
            max_value = histogram[i];
    }

    if (max_value == 0)
        return;

    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);

    float bar_width = width / 256.0f;

    for (int i = 0; i < 256; i++)
    {
        float bar_height =
            ((float)histogram[i] / (float)max_value) * height;

        SDL_FRect bar = {
            x + i * bar_width,
            y + height - bar_height,
            bar_width,
            bar_height
        };

        SDL_RenderFillRect(renderer, &bar);
    }
}

static void draw_text(SDL_Renderer *renderer, TTF_Font *font,
                      const char *text, float x, float y)
{
    if (!renderer || !font || !text)
        return;

    SDL_Color color = {255, 255, 255, 255};

    SDL_Surface *surface = TTF_RenderText_Blended(
        font,
        text,
        0,
        color
    );

    if (!surface)
    {
        SDL_Log("Erro ao criar texto: %s", SDL_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!texture)
    {
        SDL_Log("Erro ao criar textura do texto: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        return;
    }

    SDL_FRect destination = {
        x,
        y,
        (float)surface->w,
        (float)surface->h
    };

    SDL_RenderTexture(renderer, texture, NULL, &destination);

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}


//------------------------------------------------------------------------------
SDL_AppResult initialize(void)
{
  SDL_Log(">>> initialize()");

  SDL_Log("\tIniciando SDL...");
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_Log("\t*** Erro ao iniciar a SDL: %s", SDL_GetError());
    SDL_Log("<<< initialize()");
    return SDL_APP_FAILURE;
  }
SDL_Log("\tInicializando SDL_ttf...");

  if (!TTF_Init())
  {
    SDL_Log("\t*** Erro ao inicializar SDL_ttf: %s", SDL_GetError());
    SDL_Log("<<< initialize()");
    return SDL_APP_FAILURE;
  }

SDL_Log("\tCarregando fonte...");

g_font = TTF_OpenFont("assets/fonts/DejaVuSans.ttf", 18.0f);

if (!g_font)
{
    SDL_Log("\t*** Erro ao carregar fonte: %s", SDL_GetError());
    SDL_Log("<<< initialize()");
    return SDL_APP_FAILURE;
}

  SDL_Log("\tCriando janela e renderizador...");
  if (!MyWindow_initialize(&g_window, WINDOW_TITLE, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0))
  {
    SDL_Log("\t*** Erro ao criar a janela e/ou renderizador: %s", SDL_GetError());
    SDL_Log("<<< initialize()");
    return SDL_APP_FAILURE;
  }

  SDL_Log("<<< initialize()");
  return SDL_APP_CONTINUE;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void shutdown(void)
{
  SDL_Log(">>> shutdown()");

  SDL_Log("Destruindo cursores do mouse...");
  SDL_DestroyCursor(hourglassMouseCursor);
  SDL_DestroyCursor(defaultMouseCursor);
  defaultMouseCursor = NULL;
  hourglassMouseCursor = NULL;

  SDL_Log("Destruindo superfície extra (filter)...");
  SDL_DestroySurface(surfaceFilter);
  surfaceFilter = NULL;

  if (g_original_grayscale)
{
    SDL_DestroySurface(g_original_grayscale);
    g_original_grayscale = NULL;
}
  MyImage_destroy(&g_image);

  MyWindow_destroy(&g_secondary_window);
  MyWindow_destroy(&g_window);

  if (g_font)
{
    TTF_CloseFont(g_font);
    g_font = NULL;
}

  SDL_Log("\tEncerrando SDL_ttf...");
  TTF_Quit();

  SDL_Log("\tEncerrando SDL...");
  SDL_Quit();

  SDL_Log("<<< shutdown()");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void render(void)
{
  SDL_SetRenderDrawColor(g_window.renderer, 128, 128, 128, 255);
  SDL_RenderClear(g_window.renderer);

  if (g_original_resolution)
{
    SDL_RenderTexture(
        g_window.renderer,
        g_image.texture,
        &g_image.rect,
        &g_image.rect
    );
}
else
{
    SDL_FRect destination = {
        0.0f,
        0.0f,
        1024.0f,
        768.0f
    };

    SDL_RenderTexture(
        g_window.renderer,
        g_image.texture,
        &g_image.rect,
        &destination
    );
}

  SDL_RenderPresent(g_window.renderer);

  SDL_SetRenderDrawColor(g_secondary_window.renderer, 40, 40, 40, 255);
  SDL_RenderClear(g_secondary_window.renderer);

  draw_histogram(
      g_secondary_window.renderer,
      g_histogram,
      20.0f,
      20.0f,
      460.0f,
      250.0f
);
char mean_text[64];

snprintf(
    mean_text,
    sizeof(mean_text),
    "Media: %.2f",
    g_mean
);

draw_text(
    g_secondary_window.renderer,
    g_font,
    mean_text,
    20.0f,
    300.0f
);
char brightness_text[64];

snprintf(
    brightness_text,
    sizeof(brightness_text),
    "Luminosidade: %s",
    g_brightness
);

draw_text(
    g_secondary_window.renderer,
    g_font,
    brightness_text,
    20.0f,
    330.0f
);
char stddev_text[64];

snprintf(
    stddev_text,
    sizeof(stddev_text),
    "Desvio padrao: %.2f",
    g_stddev
);

draw_text(
    g_secondary_window.renderer,
    g_font,
    stddev_text,
    20.0f,
    360.0f
);
char contrast_text[64];

snprintf(
    contrast_text,
    sizeof(contrast_text),
    "Contraste: %s",
    g_contrast
);

draw_text(
    g_secondary_window.renderer,
    g_font,
    contrast_text,
    20.0f,
    390.0f
);
if (g_equalize_pressed)
{
    SDL_SetRenderDrawColor(
        g_secondary_window.renderer,
        60, 60, 60, 255
    );
}
else if (g_equalize_hover)
{
    SDL_SetRenderDrawColor(
        g_secondary_window.renderer,
        120, 120, 120, 255
    );
}
else
{
    SDL_SetRenderDrawColor(
        g_secondary_window.renderer,
        90, 90, 90, 255
    );
}

SDL_RenderFillRect(
    g_secondary_window.renderer,
    &g_equalize_button
);

draw_text(
    g_secondary_window.renderer,
    g_font,
    g_equalized ? "Mostrar original" : "Equalizar",
    35.0f,
    455.0f
);

if (g_resolution_pressed)
{
    SDL_SetRenderDrawColor(
        g_secondary_window.renderer,
        60, 60, 60, 255
    );
}
else if (g_resolution_hover)
{
    SDL_SetRenderDrawColor(
        g_secondary_window.renderer,
        120, 120, 120, 255
    );
}
else
{
    SDL_SetRenderDrawColor(
        g_secondary_window.renderer,
        90, 90, 90, 255
    );
}

SDL_RenderFillRect(
    g_secondary_window.renderer,
    &g_resolution_button
);

draw_text(
    g_secondary_window.renderer,
    g_font,
    g_original_resolution ? "Resolucao: original" : "Resolucao: 1024x768",
    35.0f,
    515.0f
);
  SDL_RenderPresent(g_secondary_window.renderer);
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void loop(void)
{
  SDL_Log(">>> loop()");

  render();

  SDL_Event event;
  bool isRunning = true;
  while (isRunning)
  {
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_EVENT_QUIT:
        isRunning = false;
        break;

        case SDL_EVENT_MOUSE_MOTION:
    if (event.motion.windowID == SDL_GetWindowID(g_secondary_window.window))
    {
        float mouse_x = event.motion.x;
        float mouse_y = event.motion.y;

        g_equalize_hover =
            mouse_x >= g_equalize_button.x &&
            mouse_x <= g_equalize_button.x + g_equalize_button.w &&
            mouse_y >= g_equalize_button.y &&
            mouse_y <= g_equalize_button.y + g_equalize_button.h;

            g_resolution_hover =
    mouse_x >= g_resolution_button.x &&
    mouse_x <= g_resolution_button.x + g_resolution_button.w &&
    mouse_y >= g_resolution_button.y &&
    mouse_y <= g_resolution_button.y + g_resolution_button.h;

        render();
    }
    break;

case SDL_EVENT_MOUSE_BUTTON_UP:
    if (event.button.button == SDL_BUTTON_LEFT &&
        event.button.windowID == SDL_GetWindowID(g_secondary_window.window))
    {
        g_equalize_pressed = false;
        g_resolution_pressed = false;
        render();
    }
    break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
         if (event.button.button == SDL_BUTTON_LEFT &&
             event.button.windowID == SDL_GetWindowID(g_secondary_window.window))
    {
             float mouse_x = event.button.x;
             float mouse_y = event.button.y;

if (mouse_x >= g_equalize_button.x &&
    mouse_x <= g_equalize_button.x + g_equalize_button.w &&
    mouse_y >= g_equalize_button.y &&
    mouse_y <= g_equalize_button.y + g_equalize_button.h)
{

  g_equalize_pressed = true;
render();

    g_equalized = !g_equalized;
if (!SDL_BlitSurface(
        g_original_grayscale,
        NULL,
        g_image.surface,
        NULL))
{
    SDL_Log("Erro ao restaurar imagem original: %s", SDL_GetError());
}
else
{
    if (g_equalized)
    {
        if (!equalize_grayscale(g_image.surface))
        {
            SDL_Log("Erro ao equalizar a imagem.");
        }
    }

if (!calculate_histogram(g_image.surface, g_histogram))
{
    SDL_Log("Erro ao recalcular histograma.");
}

int total_pixels = g_image.surface->w * g_image.surface->h;

g_mean = calculate_mean(g_histogram, total_pixels);
g_brightness = classify_brightness(g_mean);

g_stddev = calculate_stddev(
    g_histogram,
    total_pixels,
    g_mean
);

g_contrast = classify_contrast(g_stddev);

    if (!MyImage_update_texture_with_surface(
            &g_image,
            g_window.renderer,
            g_image.surface))
    {
        SDL_Log("Erro ao atualizar textura da imagem.");
    }

    render();
}
    SDL_Log(
        "Botao de equalizacao clicado. Equalizada: %s",
        g_equalized ? "sim" : "nao"
    );
}

         if (mouse_x >= g_resolution_button.x &&
             mouse_x <= g_resolution_button.x + g_resolution_button.w &&
             mouse_y >= g_resolution_button.y &&
             mouse_y <= g_resolution_button.y + g_resolution_button.h)
        {

          g_resolution_pressed = true;
render();
g_original_resolution = !g_original_resolution;

if (g_original_resolution)
{
    SDL_SetWindowSize(
        g_window.window,
        g_image.surface->w,
        g_image.surface->h
    );

    SDL_SyncWindow(g_window.window);

    SDL_Rect screen_bounds;

if (SDL_GetDisplayUsableBounds(
        SDL_GetPrimaryDisplay(),
        &screen_bounds) &&
    (g_image.surface->w > screen_bounds.w ||
     g_image.surface->h > screen_bounds.h))
{
    SDL_SetWindowPosition(
        g_window.window,
        0,
        0
    );
}
else
{
    SDL_SetWindowPosition(
        g_window.window,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED
    );
}
}
else
{
    SDL_SetWindowSize(
        g_window.window,
        1024,
        768
    );

    SDL_SyncWindow(g_window.window);

    SDL_Rect screen_bounds_1024;

if (SDL_GetDisplayUsableBounds(
        SDL_GetPrimaryDisplay(),
        &screen_bounds_1024) &&
    (1024 > screen_bounds_1024.w ||
     768 > screen_bounds_1024.h))
{
    SDL_SetWindowPosition(
        g_window.window,
        0,
        0
    );
}
else
{
    SDL_SetWindowPosition(
        g_window.window,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED
    );
}
}
render();
             SDL_Log(
                "Botao de resolucao clicado. Resolucao original: %s",
                 g_original_resolution ? "sim" : "nao"
            );
        }
    }
    break;

      case SDL_EVENT_KEY_DOWN:
        if (!event.key.repeat)
        {
          switch (event.key.key)
          {
     

            case SDLK_S:
    if (IMG_SavePNG(g_image.surface, "output_image.png"))
    {
        SDL_Log("Imagem salva com sucesso em output_image.png");
    }
    else
    {
        SDL_Log("Erro ao salvar a imagem: %s", SDL_GetError());
    }
    break;
          }
        }
        break;
      }
    }

    // Breve pausa para diminuir o processamento contínuo do programa...
    SDL_Delay(50);
  }
  
  SDL_Log("<<< loop()");
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  if (argc < 2)
{
    SDL_Log("Erro: informe o caminho de uma imagem.");
    SDL_Log("Uso: %s caminho_da_imagem.ext", argv[0]);
    return SDL_APP_FAILURE;
}
  atexit(shutdown);

  if (initialize() == SDL_APP_FAILURE)
    return SDL_APP_FAILURE;

  if (!MyWindow_initialize(
        &g_secondary_window,
        "Histograma",
        SECONDARY_WINDOW_WIDTH,
        SECONDARY_WINDOW_HEIGHT,
        0))
{
    SDL_Log("Erro ao criar a janela secundaria.");
    return SDL_APP_FAILURE;
}

  if (!SDL_SetWindowParent(
        g_secondary_window.window,
        g_window.window))
{
    SDL_Log("Erro ao definir a janela secundaria como filha: %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

   if (!SDL_SetWindowPosition(
        g_secondary_window.window,
        0,
        0))
{
    SDL_Log("Erro ao posicionar a janela secundaria: %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

  if (!load_rgba32(argv[1], g_window.renderer, &g_image))
    return SDL_APP_FAILURE;

if (is_grayscale(g_image.surface))
{

    SDL_Log("A imagem de entrada esta em escala de cinza.");
}
else
{
    SDL_Log("A imagem de entrada e colorida.");
    SDL_Log("Convertendo imagem para escala de cinza...");

    if (!convert_to_grayscale(g_image.surface))
    {
        SDL_Log("Erro ao converter a imagem para escala de cinza.");
        return SDL_APP_FAILURE;
    }

    if (!MyImage_update_texture_with_surface(
            &g_image,
            g_window.renderer,
            g_image.surface))
    {
        SDL_Log("Erro ao atualizar a imagem convertida.");
        return SDL_APP_FAILURE;
    }

    SDL_Log("Imagem convertida para escala de cinza.");
}

g_original_grayscale = SDL_DuplicateSurface(g_image.surface);

if (!g_original_grayscale)
{
    SDL_Log("Erro ao guardar copia da imagem original: %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

if (!calculate_histogram(g_image.surface, g_histogram))
{
    SDL_Log("Erro ao calcular o histograma.");
    return SDL_APP_FAILURE;
}

int total_pixels_histogram = 0;

for (int i = 0; i < 256; i++)
{
    total_pixels_histogram += g_histogram[i];
}

SDL_Log("Histograma calculado com sucesso.");
SDL_Log("Pixels contabilizados no histograma: %d", total_pixels_histogram);
SDL_Log("Pixels da imagem: %d", g_image.surface->w * g_image.surface->h);
g_mean = calculate_mean(g_histogram, total_pixels_histogram);

SDL_Log("Media de intensidade: %.2f", g_mean);

g_brightness = classify_brightness(g_mean);

SDL_Log("Classificacao de luminosidade: %s", g_brightness);

g_stddev = calculate_stddev(g_histogram, total_pixels_histogram, g_mean);

SDL_Log("Desvio padrao: %.2f", g_stddev);

g_contrast = classify_contrast(g_stddev);

SDL_Log("Classificacao de contraste: %s", g_contrast);

  SDL_Log("Criando cursores do mouse...");
  defaultMouseCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
  hourglassMouseCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
  SDL_SetCursor(defaultMouseCursor);

  SDL_Log("Criando superfície extra (filter)...");
  surfaceFilter = SDL_CreateSurface(g_image.surface->w, g_image.surface->h, g_image.surface->format);

  // A janela principal deve iniciar em 1024x768 e centralizada.
// O redimensionamento para a resolução original será implementado no item 6.

  loop();

  return 0;
}
