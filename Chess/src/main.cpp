#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>

template <typename T> void Print(T message) {
    std::cout << message << std::endl;
};

struct Resolution {
    int WIDTH;
    int HEIGHT;
};


class Piece {
private:
    std::string png_path;
    std::string def_res_dir = "../chess/resources/pieces/";
    char type = NULL;


public:
    Piece(char type): type(type) {
        this->set_png_path();
    }
    ~Piece() {
    }

    void set_type(char c) {
        this->type = c;
    }

    void set_png_path(std::string dir = "") {
        if (dir == "")
            dir = this->def_res_dir;

        switch (this->type) {
        case 'r':
        case 'R': {
            this->png_path = this->type == 'r' ? dir + "black-rook.png" : dir + "white-rook.png";
            break;
        }
        case 'n':
        case 'N': {
            this->png_path = this->type == 'n' ? dir + "black-knight.png" : dir + "white-knight.png";
            break;
        }
        case 'b':
        case 'B': {
            this->png_path = this->type == 'b' ? dir + "black-bishop.png" : dir + "white-bishop.png";
            break;
        }
        case 'q':
        case 'Q': {
            this->png_path = this->type == 'q' ? dir + "black-queen.png" : dir + "white-queen.png";
            break;
        }
        case 'k':
        case 'K': {
            this->png_path = this->type == 'k' ? dir + "black-king.png" : dir + "white-king.png";
            break;
        }
        case 'p':
        case 'P': {
            this->png_path = this->type == 'p' ? dir + "black-pawn.png" : dir + "white-pawn.png";
            break;
        }
        default: {
            this->png_path = "";
            break;
        }
        }
    }

    std::string* get_png_path() {
        return &(this->png_path);
    }
};

class Board {
private:
    float cell_width;
    std::string fen;
    std::string converted_fen;
    Piece* curr_board[8][8] = {};
    sf::Vector2i current_clicked_position = { -1, -1 };
    sf::Color WHITE_SQ = sf::Color(234, 236, 208);
    sf::Color BLACK_SQ = sf::Color(119, 149, 86);
    sf::Color CURR_SELECTION = sf::Color(184, 135, 98);
        
public:
    Board(float cell_width, std::string fen) :
        cell_width(cell_width), fen(fen) {
    }
    ~Board() {
    }

    float get_cell_width() {
        return this->cell_width;
    }

    void set_current_clicked_position(int x, int y) {
        this->current_clicked_position = sf::Vector2i(x, y);
    }

    void convertFenTo64Chars() {
        char converted_fen[65];
        int i = 0, offset = 0;
        for (char fen_char : this->fen) {
            switch (fen_char) {
            case '/':
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                offset = (int)fen_char - 48;
                for (int k = i; k < i + offset; k++) {
                    converted_fen[k] = '0';
                }
                i += offset;
                break;
            default:
                converted_fen[i++] = fen_char;
                break;
            }
        }

        if (i != 64) {
            Print("FEN Failure: Converted fen is not of length 64");
        }
        else {
            converted_fen[i] = '\0';
            this->converted_fen = (std::string)converted_fen;
        }
    }

    void drawBoard(std::string dir, sf::RenderWindow& window) {
        Print("Drawing board");
        this->convertFenTo64Chars();
        Print(this->converted_fen);
        sf::Color color;
        for (int y = 0; y < 8; y++) {       //y axis
            for (int x = 0; x < 8; x++) {   //x axis
                char curr_char = this->converted_fen[y * 8 + x];
                if ((x + y) % 2 == 0) {
                    color = this->WHITE_SQ;
                }
                else {
                    color = this->WHITE_SQ;
                }
                if (this->current_clicked_position.x == x && this->current_clicked_position.y == y) {
                    color = this->CURR_SELECTION;
                }

                this->drawSquare(x, y, curr_char, window, color);
            }
        }
    }

    void drawSquare(int x, int y, char curr_char, sf::RenderWindow& window, sf::Color color) {
        sf::Vector2f position = sf::Vector2f(float(x * this->cell_width), float(y * this->cell_width));
        Piece* piece = new Piece(curr_char);

        sf::RectangleShape square(sf::Vector2f(this->cell_width, this->cell_width));
        square.setPosition(position);
        if ((x + y) % 2 == 0) {
            square.setFillColor(this->WHITE_SQ);
        }
        else {
            square.setFillColor(this->BLACK_SQ);
        }
        if (this->current_clicked_position.x == x && this->current_clicked_position.y == y) {
            square.setFillColor(this->CURR_SELECTION);
        }

        window.draw(square);

        if (curr_char != '0') {
            sf::Sprite sprite;
            sprite.setPosition(position);

            sf::Texture texture;
            texture.loadFromFile(*(piece->get_png_path())); 
            sprite.setTexture(texture);
            window.draw(sprite);
        }

    }

    void highlightSquare(int x, int y, sf::RenderWindow& window) {
        Piece* piece = this->curr_board[x][y];
        if (piece) {
            sf::Sprite* sprite = new sf::Sprite();
            sprite->setPosition(sf::Vector2f(x * this->cell_width, y * this->cell_width));
            sf::Texture texture;
            sf::Vector2f position = {(float) x, (float)y };
            texture.loadFromFile(*(piece->get_png_path()));
            sprite->setTexture(texture);
            window.draw(*sprite);
            delete this->curr_board[x][y];
            this->curr_board[x][y] = piece;
        }
        sf::RectangleShape chess_square(sf::Vector2f(this->cell_width, this->cell_width));
        chess_square.setPosition(sf::Vector2f(x * this->cell_width, y * this->cell_width));
        chess_square.setFillColor(this->CURR_SELECTION);

        window.draw(chess_square);
    }
};

int main() {
    //std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    //std::string fen = "r1bk3r/p2pBpNp/n4n2/1p1NP2P/6P1/3P4/P1P1K3/q5b1";
    std::string starting_pos_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    Resolution res = { 960, 960 };
    Board board(float(res.WIDTH / 8), starting_pos_fen);
    sf::RenderWindow window(sf::VideoMode(res.WIDTH, res.HEIGHT), "SFML works!");

    bool rerender = true; 
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i local_position = sf::Mouse::getPosition(window);

                if (local_position.x >= 0 && local_position.x <= res.WIDTH && local_position.y >= 0 && local_position.y <= res.HEIGHT) {
                    int normalized_x = (int)(local_position.x / board.get_cell_width());
                    int normalized_y = (int)(local_position.y / board.get_cell_width());
                    board.set_current_clicked_position(normalized_x, normalized_y);
                    rerender = true;
                }
            }
        }

        if (rerender) {
            window.clear(sf::Color::White);
            board.drawBoard("../chess/resources/pieces/", window);
            window.display();
            rerender = false;
        }
    }

    return 0;
}