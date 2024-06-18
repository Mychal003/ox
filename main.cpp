#include <SFML/Graphics.hpp>
#include <iostream>
#include <limits>
#include <vector>

class TicTacToe {
public:
    TicTacToe(int size = 3);
    void play();

private:
    int size;
    char** board;
    char currentPlayer;
    sf::RenderWindow window;
    sf::Font font;

    void drawBoard();
    bool makeMove(int row, int col);
    bool checkWin(char player);
    bool isBoardFull();
    void switchPlayer();
    int getMinMaxMove();
    int minMax(int depth, bool isMax, int alpha, int beta);
    int evaluate();
    std::vector<std::pair<int, int>> getAvailableMoves();
};

TicTacToe::TicTacToe(int size) : size(size), window(sf::VideoMode(600, 600), "Tic Tac Toe") {
    board = new char*[size];
    for (int i = 0; i < size; ++i) {
        board[i] = new char[size];
        for (int j = 0; j < size; ++j) {
            board[i][j] = ' ';
        }
    }
    currentPlayer = 'X';

    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Error loading font\n";
    }
}

void TicTacToe::drawBoard() {
    window.clear(sf::Color::White);

    float cellSize = 600 / size;
    for (int i = 1; i < size; ++i) {
        sf::RectangleShape line(sf::Vector2f(600, 5));
        line.setPosition(0, i * cellSize);
        line.setFillColor(sf::Color::Black);
        window.draw(line);

        line.setSize(sf::Vector2f(5, 600));
        line.setPosition(i * cellSize, 0);
        window.draw(line);
    }
//problemy z czcionka
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (board[i][j] != ' ') {
                sf::Text text;
                text.setFont(font);
                text.setString(board[i][j]);
                text.setCharacterSize(cellSize - 20);  // Dynamiczny rozmiar czcionki
                text.setFillColor(sf::Color::Black);

                // Pobierz lokalne granice tekstu
                sf::FloatRect textRect = text.getLocalBounds();
                text.setOrigin(textRect.left + textRect.width / 2.0f,
                               textRect.top + textRect.height / 2.0f);
                text.setPosition(j * cellSize + cellSize / 2.0f,
                                 i * cellSize + cellSize / 2.0f);
                window.draw(text);
            }
        }
    }

    window.display();
}

bool TicTacToe::makeMove(int row, int col) {
    row--;
    col--;

    if (row >= 0 && row < size && col >= 0 && col < size && board[row][col] == ' ') {
        board[row][col] = currentPlayer;
        return true;
    }
    return false;
}

bool TicTacToe::checkWin(char player) {
    // Check rows
    for (int i = 0; i < size; ++i) {
        bool rowWin = true;
        for (int j = 0; j < size; ++j) {
            if (board[i][j] != player) {
                rowWin = false;
                break;
            }
        }
        if (rowWin) return true;
    }

    // Check columns
    for (int i = 0; i < size; ++i) {
        bool colWin = true;
        for (int j = 0; j < size; ++j) {
            if (board[j][i] != player) {
                colWin = false;
                break;
            }
        }
        if (colWin) return true;
    }

    // Check diagonal (top-left to bottom-right)
    bool diag1Win = true;
    for (int i = 0; i < size; ++i) {
        if (board[i][i] != player) {
            diag1Win = false;
            break;
        }
    }
    if (diag1Win) return true;

    // Check diagonal (top-right to bottom-left)
    bool diag2Win = true;
    for (int i = 0; i < size; ++i) {
        if (board[i][size - 1 - i] != player) {
            diag2Win = false;
            break;
        }
    }
    if (diag2Win) return true;

    return false;
}

bool TicTacToe::isBoardFull() {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (board[i][j] == ' ')
                return false;
    return true;
}

void TicTacToe::switchPlayer() {
    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
}

int TicTacToe::evaluate() {
    if (checkWin('O')) {
        return 10;
    } else if (checkWin('X')) {
        return -10;
    }
    return 0;
}

std::vector<std::pair<int, int>> TicTacToe::getAvailableMoves() {
    std::vector<std::pair<int, int>> moves;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (board[i][j] == ' ') {
                moves.push_back({i, j});
            }
        }
    }
    return moves;
}

int TicTacToe::minMax(int depth, bool isMax, int alpha, int beta) {
    int score = evaluate();
    if (score != 0 || isBoardFull() || depth >= 100000) {  // Limiting depth to 6 for larger boards
        return score;
    }

    if (isMax) {
        int bestScore = std::numeric_limits<int>::min();
        for (const auto& move : getAvailableMoves()) {
            board[move.first][move.second] = 'O';
            bestScore = std::max(bestScore, minMax(depth + 1, false, alpha, beta));
            board[move.first][move.second] = ' ';
            alpha = std::max(alpha, bestScore);
            if (beta <= alpha) {
                break;
            }
        }
        return bestScore;
    } else {
        int bestScore = std::numeric_limits<int>::max();
        for (const auto& move : getAvailableMoves()) {
            board[move.first][move.second] = 'X';
            bestScore = std::min(bestScore, minMax(depth + 1, true, alpha, beta));
            board[move.first][move.second] = ' ';
            beta = std::min(beta, bestScore);
            if (beta <= alpha) {
                break;
            }
        }
        return bestScore;
    }
}

int TicTacToe::getMinMaxMove() {
    int bestMove = -1;
    int bestScore = std::numeric_limits<int>::min();

    for (const auto& move : getAvailableMoves()) {
        board[move.first][move.second] = 'O';
        int moveScore = minMax(0, false, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        board[move.first][move.second] = ' ';
        if (moveScore > bestScore) {
            bestScore = moveScore;
            bestMove = move.first * size + move.second;
        }
    }
    return bestMove;
}

void TicTacToe::play() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int col = event.mouseButton.x / (600 / size);
                    int row = event.mouseButton.y / (600 / size);

                    if (makeMove(row + 1, col + 1)) {
                        drawBoard();
                        if (checkWin(currentPlayer)) {
                            std::cout << "Gracz " << currentPlayer << " wygrywa!" << std::endl;
                            window.close();
                        } else if (isBoardFull()) {
                            std::cout << "Remis" << std::endl;
                            window.close();
                        } else {
                            switchPlayer();
                            int bestMove = getMinMaxMove();
                            int moveRow = bestMove / size;
                            int moveCol = bestMove % size;
                            board[moveRow][moveCol] = currentPlayer;
                            drawBoard();
                            if (checkWin(currentPlayer)) {
                                std::cout << "Gracz " << currentPlayer << " wygrywa!" << std::endl;
                                window.close();
                            } else if (isBoardFull()) {
                                std::cout << "Remis!" << std::endl;
                                window.close();
                            } else {
                                switchPlayer();
                            }
                        }
                    }
                }
            }
        }

        drawBoard();
    }
}

int main() {
    int size;
    std::cout << "Prosze wprowadzic rozmiar planszy (np. 3 dla 3x3) ";
    std::cin >> size;

    TicTacToe game(size);
    game.play();

    return 0;
}
