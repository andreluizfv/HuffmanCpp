#include <algorithm>
#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
using namespace std;
using myByte = unsigned char;

short pairByteToShort (pair<myByte,myByte> p){
    return (((short) p.first) << 8) + p.second;
}

class CompactTreeNode{
    public:
        myByte value = 0;
        myByte offValue = 0;
        CompactTreeNode* leftNode = NULL;
        CompactTreeNode* rightNode = NULL;
        static int16_t size;

        CompactTreeNode(CompactTreeNode* leftNode, CompactTreeNode* rightNode, myByte value, myByte offValue): leftNode(leftNode),
        rightNode(rightNode), value(value), offValue(offValue){}

        CompactTreeNode(ifstream& file){
            buildTreeFromFile(file);
        }
        void printInOrder() {
            if (leftNode != NULL) (*leftNode).printInOrder();
            cout << "(" + to_string(value) + "," +to_string(offValue) + ") ";
            if (rightNode != NULL) (*rightNode).printInOrder();
        }
        void printPreOrder() {
            cout << "(" + to_string(value) + "," +to_string(offValue) + ") ";
            if (leftNode != NULL) (*leftNode).printPreOrder();
            if (rightNode != NULL) (*rightNode).printPreOrder();
        }

        void buildTreeFromFile(ifstream& file){
            short nOfNodes;
            myByte inputVal, inputOffVal;
            vector<pair<myByte,myByte>> inOrder, preOrder;
            unordered_map< short, int> shortToInIndex;
            file.read((char*) &nOfNodes, sizeof(nOfNodes));
            for( int i = 1; i <= 2*nOfNodes; i++){
                file.read((char*) &inputVal, sizeof(inputVal));
                file.read((char*) &inputOffVal, sizeof(inputOffVal));
                i <= nOfNodes ? inOrder.push_back({inputVal,inputOffVal}) : preOrder.push_back({inputVal,inputOffVal});
            }
            for(int i = 0; i < nOfNodes; i++){
                shortToInIndex[pairByteToShort(inOrder[i])] = i; 
            }
            int preOrderIndex = 0;
            CompactTreeNode* result = buildTreeFromVectorsAndMap(inOrder, preOrder, shortToInIndex, preOrderIndex);
            this->leftNode = result->leftNode;
            this->rightNode = result->rightNode;
            this->value = result->value;
            this->offValue = result->offValue;
        }
        CompactTreeNode* buildTreeFromVectorsAndMap(vector<pair<myByte,myByte>>& inOrder,vector<pair<myByte,myByte>>& preOrder,
                                                     unordered_map< short, int>& pairToInIndex,
                                                     int& preOrderIdx, int inOrderBegin = -1, int inOrderEnd = -1, bool firstInteraction = true){
            if(firstInteraction){ // first interaction
                inOrderBegin = 0;
                inOrderEnd = inOrder.size() - 1;
            }
            if(inOrderBegin > inOrderEnd){
                return NULL;
            }
            auto newNode = new CompactTreeNode(NULL, NULL, preOrder[preOrderIdx].first, preOrder[preOrderIdx].second);
            
            int inOrderSecondPart = pairToInIndex[ pairByteToShort( preOrder[ preOrderIdx++])];
            
            newNode->leftNode = buildTreeFromVectorsAndMap(inOrder, preOrder, pairToInIndex,
                                                           preOrderIdx, inOrderBegin, inOrderSecondPart -1, false);
            newNode->rightNode = buildTreeFromVectorsAndMap(inOrder, preOrder, pairToInIndex,
                                                            preOrderIdx, inOrderSecondPart + 1, inOrderEnd, false);
            return newNode;                                            
         }
};
int16_t CompactTreeNode::size = 0;

void DFSPathToByteSaver(CompactTreeNode* tree, map<string, myByte>& table, string path){
    if(tree == NULL) return;
    if(tree->leftNode == NULL && tree->rightNode == NULL){
        table[path] = tree->value;
        return;
    }
    DFSPathToByteSaver(tree->leftNode, table, path + "0");
    DFSPathToByteSaver(tree->rightNode, table, path + "1");
}

map<string, myByte> pathToByteTable (CompactTreeNode* tree){
    map<string, myByte> table;
    DFSPathToByteSaver(tree, table, "");
    return table;
}

void decodeBytes(ifstream& inputFile,string outputPath, map<string, myByte> table){
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (!outputFile.is_open()) throw runtime_error("Error opening file");
    unsigned long int nOfBytes;

    inputFile.read((char*) (&nOfBytes), sizeof(nOfBytes) );
    cout << "decoding " + to_string(nOfBytes) + " bytes\n";
    myByte inputByte, outputByte;
    string inputBytesAsString = "", currentKeyString = "";
    unsigned long int writtenBytes = 0;
    while(!inputFile.eof() && writtenBytes != nOfBytes){
        if (inputBytesAsString == ""){
            inputFile.read((char*) (&inputByte), sizeof(inputByte));
            inputBytesAsString += bitset<8>(inputByte).to_string();
        }
        currentKeyString += inputBytesAsString[0];
        inputBytesAsString = inputBytesAsString.substr(1, inputBytesAsString.length());
        if(table.find(currentKeyString) != table.end()){
            outputFile.write((char*) (&table[currentKeyString]), sizeof(myByte) );
            writtenBytes++;
            currentKeyString = "";
        }
    }
    cout << to_string(writtenBytes) + " bytes were written\n";
    inputFile.close();
    outputFile.close();
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

int main(){
    //  decode lena
    ifstream rf("lena_ascii.huff", ios::in | ios::binary);
    if (!rf.is_open()) throw runtime_error("Error opening file");
    map<string, myByte> recoveredTable = recoverInvertedTable(rf);
    decodeBytes(rf, "lena_ascii.huff.pgm", recoveredTable);
    // //  decode baboon
    // ifstream rf2("baboon_ascii.huff", ios::in | ios::binary);
    // if (!rf2.is_open()) throw runtime_error("Error opening file");
    // CompactTreeNode* testRecoverFullTree2 = new CompactTreeNode(NULL, NULL, 0, 0);
    // testRecoverFullTree2->buildTreeFromFile(rf2);
    // map<string, myByte> recoveredTable2 = pathToByteTable(testRecoverFullTree2);
    // decodeBytes(rf2, "baboon_ascii.huff.pgm", recoveredTable2);
}
