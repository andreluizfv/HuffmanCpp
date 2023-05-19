#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <cmath>
using namespace std;
using myByte = unsigned char;

/* 
    A utility function to convert a pair of bytes to a short. The reason is to use them as map keys.
    The first byte is shifted left by 8 bits and then added to the second byte.
*/
short pairByteToShort (pair<myByte,myByte> p){
    return (((short) p.first) << 8) + p.second;
}
/*
    A utility function to convert a short to a pair of bytes. The reason is recovering the bytes that generated the key
    The first byte of the pair is obtained by shifting the short right by 8 bits.
    The second byte of the pair is obtained by masking the short with the 8 least significant bits.
*/
pair<myByte,myByte> shortToPair( short hash){
    return {hash >> 8, hash & (myByte) 0xff};
}


/*
    Used to save compressed trees and recover them.
*/
class CompactTreeNode{
    public:
        myByte value = 0; // actual data, just leafs have it
        myByte offValue = 0; // used to each node be different. For leafs is 0
        CompactTreeNode* leftNode = NULL; // left Child
        CompactTreeNode* rightNode = NULL; // right Child
        static int16_t size; // static size to numerate internal nodes

        /*
            Default constructor
        */
        CompactTreeNode(CompactTreeNode* leftNode, CompactTreeNode* rightNode, myByte value, myByte offValue): leftNode(leftNode),
        rightNode(rightNode), value(value), offValue(offValue){}

        /*
            For debug usage
        */
        void printInOrder() {
            if (leftNode != NULL) (*leftNode).printInOrder();
            cout << "(" + to_string(value) + "," +to_string(offValue) + ") ";
            if (rightNode != NULL) (*rightNode).printInOrder();
        }
        /*
            For debug usage
        */
        void printPreOrder() {
            cout << "(" + to_string(value) + "," +to_string(offValue) + ") ";
            if (leftNode != NULL) (*leftNode).printPreOrder();
            if (rightNode != NULL) (*rightNode).printPreOrder();
        }

        /*
            Saves the size, and value-off Value pair for each node in order and in pre order        
        */
        // void save(ofstream& ofs) {
        //     ofs.write((char*)&size, sizeof(size));
        //     saveInOrder(ofs);
        //     savePreOrder(ofs);
        //     size = 0; // reset to encode another file
        // }
        
        // private:
        //     /*
        //         Save tree nodes in in-order in the end of the output file
        //     */
        //     void saveInOrder(ofstream& ofs) {
        //         if (leftNode != NULL) (*leftNode).saveInOrder(ofs);
        //         ofs.write((char*)&this->value, sizeof(this->value));
        //         ofs.write((char*)&this->offValue, sizeof(this->offValue));
        //         if (rightNode != NULL) (*rightNode).saveInOrder(ofs);
        //     }
        //     /*
        //         Save tree nodes in pre-order in the end of the output file
        //     */
        //     void savePreOrder(ofstream& ofs) {
        //         ofs.write((char*)&this->value, sizeof(this->value));
        //         ofs.write((char*)&this->offValue, sizeof(this->offValue));
        //         if (leftNode != NULL) (*leftNode).savePreOrder(ofs);
        //         if (rightNode != NULL) (*rightNode).savePreOrder(ofs);
        //     }

};
int16_t CompactTreeNode::size = 0;

/*
    Used tu build trees from original files and create Compacted Tree
*/
class HuffmanTreeNode{
    protected: 
        myByte offValue; // for leafs it is zero
        static myByte noLeafNodes; // used for counting and naming internal nodes
        int frequency = 0; //frequency that value has. Just has meaning for leafs
        static int16_t size; //size of the tree

    public: 
        myByte value = 0; // just leafs has it has usage
        HuffmanTreeNode* leftNode = NULL; // left Child
        HuffmanTreeNode* rightNode = NULL; // right child
        map<myByte, int> frequencies; // It has byte frequencies to build and to use compute average length code later. It
        /*
        Construct the tree using the file to be compressed

       HuffmanTreeNode(HuffmanTreeNode* leftNode, HuffmanTreeNode* rightNode, myByte value, myByte offValue): leftNode(leftNode),
        rightNode(rightNode), value(value), offValue(offValue){}

        /*
            For debug usage
        */
        void printInOrder() {
            if (leftNode != NULL) (*leftNode).printInOrder();
            cout << "(" + to_string(value) + "," +to_string(offValue) + ") ";
            if (rightNode != NULL) (*rightNode).printInOrder();
        }
        /*
            For debug usage
        */
        void printPreOrder() {
            cout << "(" + to_string(value) + "," +to_string(offValue) + ") ";
            if (leftNode != NULL) (*leftNode).printPreOrder();
            if (rightNode != NULL) (*rightNode).printPreOrder();
        }

        /*
            Saves the size, and value-off Value pair for each node in order and in pre order        
        */
        void save(ofstream& ofs) {
            ofs.write((char*)&size, sizeof(size));
            saveInOrder(ofs);
            savePreOrder(ofs);
            size = 0; // reset to encode another file
        }
        HuffmanTreeNode( ifstream& file){
            refresh();
            vector<HuffmanTreeNode*> leafs;
            myByte c;
            while (!file.eof()){
                file.read((char*) (&c), sizeof(c));;
                frequencies[c]++;
            }
            vector<pair<myByte, int>> freqList(frequencies.begin(), frequencies.end());
            sort(freqList.begin(), freqList.end(), [](const auto& lhs, const auto& rhs) {
                return lhs.second < rhs.second;
            });

            for (auto pair : freqList){
                leafs.push_back(new HuffmanTreeNode(pair.first, pair.second));
            }

            TreeRecursion(leafs);
        }
        /*
            Print using in-order
        */
        void print() {
            if (leftNode != NULL) (*leftNode).print();
            cout << "(" + to_string(value)+ "," + to_string(frequency) + ")";
            if (rightNode != NULL) (*rightNode).print();
        }
        
        /*
        reset size and no leaf nodes number to compress another file
        */
        void refresh(){ // reset to encode another file
            noLeafNodes = 0; 
            size = 0;
        }
        
        CompactTreeNode* getCompact(){
            CompactTreeNode* compactLeft = NULL;
            CompactTreeNode* compactRight = NULL;
            if(leftNode != NULL){
                compactLeft = leftNode->getCompact();
            }
            if(rightNode != NULL){
                compactRight = rightNode->getCompact();
            }
            CompactTreeNode* result = NULL;
            result = new CompactTreeNode(compactLeft, compactRight,this->value, this->offValue);
            result->size++;
            return result;
        }
    private: 
        /*
            Save tree nodes in in-order in the end of the output file
        */
        void saveInOrder(ofstream& ofs) {
            if (leftNode != NULL) (*leftNode).saveInOrder(ofs);
            ofs.write((char*)&this->value, sizeof(this->value));
            ofs.write((char*)&this->offValue, sizeof(this->offValue));
            if (rightNode != NULL) (*rightNode).saveInOrder(ofs);
        }
        /*
            Save tree nodes in pre-order in the end of the output file
        */
        void savePreOrder(ofstream& ofs) {
            ofs.write((char*)&this->value, sizeof(this->value));
            ofs.write((char*)&this->offValue, sizeof(this->offValue));
            if (leftNode != NULL) (*leftNode).savePreOrder(ofs);
            if (rightNode != NULL) (*rightNode).savePreOrder(ofs);
        }
        /*
            Used to generate leaf nodes
        */
        HuffmanTreeNode(myByte value, int frequency){
            this->frequency = frequency;
            this->value = value;
            this->offValue = 0;
            size++;
        }

        /*
            Used to generate one Internal Node from its children
        */
        HuffmanTreeNode(HuffmanTreeNode& left, HuffmanTreeNode& right){
            this->frequency = left.frequency + right.frequency;
            this->value = 0;
            this->leftNode = &left;
            this->rightNode = &right;
            this->offValue = ++noLeafNodes;
            size++;
        }

        /*
            Used to generate all internal nodes
        */
        void TreeRecursion(vector<HuffmanTreeNode*>& leafs){
            while (leafs.size() > 1){
                HuffmanTreeNode* left = leafs.front();
                leafs.erase(leafs.begin());
                HuffmanTreeNode* right = leafs.front();
                leafs.erase(leafs.begin());
                HuffmanTreeNode* parent = new HuffmanTreeNode(*left, *right);
                for (auto it = leafs.begin(); it != leafs.end() + 1; ++it){
                    if(it == leafs.end()){
                        leafs.insert(it, parent);
                        break;
                    }
                    if ((*it)->frequency >= parent->frequency){
                        leafs.insert(it, parent);
                        break;
                    }
                }
            }
            if (!leafs.empty()){
                this->leftNode = leafs.front()->leftNode;
                this->rightNode = leafs.front()->rightNode;
                this->frequency = leafs.front()->frequency;
                this->value = leafs.front()->value;
                this->offValue = leafs.front()->offValue;
            }
        }
};
myByte HuffmanTreeNode::noLeafNodes = 0;
int16_t HuffmanTreeNode::size = 0;


/*
    Used to insert bit's in a file
*/
class BitBuffer {
    private:
        ofstream* file = NULL; // file to write bits
        myByte buffer = 0; // stores bits to write
        unsigned count = 0; // stores the number of bits stored in buffer
    public:

        /**
         *   @param {outputFile} file - where bits will be written 
         */
        BitBuffer(ofstream& file){
            this->file = &file;
        }
        void refresh(){
            this->buffer = 0;
            this->count = 0;
        }
        /**
         * Write a string of 0's and 1's as bit's 
         * 
         * @param {string} code - bit string to be written
         * @param {bool} last - true if it is the last string to be written in that file
         * 
         */ 
        void writeStringByBit(string code, bool last) {
            while(code.length() != 0){
                buffer <<= 1;          // Make room for next bit.
                char c = code[0];
                code = code.substr(1, code.length());

                if (c == '1') buffer |= 1; // Set if necessary.
                count++;              // Remember we have added a bit.
                if (count == 8) {
                    file->write((char*) (&buffer), sizeof(myByte) );
                    buffer = 0;
                    count = 0;
                }
            }
            if (last && count != 0){
                for(; count != 8; count ++) buffer <<= 1;
                file->write((char*) (&buffer), sizeof(myByte));
                this->refresh();
            }
        }
};
/**
 *  Auxiliar function to get save path to each leaf
 * 
 * @param {map} table - map to store
 * @param {string} path - path to current node
 * @param {node} tree - node to interate
*/
void DFSByteToPathSaver(HuffmanTreeNode* tree, map<myByte, string>& table, string path){
    if(tree == NULL) return;
    if(tree->leftNode == NULL && tree->rightNode == NULL){
        table[tree->value] = path;
        return;
    }
    DFSByteToPathSaver(tree->leftNode, table, path + "0");
    DFSByteToPathSaver(tree->rightNode, table, path + "1");
}
/**
 * Used to get byte -> path table used in encoding
 * 
 * @param {node} tree - root of the Tree
 * 
 * @return {map} byte -> path table
 */
map<myByte, string> byteToStringTable (HuffmanTreeNode* tree){
    map<myByte, string> table;
    DFSByteToPathSaver(tree, table, "");
    return table;
}

void encodeBytes(ofstream& outputFile,string inputPath, map<myByte,string> table){
    ifstream inputFile(inputPath, ios::ate | ios::binary);
    if (!inputFile.is_open()) throw runtime_error("Error opening file");
    unsigned long int nOfBytes = inputFile.tellg();
    outputFile.write((char*) (&nOfBytes), sizeof(nOfBytes) );
    BitBuffer buffer(outputFile);
    myByte inputByte;
    inputFile.seekg(0);
    for(unsigned long int writtenBytes = 1; writtenBytes <= nOfBytes; writtenBytes++){
        inputFile.read((char*) (&inputByte), sizeof(inputByte));
        buffer.writeStringByBit(table[inputByte], (writtenBytes == nOfBytes) );
    }
    outputFile.close();
    inputFile.close();
}

map<string, myByte> invertTable(map<myByte, string> table){
    map<string, myByte> invertedTable;
    for(auto x : table) invertedTable[x.second] = x.first;
    return invertedTable;
}

void saveInvertedTable(ofstream& file, map<string, myByte> table){
    int16_t size = table.size();
    file.write((char*)&size, sizeof(size));
    BitBuffer buffer(file);
    myByte nOfBits;
    for (auto x : table){
        nOfBits = x.first.length();
        file.write((char*)&nOfBits, sizeof(nOfBits));
        buffer.writeStringByBit(x.first,true);
        file.write((char*)&x.second, sizeof(x.second));
    }
}
string myByte_toString(myByte byte){
    string result = "";
    for(int i = 0; i < 8; i++){
        (byte & 0x80) ? result += "1" : result += "0";
        byte <<= 1;
    }
    return result;
}
map<string, myByte> recoverInvertedTable(ifstream& file){
    map<string, myByte> table;
    int16_t size, numberOfBytes;
    file.read((char*)&size, sizeof(size));
    myByte nOfBits, value, inputString;
    string key = "";
    for(; size > 0; size --){
        file.read((char*)&nOfBits, sizeof(nOfBits));
        numberOfBytes = ceil( nOfBits / 8.0);
        for(; numberOfBytes > 0 ; numberOfBytes --){
            file.read((char*)&inputString, sizeof(inputString));
            key += myByte_toString(inputString);
        }
        key = key.substr(0,nOfBits);
        file.read((char*)&value, sizeof(value));
        table[key] = value;
        key = "";
    }
    return table;
}

string getAverageLength(map<myByte,string> table, map<myByte, int> frequencies){
    long double average = 0, totalFrequencies = 0;
    if(table.size() == 0){
        cout << 0;
        return "0";
    }
    for(auto x : table){
        average += ((double) x.second.length())*frequencies[x.first];
        totalFrequencies += frequencies[x.first];
    }
    average /= totalFrequencies;
    return to_string(average);
}



int main(){

    // encode lena
    ifstream inputFile("lena_ascii.pgm", ios::in | ios::binary);
    if (!inputFile.is_open()) throw runtime_error("Error opening file");
    HuffmanTreeNode fullTree(inputFile);
    // CompactTreeNode* compactTree = fullTree.getCompact();
    inputFile.close();
    ofstream outputFile("lena_ascii.huff", ios::out | ios::binary);
    if (!outputFile.is_open()) throw runtime_error("Error opening file");
    // compactTree->save(outputFile);
    map<myByte,string> table = byteToStringTable(&fullTree);
    map<string, myByte> invertedTable = invertTable(table);
    saveInvertedTable(outputFile, invertedTable);
    encodeBytes(outputFile, "lena_ascii.pgm", table);
    cout << "lena image average huffman code length: " + getAverageLength(table, fullTree.frequencies) + "\n";

    // // encode baboon
    // ifstream inputFile2("baboon_ascii.pgm", ios::in | ios::binary);
    // if (!inputFile2.is_open()) throw runtime_error("Error opening file");
    // HuffmanTreeNode fullTree2(inputFile2);
    // CompactTreeNode* compactTree2 = fullTree2.getCompact();
    // inputFile2.close();
    // ofstream outputFile2("baboon_ascii.huff", ios::out | ios::binary);
    // if (!outputFile2.is_open()) throw runtime_error("Error opening file");
    // compactTree2->save(outputFile2);
    // map<myByte,string> table2 = byteToStringTable(compactTree2);
    // encodeBytes(outputFile2, "baboon_ascii.pgm", table2);
    // cout << "baboon image average huffman code length: " + getAverageLength(table2, fullTree2.frequencies) + "\n";

    return 0;
}


