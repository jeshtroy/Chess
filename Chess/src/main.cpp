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

enum PieceType {
    KING,
    QUEEN,
    BISHOP,
    KNIGHT,
    ROOK,
    PAWN,
    BLANK
};

enum PieceColor {
    WHITE,
    BLACK,
};




class Piece {
private:
    std::string png_path;
    std::string def_res_dir = "../chess/resources/pieces/";
    int color, piece_type;
    sf::Vector2i coords;

public:
    Piece(char type) {
        this->set_png_path(type);
    }
    ~Piece() {
    }

    int get_piece_type() const {
        return this->piece_type;
    }

    void set_coords(int x, int y) {
        this->coords = sf::Vector2i(x, y);
    }

    void set_png_path(char type, std::string dir = "") {
        if (dir == "")
            dir = this->def_res_dir;

        switch (type) {
        case 'r':
        case 'R': {
            this->png_path = type == 'r' ? dir + "black-rook.png" : dir + "white-rook.png";
            this->color = type == 'r' ? PieceColor::BLACK : PieceColor::WHITE;
            this->piece_type = PieceType::ROOK;
            break;
        }
        case 'n':
        case 'N': {
            this->png_path = type == 'n' ? dir + "black-knight.png" : dir + "white-knight.png";
            this->color = type == 'n' ? PieceColor::BLACK : PieceColor::WHITE;
            this->piece_type = PieceType::KNIGHT;
            break;
        }
        case 'b':
        case 'B': {
            this->png_path = type == 'b' ? dir + "black-bishop.png" : dir + "white-bishop.png";
            this->color = type == 'b' ? PieceColor::BLACK : PieceColor::WHITE;
            this->piece_type = PieceType::BISHOP;
            break;
        }
        case 'q':
        case 'Q': {
            this->png_path = type == 'q' ? dir + "black-queen.png" : dir + "white-queen.png";
            this->color = type == 'q' ? PieceColor::BLACK : PieceColor::WHITE;
            this->piece_type = PieceType::QUEEN;
            break;
        }
        case 'k':
        case 'K': {
            this->png_path = type == 'k' ? dir + "black-king.png" : dir + "white-king.png";
            this->color = type == 'k' ? PieceColor::BLACK : PieceColor::WHITE;
            this->piece_type = PieceType::KING;
            break;
        }
        case 'p':
        case 'P': {
            this->png_path = type == 'p' ? dir + "black-pawn.png" : dir + "white-pawn.png";
            this->color = type == 'p' ? PieceColor::BLACK : PieceColor::WHITE;
            this->piece_type = PieceType::PAWN;
            break;
        }
        default: {
            this->png_path = "";
            this->piece_type = PieceType::BLANK;
            break;
        }
        }
    }

    std::string* get_png_path() {
        return &(this->png_path);
    }

    void move(int to_x, int to_y) {
        if (this->coords.x != -1 && this->coords.y != -1) {
            this->set_coords(to_x, to_y);
        }
    }
};

struct CustomColors {
    const sf::Color WHITE_SQ = sf::Color(234, 236, 208);
    const sf::Color BLACK_SQ = sf::Color(119, 149, 86);
    const sf::Color CURR_SELECTION = sf::Color(245, 246, 130);
} custom_colors;

class Board {
private:
    float cell_width;
    std::string fen;
    std::string converted_fen;
    Piece* curr_board[8][8] = {};

public:
    sf::Vector2i current_clicked_position = { -1, -1 };
    sf::Vector2i mouse_down_pos = { -1, -1 };
    sf::Vector2i mouse_up_pos = { -1, -1 };

        
public:
    Board(float cell_width, std::string fen) :
        cell_width(cell_width), fen(fen) {
    }
    ~Board() {
    }

    float get_cell_width() {
        return this->cell_width;
    }

    Piece* getPieceByCoords(int x, int y) {
        return this->curr_board[x][y];
    }

    void setPieceByCords(int x, int y, Piece* piece) {
        this->curr_board[x][y] = piece;
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

    void drawBoard(sf::RenderWindow& window) {
        Print("Drawing board");
        this->convertFenTo64Chars();
        Print(this->converted_fen);
        for (int y = 0; y < 8; y++) {       //y axis
            for (int x = 0; x < 8; x++) {   //x axis
                char curr_char = this->converted_fen[y * 8 + x];
                this->drawSquare(x, y, curr_char, window);
            }
        }
    }

    void drawBoardUsingPieceArray(sf::RenderWindow& window) {
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                this->drawSquare(x, y, NULL, window);
            }
        }
    }

    void drawSquare(int x, int y, char curr_char, sf::RenderWindow& window) {
        sf::Vector2f position = sf::Vector2f(float(x * this->cell_width), float(y * this->cell_width));

        Piece* piece;
        if (!curr_char) {
            piece = this->curr_board[x][y]; // existing piece
            if (!piece) {
                piece = new Piece(NULL);    // blank square
            }
        }
        else {
            piece = new Piece(curr_char);   // whole new piece; would be used for first render
        }


        // drawing the background color for the individual squares 
        sf::RectangleShape square(sf::Vector2f(this->cell_width, this->cell_width));
        square.setPosition(position);
        if ((x + y) % 2 == 0) {
            square.setFillColor(custom_colors.WHITE_SQ);
        }
        else {
            square.setFillColor(custom_colors.BLACK_SQ);
        }
        if (this->current_clicked_position.x == x && this->current_clicked_position.y == y && piece->get_piece_type() != PieceType::BLANK) {
            square.setFillColor(custom_colors.CURR_SELECTION);
        }

        window.draw(square);

        // only draw sprite when its an actual piece
        if (piece && piece->get_piece_type() != PieceType::BLANK) {
            piece->set_coords(x, y);

            sf::Sprite sprite;
            sprite.setPosition(position);

            sf::Texture texture;
            texture.loadFromFile(*(piece->get_png_path())); 
            sprite.setTexture(texture);
            window.draw(sprite);
        }
        this->curr_board[x][y] = piece;
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
        while(window.waitEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left) {
                Print("Mouse left clicked");
                sf::Vector2i local_position = sf::Mouse::getPosition(window);

                if (local_position.x >= 0 && local_position.x <= res.WIDTH && local_position.y >= 0 && local_position.y <= res.HEIGHT) {
                    int normalized_x = (int)(local_position.x / board.get_cell_width());
                    int normalized_y = (int)(local_position.y / board.get_cell_width());
                    board.mouse_down_pos = sf::Vector2i(normalized_x, normalized_y);
                    board.current_clicked_position = sf::Vector2i(normalized_x, normalized_y);
                    window.clear(sf::Color::White);
                    board.drawBoardUsingPieceArray(window);
                    window.display();
                }

            }

            else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left) {
                Print("Mouse left released");
                sf::Vector2i local_position = sf::Mouse::getPosition(window);

                if (local_position.x >= 0 && local_position.x <= res.WIDTH && local_position.y >= 0 && local_position.y <= res.HEIGHT) {
                    int normalized_x = (int)(local_position.x / board.get_cell_width());
                    int normalized_y = (int)(local_position.y / board.get_cell_width());

                    board.mouse_up_pos = sf::Vector2i(normalized_x, normalized_y);

                    if (board.mouse_up_pos != board.mouse_down_pos){
                        Piece* piece = board.getPieceByCoords(board.mouse_down_pos.x, board.mouse_down_pos.y);
                        piece->move(board.mouse_up_pos.x, board.mouse_up_pos.y);
                        board.setPieceByCords(board.mouse_up_pos.x, board.mouse_up_pos.y, piece);
                        board.setPieceByCords(board.mouse_down_pos.x, board.mouse_down_pos.y, NULL);
                    } 
                    board.current_clicked_position = sf::Vector2i(normalized_x, normalized_y);
                    window.clear(sf::Color::White);
                    board.drawBoardUsingPieceArray(window);
                    window.display();
                }

            }
            
            else if (rerender) {
                window.clear(sf::Color::White);
                board.drawBoard(window);
                window.display();
                rerender = false;
            }
        }
    }

    return 0;
}