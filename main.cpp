//
//  main.cpp
//  Parser
//
//  Created by David Shechtman on 1/15/15.
//  Copyright (c) 2015 ___DAVID___. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <cstdlib>
#include <mutex>
#include <time.h>

std::mutex mtx;
using namespace std;

//Globals
int threadCount;
vector<string> noDot;
vector<string> oneDot;
vector<string> twoDot;
vector<string> firstNameData;
vector<string> lastNameData;
struct thread_data{
    size_t size;
    size_t index;
    int threadID;
};

//searches chosen database for match to findName
string runAgainstData(string findName, bool useFirstNameDataBase){
    string retval="noNameFound";
    if(useFirstNameDataBase){
        for(int r=0; r<firstNameData.size(); r++){
            if(strcasecmp(findName.c_str(), firstNameData[r].c_str())==0){
                return findName; //could return a number of things here
            }
        }
        
    }
    else{//use last name database
        retval="lastNameDatabase";
    }
    return retval;
}

//Prints a username that has no dots
void printNoDot(string foundName, string userName, bool isFirstName){
    mtx.lock();
    if(isFirstName){
        size_t fNameIndex = foundName.length();
        string fName=userName.substr(0,fNameIndex);
        if(fNameIndex!=userName.length()){
            string lName=userName.substr(fNameIndex,userName.length()-fNameIndex);
            cout<<"{"<<userName<<"} - {"<<fName<<" "<<lName<<"}"<<endl;
        }
        else{
            cout<<"{"<<userName<<"} - {"<<fName<<"}"<<endl;
        }
        
        mtx.unlock();
        
    }
    else{//last name printing
        
    }
}



//runs "bad" algorithm to find longest first name--error prone for something like maxwellington or alexanderson
void runNoDot(vector<string> noDotSub){
    size_t count = noDotSub.size();
    string foundName="";
    for(int k=0; k<count; k++){
        //find the name
        string name = noDotSub[k];
        size_t nameLen= name.length();
        for(int a =1; a<nameLen+1; a++){
            string temp = runAgainstData(name.substr(0,a),true);
            if(strcmp(temp.c_str(),"noNameFound")==0){
                continue;
            }
            if(strcmp(temp.c_str(),"noNameFound")!=0){
                if(temp.length()>foundName.length()){
                    foundName=temp;
                }
            }
        }
        if(strcmp(foundName.c_str(),"")==0){//call last name
            
        }
        else{
            printNoDot(foundName,name, true);
            foundName="";
        }
        
        
        //check last name
        /*if(strcmp(foundName.c_str(),"noNameFound")==0){//check back to front
         for(size_t a =nameLen; a>0; a--){
         string temp = runAgainstData(name.substr(a,name.length()-a),false);
         if(temp.length()>foundName.length()){
         foundName=temp;
         }
         }
         }
         else{*/
        
        //}
    }
    
}

void * helperThread(void * data){
    mtx.lock();
    thread_data * temp2 = (thread_data*)data;
    vector<string>::const_iterator first = noDot.begin() + temp2->index;
    vector<string>::const_iterator last = noDot.begin() + (temp2->index)+(temp2->size);
    vector<string> newVec(first,last);
    mtx.unlock();
    runNoDot(newVec);
    return NULL;
}

//Prints username that contains one or two dots
void printWithDots(string strWork, bool doubleDot){
    //would like to use strtok but was getting errors referencing the function
    
    string fName,mName,lName;
    size_t firstDot = strWork.find_first_of('.');
    fName=strWork.substr(0,firstDot);
    if(doubleDot){
        size_t secondDot = strWork.find_first_of('.',firstDot+1);
        mName=strWork.substr(firstDot+1,secondDot-firstDot-1);
        lName=strWork.substr(secondDot+1,strWork.npos);
        cout<<"{"<<strWork<<"} - {"<<fName<<" "<<mName<<" "<<lName<<"}"<<endl;
    }
    else{//single dot
        lName=strWork.substr(firstDot+1,strWork.length()-1);
        cout<<"{"<<strWork<<"} - {"<<fName<<" "<<lName<<"}"<<endl;
    }
    
}


void printAll(){//used for testing only
    cout<<"No dot list:"<<endl;
    for( int i = 0; i < noDot.size(); i++){
        cout << noDot[i] << endl;
    }
    cout<<"One dot list:"<<endl;
    for( int i = 0; i < oneDot.size(); i++){
        cout << oneDot[i] << endl;
    }
    cout<<"Two dot list:"<<endl;
    for( int i = 0; i < twoDot.size(); i++){
        cout << twoDot[i] << endl;
    }
    
    
}



int main(int argc, const char * argv[])
{
    cout<<"Thread Count?(0-10): ";
    threadCount=0;
    
    while(!(cin>>threadCount)||(threadCount>10)||(threadCount<0)){
        cin.clear();
        cin.ignore(1000,'\n');
        cout<<"Invalid input, try again\nThread Count?(1-10) ";
    }
    
    clock_t tStart = clock();
    //Grab database to run against
    ifstream infile2;
    string readIn2;
    infile2.open ("/Users/d9000123/Desktop/Parser/Parser/firstNameData.txt");
   
    if (infile2.fail())
	{
		cout << "Error opening firstname data\n";
		return 1;
	}
    while ( getline(infile2,readIn2)) {
        firstNameData.push_back(readIn2);
    }
    
    infile2.close();
    ifstream infile3;
    string readIn3;
    infile3.open ("/Users/d9000123/Desktop/Parser/Parser/lastnames.txt");
    if (infile3.fail())
	{
		cout << "Error opening firstname data\n";
		return 1;
	}
    while ( getline(infile3,readIn3)){
        lastNameData.push_back(readIn3);
    }
    
    infile3.close();
    
    //seperate input data
    string regex_str = "[a-z]+\\.[a-z]+";
    regex regOneDot(regex_str, regex_constants::icase);
    
    regex_str = "[a-z]+\\.[a-z]+\\.[a-z]+";
    regex regTwoDot(regex_str, regex_constants::icase);
    
    string readIn;
    ifstream infile;
    
    infile.open ("/Users/d9000123/Desktop/Parser/Parser/usernames.txt");
    if (infile.fail())
	{
		cout << "Error opening username data\n";
		return 1;
	}
    
    while(infile) // To get all the lines.
    {
        getline(infile,readIn);
        if (regex_search(readIn, regTwoDot))
        {
            //twoDot.push_back(readIn);
            printWithDots(readIn,true);
        }
        else if (regex_search(readIn, regOneDot)){
            //oneDot.push_back(readIn);
            printWithDots(readIn,false);
        }
        else{
            noDot.push_back(readIn);
        }
        
    }
    size_t noDotSize=noDot.size();
    if(threadCount==0||(noDotSize<threadCount)){
        runNoDot(noDot);
        printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
        return 0;
    }
    
    
    //parralelize if necessary
    size_t itemsPerThread=noDotSize/threadCount;//floors by def of int
    int extra = noDotSize%threadCount;
    pthread_t threads[threadCount];
    
    int rc;
    int i;
    int threadCountCreate=threadCount;
    
    for(i=0; i<threadCountCreate;i++){
        thread_data * temp = new thread_data;
        temp->size=itemsPerThread;
        temp->threadID=i;
        temp->index=(i*itemsPerThread);
        rc =  pthread_create(&threads[i], NULL, &helperThread, (void *)temp);
        if(rc != 0) {
            printf("Error: pthread_create() failed\n");
            exit(EXIT_FAILURE);
        }
    }
    if (extra!=0) {//create the last or "extra" thread
        thread_data * temp2 = new thread_data;
        temp2->size=extra;
        temp2->threadID=threadCount-1;
        temp2->index=(threadCountCreate*itemsPerThread);
        pthread_create(&threads[threadCount-1],NULL, &helperThread,(void*)temp2);
        pthread_join(threads[threadCount-1],NULL);
    }
    for(int w=0; w<threadCountCreate; w++){
        pthread_join(threads[w], NULL);
    }
    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
    pthread_exit(NULL);
    
    
    
    return 0;
}
