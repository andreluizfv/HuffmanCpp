#include <bits\stdc++.h>
#include <filesystem>
using namespace std;
using myByte = unsigned char;

short pairByteToShort (pair<myByte,myByte> p){
    return (((short) p.first) << 8) + p.second;
}
pair<myByte,myByte> shortToPair( short hash){
    return {hash >> 8, hash & (myByte) 0xff};
}

class CompactTreeNode{
    public:
        myByte value = 0;
        myByte offValue = 0;
        CompactTreeNode* leftNode = NULL;
        CompactTreeNode* rightNode = NULL;
        static __int16 size;

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

        void save(ofstream& ofs) {
            ofs.write((char*)&size, sizeof(size));
            saveInOrder(ofs);
            savePreOrder(ofs);
        }
        void saveInOrder(ofstream& ofs) {
            if (leftNode != NULL) (*leftNode).saveInOrder(ofs);
            ofs.write((char*)&this->value, sizeof(this->value));
            ofs.write((char*)&this->offValue, sizeof(this->offValue));
            if (rightNode != NULL) (*rightNode).saveInOrder(ofs);
        }
        void savePreOrder(ofstream& ofs) {
            ofs.write((char*)&this->value, sizeof(this->value));
            ofs.write((char*)&this->offValue, sizeof(this->offValue));
            if (leftNode != NULL) (*leftNode).savePreOrder(ofs);
            if (rightNode != NULL) (*rightNode).savePreOrder(ofs);
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
__int16 CompactTreeNode::size = 0;

class HuffmanTreeNode{
    protected: 
        HuffmanTreeNode* leftNode = NULL;
        HuffmanTreeNode* rightNode = NULL;
        myByte value = 0;
        myByte offValue;
        static myByte noLeafNodes;
        int frequency = 0;
        static __int16 size;
    public: 

        HuffmanTreeNode( ifstream& file){
            map<myByte, int> frequencies;
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
        
        void print() {
            if (leftNode != NULL) (*leftNode).print();
            cout << "(" + to_string(value)+ "," + to_string(frequency) + ")";
            if (rightNode != NULL) (*rightNode).print();
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
        HuffmanTreeNode(myByte value, int frequency){
            this->frequency = frequency;
            this->value = value;
            this->offValue = 0;
            size++;
        }

        HuffmanTreeNode(HuffmanTreeNode& left, HuffmanTreeNode& right){
            this->frequency = left.frequency + right.frequency;
            this->value = 0;
            this->leftNode = &left;
            this->rightNode = &right;
            this->offValue = noLeafNodes++;
            size++;
        }

        void TreeRecursion(vector<HuffmanTreeNode*>& leafs){
            while (leafs.size() > 1)
            {
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

            if (!leafs.empty())
            {
                this->leftNode = leafs.front()->leftNode;
                this->rightNode = leafs.front()->rightNode;
                this->frequency = leafs.front()->frequency;
                this->value = leafs.front()->value;
                this->offValue = leafs.front()->offValue;
            }
        }
};
myByte HuffmanTreeNode::noLeafNodes = 0;
__int16 HuffmanTreeNode::size = 0;

ifstream openFile(string path){
    ifstream file(path, ios::binary);
    if (!file.is_open()) throw runtime_error("Error opening file");
    return file;
}

class BitBuffer {
    public:
        ofstream* file = NULL; // Initialization skipped.
        myByte buffer = 0;
        unsigned count = 0;

        BitBuffer(ofstream& file){
            this->file = &file;
        }
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
                file->write((char*) (&buffer), sizeof(myByte) );
            }
        }
};

void DFSByteToPathSaver(CompactTreeNode* tree, map<myByte, string>& table, string path){
    if(tree == NULL) return;
    if(tree->leftNode == NULL && tree->rightNode == NULL){
        table[tree->value] = path;
        return;
    }
    DFSByteToPathSaver(tree->leftNode, table, path + "0");
    DFSByteToPathSaver(tree->rightNode, table, path + "1");
}

map<myByte, string> byteToStringTable (CompactTreeNode* tree){
    map<myByte, string> table;
    DFSByteToPathSaver(tree, table, "");
    return table;
}

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


void encodeBytes(ofstream& outputFile,string inputPath, map<myByte,string> table){
    ifstream inputFile(inputPath, ios::ate | ios::binary);
    unsigned long int nOfBytes = inputFile.tellg();
    cout << nOfBytes;
    outputFile.write((char*) (&nOfBytes), sizeof(nOfBytes) );
    BitBuffer buffer(outputFile);
    myByte inputByte;
    inputFile.seekg(0);
    for(unsigned long int writtenBytes = 1; writtenBytes <= nOfBytes; writtenBytes++){
        inputFile.read((char*) (&inputByte), sizeof(inputByte));
        buffer.writeStringByBit(table[inputByte], (writtenBytes == nOfBytes) );
    }
    outputFile.close();
}

void decodeBytes(ifstream& inputFile,string outputPath, map<string, myByte> table){
    ofstream outputFile(outputPath, ios::out | ios::binary);
    unsigned long int nOfBytes;
    inputFile.read((char*) (&nOfBytes), sizeof(nOfBytes) );
    cout << "decoding" << endl;
    cout << nOfBytes <<endl;
    BitBuffer buffer(outputFile);
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
}

int main(){
    // encode lena
    ifstream inputTestFile("lena_ascii.pgm", ios::in | ios::binary);
    HuffmanTreeNode fullTree(inputTestFile);
    CompactTreeNode* compactTree = fullTree.getCompact();
    inputTestFile.close();
    ofstream outputFile("lena_ascii.huff", ios::out | ios::binary);
    compactTree->save(outputFile);
    map<myByte,string> table = byteToStringTable(compactTree);
    encodeBytes(outputFile, "lena_ascii.pgm", table);

    //decide lena
    ifstream rf("lena_ascii.huff", ios::in | ios::binary);
    CompactTreeNode* testRecoverFullTree = new CompactTreeNode(NULL, NULL, 0, 0);
    testRecoverFullTree->buildTreeFromFile(rf);
    map<string, myByte> recoveredTable = pathToByteTable(testRecoverFullTree);
    decodeBytes(rf, "recoveredLenaFile.bin", recoveredTable);
    return 0;
}


