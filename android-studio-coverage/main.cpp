#include <vector>
#include <string>
#include <stdio.h>
#include <fstream>
#include <iostream>

using namespace std;

const string TFOOT_TAG = "tfoot";

// Pre: This functions assumes that we read the output that jacoco generates, otherwise it wont work.
string getPercentageFromFile(string &readedFile){
    int tfootStart = readedFile.find("<"  + TFOOT_TAG + ">");
    int tfootEnd = readedFile.find("</" + TFOOT_TAG + ">");
    string tfoot = readedFile.substr(tfootStart, tfootStart - tfootEnd);
    int percentageStart = tfoot.find("ctr2") + 6; // offset to finish the tag close
    int percentageEnd = tfoot.find("%");
    string percentage = tfoot.substr(percentageStart, percentageEnd - percentageStart);
    return percentage;
}

void writePercentageAsJson(string percentage, string outputName){
    string output = "{ \"lines\" : " + percentage + " }";
    ofstream outputFile;
    outputFile.open(outputName);
    outputFile << output;
    cout << output << endl;
    outputFile.close();
}

class HTMLParser {
public:    
    HTMLParser(string fileName){
        file.open(fileName.c_str());
        if(file.fail()) {
            cout << "Input file opening failed.\n";
            state = -1;
		    exit(state);
        }
        else 
            state = 1;
    }
    ~HTMLParser(){
        file.close();
    }
    void watchState(){
        cout << state << endl;
    }
    string getFileAsString(){
        string res = "";
        while(!file.eof()){
            string temp = "";
            getline(file, temp);    
            res += temp;
        }
        return res;
    }
private:
    ifstream file;
    int state = 0;
};

int main(int argc, char const *argv[]) {   
    string filename = string(argv[1]);
    HTMLParser parser(filename);
    string file = parser.getFileAsString();
    writePercentageAsJson(getPercentageFromFile(file), "code_coverage.json");
    return 0;
}
