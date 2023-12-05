#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define file "fileloadtext.txt"

void load(){
    cout<<"loading\n";
    FILE *fp;
    fp = fopen("fileloadtext.txt","r");
    string output,output2;
    fscanf(fp,"%s %s", output.c_str(),output2.c_str());
    output = (string)output.c_str();
    output2 = (string)output2.c_str();
    cout<<output<<endl<<output2<<endl;
    fclose(fp);
}

void write(){
    cout<<"writing\n";
    FILE *fp;
    fp = fopen("fileloadtext.txt","w");
    string input;
    cin>>input;
    fprintf(fp,input.c_str());
    fclose(fp);
}

int main(){
    //write();
    load();

    return 0;
}