#include "definitions.h"
#include "sltr.h"
#include "sources.h"
#include "vertices.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

const int gap =
    (WIDTH - LPAD - (CARD_TEXTURE_WIDTH * PILE_NUM + LPAD * PILE_NUM)) /
    PILE_NUM;

typedef struct {
  int originX;
  int originY;
  int deltaX;
  int deltaY;
  char pressing;
} Mouse;

typedef struct {
  SLTR_Context *sltrContext;
  SDL_Window *window;
  SDL_GLContext context;
  int oldTime;
  Mouse mouse;

  GLint positionAttributeLocation;
  GLint textureCoordinateAttributeLocation;
  GLint resolutionUniformLocation;
  GLint translationUniformLocation;

  GLuint cardPositionBuffer;
  GLuint cardFaceDownTextureCoordinateBuffer;
  GLuint cardBlankTextureCoordinateBuffer;
  GLuint cardClubATextureCoordinateBuffer;
  GLuint cardClub2TextureCoordinateBuffer;
  GLuint cardClub3TextureCoordinateBuffer;
  GLuint cardClub4TextureCoordinateBuffer;
  GLuint cardTextureId;

  GLuint textPositionBuffer;
  GLuint textTextureBuffer;
  GLuint textTextureId;
} UserData;

void ProcessEvent(UserData *userData) {
  SDL_Event e;
  // Process all events in a queue.
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_EVENT_QUIT: {
      SDL_GL_DestroyContext(userData->context);
      SDL_DestroyWindow(userData->window);
      SDL_Quit();
      exit(EXIT_SUCCESS);
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
      SDL_MouseButtonEvent *ev = (SDL_MouseButtonEvent *)&e;
      if (ev->button == SDL_BUTTON_LEFT) {
        userData->mouse.pressing = TRUE;
        userData->mouse.originX = ev->x;
        userData->mouse.originY = ev->y;
      }
      break;
    }
    case SDL_EVENT_MOUSE_BUTTON_UP: {
      SDL_MouseButtonEvent *ev = (SDL_MouseButtonEvent *)&e;
      if (ev->button == SDL_BUTTON_LEFT) {
        userData->mouse.pressing = FALSE;
      }
      break;
    }
    case SDL_EVENT_MOUSE_MOTION: {
      SDL_MouseMotionEvent *ev = (SDL_MouseMotionEvent *)&e;
      if (userData->mouse.pressing) {
        userData->mouse.deltaX = ev->x - userData->mouse.originX;
        userData->mouse.deltaY = ev->y - userData->mouse.originY;
      }
      int collisionX = ev->x >= 300 && 300 + CARD_WIDTH >= ev->x;
      int collisionY = ev->y >= 150 && 150 + CARD_HEIGHT >= ev->y;
      if (collisionX && collisionY) {
        // printf("[%d]: Collision detected!\n", SDL_GetTicks());
      }
      break;
    }
    }
  }
}

void Render(void *arg) {
  UserData *userData = (UserData *)arg;
  ProcessEvent(userData);

  int newTime = SDL_GetTicks();
  float fps = 1000.f / (newTime - userData->oldTime);
  userData->oldTime = newTime;

  int width, height;
  SDL_GetWindowSize(userData->window, &width, &height);
  glViewport(0, 0, width, height);
  glUniform2f(userData->resolutionUniformLocation, (float)width, (float)height);

  glClear(GL_COLOR_BUFFER_BIT);

  glBindBuffer(GL_ARRAY_BUFFER, userData->cardPositionBuffer);
  glVertexAttribPointer(userData->positionAttributeLocation, 2, GL_FLOAT,
                        GL_FALSE, 0, NULL);
  glBindTexture(GL_TEXTURE_2D, userData->cardTextureId);

  // Piles
  for (int i = 0; i < PILE_NUM; i++) {
    int lpad = LPAD * (i + 1);
    if (i > 0)
      lpad += CARD_TEXTURE_WIDTH * i + gap * i;
    glUniform2f(userData->translationUniformLocation, lpad, 120.f);

    if (userData->sltrContext->piles[i][0] == SLTR_CARD_NONE) {
      glBindBuffer(GL_ARRAY_BUFFER, userData->cardBlankTextureCoordinateBuffer);
      glVertexAttribPointer(userData->textureCoordinateAttributeLocation, 2,
                            GL_FLOAT, GL_FALSE, 0, NULL);
    } else {
      glBindBuffer(GL_ARRAY_BUFFER,
                   userData->cardFaceDownTextureCoordinateBuffer);
      glVertexAttribPointer(userData->textureCoordinateAttributeLocation, 2,
                            GL_FLOAT, GL_FALSE, 0, NULL);
    }

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  // Foundations
  glBindBuffer(GL_ARRAY_BUFFER, userData->cardBlankTextureCoordinateBuffer);
  glVertexAttribPointer(userData->textureCoordinateAttributeLocation, 2,
                        GL_FLOAT, GL_FALSE, 0, NULL);
  for (int i = 0; i < SLTR_CARD_SYMBOL_MAX; i++) {
    int lcardNum = i + 3;
    int lpad = LPAD * (lcardNum + 1);
    lpad += CARD_TEXTURE_WIDTH * lcardNum + gap * lcardNum;
    glUniform2f(userData->translationUniformLocation, lpad, 10.f);

    if (userData->sltrContext->foundations[i][0] == SLTR_CARD_NONE) {
      glBindBuffer(GL_ARRAY_BUFFER, userData->cardBlankTextureCoordinateBuffer);
      glVertexAttribPointer(userData->textureCoordinateAttributeLocation, 2,
                            GL_FLOAT, GL_FALSE, 0, NULL);
    } else {
      glBindBuffer(GL_ARRAY_BUFFER, userData->cardClubATextureCoordinateBuffer);
      glVertexAttribPointer(userData->textureCoordinateAttributeLocation, 2,
                            GL_FLOAT, GL_FALSE, 0, NULL);
    }

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  // Stock
  glUniform2f(userData->translationUniformLocation, LPAD, 10.f);

  if (userData->sltrContext->stock[0] == SLTR_CARD_NONE) {
    glBindBuffer(GL_ARRAY_BUFFER, userData->cardBlankTextureCoordinateBuffer);
    glVertexAttribPointer(userData->textureCoordinateAttributeLocation, 2,
                          GL_FLOAT, GL_FALSE, 0, NULL);
  } else {
    glBindBuffer(GL_ARRAY_BUFFER,
                 userData->cardFaceDownTextureCoordinateBuffer);
    glVertexAttribPointer(userData->textureCoordinateAttributeLocation, 2,
                          GL_FLOAT, GL_FALSE, 0, NULL);
  }

  glDrawArrays(GL_TRIANGLES, 0, 6);

  // Waste pile
  int lpad = LPAD;
  lpad += LPAD + CARD_TEXTURE_WIDTH + gap;
  glUniform2f(userData->translationUniformLocation, lpad, 10.f);

  if (userData->sltrContext->wastePile[0] == SLTR_CARD_NONE) {
    glBindBuffer(GL_ARRAY_BUFFER, userData->cardBlankTextureCoordinateBuffer);
    glVertexAttribPointer(userData->textureCoordinateAttributeLocation, 2,
                          GL_FLOAT, GL_FALSE, 0, NULL);
  } else {
    glBindBuffer(GL_ARRAY_BUFFER, userData->cardClubATextureCoordinateBuffer);
    glVertexAttribPointer(userData->textureCoordinateAttributeLocation, 2,
                          GL_FLOAT, GL_FALSE, 0, NULL);
  }

  glDrawArrays(GL_TRIANGLES, 0, 6);

  // Moving card
  glBindBuffer(GL_ARRAY_BUFFER, userData->cardClubATextureCoordinateBuffer);
  glVertexAttribPointer(userData->textureCoordinateAttributeLocation, 2,
                        GL_FLOAT, GL_FALSE, 0, NULL);
  glUniform2f(userData->translationUniformLocation,
              300.f + userData->mouse.deltaX, 150.f + userData->mouse.deltaY);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // Text
  if (SLTR_is_won(userData->sltrContext)) {
    glBindBuffer(GL_ARRAY_BUFFER, userData->textPositionBuffer);
    glVertexAttribPointer(userData->positionAttributeLocation, 2, GL_FLOAT,
                          GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, userData->textTextureBuffer);
    glVertexAttribPointer(userData->textureCoordinateAttributeLocation, 2,
                          GL_FLOAT, GL_FALSE, 0, NULL);
    glBindTexture(GL_TEXTURE_2D, userData->textTextureId);
    glUniform2f(userData->translationUniformLocation, 180.f, 160.f);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  SDL_GL_SwapWindow(userData->window);
}

GLuint CompileShader(GLenum type, const char *source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  GLint is_successful;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &is_successful);
  if (!is_successful) {
    int bufferSize;
    int logLength;
    char *errorLog;

    const char *typeName = type == GL_VERTEX_SHADER ? "vertex" : "fragment";

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufferSize);
    errorLog = (char *)malloc(bufferSize);

    glGetShaderInfoLog(shader, bufferSize, &logLength, errorLog);
    fprintf(stderr, "Failed to compile %s shader source\n", typeName);
    fprintf(stderr, "%s\n", errorLog);

    free(errorLog);
    abort();
  }

  return shader;
}

GLuint PutVertices(const GLfloat *vertices, size_t size) {
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

  if (glGetError() != GL_NO_ERROR) {
    fprintf(stderr, "Failed to put vertex data\n");
    abort();
  }

  return buffer;
}

GLuint PutTexture(int width, int height, const void *pixels) {
  GLuint textureId;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, pixels);

  if (glGetError() != GL_NO_ERROR) {
    fprintf(stderr, "Failed to put texture data\n");
    abort();
  }

  return textureId;
}

int main(int argc, char **argv) {
  UserData userData;
  memset(&userData, 0, sizeof(UserData));

  userData.sltrContext = SLTR_init();

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");

  userData.window =
      SDL_CreateWindow("Solitaire", WIDTH, HEIGHT, SDL_WINDOW_OPENGL);

  userData.context = SDL_GL_CreateContext(userData.window);

  if (!SDL_GL_SetSwapInterval(1)) {
    fprintf(stderr, "Failed to set swap interval\n");
    abort();
  }

  std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

  // Background color is light green.
  glClearColor(50.f / 255.f, 168.f / 255.f, 82.f / 255.f, 1.f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  GLuint vertShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
  GLuint fragShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertShader);
  glAttachShader(program, fragShader);
  glLinkProgram(program);

  glUseProgram(program);

  userData.positionAttributeLocation = glGetAttribLocation(program, "position");
  glEnableVertexAttribArray(userData.positionAttributeLocation);
  userData.textureCoordinateAttributeLocation =
      glGetAttribLocation(program, "aTexCoord");
  glEnableVertexAttribArray(userData.textureCoordinateAttributeLocation);

  userData.resolutionUniformLocation =
      glGetUniformLocation(program, "resolution");
  userData.translationUniformLocation =
      glGetUniformLocation(program, "translation");

  userData.cardPositionBuffer =
      PutVertices(cardPositionVertices, sizeof(cardPositionVertices));
  userData.cardFaceDownTextureCoordinateBuffer =
      PutVertices(textureFaceDownVertices, sizeof(textureFaceDownVertices));
  userData.cardBlankTextureCoordinateBuffer =
      PutVertices(textureBlankVertices, sizeof(textureBlankVertices));
  userData.cardClubATextureCoordinateBuffer =
      PutVertices(textureClubAVertices, sizeof(textureClubAVertices));

  int cardTextureWidth;
  int cardTextureHeight;
  int cardTextureChannels;
  unsigned char *cardTexture =
      stbi_load(TEXTURE_PATH, &cardTextureWidth, &cardTextureHeight,
                &cardTextureChannels, STBI_rgb_alpha);
  if (!cardTexture) {
    fprintf(stderr, "Failed to load texture: %s\n", stbi_failure_reason());
    abort();
  }

  userData.cardTextureId =
      PutTexture(cardTextureWidth, cardTextureHeight, cardTexture);
  stbi_image_free(cardTexture);

  TTF_Init();
  TTF_Font *font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
  if (!font) {
    fprintf(stderr, "Failed to load font\n");
    abort();
  }

  // Text color is black.
  SDL_Color textColor = {0, 0, 0};
  SDL_Surface *textTextureTmp =
      TTF_RenderText_Solid(font, MESSAGE, 0, textColor);
  if (!textTextureTmp) {
    fprintf(stderr, "Failed to render text onto texture\n");
    abort();
  }

  SDL_Surface *textTexture =
      SDL_ConvertSurface(textTextureTmp, SDL_PIXELFORMAT_RGBA32);
  if (!textTexture) {
    fprintf(stderr, "Failed to convert text texture format\n");
    abort();
  }

  // clang-format off
    const GLfloat textPositionVertices[] = {
        0.f,            0.f,
        0.f,            textTexture->h,
        textTexture->w, 0.f,
        0.f,            textTexture->h,
        textTexture->w, textTexture->h,
        textTexture->w, 0.f,
    };
  // clang-format on

  userData.textPositionBuffer =
      PutVertices(textPositionVertices, sizeof(textPositionVertices));
  userData.textTextureBuffer =
      PutVertices(textTextureVertices, sizeof(textTextureVertices));

  userData.textTextureId =
      PutTexture(textTexture->w, textTexture->h, textTexture->pixels);
  SDL_DestroySurface(textTexture);
  SDL_DestroySurface(textTextureTmp);
  TTF_CloseFont(font);

  void *arg = &userData;

#ifdef __EMSCRIPTEN__
  // By specifying 0 for FPS, requestAnimationFrame() executes main loop.
  int fps = 0;
  emscripten_set_main_loop_arg(Render, arg, fps, true);
#else
  while (TRUE) {
    Render(arg);
  }
#endif

  glDeleteTextures(1, &userData.textTextureId);
  glDeleteTextures(1, &userData.cardTextureId);
  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
  glDeleteProgram(program);
  SDL_DestroyWindow(userData.window);
  SLTR_destroy(userData.sltrContext);

  return 0;
}
