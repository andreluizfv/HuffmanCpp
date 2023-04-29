#include <bits\stdc++.h>
using namespace std;
using byte = unsigned char;
class HuffmanTreeNode{
    HuffmanTreeNode* leftNode = NULL;
    HuffmanTreeNode* rightNode = NULL;
    byte value = 0;
    int frequency;

    HuffmanTreeNode(byte value, int frequency){
        this->frequency = frequency;
        this->value = value;
    }

    HuffmanTreeNode(HuffmanTreeNode& left, HuffmanTreeNode& right){
        this->frequency = left.frequency + right.frequency;
        this->value = 0;
        this->leftNode = &left;
        this->rightNode = &right;
    }

    HuffmanTreeNode( ifstream file){
        map<byte, int> frequencies;
        vector<HuffmanTreeNode*> leafs;
        byte c;
        while (!file.eof()) frequencies[c]++;

        vector<pair<byte, int>> freqList(frequencies.begin(), frequencies.end());
        sort(freqList.begin(), freqList.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.second < rhs.second;
        });

        for (auto pair : freqList){
            leafs.push_back(new HuffmanTreeNode(pair.first, pair.second));
        }

        // TreeRecursion(leafs);
    }
    void print() {
            if (leftNode != NULL) (*leftNode).print();
            cout << "(" + to_string(value) + ", " + to_string(frequency) + ")";
            if (rightNode != NULL) (*rightNode).print();
    }

    private: 
    void TreeRecursion(vector<HuffmanTreeNode*>& leafs)
    {
        while (leafs.size() > 1)
        {
            HuffmanTreeNode* left = leafs.front();
            leafs.erase(leafs.begin());
            HuffmanTreeNode* right = leafs.front();
            leafs.erase(leafs.begin());

            HuffmanTreeNode* parent = new HuffmanTreeNode(*left, *right);

            for (auto it = leafs.begin(); it != leafs.end(); ++it){
                if ((*it)->frequency >= parent->frequency){
                    leafs.insert(it, parent);
                    break;
                }
            }
        }

        if (!leafs.empty())
        {
            this->leftNode = leafs.front()->leftNode;
            this->rightNode = leafs.front()->rightNode;
            this->frequency = leafs.front()->frequency;
            this->value = leafs.front()->value;
        }
    }
};

ifstream openFile(string path){
    ifstream file(path, ios::binary);
    if (!file.is_open()) throw runtime_error("Error opening file");
    return file;
}


int main(){
    // string lenaPath = "lena_ascii.pgm";
    // ifstream lenaFile = openFile(lenaPath);
    // byte nextByte;
    // lenaFile >> nextByte;
    // cout <<  (int) nextByte;
    // lenaFile >> nextByte;
    // cout <<  (int) nextByte;
    // lenaFile.seekg(ios_base::beg);
    // lenaFile >> nextByte;
    // cout <<  (int) nextByte;
    stringstream testStream;
   vector<byte> testArray = {1, 2, 2, 2, 2, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5};
    for (auto b : testArray)
        testStream << b;
    
    cout << to_string(sizeof(testArray)/sizeof(unsigned int)) << endl;
    byte output;
    while (!testStream.eof()){
        testStream >> output;
        cout << to_string(output) + " ";
    }    

    // lenaFile.close();
    return 0;
}


