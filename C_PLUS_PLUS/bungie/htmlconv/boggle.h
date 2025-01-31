#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
#include <set>

using namespace std;

// Trie Node structure
class TrieNode {
public:
    TrieNode* children[26] = { nullptr }; //only english lowercase for now
    bool isEndOfWord = false;
};

// Trie for word storage
class Trie {
public:
    TrieNode* root;

    Trie() {
        root = new TrieNode();
    }

    void insert(const string& word) {
        TrieNode* node = root;
        for (char ch : word) {
            int index = ch - 'a';
            if (!node->children[index])
                node->children[index] = new TrieNode();
            node = node->children[index];
        }
        node->isEndOfWord = true;
    }

    bool search(const string& word) {
        TrieNode* node = root;
        for (char ch : word) {
            int index = ch - 'a';
            if (!node->children[index])
                return false;
            node = node->children[index];
        }
        return node->isEndOfWord;
    }
};

// Boggle Solver Class
// I didn't cover the case where you don't give it enough of a board, this assumes that the board is legal 
// for example if you said size = 2x2 and gave it a 1x2 it does crash
// the opposite size 1x2 and you give it a 2x2 though does work, but it does treat it as a 1x2 and ignores half the "board"
class c_boggle {
private:
    Trie trie;
    unordered_set<string> foundWords;
    vector<vector<char>> board;
    vector<vector<bool>> visited;
    int width, height;

    // DFS Search in all 8 possible directions
    void dfs(int x, int y, string word, TrieNode* node) {
        if (x < 0 || x >= height || y < 0 || y >= width || visited[x][y])
            return;

        char ch = board[x][y];
        int index = ch - 'a';
        if (!node->children[index])
            return;

        word += ch;
        node = node->children[index];

        // Only add words that are at least 3 letters long
        if (word.length() >= 3 && node->isEndOfWord)
            foundWords.insert(word);

        visited[x][y] = true;

        int dx[] = { -1, -1, -1, 0, 1, 1, 1, 0 }; //eight dirs
        int dy[] = { -1, 0, 1, 1, 1, 0, -1, -1 }; //eight dirs

        for (int i = 0; i < 8; i++)
            dfs(x + dx[i], y + dy[i], word, node);

        visited[x][y] = false;
    }

public:
    void set_legal_words(const vector<string>& all_words) {
        for (const auto& word : all_words)
            trie.insert(word);
    }

    vector<string> solve_board(int board_width, int board_height, const char* board_letters) {
        width = board_width;
        height = board_height;
        board = vector<vector<char>>(height, vector<char>(width));
        visited = vector<vector<bool>>(height, vector<bool>(width, false));

        // Correct Board Mapping: Fill 2D board correctly
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                board[i][j] = board_letters[i * width + j];

        foundWords.clear();

        // Start DFS from every cell
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                dfs(i, j, "", trie.root);

        return vector<string>(foundWords.begin(), foundWords.end());
    }
};



#pragma once