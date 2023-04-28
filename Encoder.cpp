#include <bits\stdc++.h>
using namespace std;

class HuffmanTreeNode{
    HuffmanTreeNode* leftNode = NULL;
    HuffmanTreeNode* rightNode = NULL;
    unsigned char value = 0;
    int frequency;

    HuffmanTreeNode(unsigned char value, int frequency){
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
        map<unsigned char, int> frequencies;
        vector<HuffmanTreeNode*> leafs;
        char c;
        while (!file.eof()) frequencies[c]++;

        vector<pair<char, int>> freqList(frequencies.begin(), frequencies.end());
        sort(freqList.begin(), freqList.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.second < rhs.second;
        });

        for (auto pair : freqList)
        {
            leafs.push_back(new HuffmanTreeNode(pair.first, pair.second));
        }

        TreeRecursion(leafs);
    }

    void TreeRecursion(std::vector<HuffmanTreeNode*>& leafs)
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
            m_byte = leafs.front()->m_byte;
            m_frequency = leafs.front()->m_frequency;
            m_left = leafs.front()->m_left;
            m_right = leafs.front()->m_right;
        }
    }
};

ifstream openFile(string path){
    ifstream file(path, ios::binary);
    if (!file.is_open()) throw runtime_error("Error opening file");
    return file;
}


int main(){
    string lenaPath = "lena_ascii.pgm";
    ifstream lenaFile = openFile(lenaPath);
    unsigned char nextByte;
    lenaFile >> nextByte;
    cout <<  (int) nextByte;
    lenaFile >> nextByte;
    cout <<  (int) nextByte;
    lenaFile.seekg(ios_base::beg);
    lenaFile >> nextByte;
    cout <<  (int) nextByte;
    return 0;

    lenaFile.close();
}


