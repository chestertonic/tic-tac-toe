#include <iostream>
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <algorithm>
using std::find_if;
#include <regex>
using std::regex;
class GameMgr;
class Player;
class Board;

class Board {
  friend class GameMgr;
  friend class Player;
  friend int checkWin(int, int, Player &, Board &, const unsigned &);

public:
  Board(const int &size = 3) { initBoard(size); }

  void initBoard(const int &);
  bool setChar(const int &, const int &, Player &);

private:
  static const char emptyCellChar;
  vector<vector<char>> field;
};

const char Board::emptyCellChar = ' ';

void Board::initBoard(const int &size) {
  for (int y = 0; y < size; ++y) {
    vector<char> row;
    for (int x = 0; x < size; ++x) {
      row.push_back(emptyCellChar);
    }
    field.push_back(row);
  }
}

class Player {
  friend class GameMgr;
  friend class Board;
  friend int checkWin(int, int, Player &, Board &, const unsigned &);

public:
  Player(const string &s, const char &c) : name(s), token(c), status(0) {}

  const string &getName() const { return name; }
  const char &getToken() const { return token; }

private:
  string name;
  char token;
  int status;
};

bool Board::setChar(const int &x, const int &y, Player &p) {
  if (y >= 0 && y < field.size() && x >= 0 && x < field.size()) {
    char curr = field[y][x];
    if (curr == emptyCellChar) {
      field[y][x] = p.token;
      return true;
    }
  }
  return false;
}

class GameMgr {
public:
  GameMgr(const int &size = 3) : board(size) {}
  GameMgr(const int &size, const vector<Player> &p)
      : board(Board(size)), players(p) {}
  GameMgr(const int &size, const vector<Player> &p, const unsigned &n)
      : board(Board(size)), players(p), numToWin(n - 1) {}
  GameMgr(const GameMgr &rhs) {
    board = rhs.board;
    players = rhs.players;
  }
  GameMgr &operator=(const GameMgr &rhs) {
    board = rhs.board;
    players = rhs.players;
    return *this;
  }
  void print();
  void turn(vector<Player>::iterator &);
  bool over() {
    for (auto p : players)
      if (p.status == 1 || p.status == 2)
        return true;
    return false;
  }
  vector<Player>::iterator getCurrentPlayer() { return players.begin(); }

private:
  Board board;
  vector<Player> players;
  unsigned numToWin = 2; /* One less than actual win count since we consider
                            the last placed token as the first in win total.
                            So last token + numToWin = actual num to win */
};

void GameMgr::print() {
  auto fieldSize = board.field.size();
  int rowCnt = 1;
  std::cout << "    | ";
  for (unsigned long i = 1; i <= fieldSize; ++i) {
    std::cout << i << " | ";
  }
  std::cout << "\n-----";
  for (unsigned long i = 1; i <= fieldSize; ++i) {
    std::cout << "----";
  }

  for (auto y = board.field.cbegin(); y != board.field.cend(); ++y) {
    std::cout << "\n " << rowCnt++ << "  | ";
    for (auto x = y->cbegin(); x != y->cend(); ++x) {
      std::cout << *x << " | ";
    }
    std::cout << "\n-----";
    for (unsigned long i = 1; i <= fieldSize; ++i) {
      std::cout << "----";
    }
  }
  std::cout << std::endl;
}

void GameMgr::turn(vector<Player>::iterator &currPlyr) {
  int moveX;
  int moveY;

  print();
  std::cout << currPlyr->name << "'s Turn!\n";

  do {
    do {
      std::cout
          << "Enter the X-Coordinate of where you wish to place your marker:\n";
      std::cin >> moveX;
      --moveX;
    } while (moveX < 0 || moveX > board.field.size());

    do {
      std::cout
          << "Enter the Y-Coordinate of where you wish to place your marker:\n";
      std::cin >> moveY;
      --moveY;
    } while (moveY < 0 || moveY > board.field.size());
  } while (!board.setChar(moveX, moveY, *currPlyr));

  currPlyr->status = checkWin(moveY, moveX, *currPlyr, board, numToWin);
  if (currPlyr->status == 2) {
    std::cout << currPlyr->name << " Won!!! ... Great ... Job ... meh"
              << std::endl;
    print();
  } else if (currPlyr->status == 1) {
    std::cout << "OH NO!!! ... You have AIDS .. JK ... its a draw!!"
              << std::endl;
    print();
  } else {
    currPlyr++;
    if (currPlyr == players.end())
      currPlyr = players.begin();
  }
}

bool inBounds(const int &row, const int &col, const int &maxSize) {
  return ((row >= 0 && row < maxSize) && (col >= 0 && col < maxSize));
}

int checkWin(int row, int col, Player &p, Board &b, const unsigned &numToWin) {
  int totalInRow = 0;
  int winCount = numToWin;
  unsigned long fieldSize = b.field.size();
  // check first diagonals (top-left - bottom-right)
  for (int y = -1; y >= -winCount; --y) {
    for (int x = -1; x >= -winCount; --x) {
      if (inBounds(row + y, col + x, fieldSize) &&
          (b.field[row + y][col + x] == p.token)) {
        totalInRow++;
        if (totalInRow == winCount) {
          return 2;
        }
      }
    }
  }
  for (int y = 1; y <= winCount; ++y) {
    for (int x = 1; x <= winCount; ++x) {
      if (inBounds(row + y, col + x, fieldSize) &&
          (b.field[row + y][col + x] == p.token)) {
        totalInRow++;
        if (totalInRow == winCount) {
          return 2;
        }
      }
    }
  }
  // check column (top - bottom)
  totalInRow = 0;
  for (int y = -1; y >= -winCount; --y) {
    if (inBounds(row + y, col, fieldSize) &&
        (b.field[row + y][col] == p.token)) {
      totalInRow++;
      if (totalInRow == winCount) {
        return 2;
      }
    }
  }
  for (int y = 1; y <= winCount; ++y) {
    if (inBounds(row + y, col, fieldSize) &&
        (b.field[row + y][col] == p.token)) {
      totalInRow++;
      if (totalInRow == winCount) {
        return 2;
      }
    }
  }
  // check second diagonals (top-right - bottom-left)
  totalInRow = 0;
  for (int y = -1; y >= -winCount; --y) {
    for (int x = 1; x <= winCount; ++x) {
      if (inBounds(row + y, col + x, fieldSize) &&
          (b.field[row + y][col + x] == p.token)) {
        totalInRow++;
        if (totalInRow == winCount) {
          return 2;
        }
      }
    }
  }
  // could put repeated check cell code and return p.won = true
  for (int y = 1; y <= winCount; ++y) {
    for (int x = -1; x >= -winCount; --x) {
      if (inBounds(row + y, col + x, fieldSize) &&
          (b.field[row + y][col + x] == p.token)) {
        totalInRow++;
        if (totalInRow == winCount) {
          return 2;
        }
      }
    }
  }
  // check row (left - right)
  totalInRow = 0;
  for (int x = -1; x >= -winCount; --x) {
    if (inBounds(row, col + x, fieldSize) &&
        (b.field[row][col + x] == p.token)) {
      totalInRow++;
      if (totalInRow == winCount) {
        return 2;
      }
    }
  }
  for (int x = 1; x <= winCount; ++x) {
    if (inBounds(row, col + x, fieldSize) &&
        (b.field[row][col + x] == p.token)) {
      totalInRow++;
      if (totalInRow == winCount) {
        return 2;
      }
    }
  }
  // if still here check for draw
  bool isDraw = true;
  for (int y = 0; y < fieldSize; ++y) {
    for (int x = 0; x < fieldSize; ++x) {
      if (b.field[y][x] == b.emptyCellChar)
        isDraw = false;
    }
  }
  if (isDraw) {
    return 1;
  }
  return 0;
}
bool init(GameMgr &game) {
  bool success = false;
  int fieldSize = 3;
  int playerCnt = 2;
  unsigned numToWin = 3;
  vector<Player> plyrs;
  std::cout << "Welcome to Ultimate Tic-Tac-Toe\n\nPress ENTER for defaults or "
               "'q' to quit at any time.\n\n";
  std::cout
      << "Enter the size of game board (default 3, must be >= 3 & < 100) : \n";
  do {
    string line;
    std::getline(std::cin, line);
    if (line == "q") {
      std::cout << "Quitting" << std::endl;
      return false;
    } else if (regex_match(line, regex("[[:digit:]]+")) &&
               (std::stoi(line) < 100)) {
      fieldSize = std::stoi(line);
      std::cout << "Board Size: " << fieldSize << "\n";
      success = true;
    } else if (regex_match(line, regex("[[:digit:]]+")) &&
               (std::stoi(line) >= 100)) {
      std::cout << "Board size too larger, please try again!!!\n";
    } else if (regex_match(line, regex("[[:digit:]]+")) &&
               (std::stoi(line) < 3)) {
      std::cout << "Board size too small, please try again!!!\n";
    } else {
      std::cout << "Going with the default\nBoard Size: " << fieldSize << "\n";
      success = true;
    }
  } while (!success);

  success = false;

  std::cout << "How many players? (default 2, must be >= 2 & <= 25): \n";
  do {
    string line;
    std::getline(std::cin, line);
    if (line == "q") {
      std::cout << "Quitting" << std::endl;
      return false;
    } else if (regex_match(line, regex("[[:digit:]]+")) &&
               (std::stoi(line) <= 25)) {
      playerCnt = std::stoi(line);
      std::cout << "Players: " << playerCnt << "\n";
      success = true;
    } else if (regex_match(line, regex("[[:digit:]]+")) &&
               (std::stoi(line) > 25)) {
      std::cout << "Too many players, please try again!!!\n";
    } else if (regex_match(line, regex("[[:digit:]]+")) &&
               (std::stoi(line) < 2)) {
      std::cout
          << "This isn't the time to play with yourself, please try again!!!\n";
    } else {
      std::cout << "Going with the default\nPlayers: " << playerCnt << "\n";
      success = true;
    }
  } while (!success);

  success = false;
  std::cout << "How many in a row to win? (default is 3, must be >= 3 & <= "
            << fieldSize << "): \n";
  do {
    string line;
    std::getline(std::cin, line);
    if (line == "q") {
      std::cout << "Quitting" << std::endl;
      return false;
    } else if (regex_match(line, regex("[[:digit:]]+")) &&
               (std::stoi(line) <= fieldSize)) {
      numToWin = std::stoi(line);
      std::cout << "Total To Win: " << numToWin << "\n";
      success = true;
    } else if (regex_match(line, regex("[[:digit:]]+")) &&
               (std::stoi(line) > fieldSize)) {
      std::cout << "Can't be larger than board size (" << fieldSize
                << "), please try again!!!\n";
    } else if (regex_match(line, regex("[[:digit:]]+")) &&
               (std::stoi(line) < 3)) {
      std::cout << "You must be religous...please try again!!!\n";
    } else {
      std::cout << "Going with the default\nTotal To Win: " << numToWin << "\n";
      success = true;
    }
  } while (!success);

  for (int i = 0; i < playerCnt; ++i) {
    string name;
    string token;
    success = false;
    std::cout << "Enter Player " << (i + 1)
              << " Name (must be unique; no default; 'q' to quit): \n";

    while (!success) {
      std::cin >> name;
      if (name == "q") {
        std::cout << "Quitting" << std::endl;
        return false;
      }
      auto found = find_if(plyrs.begin(), plyrs.end(), [name](Player const &p) {
        return p.getName() == name;
      });
      if (found == plyrs.end()) {
        success = true;
      } else
        std::cout << "Name taken, please enter a new name!!!: \n";
    }
    success = false;
    std::cout << "Enter Player " << (i + 1) << " Token (must be a unique, "
                                               "single character; no default; "
                                               "'q' to quit): \n";

    while (!success) {
      std::cin >> token;
      if (token.size() > 1) {
        std::cout << "Must be single character, please try again!!!\n";
        continue;
      } else if (token == "q") {
        std::cout << "Quitting" << std::endl;
        return false;
      }
      auto found =
          find_if(plyrs.begin(), plyrs.end(), [token](Player const &p) {
            return p.getToken() == token[0];
          });
      if (found == plyrs.end()) {
        success = true;
      } else
        std::cout << "Token already in use, please enter a new token!!!: \n";
    }
    plyrs.push_back(Player(name, token[0]));
  }
  game = GameMgr(fieldSize, plyrs, numToWin);
  return true;
}

int main() {

  bool quit = false;

  GameMgr game;
  if (init(game)) {
    auto currPlyr = game.getCurrentPlayer();
    while (!quit) {

      game.turn(currPlyr);

      if (game.over()) {
        quit = true;
      }
    }
  }
  return 0;
}
