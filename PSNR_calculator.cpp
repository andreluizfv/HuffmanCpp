#include<cmath>
#include<fstream>
#include<iostream>
#include<string>
#include<vector>

using namespace std;
using myByte = unsigned char ;

void skipToPix(ifstream& inputFile1, ifstream& inputFile2, myByte inputVal1, myByte inputVal2){
    short newLines1 = 0, newLines2 = 0;
    while(!inputFile1.eof() && !inputFile2.eof() && newLines1 < 4 && newLines2 < 4){
        if(newLines1 < 4)
            inputFile1.read((char*) &inputVal1, sizeof(inputVal1));
        if(newLines2 < 4)
            inputFile2.read((char*) &inputVal2, sizeof(inputVal2));
        if (inputVal1 == '\n' )
            newLines1++;
        if (inputVal2 == '\n' )
            newLines2++;
    }
}

int calculeSimbol(myByte* chars, short lastsize){
    int result = 0, i = 0; 
    while(lastsize > 0){
        result += (chars[i++] - '0') * pow(10, lastsize - 1);
        lastsize--;
    }
    return result;
}

double meanSquareError(string path1, string path2){
    ifstream inputFile1(path1, ios::in | ios::binary);
    ifstream inputFile2(path2, ios::in | ios::binary);
    myByte inputVal1, inputVal2, chars1[3], chars2[3], index1 = 0, index2 = 0;
    int value1 = 0, value2 = 0;
    long double result = 0;
    long int totalBytes = 0;
    short lastsize1, lastsize2;
    bool firstSpace1 = false, firstSpace2 = false, secondSpace1 = false, secondSpace2 = false;
    //skipping header
    skipToPix(inputFile1, inputFile2, inputVal1, inputVal2);

    while(!inputFile1.eof() && !inputFile2.eof()){
        if(!secondSpace1) inputFile1.read((char*) &inputVal1, sizeof(inputVal1));
        if(!secondSpace2) inputFile2.read((char*) &inputVal2, sizeof(inputVal2));
        if(inputVal1 != 32 && inputVal1 != 10){
            chars1[index1] = inputVal1;
            lastsize1++;
            index1 = (index1 + 1) % 3;
        }
        else{
            if(firstSpace1){
                secondSpace1 = true;
            }
            else{
                firstSpace1 = true;
                value1 = calculeSimbol(chars1,lastsize1);
            }
        }
        if(inputVal2 != 32 && inputVal1 != 10){
            chars2[index2] = inputVal2;
            lastsize2++;
            index2 = (index2 + 1) % 3;
        }
        else{
            if(firstSpace2){
                secondSpace2 = true;
            }
            else{
                firstSpace2 = true;
                value2 = calculeSimbol(chars2,lastsize2);
            } 
        }
        if(firstSpace2 && firstSpace1 && !secondSpace1 && !secondSpace2){
            result += ( (value1 - value2)*(value1 - value2) );
            if(totalBytes == 262143)
                cout <<"problem\n";
            totalBytes++;
        }
        if(secondSpace1 && secondSpace2){
            secondSpace1 = false;
            secondSpace2 = false;
            firstSpace1 = false;
            firstSpace2 = false;
            lastsize2 = 0;
            lastsize1 = 0;
            index1 = 0;
            index2 = 0;
        }
    }
    inputFile1.close();
    inputFile2.close();
     cout<<"total pixels in mse: " + to_string(totalBytes - 1) + "\n";
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