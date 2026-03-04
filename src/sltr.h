#ifndef SLTR_H
#define SLTR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef bool SLTR_Bool;
#define SLTR_TRUE 1
#define SLTR_FALSE 0

#define SLTR_CARD_MAX 52
#define SLTR_CARD_SYMBOL_MAX 4
#define SLTR_CARD_RANK_MAX 13
#define SLTR_CARD_NONE -1

#define SLTR_PILE_MAX 7

typedef uint8_t SLTR_CardSymbol;
#define SLTR_CARD_SYMBOL_CLUB 0
#define SLTR_CARD_SYMBOL_DIAMOND 1
#define SLTR_CARD_SYMBOL_HEART 2
#define SLTR_CARD_SYMBOL_SPADE 3

typedef struct {
  SLTR_Bool faceDown;
  SLTR_CardSymbol symbol;
  uint8_t rank;
} SLTR_Card;

typedef struct {
  SLTR_Card cards[SLTR_CARD_MAX];
  int8_t foundations[SLTR_CARD_SYMBOL_MAX][SLTR_CARD_MAX];
  int8_t piles[SLTR_PILE_MAX][SLTR_CARD_MAX];
  int8_t stock[SLTR_CARD_MAX];
  int8_t wastePile[SLTR_CARD_MAX];
} SLTR_Context;

SLTR_Context *SLTR_init(void);
void SLTR_destroy(SLTR_Context *context);
void SLTR_reset(SLTR_Context *context);
void SLTR_sort(SLTR_Context *context);
void SLTR_shuffle(SLTR_Context *context);
SLTR_Bool SLTR_is_won(SLTR_Context *context);

SLTR_Context *SLTR_init(void) {
  SLTR_Context *context = (SLTR_Context *)malloc(sizeof(SLTR_Context));
  SLTR_sort(context);
  SLTR_reset(context);
  return context;
}

void SLTR_destroy(SLTR_Context *context) { free(context); }

void SLTR_reset(SLTR_Context *context) {
  SLTR_shuffle(context);

  memset(context->foundations, SLTR_CARD_NONE, sizeof(context->foundations));
  memset(context->piles, SLTR_CARD_NONE, sizeof(context->piles));
  memset(context->stock, SLTR_CARD_NONE, sizeof(context->stock));
  memset(context->wastePile, SLTR_CARD_NONE, sizeof(context->wastePile));

  int pilesCardsNum = 0;
  for (int i = 0; i < SLTR_PILE_MAX; i++) {
    for (int j = 0; j < i + 1; j++) {
      context->piles[i][j] = pilesCardsNum;
      pilesCardsNum++;
    }
  }

  // Piles pickup (1/2)n(n+1) cards where n is the maximum number of cards.
  assert(pilesCardsNum == SLTR_PILE_MAX * (SLTR_PILE_MAX + 1) / 2);

  // The remaining cards go to stock.
  int stockCardsNum = SLTR_CARD_MAX - pilesCardsNum;
  for (int i = 0; i < stockCardsNum; i++) {
    context->stock[i] = SLTR_CARD_MAX - i - 1;
  }
}

void SLTR_sort(SLTR_Context *context) {
  memset(context->cards, 0, sizeof(SLTR_Card));

  for (int i = 0; i < SLTR_CARD_RANK_MAX; i++) {
    context->cards[i].symbol = SLTR_CARD_SYMBOL_CLUB;
    context->cards[i].rank = i;
    context->cards[i].faceDown = SLTR_TRUE;

    context->cards[i + SLTR_CARD_RANK_MAX].symbol = SLTR_CARD_SYMBOL_DIAMOND;
    context->cards[i + SLTR_CARD_RANK_MAX].rank = i;
    context->cards[i + SLTR_CARD_RANK_MAX].faceDown = SLTR_TRUE;

    context->cards[i + SLTR_CARD_RANK_MAX * 2].symbol = SLTR_CARD_SYMBOL_HEART;
    context->cards[i + SLTR_CARD_RANK_MAX * 2].rank = i;
    context->cards[i + SLTR_CARD_RANK_MAX * 2].faceDown = SLTR_TRUE;

    context->cards[i + SLTR_CARD_RANK_MAX * 3].symbol = SLTR_CARD_SYMBOL_SPADE;
    context->cards[i + SLTR_CARD_RANK_MAX * 3].rank = i;
    context->cards[i + SLTR_CARD_RANK_MAX * 3].faceDown = SLTR_TRUE;
  }
}

void SLTR_shuffle(SLTR_Context *context) {
  srand(time(NULL));

  // Fisher–Yates shuffle
  for (int i = 0; i < SLTR_CARD_MAX - 2; i++) {
    SLTR_Card tmp = context->cards[i];

    // Uniform distribution
    int randomIndex = rand() / (RAND_MAX / SLTR_CARD_MAX + 1);

    context->cards[i] = context->cards[randomIndex];
    context->cards[randomIndex] = tmp;
  }
}

SLTR_Bool SLTR_is_won(SLTR_Context *context) {
  for (int i = 0; i < SLTR_CARD_SYMBOL_MAX; i++)
    if (context->foundations[i][SLTR_CARD_RANK_MAX - 1] == SLTR_CARD_NONE)
      return SLTR_FALSE;

  return SLTR_TRUE;
}

#ifdef __cplusplus
}
#endif

#endif // SLTR_H
