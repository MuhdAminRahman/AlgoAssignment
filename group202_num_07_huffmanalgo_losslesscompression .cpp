#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>

using namespace std;

struct Node {
    char data;
    int freq;
    Node* left;
    Node* right;

    Node(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
    ~Node() {
        delete left;
        delete right;
    }
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

void build_huffman_tree(const string& file_name, unordered_map<char, int>& freq_map, Node*& root) {
   ifstream file(file_name);
    if (!file.is_open()) {
        cerr << "Error opening file: " << file_name << endl;
        return;
    }

    char c;
    while (file.get(c)) {
        freq_map[c]++;
    }

    file.close();

    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto& p : freq_map) {
        pq.push(new Node(p.first, p.second));
    }

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* parent = new Node('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }

    root = pq.top();
}

void build_huffman_codes(Node* root, string code, unordered_map<char, string>& code_map) {
    if (!root) {
        return;
    }

    if (!root->left && !root->right) {
        code_map[root->data] = code;
    }

    build_huffman_codes(root->left, code + "0", code_map);
    build_huffman_codes(root->right, code + "1", code_map);
}

void compress_file(const string& input_file_name, const string& output_file_name, unordered_map<char, string>& code_map) {
   ifstream input_file(input_file_name);
    if (!input_file.is_open()) {
        cerr << "Error opening file: " << input_file_name << endl;
        return;
    }

    ofstream output_file(output_file_name);
    if (!output_file.is_open()) {
        cerr << "Error opening file: " << output_file_name << endl;
        return;
    }

    char c;
    string code;
    while (input_file.get(c)) {
        code += code_map[c];
        while (code.length() >= 8) {
            char byte = 0;
            for (int i = 0; i < 8; i++) {
                if (code[i] == '1') 
                {
                    byte |= (1 << (7 - i));
                }
            }
            output_file << byte;
            code.erase(0, 8);
        }
    }

    if (!code.empty()) {
        char byte = 0;
        for (int i = 0; i < code.length(); i++) {
            if (code[i] == '1') {
                byte |= (1 << (7 - i));
            }
        }
        output_file << byte;
    }

    input_file.close();
    output_file.close();
}

void decompress_file(const string& input_file_name, const string& output_file_name, Node* root) {
    ifstream input_file(input_file_name);
    if (!input_file.is_open()) {
        cerr << "Error opening file: " << input_file_name << endl;
        return;
    }

    ofstream output_file(output_file_name);
    if (!output_file.is_open()) {
        cerr << "Error opening file: " << output_file_name << endl;
        return;
    }

    char c;
    Node* current = root;
    while (input_file.get(c)) {
        for (int i = 7; i >= 0; i--) {
            if ((c >> i) & 1) {
                current = current->right;
            } else {
                current = current->left;
            }
            if (!current->left && !current->right) {
                output_file << current->data;
                current = root;
            }
        }
    }

    input_file.close();
    output_file.close();
}
void print_output_file(const string& output_file_name, const unordered_map<char, int>& freq_map, const unordered_map<char, string>& code_map) {
    ofstream output_file(output_file_name);
    if (!output_file.is_open()) {
        cerr << "Error opening file: " << output_file_name << endl;
        return;
    }

    int unique_chars = freq_map.size();
    output_file << "Number of unique characters: " << unique_chars << endl;

    int total_bits = 0;
    int total_freq = 0;
    for (const auto& p : freq_map) {
        char data = p.first;
        int freq = p.second;
        string code = code_map.at(data);
        int bits = code.length() * freq;
        total_freq += freq;

        total_bits += bits;
        output_file << "Character: '" << data << "', Frequency: " << freq
                    << ", Code: " << code << ", Bits: " << bits << endl;
    }

    double original_bits = 7.0 * total_freq;
    output_file << total_bits << "-bits out of " << original_bits << "-bits. " << endl;

    double compression_ratio = 100.0 * (original_bits - total_bits) / original_bits;
    output_file << "total space " << compression_ratio << "%" << endl;

    output_file.close();
}
int main() {
    srand(1211302855);

    unordered_map<char, int> freq_map;
    Node* root = nullptr;

    clock_t start_time = clock();
    build_huffman_tree("huffmancoding_00000010_input.txt", freq_map, root);
    unordered_map<char, string> code_map;
    build_huffman_codes(root, "", code_map);

    compress_file("huffmancoding_00000010_input.txt", "compressed_10_words.txt", code_map);
    decompress_file("compressed_10_words.txt", "decompressed_10_words.txt", root);
    clock_t end_time = clock();

    double time_taken = static_cast<double>(end_time - start_time) / static_cast<double>(CLOCKS_PER_SEC);
    print_output_file("huffmancoding_00000010_ouput.txt", freq_map, code_map);
    ofstream output_file("huffmancoding_00000010_ouput.txt", ios_base::app);
    output_file << "Time taken: " << time_taken << " seconds" << endl;
    output_file.close();

    delete root;

    return 0;
}