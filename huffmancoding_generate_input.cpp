#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <cstdlib>

using namespace std;

string generate_random_word(int min_length, int max_length) {
    int length = rand() % (max_length - min_length + 1) + min_length;
    string word;
    for (int i = 0; i < length; i++) {
        char c = 'a' + rand() % 26;
        word.push_back(c);
    }
    return word;
}

void generate_input_file(const string& file_name, int num_words) {
    ofstream file(file_name);
    if (!file.is_open()) {
        cerr << "Error opening file: " << file_name << endl;
        return;
    }

    for (int i = 0; i < num_words; i++) {
        string word = generate_random_word(1, 10); // Generate words with length between 1 and 10
        file << word << (i < num_words - 1 ? " " : "");
    }

    file.close();
}

int main() {
    srand(1211302855);

    generate_input_file("huffmancoding_00000010_input.txt", 10);
    generate_input_file("huffmancoding_00000100_input.txt", 100);
    generate_input_file("huffmancoding_00001000_input.txt", 1000);
    generate_input_file("huffmancoding_00010000_input.txt", 10000);
    generate_input_file("huffmancoding_00100000_input.txt", 100000);

    return 0;
}