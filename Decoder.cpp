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

void decodeBytes(ifstream& inputFile,string outputPath, map<string, myByte> table){
    ofstream outputFile(outputPath, ios::out | ios::binary);
    if (!outputFile.is_open()) throw runtime_error("Error opening file");
    unsigned long int nOfBytes;
    myByte inputByte, outputByte;
    string inputBytesAsString = "", currentKeyString = "";
    inputFile.read((char*) (&nOfBytes), sizeof(nOfBytes) );
    cout << "decoding " + to_string(nOfBytes) + " bytes\n";
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
    myByte nOfBits, value, inputString;
    string key = "";
    file.read((char*)&size, sizeof(size));
    for(; size > 0; size --){
        file.read((char*)&nOfBits, sizeof(nOfBits));
        numberOfBytes = ceil( nOfBits / 8.0);
        for(; numberOfBytes > 0 ; numberOfBytes --){
            file.read((char*)&inputString, sizeof(inputString));
            key += myByte_toString(inputString);
        }
        key = key.substr(0, nOfBits);
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
    //  decode baboon
    ifstream rf2("baboon_ascii.huff", ios::in | ios::binary);
    if (!rf2.is_open()) throw runtime_error("Error opening file");
    map<string, myByte> recoveredTable2 = recoverInvertedTable(rf2);
    decodeBytes(rf2, "baboon_ascii.huff.pgm", recoveredTable2);
}
