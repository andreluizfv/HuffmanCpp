#include<cmath>
#include<fstream>
#include<iostream>
#include<string>
#include<vector>

using namespace std;
using myByte = unsigned char ;

double meanSquareError(string path1, string path2){
    ifstream inputFile1(path1, ios::in | ios::binary);
    ifstream inputFile2(path2, ios::in | ios::binary);
    myByte inputVal1, inputVal2;
    long double result = 0;
    long int totalBytes = 0;
    while(!inputFile1.eof() && !inputFile2.eof()){
        inputFile1.read((char*) &inputVal1, sizeof(inputVal1));
        inputFile2.read((char*) &inputVal2, sizeof(inputVal2));
        result += ( (inputVal1-inputVal2)*(inputVal1-inputVal2) );
        totalBytes++;
    }
    inputFile1.close();
    inputFile2.close();
     cout<<"total bytes in mse: " + to_string(totalBytes) + "\n";
    return (result/(long double) totalBytes);
}

double variancy(string path){
    ifstream inputFile(path, ios::in | ios::binary);
    long double result = 0, mean = 0;
    long int totalBytes = 0;
    myByte inputVal;
    while(!inputFile.eof()){
        inputFile.read((char*) &inputVal, sizeof(inputVal));
        mean += inputVal;
        totalBytes++;
    }
    inputFile.close();
    inputFile.open(path, ios::in | ios::binary);
    mean /= (double) totalBytes;
    inputFile.seekg(0);
    while(!inputFile.eof()){
        inputFile.read((char*) &inputVal, sizeof(inputVal));
        result += ((inputVal-mean)*(inputVal-mean));
    }
    inputFile.close();
    cout<<"total bytes in variancy: " + to_string(totalBytes) + "\n";
    return result / (double) totalBytes;

}

string PSNR(long double mse){
    long double r = 255; //for a byte
    cout << "mse: " + to_string(mse) + "\n";
    if (mse == 0){
        cout <<"mse is 0: division by 0! \n";
        return "PSNR = " + to_string(10*log10(r*r/mse));
    }
    return "PSNR = " + to_string(10*log10(r*r/mse));
}


int main(){
    cout<<"Lena image:\n";
    cout << PSNR(meanSquareError("lena_ascii.huff.pgm", "lena_ascii.pgm")) <<endl;
    cout <<"Baboon image:\n";
    cout << PSNR(meanSquareError("baboon_ascii.huff.pgm", "baboon_ascii.pgm")) <<endl;

    return 0;
}