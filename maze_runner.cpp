#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>

// Representação do labirinto
using Maze = std::vector<std::vector<char>>;

// Estrutura para representar uma posição no labirinto
struct Position {
    int row;
    int col;
};

// Variáveis globais
Maze maze;
int num_rows;
int num_cols;
std::stack<Position> valid_positions;

// Função para carregar o labirinto de um arquivo
Position load_maze(const std::string& file_name) {
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << file_name << std::endl;
        return {-1, -1};
    }

    // Lê o número de linhas e colunas
    file >> num_rows >> num_cols;
    file.ignore(); // Ignora o caractere de nova linha após os números

    maze.resize(num_rows, std::vector<char>(num_cols));

    Position start_pos = {-1, -1};

    // Lê o conteúdo do labirinto
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            file >> maze[i][j];
            if (maze[i][j] == 'e') {
                start_pos = {i, j}; // Encontra a posição inicial
            }
        }
    }

    file.close();
    return start_pos;
}

// Função para imprimir o labirinto
void print_maze() {
    for (const auto& row : maze) {
        for (char cell : row) {
            std::cout << cell << ' ';
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
}

// Função para verificar se uma posição é válida
bool is_valid_position(int row, int col) {
    return (row >= 0 && row < num_rows && col >= 0 && col < num_cols && 
            (maze[row][col] == 'x' || maze[row][col] == 's'));
}

// Função principal para navegar pelo labirinto
bool walk(Position pos) {
    // Marcar a posição atual como visitada
    maze[pos.row][pos.col] = 'o'; // Posição corrente
    print_maze();
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Pequeno atraso para visualização

    if (maze[pos.row][pos.col] == 's') {
        return true; // Saída encontrada
    }

    maze[pos.row][pos.col] = '.'; // Marcar como visitada

    // Verificar as posições adjacentes (cima, baixo, esquerda, direita)
    std::vector<Position> directions = {
        {pos.row - 1, pos.col}, // cima
        {pos.row + 1, pos.col}, // baixo
        {pos.row, pos.col - 1}, // esquerda
        {pos.row, pos.col + 1}  // direita
    };

    for (const auto& next_pos : directions) {
        if (is_valid_position(next_pos.row, next_pos.col)) {
            valid_positions.push(next_pos);
        }
    }

    while (!valid_positions.empty()) {
        Position next_pos = valid_positions.top();
        valid_positions.pop();

        if (walk(next_pos)) {
            return true; // Caminho correto encontrado
        }
    }

    return false; // Caminho incorreto, retrocede
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_labirinto>" << std::endl;
        return 1;
    }

    Position initial_pos = load_maze(argv[1]);
    if (initial_pos.row == -1 || initial_pos.col == -1) {
        std::cerr << "Posição inicial não encontrada no labirinto." << std::endl;
        return 1;
    }

    bool exit_found = walk(initial_pos);

    if (exit_found) {
        std::cout << "Saída encontrada!" << std::endl;
    } else {
        std::cout << "Não foi possível encontrar a saída." << std::endl;
    }

    return 0;
}
