#include <bits\stdc++.h>
using namespace std;
using byte = unsigned char;


short pairByteToShort (pair<byte,byte> p){
    return (((short) p.first) << 8) + p.second;
}
pair<byte,byte> shortToPair( short hash){
    return {hash >> 8, hash & (byte) 0xff};
}

class CompactTreeNode{
    protected: 
        CompactTreeNode* leftNode = NULL;
        CompactTreeNode* rightNode = NULL;
        byte value = 0;
        byte offValue = 0; //used to internal nodes also has an ID
    
    public:
        static __int16 size;
        CompactTreeNode(CompactTreeNode* leftNode, CompactTreeNode* rightNode, byte value, byte offValue): leftNode(leftNode),
        rightNode(rightNode), value(value), offValue(offValue){}

        void printInOrder() {
            if (leftNode != NULL) (*leftNode).printInOrder();
            cout <<  to_string(value) + "," +to_string(offValue) + " ";
            if (rightNode != NULL) (*rightNode).printInOrder();
        }

        void save(ofstream& ofs) {
            ofs.write((char*)&size, sizeof(size));
            saveInOrder( ofs);
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
            byte inputVal, inputOffVal;
            vector<pair<byte,byte>> inOrder, preOrder;
            unordered_map< short, int> pairToInIndex;
            file.read((char*) &nOfNodes, sizeof(nOfNodes));
            for( int i = 1; i <= 2*nOfNodes; i++){
                file.read((char*) &inputVal, sizeof(inputVal));
                file.read((char*) &inputOffVal, sizeof(inputOffVal));
                i <= nOfNodes ? inOrder.push_back({inputVal,inputOffVal}) : preOrder.push_back({inputVal,inputOffVal});
            }
            cout << endl;
            for(int i = 0; i < nOfNodes; i++){
                pairToInIndex[(inOrder[i].first << 8) + inOrder[i].second] = i; 
            }
            CompactTreeNode* result = buildTreeFromVectorsAndMap(inOrder, preOrder, pairToInIndex);
        }

         CompactTreeNode* buildTreeFromVectorsAndMap(vector<pair<byte,byte>>& inOrder,vector<pair<byte,byte>>& preOrder,
                                                     unordered_map< short, int>& pairToInIndex, int preIndex){
            if(inOrder.size() == 1){
                return new CompactTreeNode(NULL, NULL, inOrder[0].first, inOrder[0].second);
            }
            else{
                auto root = preOrder.front();
                preOrder.erase(preOrder.begin());
                auto rootInOrder = inOrder.begin();
                for (; rootInOrder != preOrder.end(); ++rootInOrder){
                    if( (*rootInOrder) == root ) break;
                }
            }
         }
};
__int16 CompactTreeNode::size = 0;

class HuffmanTreeNode{
    protected: 
        HuffmanTreeNode* leftNode = NULL;
        HuffmanTreeNode* rightNode = NULL;
        byte value = 0;
        static byte offValue;
        int frequency = 0;
        static __int16 size;
    public: 

        HuffmanTreeNode( istream& file){
            map<byte, int> frequencies;
            vector<HuffmanTreeNode*> leafs;
            byte c;
            while (!file.eof()){
                file >> c;
                frequencies[c]++;
            }
            vector<pair<byte, int>> freqList(frequencies.begin(), frequencies.end());
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
            if(rightNode == NULL && leftNode == NULL) result = new CompactTreeNode(compactLeft, compactRight, this->value, this->offValue);
            else{
                result = new CompactTreeNode(compactLeft, compactRight,this->value, this->offValue);
                offValue++;
            }
            result->size++;
            return result;
        }
    private: 
        HuffmanTreeNode(byte value, int frequency){
            this->frequency = frequency;
            this->value = value;
            size++;
        }

        HuffmanTreeNode(HuffmanTreeNode& left, HuffmanTreeNode& right){
            this->frequency = left.frequency + right.frequency;
            this->value = 0;
            this->leftNode = &left;
            this->rightNode = &right;
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
        }
    }
};
byte HuffmanTreeNode::offValue = 0;
__int16 HuffmanTreeNode::size = 0;

ifstream openFile(string path){
    ifstream file(path, ios::binary);
    if (!file.is_open()) throw runtime_error("Error opening file");
    return file;
}


int main(){
    // string lenaPath = "lena_ascii.pgm";
    // ifstream lenaFile = openFile(lenaPath);

    stringstream testStream;
    vector<byte> testArray = {1, 2, 2, 2, 2, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5};
    for (auto b : testArray)
       testStream << b;
    HuffmanTreeNode testTree(testStream);
    // testTree.print();
    CompactTreeNode* testCompact = testTree.getCompact();
    // cout << endl;
    // testCompact->printInOrder();
    ofstream fileTree("tree.bin", ios::out | ios::binary);
    testCompact->save(fileTree);
    fileTree.close();
    // lenaFile.close();
    // ifstream rf("tree.bin", ios::in | ios::binary);
    // CompactTreeNode* testRecover = new CompactTreeNode(NULL, NULL, 0, 0);
    // testRecover->buildTreeFromFile(rf);
    // cout<<endl;
    // cout<<testCompact->size;


    // test unsigned char to char and to unsigned char
    // ofstream testFile("charConversionsByte.bin", ios::out | ios::binary);
    // byte great = 129;
    // testFile.write((char*) (&great), sizeof(byte) );
    // testFile.close();
    // ifstream readTestFile("charConversionsByte.bin", ios::in | ios::binary);
    // byte result;
    // char resultAsChar;
    // readTestFile.read((char*) (&result), sizeof(byte));
    // cout << to_string((int) result) <<endl;
    // readTestFile.seekg(0);
    // readTestFile.read((char*) (&resultAsChar), sizeof(byte));
    // cout << to_string((int) resultAsChar) <<endl;

    pair<byte, byte> testPair = {254,255};
    short testHash = pairByteToShort(testPair);
    cout << testHash << endl;
    pair<byte,byte> testFromHash = shortToPair(testHash);
    cout << to_string((int) testFromHash.first) << " " << to_string( (int) testFromHash.second ) << endl;

    return 0;
}


