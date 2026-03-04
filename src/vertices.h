#ifndef VERTICES_H
#define VERTICES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "definitions.h"
#include "sltr.h"

const float cardPositionVertices[] = {
    0.f,        0.f,         //
    0.f,        CARD_HEIGHT, //
    CARD_WIDTH, 0.f,         //
    0.f,        CARD_HEIGHT, //
    CARD_WIDTH, CARD_HEIGHT, //
    CARD_WIDTH, 0.f,         //
};

const float textureFaceDownVertices[] = {
    0.154f, 0.8f, //
    0.154f, 1.f,  //
    0.230f, 0.8f, //
    0.154f, 1.f,  //
    0.230f, 1.f,  //
    0.230f, 0.8f, //
};

// clang-format off
const float textureBlankVertices[] = {
    0.f,                                                                        0.8f,
    0.f,                                                                        1.f,
    (float)CARD_TEXTURE_ATLAS_WIDTH / SLTR_CARD_RANK_MAX / CARD_TEXTURE_ATLAS_WIDTH, 0.8f,
    0.f,                                                                        1.f,
    (float)CARD_TEXTURE_ATLAS_WIDTH / SLTR_CARD_RANK_MAX / CARD_TEXTURE_ATLAS_WIDTH, 1.f,
    (float)CARD_TEXTURE_ATLAS_WIDTH / SLTR_CARD_RANK_MAX / CARD_TEXTURE_ATLAS_WIDTH, 0.8f,
};
// clang-format on

const float textureClubAVertices[] = {
    0.f,    0.f,  //
    0.f,    0.2f, //
    0.076f, 0.f,  //
    0.f,    0.2f, //
    0.076f, 0.2f, //
    0.076f, 0.f,  //
};

const float textTextureVertices[] = {
    0.f, 0.f, //
    0.f, 1.f, //
    1.f, 0.f, //
    0.f, 1.f, //
    1.f, 1.f, //
    1.f, 0.f, //
};

#ifdef __cplusplus
}
#endif

#endif // VERTICES_H
