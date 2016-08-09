#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

///MOVE///

typedef struct move move;
struct move {
  int startX;
  int startY;
  int endX;
  int endY;
};

move* move_new(int startY, int startX, int endY, int endX) {
  move* M = malloc(sizeof(move));
  M->startX = startX;
  M->startY = startY;
  M->endX = endX;
  M->endY = endY;
  return M;
}

move* copy_move(move* mv) {
  if(mv == NULL) {
    return NULL;
  }
  return move_new(mv->startY, mv->startX, mv->endY, mv->endX);
}

move* move_from_string(char* mv) {
  char* ch;
  int* moves = malloc(sizeof(int) * 4);
  ch = strtok(mv, " ");
  moves[0] = atoi(ch);
  for(int i = 1; i <= 3; i++) {
    ch = strtok(mv, " ");
    moves[i] = atoi(ch);
  }
  return move_new(moves[0], moves[1], moves[2], moves[3]);
}

///BOARD///

typedef struct board board;
struct board {
  int** array;
  int length;
};

board* board_new(int length) {
  board* B = malloc(sizeof(board));
  B->array = malloc(sizeof(move*) * length);
  B->length = length;
  for(int i = 0; i < length; i++) {
    B->array[i] = malloc(sizeof(int) * length);
    for(int j = 0; j < length; j++) {
      B->array[i][j] = 0;
    }
  }
  return B;
}

void free_board(board* B) {
  for(int i = 0; i < B->length; i++) {
    free(B->array[i]);
  }
  free(B->array);
  free(B);
}

board* copy_board(board* B) {
  if(B == NULL) {
    return NULL;
  }
  board* BD = board_new(B->length);
  for(int i = 0; i < B->length; i++) {
    for(int j = 0; j < B->length; j++) {
      BD->array[i][j] = B->array[i][j];
    }
  }
  BD->length = B->length;
  return BD;
}

board* board_from_string(char* bd) {
  char* ch;
  ch = strtok(bd, " ");
  int length = atoi(ch);
  board* BD = board_new(length);
  for(int i = 0; i < length; i++) {
    for(int j = 0; j < length; j++) {
      ch = strtok(bd, " ");
      BD->array[i][j] =  atoi(ch);
    }
  }
  return BD;
}

void set_square(board* B, int y, int x, int val) {
  B->array[y][x] = val;
}

int get_square(board* B, int y, int x) {
  return B->array[y][x];
}

bool board_eq(board* B1, board* B2) {
  if(B1 == NULL || B2 == NULL) {
    return false;
  }
  int len = B1->length;
  for(int i = 0; i < len; i++) {
    for(int j = 0; j < len; j++) {
      if((B1->array)[i][j] != (B2->array)[i][j]) {
        return false;
      }
    }
  }
  return true;
}

///WRAP LIST///

typedef struct wrap_list wrap_list;
struct wrap_list {
  void** list;
  int length;
  int next;
};

wrap_list* wrap_list_new(int length) {
  wrap_list* M = malloc(sizeof(wrap_list));
  M->list = malloc(sizeof(void*) * length);
  for(int i = 0; i < length; i++) {
    M->list[i] = NULL;
  }
  M->length = length;
  M->next = 0;
  return M;
}

void free_move_list(wrap_list* M) {
  for(int i = 0; i < M->length; i++) {
    free(M->list[i]);
  }
  free(M);
}

void free_board_list(wrap_list* L) {
  for(int i = 0; i < L->length; i++) {
    if(L->list[i] != NULL) {
      free_board(L->list[i]);
    }
  }
  free(L);
}

wrap_list* copy_move_list(wrap_list* L) {
  if(L == NULL) {
    return NULL;
  }
  wrap_list* M = wrap_list_new(L->length);
  for(int i = 0; i < L->length; i++) {
    if(M->list[i] != NULL) {
      M->list[i] = copy_move(L->list[i]);
    }
  }
  M->length = L->length;
  M->next = L->next;
  return M;
}

wrap_list* move_list_from_string(char* mvs) {
  char* c = strtok(mvs, " ");
  int length = atoi(c);
  wrap_list* M = wrap_list_new(length);
  for(int i = 0; i < length; i++) {
    c = strtok(mvs, " | ");
    M->list[i] = move_from_string(c);
  }
  return M;
}

wrap_list* copy_board_list(wrap_list* L) {
  if(L == NULL) {
    return NULL;
  }
  wrap_list* M = wrap_list_new(L->length);
  for(int i = 0; i < L->length; i++) {
    M->list[i] = copy_board(L->list[i]);
  }
  M->length = L->length;
  M->next = L->next;
  return M;
}

wrap_list* board_list_from_string(char* mvs) {
  char* c = strtok(mvs, " ");
  int length = atoi(c);
  wrap_list* M = wrap_list_new(length);
  for(int i = 0; i < length; i++) {
    c = strtok(mvs, " | ");
    M->list[i] = board_from_string(c);
  }
  return M;
}

///MOVE MEMORY///

typedef struct move_memory move_memory;
struct move_memory {
  wrap_list* move_list;
  wrap_list* board_list;
  board* first_moves;
};

move_memory* move_memory_new() {
  move_memory* M = malloc(sizeof(move_memory));
  M->move_list = wrap_list_new(6);
  M->board_list = wrap_list_new(10);
  M->first_moves = board_new(8);
  return M;
}

void free_memory(move_memory* M) {
  free_move_list(M->move_list);
  free_board_list(M->board_list);
  free_board(M->first_moves);
  free(M);
}

move_memory* copy_memory(move_memory* M) {
  if(M == NULL) {
    return NULL;
  }
  move_memory* MM = move_memory_new();
  MM->move_list = copy_move_list(M->move_list);
  MM->board_list = copy_board_list(M->board_list);
  MM->first_moves = copy_board(M->first_moves);
  return MM;
}

move_memory* move_memory_from_string(char* MM) {
  move_memory* M = move_memory_new();
  char* c = strtok(MM, ",");
  M->move_list = move_list_from_string(c);
  c = strtok(MM, ",");
  M->board_list = board_list_from_string(c);
  c = strtok(MM, ",");
  M->first_moves = board_from_string(c);
  return M;
}

//REQUIRES: L != NULL
void set_next_move(move_memory* mvs, move* mv) {
  wrap_list* L = mvs->move_list;
  move* M = copy_move(mv);
  L->list[L->next] = M;
  L->next = ((L->next) + 1) % (L->length);
}

//REQUIRES: L->list[i] is a move
move* get_last_move(move_memory* mvs) {
  wrap_list* L = mvs->move_list;
  return (move*) (L->list[(L->length + ((L->next) - 1)) % (L->length)]);
}

void set_next_board(move_memory* mvs, board* bd) {
  wrap_list* L = mvs->board_list;
  board* B = copy_board(bd);
  L->list[L->next] = B;
  L->next = ((L->next) + 1) % (L->length);
}

bool same_two_boards(board* B, move_memory* mvs) {
  bool second = false;
  bool third = false;
  wrap_list* boards = mvs->board_list;
  int len = boards->length;
  for(int i = 0; i < len; i++) {
    bool eq = board_eq(B, (boards->list)[i]);
    if(second && eq) {
      third = true;
      second = false;
    }
    else if(third && eq) {
      return true;
    }
    else if(eq) {
      second = true;
    }
  }
  return false;
}

bool three_same(move_memory* mvs) {
  wrap_list* boards = mvs->board_list;
  int len = boards->length;
  for(int i = 0; i < len; i++) {
    if(same_two_boards((boards->list)[i], mvs)) {
      return true;
    }
  }
  return false;
}

void set_moved(move_memory* mvs, int locY, int locX) {
  (mvs->first_moves->array)[locY][locX] = 1;
}

bool was_moved(move_memory* mvs, int locY, int locX) {
  return ((mvs->first_moves->array)[locY][locX] == 1);
}

///CHESS BOARD///

typedef struct chess_board chess_board;
struct chess_board {
  board* B;
  bool white_turn;
  bool in_play;
  move_memory* M;
  bool* moved;
};

chess_board* chess_board_new() {
  chess_board* CB = malloc(sizeof(chess_board));
  CB->B = board_new(8);
  CB->white_turn = true;
  CB->in_play = true;
  CB->M = move_memory_new();
  CB->moved = malloc(sizeof(bool) * 6);
  for(int i = 0; i < 6; i++) {
    CB->moved[i] = false;
  }
  return CB;
}

chess_board* copy_chess_board(chess_board* BD) {
  if(BD == NULL) {
    return NULL;
  }
  chess_board* CB = chess_board_new();
  CB->B = copy_board(BD->B);
  CB->white_turn = BD->white_turn;
  CB->in_play = BD->in_play;
  CB->M = copy_memory(BD->M);
  for(int i = 0; i < 6; i++) {
    CB->moved[i] = BD->moved[i];
  }
  return CB;
}

void free_chess_board(chess_board* CB) {
  free_board(CB->B);
  free_memory(CB->M);
  free(CB->moved);
  free(CB);
}

int fromChar(char a) {
  return ((int) a) - 48;
}

int fromCharTwo(char a, char b) {
  int tens = (int) a;
  int ones = (int) b;
  int num = (tens - 48) * 10 + (ones - 48);
  return num;
}

chess_board* from_string(char* ch) {
  chess_board* CB = chess_board_new();
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      int index = 2 * ((i * 8) + j);
      CB->B->array[i][j] = fromCharTwo(ch[index], ch[index + 1]);
    }
  }
  set_next_move(CB->M, move_new(fromChar(ch[128]), fromChar(ch[129]),
				fromChar(ch[130]), fromChar(ch[131])));
  CB->white_turn = fromChar(ch[132]) == 1;
  for(int i = 0; i < 6; i++) {
    CB->moved[i] = fromChar(ch[133 + i]) == 1;
  }
  return CB;
}


chess_board* chess_board_from_string(char* CB) {
  chess_board* BD = chess_board_new();
  char* c = strtok(CB, ";");
  BD->B = board_from_string(c);
  c = strtok(CB, ";");
  if(atoi(c) == 1) {
    BD->white_turn = true;
  } else {
    BD->white_turn = false;
  }
  c = strtok(CB, ";");
  if(atoi(c) == 1) {
    BD->in_play = true;
  } else {
    BD->in_play = false;
  }
  c = strtok(CB, ";");
  BD->M = move_memory_from_string(c);
  for(int i = 0; i < 6; i++) {
    c = strtok(CB, ";");
    BD->moved[i] = atoi(c);
  }
  return BD;
}

void set_board(chess_board* CB) {
  set_square(CB->B, 0, 0, 24);
  set_square(CB->B, 0, 1, 22);
  set_square(CB->B, 0, 2, 23);
  set_square(CB->B, 0, 3, 25);
  set_square(CB->B, 0, 4, 26);
  set_square(CB->B, 0, 5, 23);
  set_square(CB->B, 0, 6, 22);
  set_square(CB->B, 0, 7, 24);
  set_square(CB->B, 1, 0, 21);
  set_square(CB->B, 1, 1, 21);
  set_square(CB->B, 1, 2, 21);
  set_square(CB->B, 1, 3, 21);
  set_square(CB->B, 1, 4, 21);
  set_square(CB->B, 1, 5, 21);
  set_square(CB->B, 1, 6, 21);
  set_square(CB->B, 1, 7, 21);

  set_square(CB->B, 7, 0, 14);
  set_square(CB->B, 7, 1, 12);
  set_square(CB->B, 7, 2, 13);
  set_square(CB->B, 7, 3, 15);
  set_square(CB->B, 7, 4, 16);
  set_square(CB->B, 7, 5, 13);
  set_square(CB->B, 7, 6, 12);
  set_square(CB->B, 7, 7, 14);
  set_square(CB->B, 6, 0, 11);
  set_square(CB->B, 6, 1, 11);
  set_square(CB->B, 6, 2, 11);
  set_square(CB->B, 6, 3, 11);
  set_square(CB->B, 6, 4, 11);
  set_square(CB->B, 6, 5, 11);
  set_square(CB->B, 6, 6, 11);
  set_square(CB->B, 6, 7, 11);
  CB->in_play = true;
  CB->white_turn = true;
}

///MISC FUNCTIONS///

void centerFill(chess_board* BD, int* white, int* black) {
  for(int y = 3; y <= 4; y++) {
    for(int x = 2; x <= 5; x++) {
      if(get_square(BD->B, y, x) == 11) {
        *white = (*white) + 1;
      }
      if(get_square(BD->B, y, x) == 21) {
        *black = (*black) + 1;
      }
    }
  }
}

int piece_value(chess_board* BD, int num) {
  int pieces[] = {0, 1, 3, 3, 5, 9, 40};
  int white;
  int black;
  centerFill(BD, &white, &black);
  if(white + black > 4) {
    pieces[2] = 4;
    pieces[3] = 2;
  } else if(white + black < 3) {
    pieces[2] = 2;
    pieces[3] = 4;
  }
  if(num > 0 && num < 20) {
    num = num - 10;
  } else if(num > 20) {
    num = num - 20;
  }
  return pieces[num];
}

bool is_white(int piece) {
  return piece != 0 && piece < 20;
}

int abs(int num) {
  if(num < 0) {
    return -num;
  } else {
    return num;
  }
}

void switchTurn(chess_board* BD) {
  BD->white_turn = !BD->white_turn;
}

move* getKing(chess_board* BD, bool white) {
  for(int x = 0; x < 8; x++) {
    for(int y = 0; y < 8; y++) {
      if((get_square(BD->B, y,x) == 16 && white) ||
         (get_square(BD->B, y,x) == 26 && !white)) {
        return move_new(y, x, -1, -1);
      }
    }
  }
  return NULL;
}

bool checkLegalMove(chess_board* BD, move* mv);

bool inCheck(chess_board* BD, bool white) {
  move* kingPosition = getKing(BD, white);
    if(kingPosition == NULL) {
      return true;
    }
  int kingX = kingPosition->startX;
  int kingY = kingPosition->startY;
  switchTurn(BD);
  for(int x = 0; x < 8; x++) {
    for(int y = 0; y < 8; y++) {
      int piece = get_square(BD->B, y, x);
      if((piece > 20 && white) || (piece < 20 && piece != 0 && !white)) {
	move* mv = move_new(y, x, kingY, kingX);
	if(checkLegalMove(BD, mv)) {
	  free(mv);
	  switchTurn(BD);
          return true;
        }
      }
    }
  }
  switchTurn(BD);
  return false;
}

//REQUIRES: is a legal move other than causing check
bool causesSelfCheck(chess_board* BD, move* mv) {
  int startY = mv->startY;
  int startX = mv->startX;
  int endX = mv->endX;
  int endY = mv->endY;
  bool white = is_white(get_square(BD->B, startY, startX));
  int original = get_square(BD->B, endY, endX);
  set_square(BD->B, endY, endX, get_square(BD->B, startY, startX));
  set_square(BD->B, startY, startX, 0);
  bool selfCheck = inCheck(BD, white);
  set_square(BD->B, startY, startX, get_square(BD->B, endY, endX));
  set_square(BD->B, endY, endX, original);
  return selfCheck;
}


bool enPassant(chess_board* BD, move* mv) {
  if(get_last_move(BD->M) == NULL) {
    return false;
  }
  move* last_mv = get_last_move(BD->M);
  int startPY = last_mv->startY;
  int startPX = last_mv->startX;
  int endPX = last_mv->endX;
  int endPY = last_mv->endY;
  int piece = get_square(BD->B, endPY, endPX);
  if((BD->white_turn && piece != 21) || (!BD->white_turn && piece != 11)) {
    return false;
  }
  int startY = mv->startY;
  int startX = mv->startX;
  int endX = mv->endX;
  int endY = mv->endY;
  bool sameX = startPX == endPX && startPX == endX && abs(startX - endX) == 1;
  bool correctY = (abs(startPY - endPY) == 2 && (abs(startPY - endY) == 1 &&
                                                 abs(endPY - endY) == 1) && abs(startY - endY) == 1);
  return sameX && correctY;
}

bool checkPawnMove(chess_board* BD, move* mv) {
  int startY = mv->startY;
  int startX = mv->startX;
  int endX = mv->endX;
  int endY = mv->endY;
  bool white = is_white(get_square(BD->B, startY, startX));
  if(endY < 0 || endY >= 8 || endX < 0 || endX >= 8) {
    return false;
  }
  int endPiece = get_square(BD->B, endY, endX);
  bool emptySquare =  endPiece == 0;
  bool takeWhite = is_white(endPiece);
  bool legalTake = takeWhite != white && !emptySquare;
  bool taking = (legalTake && abs(startX - endX) == 1 &&
                 ((white && startY - endY == 1) || (!white && endY - startY == 1)));
  bool moving = (emptySquare && startX == endX &&
                 ((white && startY - endY == 1) || (!white && endY - startY == 1)));
  bool firstMove = (emptySquare && startX == endX &&
                    ((white && startY - endY == 2 && startY == 6 &&
                      get_square(BD->B, startY - 1, startX) == 0) ||
                     (!white && endY - startY == 2 && startY == 1 &&
                      get_square(BD->B, startY + 1, startX) == 0)));
  bool en_passant = enPassant(BD, mv);
  return taking || moving || firstMove || en_passant;
}

bool checkBishopClean(chess_board* BD, move* mv) {
  int startY = mv->startY;
  int startX = mv->startX;
  int endX = mv->endX;
  int endY = mv->endY;
  int x = endX;
  int y = endY;
  bool plusX = false;
  bool plusY = false;
  if(startX > endX) {
    plusX = true;
    x++;
  } else {
    x--;
  }
  if(startY > endY) {
    plusY = true;
    y++;
  } else {
    y--;
  }
  while(x != startX && y != startY) {
    if(get_square(BD->B, y, x) != 0) {
      return false;
    }
    if(plusX) {
      x++;
    } else {
      x--;
    }
    if(plusY) {
      y++;
    } else {
      y--;
    }
  }
  return true;
}

bool checkBishopMove(chess_board* BD, move* mv) {
  int startY = mv->startY;
  int startX = mv->startX;
  int endX = mv->endX;
  int endY = mv->endY;
  bool white = is_white(get_square(BD->B, startY, startX));
  if(endY < 0 || endY >= 8 || endX < 0 || endX >= 8) {
    return false;
  }
  int endPiece = get_square(BD->B, endY, endX);
  bool emptySquare =  endPiece == 0;
  bool takeWhite = is_white(endPiece);
  bool legalTake = emptySquare || takeWhite != white;
  int moveVal = (startX - endX) + (startY - endY);
  int otherMoveVal = (startX - endX) - (startY - endY);
  return (legalTake && (moveVal == 0 || otherMoveVal == 0) &&
          (startX != endX && startY != endY) && checkBishopClean(BD, mv));
}

bool checkRookClean(chess_board* BD, move* mv) {
  int startY = mv->startY;
  int startX = mv->startX;
  int endX = mv->endX;
  int endY = mv->endY;
  int x = endX;
  int y = endY;
  bool moveX = startY == endY;
  bool negative = (startY - endY) + (startX - endX) <= 0;
  if(moveX) {
    if(negative) {
      x--;
    } else {
      x++;
    }
  } else {
    if(negative) {
      y--;
    } else {
      y++;
    }
  }
  while(x != startX || y != startY) {
    if(get_square(BD->B, y, x) != 0) {
      return false;
    }
    if(moveX) {
      if(negative) {
	x-=1;
        } else {
	x+=1;
      }
    } else {
      if(negative) {
	y-=1;
      } else {
	y+=1;
      }
    }
  }
  return true;
}

bool checkRookMove(chess_board* BD, move* mv) {
  int startY = mv->startY;
  int startX = mv->startX;
  int endX = mv->endX;
  int endY = mv->endY;
  bool white = is_white(get_square(BD->B, startY, startX));
  if(endY < 0 || endY >= 8 || endX < 0 || endX >= 8) {
    return false;
  }
  int endPiece = get_square(BD->B, endY, endX);
  bool emptySquare =  endPiece == 0;
  bool takeWhite = is_white(endPiece);
  bool legalTake = emptySquare || takeWhite != white;
  bool moveVal = startX == endX || startY == endY;
  return (legalTake && moveVal && (startX != endX || startY != endY) &&
          checkRookClean(BD, mv));
}

bool checkQueenMove(chess_board* BD, move* mv) {
  return (checkRookMove(BD, mv) || checkBishopMove(BD, mv));
}

bool checkKnightMove(chess_board* BD, move* mv) {
  int startY = mv->startY;
  int startX = mv->startX;
  int endX = mv->endX;
  int endY = mv->endY;
  bool white = is_white(get_square(BD->B, startY, startX));
  if(endY < 0 || endY >= 8 || endX < 0 || endX >= 8) {
    return false;
  }
  int endPiece = get_square(BD->B, endY, endX);
  bool emptySquare =  endPiece == 0;
  bool takeWhite = is_white(endPiece);
  bool legalTake = emptySquare || takeWhite != white;
  if(!legalTake) {
    return false;
  }
  if(abs(endX - startX) == 1) {
    return abs(endY - startY) == 2;
  } else if(abs(endY - startY) == 1) {
    return abs(endX - startX) == 2;
  } else {
    return false;
  }
}

bool checkKingMove(chess_board* BD, move* mv) {
  int startY = mv->startY;
  int startX = mv->startX;
  int endX = mv->endX;
  int endY = mv->endY;
  bool white = is_white(get_square(BD->B, startY, startX));
  if(endY < 0 || endY >= 8 || endX < 0 || endX >= 8) {
    return false;
  }
  int endPiece = get_square(BD->B, endY, endX);
  bool emptySquare =  endPiece == 0;
  bool takeWhite = is_white(endPiece);
  bool legalTake = emptySquare || takeWhite != white;
  //updates for castling rule
  if(white) {
    move* firstMV = move_new(7, 4, 7, 5);
    move* secondMV = move_new(7, 4, 7, 3);
    if(startX == 4 && startY == 7 && endX == 6 && endY == 7 && !BD->moved[0] && !BD->moved[1] && get_square(BD->B, 7, 5) == 0 && get_square(BD->B, 7, 6) == 0
       && !causesSelfCheck(BD, firstMV)) {
      free(firstMV);
      free(secondMV);
      return true;
    } else if(startX == 4 && startY == 7 && endX == 2 && endY == 7 &&
              !BD->moved[0] && !BD->moved[4] && get_square(BD->B, 7, 3) == 0 && get_square(BD->B, 7, 2) == 0 && get_square(BD->B, 7, 1) == 0 && !causesSelfCheck(BD, secondMV)) {
      free(firstMV);
      free(secondMV);
      return true;
    }
  } else {
        if(startX == 4 && startY == 0 && endX == 6 && endY == 0 && !BD->moved[2] && !BD->moved[3] && get_square(BD->B, 0,5) == 0 && get_square(BD->B, 0,6) == 0
           && !causesSelfCheck(BD, move_new(0,4,0,5))) {
          return true;
        } else if(startX == 4 && startY == 0 && endX == 2 && endY == 0 &&
                  !BD->moved[2] && !BD->moved[5] && get_square(BD->B, 0,3) == 0 && get_square(BD->B, 0,2) == 0 && get_square(BD->B, 0,1) == 0 && !causesSelfCheck(BD, move_new(0,4,0,3))) {
          return true;
        }
  }
  if(!legalTake) {
    return false;
  }
  if(abs(startX - endX) == 1) {
    return abs(startY - endY) <= 1;
  } else if(abs(startY - endY) == 1) {
    return abs(startX - endX) <= 1;
  }
  return false;
}

bool checkLegalMove(chess_board* BD, move* mv) {
  int startX = mv->startX;
  int startY = mv->startY;
  int piece = get_square(BD->B, startY, startX);
  bool legal_move = false;
  if(piece == 11 || piece == 21) {
    legal_move = checkPawnMove(BD, mv);
  } else if (piece == 13 || piece == 23) {
    legal_move = checkBishopMove(BD, mv);
  } else if (piece == 14 || piece == 24) {
    legal_move = checkRookMove(BD, mv);
  } else if (piece == 15 || piece == 25) {
    legal_move = checkQueenMove(BD, mv);
  } else if (piece == 12 || piece == 22) {
    legal_move = checkKnightMove(BD, mv);
  } else if (piece == 16 || piece == 26) {
    legal_move = checkKingMove(BD, mv);
  }
  return (legal_move && !causesSelfCheck(BD, mv) && (BD->white_turn) == (is_white(piece)));
}

bool checkCastle(chess_board* BD, move* mv) {
  int startX = mv->startX;
  int startY = mv->startY;
  int endX = mv->endX;
  int endY = mv->endY;
  int piece = get_square(BD->B, startY, startX);
  if(piece == 16 && startY == 7 && startX == 4 && endY == 7 && endX == 6) {
    return true;
  } else if(piece == 16 && startY == 7 && startX == 4 && endY == 7 && endX == 2) {
    return true;
  } else if(piece == 26 && startY == 0 && startX == 4 && endY == 0 && endX == 6) {
    return true;
  } else if(piece == 26 && startY == 0 && startX == 4 && endY == 0 && endX == 2) {
    return true;
  } else {
    return false;
  }
}

void checkQueen(chess_board* BD, int y, int x) {
  int piece = get_square(BD->B, y, x);
  if(piece == 11 && y == 0) {
    set_square(BD->B, y, x, 15);
  } else if(piece == 21 && y == 7) {
    set_square(BD->B, y, x, 25);
  }
}
void checkKingRook(chess_board* BD, int y, int x) {
  int piece = get_square(BD->B, y, x);
  if(piece == 16) {
    BD->moved[0] = true;
  } else if(piece == 26) {
    BD->moved[2] = true;
  } else if(piece == 14 && y == 7 && x == 7) {
    BD->moved[1] = true;
  } else if(piece == 24 && y == 0 && x == 7) {
    BD->moved[3] = true;
  } else if(piece == 14 && y == 7 && x == 0) {
    BD->moved[4] = true;
  } else if(piece == 24 && y == 0 && x == 0) {
    BD->moved[5] = true;
  }
}

int get_piece_moves(chess_board* BD, int y, int x, move*** mvs, int index) {
  move** moves = *mvs;
  for(int i = 4; i < 12; i++) {
    for(int j = 4; j < 12; j++) {
      move* mv = move_new(y, x, j % 8, i % 8);
      if(checkLegalMove(BD, mv)) {
        moves[index] = mv;
        index++;
      }
    }
  }
  return index;
}

int legal_moves(chess_board* BD, move*** moves) {
  bool white = BD->white_turn;
  int index = 0;
  for(int i = 4; i < 12; i++) {
    for(int j = 4; j < 12; j++) {
      int piece = get_square(BD->B, j % 8, i % 8);
      if((piece > 20 && !white) || (piece < 20 && piece != 0 && white)) {
        index = get_piece_moves(BD, j % 8, i % 8, moves, index);
      }
    }
  }
  return index;
}


int checkMate(chess_board* BD) {
  //this is not the most efficient way to do this
  move** mvs = malloc(sizeof(move*) * 1024);
  int len = legal_moves(BD, &mvs);
  for(int i = 0; i < len; i++) {
    free(mvs[i]);
  }
  free(mvs);
  if(!(len > 0)) {
    if(inCheck(BD, BD->white_turn)) {
      return 2;
    } else {
      return 1;
    }
  }
  return 0;
}

bool makeMove(chess_board* BD, move* mv) {
  if(BD->in_play) {
    if(checkLegalMove(BD, mv)) {
        int startX = mv->startX;
        int startY = mv->startY;
        int endX = mv->endX;
        int endY = mv->endY;
        if(checkCastle(BD, mv)){
          bool white = is_white(get_square(BD->B, startY, startX));
          if(white) {
            if(endY == 7 && endX == 6) {
              set_square(BD->B, 7, 4, 0);
              set_square(BD->B, 7,6, 16);
              set_square(BD->B, 7,7, 0);
              set_square(BD->B, 7,5, 14);
            }  else {
              set_square(BD->B, 7,4, 0);
              set_square(BD->B, 7,2, 16);
              set_square(BD->B, 7,0, 0);
              set_square(BD->B, 7,3, 14);
            }
          }  else {
            if(endY == 0 && endX == 6) {
              set_square(BD->B, 0,4, 0);
              set_square(BD->B, 0,6, 26);
              set_square(BD->B, 0,7, 0);
              set_square(BD->B, 0,5, 24);
            }  else {
              set_square(BD->B, 0,4, 0);
              set_square(BD->B, 0,2, 26);
              set_square(BD->B, 0,0, 0);
              set_square(BD->B, 0,3, 24);
            }
          }
        } else if(enPassant(BD, mv)) {
          set_square(BD->B, endY, endX, get_square(BD->B, startY, startX));
          set_square(BD->B, startY, startX, 0);
          if(BD->white_turn) {
            set_square(BD->B, endY + 1, endX, 0);
                } else {
            set_square(BD->B, endY - 1, endX, 0);
          }
        }  else {
          set_square(BD->B, endY, endX, get_square(BD->B, startY, startX));
          set_square(BD->B, startY, startX, 0);
          checkQueen(BD, endY, endX);
        }
        set_next_move(BD->M, mv);
        set_moved(BD->M, startY, startX);
        set_next_board(BD->M, copy_board(BD->B));
        if(three_same(BD->M)) {
          BD->in_play = false;
          printf("Threefold  Repetition");
        }
        checkKingRook(BD, endY, endX);
        switchTurn(BD);
        int mated = checkMate(BD);
        if(mated == 2) {
          BD->in_play = false;
          printf("Checkmate!");
        } else if(mated == 1) {
          BD->in_play = false;
          printf("Stalemate!");
        }
        return true;
      } else {
      printf("Illegal Move");
      return false;
    }
  }
  return false;
}

typedef struct branch branch;
struct branch {
  move* move;
  int estimate;
  int num;
};

branch* new_branch(move* m, int est, int n) {
  branch* B = malloc(sizeof(branch));
  B->move = m;
  B->estimate = est;
  B->num = n;
  return B;
}

branch* copy_branch(branch* B) {
  if(B == NULL) {
    return NULL;
  }
  return new_branch(B->move, B->estimate, B->num);
}

void makeAIMove(chess_board* BD, move* mv) {
  int startX = mv->startX;
  int startY = mv->startY;
  int endX = mv->endX;
  int endY = mv->endY;
  if(checkCastle(BD, mv)) {
    bool white = is_white(get_square(BD->B, startY, startX));
    if(white) {
      if(endY == 7 && endX == 6) {
        set_square(BD->B, 7, 4, 0);
        set_square(BD->B, 7,6, 16);
        set_square(BD->B, 7,7, 0);
        set_square(BD->B, 7,5, 14);
      }  else {
        set_square(BD->B, 7,4, 0);
        set_square(BD->B, 7,2, 16);
        set_square(BD->B, 7,0, 0);
        set_square(BD->B, 7,3, 14);
      }
    }  else {
      if(endY == 0 && endX == 6) {
        set_square(BD->B, 0,4, 0);
        set_square(BD->B, 0,6, 26);
        set_square(BD->B, 0,7, 0);
        set_square(BD->B, 0,5, 24);
      }  else {
        set_square(BD->B, 0,4, 0);
        set_square(BD->B, 0,2, 26);
        set_square(BD->B, 0,0, 0);
        set_square(BD->B, 0,3, 24);
      }
    }
  } else if(enPassant(BD, mv)) {
    set_square(BD->B, endY, endX, get_square(BD->B, startY, startX));
    set_square(BD->B, startY, startX, 0);
    if(BD->white_turn) {
      set_square(BD->B, endY + 1, endX, 0);
    } else {
      set_square(BD->B, endY - 1, endX, 0);
    }
  }  else {
    set_square(BD->B, endY, endX, get_square(BD->B, startY, startX));
    set_square(BD->B, startY, startX, 0);
    checkQueen(BD, endY, endX);
  }
  set_next_move(BD->M, mv);
  set_moved(BD->M, startY, startX);
  set_next_board(BD->M, copy_board(BD->B));
  checkKingRook(BD, endY, endX);
  switchTurn(BD);
}

int estimate(chess_board* BD, int* len) {
  int maxie = 0;
  int minnie = 0;
  int black = 0;
  int white = 0;
  centerFill(BD, &white, &black);
  maxie += white / 2;
  minnie += black / 2;
  if(len != NULL && *len == 0) {
    if(BD->white_turn) {
      if(inCheck(BD, true)) {
        return -1000;
      } else {
        return -50;
      }
    }  else {
      if(inCheck(BD, false)) {
        return 1000;
      } else {
        return 50;
      }
    }
  }
  for(int x = 0; x < 8; x++) {
    for(int y = 0; y < 8; y++) {
      int piece = get_square(BD->B, y, x);
      //printf("PV: %d | ", piece_value(BD, piece));
      if(piece > 20) {
        minnie += piece_value(BD, piece);
      } else {
        maxie += piece_value(BD, piece);
      }
    }
  }
  //int val = maxie - minnie;
  //printf("%d | ", val);
  //printf("%d %d | ", maxie, minnie);
  return maxie - minnie;
}

void free_movelist(move** moveList, int len) {
  for(int i = 0; i < len; i++) {
    free(moveList[i]);
  }
  free(moveList);
}

branch* compare(branch** comp, int length, bool greater) {
  branch* extrema;
  extrema = comp[0];
  for(int i = 1; i < length; i++) {
    if(greater) {
      if(comp[i]->estimate > extrema->estimate) {
	extrema = comp[i];
      }
    } else {
      if(comp[i]->estimate < extrema->estimate) {
	extrema = comp[i];
      }
    }
  }
  return extrema;
}

branch* minimax(chess_board* BD, int depth) {
  bool maxie = BD->white_turn;
  if(depth == 0) {
    return new_branch(NULL, estimate(BD, NULL), 0);
  }
  move** moveList = malloc(sizeof(move*) * 1024);
  int index = legal_moves(BD, &moveList);
  branch** comparisons = malloc(sizeof(branch*) * index);
  if(index == 0) {
    return new_branch(NULL, estimate(BD, NULL), 0);
  }
  for(int i = 0; i < index; i++) {
    move* move = moveList[i];
    chess_board* newBoard = copy_chess_board(BD);
    makeAIMove(newBoard, move);
    branch* branch = minimax(newBoard, depth - 1);
    free_chess_board(newBoard);
    branch->move = move;
    branch->num = i;
    comparisons[i] = branch;
  }
  return compare(comparisons, index, maxie);
}

branch* alphabeta(chess_board* BD, int depth, branch* alpha, branch* beta) {
  bool maxie = BD->white_turn;
  if(depth == 0) {
    return new_branch(NULL, estimate(BD, NULL), 0);
  }
  //moveList = board.getAIMoves()
  move** moveList = malloc(sizeof(move*) * 1024);
  int index = legal_moves(BD, &moveList);
  if(index == 0) {
    return new_branch(NULL, estimate(BD, &index), 0);
  }
  for(int i = 0; i < index; i++) {
    move* move = moveList[i];
    //printf("(%d, %d)(%d, %d) | ", move->startY, move->startX, move->endY, move->endX);
    chess_board* newBoard = copy_chess_board(BD);
    makeAIMove(newBoard, move);
    branch* branch = alphabeta(newBoard, depth - 1, copy_branch(alpha), copy_branch(beta));
    free_chess_board(newBoard);
    branch->move = copy_move(move);
    branch->num = i;
    if(maxie) {
      if(alpha == NULL) {
        alpha = copy_branch(branch);
	free(branch);
      } else if(!(beta == NULL) && beta->estimate <= branch->estimate) {
	//printf("RA:%d|", branch->move->startX);
	free_movelist(moveList, index);
	return branch;
      } else if(alpha->estimate < branch->estimate) {
        alpha = copy_branch(branch);
	free(branch);
      }
    } else {
      if(beta == NULL) {
        beta = copy_branch(branch);
	free(branch);
      } else if(!(alpha == NULL) && alpha->estimate >= branch->estimate) {
	//printf("RB:%d|", branch->move->startX);
	free_movelist(moveList, index);
	return branch;
      } else if(beta->estimate > branch->estimate) {
        beta = copy_branch(branch);
	free(branch);
      }
    }
  }
  //printf("\n");
  if(maxie) {
    //printf("A:%d|", alpha->move->startX);
    free_movelist(moveList, index);
    return alpha;
  } else {
    //printf("B:%d|", beta->move->startX);
    free_movelist(moveList, index);
    return beta;
  }
}

void materialAI(chess_board* BD) {
  branch* move = alphabeta(BD, 4, NULL, NULL);
  //branch* move = minimax(BD, 3);
  //printf("%d %d\n", move->num, move->estimate);
  makeMove(BD, move->move);
}

move* materialAIMove(chess_board* BD) {
  branch* B = alphabeta(BD, 4, NULL, NULL);
  return B->move;
}


int main(int argc, char* argv[]) {
  if(argc != 2) {
    printf("Incorrect Input");
  } else {
    chess_board* BD = from_string(argv[1]);
    /*
    int sY = fromChar(argv[1][139]);
    int sX = fromChar(argv[1][140]);
    int eY = fromChar(argv[1][141]);
    int eX = fromChar(argv[1][142]);
    move* mv = move_new(sY, sX, eY, eX);
    if(checkLegalMove(BD, mv)) {
      printf("1");
    } else {
      printf("0");
    }
    */
    move* M = materialAIMove(BD);
    printf("%d %d %d %d", M->startY, M->startX, M->endY, M->endX);
  }
  return 0;
}

