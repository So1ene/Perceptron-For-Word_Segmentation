/*
----------------------------
Structural Perceptron   -
for Word Segment.          -
----------------------------

----------------
By Qiu Lisong  -
----------------
 */
#include<iostream>
#include<algorithm>
#include<string>
#include<fstream>
#include<vector>
#include<unordered_map>
using namespace std;

//function that converts char* to wstring
std::wstring c2w(const char *pc)
{
    std::wstring val = L"";
    if(NULL == pc)
        return val;
    size_t size_of_wc;
    size_t destlen = mbstowcs(0,pc,0);
    if (destlen ==(size_t)(-1))
        return val;
    size_of_wc = destlen+1;
    wchar_t * pw  = new wchar_t[size_of_wc];
    mbstowcs(pw,pc,size_of_wc);
    val = pw;
    delete pw;
    return val;
}
//function that converts the wstring to char*
std::string w2c(wchar_t * pw)
{
    std::string val = "";
    if(!pw)
        return val;
    size_t size= wcslen(pw)*sizeof(wchar_t);
    char *pc = NULL;
    if(!(pc = (char*)malloc(size)))
    	return val;
    size_t destlen = wcstombs(pc,pw,size);
    /*转换不为空时，返回值为-1。如果为空，返回值0*/
    if (destlen ==(size_t)(0))
        return val;
    val = pc;
    delete pc;
    return val;
}
//function used to output
void output(wstring& wstr,string fileName = "testFile.txt") {
	setlocale(LC_ALL, "");
	ofstream ofs;
	ofs.open(fileName,ios::app|ofstream::binary);
	wchar_t wchars[10000];
	for(unsigned int i=0;i<wstr.size();i++){
		wchars[i]=wstr[i];
	}
	wchars[wstr.size()]=L'\0';
	ofs << w2c(wchars) << endl;
	ofs.close();
}

//class to get input;
class TrainSet{
public:
	TrainSet(){
		this->initialize();
		wstring tempStr;
		//add edge feature with the form {*,B,M,E,S}+'_'+{*,B,M,E,S};
		wchar_t types[5]={L'*',L'B',L'M',L'E',L'S'};
		for(unsigned int i=0;i<5;i++){
			for(unsigned int j=0;j<5;j++){
				tempStr={types[i],L'_',types[j]};
				this->feature.insert(pair<wstring,int>(tempStr,this->feature.size()));
			}
		}
	}
	~TrainSet(){
		this->initialize();
	}
	void initialize(){
		this->trainSet.clear();
		this->typeTag.clear();
		this->feature.clear();	
	}
	//input the training data;
	//note: we need to convert the string&char to wstring&wchar before we conduct the training set.
	void readIn(string fname="train.txt"){
		cout<<"========================NOW INPUT THE TRAINING DATA====================PLZ WAIT!"<<endl;
		setlocale(LC_ALL, "");
		ifstream ifs;
		ifs.open(fname);
		char input[10000];
		while (!ifs.eof()) {
			ifs.getline(input, sizeof(input));
			wstring wstr=c2w(input);
			//extract the string immediately without saving the inputted string;
			if(wstr.size()>=3){
				try{
					this->extract(wstr);
				}catch(int){
					cout<<"Type Error! Plz Check!"<<endl;
					return;
				}
			}
		}
		ifs.close();
		cout << "=========================END OF INPUT======================="<<endl;
		cout<<"TRAINSET SIZE:"<<this->trainSet.size()<<"  FEATURE SIZE:"<<this->feature.size()<<endl;
	}
	//Function used to insert node feature of the token ABC(B's type is T) with the form {A_T,BT,CT_,AC_T_,ABT,BTC,ABTC};
	void insertFeature(wchar_t type,wchar_t lchar,wchar_t cchar,wchar_t nchar){
		insertFeature(type,lchar,cchar);
		wstring tempStr={lchar,nchar,L'_',type,L'_'};
		this->feature.insert(pair<wstring,int>(tempStr,this->feature.size()));
		tempStr={lchar,cchar,type,nchar};
		this->feature.insert(pair<wstring,int>(tempStr,this->feature.size()));
		tempStr={cchar,type,nchar};
		this->feature.insert(pair<wstring,int>(tempStr,this->feature.size()));
		tempStr={nchar,type,L'_'};
		this->feature.insert(pair<wstring,int>(tempStr,this->feature.size()));
	}
	void insertFeature(wchar_t type,wchar_t lchar,wchar_t cchar){
		wstring tempStr={lchar,L'_',type};
		this->feature.insert(pair<wstring,int>(tempStr,this->feature.size()));
		tempStr={lchar,cchar,type};
		this->feature.insert(pair<wstring,int>(tempStr,this->feature.size()));
		tempStr={cchar,type};
		this->feature.insert(pair<wstring,int>(tempStr,this->feature.size()));
	}
	//Extract the feature of one sentence and tag the type each word belongs to;
	void extract(wstring& strToExtr) throw(int){
		wstring typeStr=L"*";
		wstring trainStr=L"*";
		//add the wchar'*' at the start of a wstring;
		wchar_t lchar=L'*';
		wchar_t cchar;
		unsigned int index=1;
		unsigned int upper=strToExtr.size();
		while(index<upper&&strToExtr[index]==L' ')
			index++;
		while(index<upper){
			cchar=strToExtr[index];
			trainStr+=cchar;
			//get the next char which is not block;
			bool isNear=true;
			index++;
			while(index<upper&&strToExtr[index]==L' '){
				index++;
				isNear=false;
			}
			//if reach the end of the string
			if(index>=upper){
				//if the type of last wchar is S|E|*, then the type of cchar will be S;
				if(typeStr[typeStr.size()-1]==L'S'||typeStr[typeStr.size()-1]==L'E'||typeStr[typeStr.size()-1]==L'*'){
					typeStr+=L'S';
					insertFeature(L'S',lchar,cchar);
					if(trainStr.size()>=3){
						this->trainSet.push_back(trainStr);
						this->typeTag.push_back(typeStr);
					}
					return;
				}
				//if the type of last wchar is B|M, then the type of cchar will be E;
				else if(typeStr[typeStr.size()-1]==L'B'||typeStr[typeStr.size()-1]==L'M'){
					typeStr+=L'E';
					insertFeature(L'E',lchar,cchar);
					if(trainStr.size()>=3){
						this->trainSet.push_back(trainStr);
						this->typeTag.push_back(typeStr);
					}
					return;
				}
				else{
					//type Error;
					throw(1);
				}
			}
			else{
				//if the next char is not block
				if(isNear){
					//if the type of last char is S|E|*, then the type of cchar will be B; 
					if(typeStr[typeStr.size()-1]==L'S'||typeStr[typeStr.size()-1]==L'E'||typeStr[typeStr.size()-1]==L'*'){
						typeStr+=L'B';
						insertFeature(L'B',lchar,cchar,strToExtr[index]);
					}
					//if the type of last char is B|M, then the type of cchar will be M;
					else if(typeStr[typeStr.size()-1]==L'B'||typeStr[typeStr.size()-1]==L'M'){
						typeStr+=L'M';
						insertFeature(L'M',lchar,cchar,strToExtr[index]);
					}
					else{
						//type Error;
						throw(1);
					}
				}
				else{
					//if the type of last char is S|E|*, then the type of cchar will be S
					if(typeStr[typeStr.size()-1]==L'S'||typeStr[typeStr.size()-1]==L'E'||typeStr[typeStr.size()-1]==L'*'){
						typeStr+=L'S';
						insertFeature(L'S',lchar,cchar,strToExtr[index]);
					}
					//if the type of cchar is E
					else if(typeStr[typeStr.size()-1]==L'B'||typeStr[typeStr.size()-1]==L'M'){
						typeStr+=L'E';
						insertFeature(L'E',lchar,cchar,strToExtr[index]);
					}
					else{
						//type Error;
						throw(1);
					}
				}
				lchar=cchar;
			}
		}
	}

	friend class Perceptron;
	friend class DoTest;

private:
	vector<wstring> trainSet;
	vector<wstring> typeTag;
	unordered_map<wstring,int> feature;
};

class Perceptron{
public:
	Perceptron(TrainSet* ts){
		this->weightVec.clear();
		this->finalVec.clear();
		this->tSet=ts;
		//initialize the first weight vector with all zero;
		for(unsigned int i=0;i<this->tSet->feature.size();i++)
			this->finalVec.push_back(2);
	}
	~Perceptron(){
		delete this->tSet;
		this->weightVec.clear();
		this->finalVec.clear();
	}
	
	double getScore(vector<double>& wVec,wstring& wStr,unsigned int index,wchar_t ltype,wchar_t ctype){
		//get the score that the wchar gets, according to the weight vector;
		double score=0;
		wstring tempStr={wStr[index-1],L'_',ctype};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=wVec[this->tSet->feature[tempStr]];
		tempStr={wStr[index-1],wStr[index],ctype};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=wVec[this->tSet->feature[tempStr]];
		tempStr={wStr[index],ctype};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=wVec[this->tSet->feature[tempStr]];
		tempStr={wStr[index],ctype,wStr[index+1]};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=wVec[this->tSet->feature[tempStr]];
		tempStr={wStr[index-1],wStr[index],ctype,wStr[index+1]};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=wVec[this->tSet->feature[tempStr]];
		tempStr={wStr[index-1],wStr[index+1],L'_',ctype,L'_'};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=wVec[this->tSet->feature[tempStr]];
		tempStr={wStr[index+1],ctype,L'_'};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=wVec[this->tSet->feature[tempStr]];
		tempStr={ltype,L'_',ctype};
		score+=wVec[this->tSet->feature[tempStr]];
		return score;
	}

	//assistant struct used to record the path(pointer to the wchar before) and score in viterbi algorithm;
	struct VTB{
	public:
		double scores[4]={0,0,0,0};
		int pointer[4]={0,0,0,0};
	};

	//use the dynamic algorithm <viterbi> to get the type sequence with the hightest score;
	//the type of each char is related to the former and the latter char near it, and we 
	//calculate the scores of different possible types with the result of the former char that we already got;
	wstring viterbi(vector<double>& wVec,wstring& wStr,bool flag){
		wstring res=L"E";
		wchar_t types[5]={L'B',L'M',L'E',L'S',L'*'};
		vector<VTB> path;
		path.push_back(VTB());
		//deal with the first word after char'*';
		VTB fstOne;
		for(unsigned int i=0;i<4;i++){
			fstOne.pointer[i]=4;
			fstOne.scores[i]=this->getScore(wVec,wStr,1,L'*',types[i]);
		}
		path.push_back(fstOne);
		//Score refers to the highest score got among the four possible last types;
		//And ptr is the type according to the score;
		double score;
		int ptr;
		for(unsigned int i=2;i<wStr.size()-1;i++){
			VTB newOne;
			for(unsigned int j=0;j<4;j++){
				score=-0x3f3f3f3f;
				//considering the 4 types of the last wchar;
				for(unsigned int k=0;k<4;k++){
					//calculate the score of current char by summing the score each feature gets;
					//then sum the score of former char and the current char as the current char's score;
					double tpScore=path[i-1].scores[k]+this->getScore(wVec,wStr,i,types[k],types[j]);
					if(tpScore>score){
						score=tpScore;
						ptr=k;
					}
				}
				newOne.scores[j]=score;
				newOne.pointer[j]=ptr;
			}
			path.push_back(newOne);
		}
		//get path with the highest score;
		score=-0x3f3f3f3f;
		for(unsigned int i=0;i<4;i++){
			if(path[path.size()-1].scores[i]>score){
				score=path[path.size()-1].scores[i];
				ptr=i;
			}
		}
		//the final tag sequence is determined by the scores of the last char;
		res=types[ptr]+res;
		//get the path with highest score according to the pointers that got before;
		for(unsigned int i=wStr.size()-2;i>=1;i--){
			ptr=path[i].pointer[ptr];
			res=types[ptr]+res;
		}
		
		if(flag)
			if(wStr[wStr.size()-2]!=L'。'&&wStr[wStr.size()-2]!=L'！'
				&&wStr[wStr.size()-2]!=L'？'&&wStr[wStr.size()-2]!=L'…'
				&&wStr[wStr.size()-2]!=L'：'&&wStr[wStr.size()-2]!=L'）'
				&&wStr[wStr.size()-2]!=L'＊'&&wStr[wStr.size()-2]!=L'-'
				&&wStr[wStr.size()-2]!=L'”')
			res[res.size()-2]=L'E';
		
		//Used to debug;
		//output(res,"show.txt");
		return res;
	}

	//function used to adjust the weight vector;
	void adjust(wstring& tStr,wstring& cTag,wstring& wTag){
		//rate represents the delta that the weight vector adjust each time;
		//Here for convenience, param rate is set to be 1;
		const double rate=1;
		wstring tags[2]={cTag,wTag};
		//adjust both by adding the correct vector and subtracting the wrong vector;
		for(unsigned int k=0;k<2;k++){
			double adj=(k==0)?rate:-rate;
			wstring tempStr;
			//consider each token of a wstring;
			for(unsigned int i=1;i<tStr.size();i++){
				if(tags[0][i]==tags[1][i])
					continue;
				//adjust the weight vector for each feature of a word;
				tempStr={tags[k][i-1],L'_',tags[k][i]};
				this->finalVec[this->tSet->feature[tempStr]]+=adj;
				tempStr={tStr[i-1],L'_',tags[k][i]};
				if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
					this->finalVec[this->tSet->feature[tempStr]]+=adj;
				tempStr={tStr[i],tags[k][i]};
				if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
					this->finalVec[this->tSet->feature[tempStr]]+=adj;
				tempStr={tStr[i-1],tStr[i],tags[k][i]};
				if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
					this->finalVec[this->tSet->feature[tempStr]]+=adj;
				tempStr={tStr[i-1],tStr[i],tags[k][i],tStr[i+1]};
				if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
					this->finalVec[this->tSet->feature[tempStr]]+=adj;
				tempStr={tStr[i-1],tStr[i+1],L'_',tags[k][i],L'_'};
				if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
					this->finalVec[this->tSet->feature[tempStr]]+=adj;
				tempStr={tStr[i+1],tags[k][i],L'_'};
				if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
					this->finalVec[this->tSet->feature[tempStr]]+=adj;
				tempStr={tStr[i],tags[k][i],tStr[i+1]};
				if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
					this->finalVec[this->tSet->feature[tempStr]]+=adj;
			}
		}
		return;
	}
	void train(){
		cout<<"=========================NOW START TRAINING=========================="<<endl;
		int repeat=0;
		const int iterTimes=6;
		while((repeat++)<iterTimes){
			vector<double> initVec;
			for(unsigned int i=0;i<this->tSet->feature.size();i++){
				initVec.push_back(0);
			}
			this->weightVec.push_back(initVec);
			
			//in the iteration, change the order of sentence trained to get weight vectors with different inclination;
			unsigned int upper=this->tSet->trainSet.size(),lower=0;
			int aug=1; int cnt=1;
			if(repeat>=iterTimes/2){
				upper=0;
				lower=this->tSet->trainSet.size()-1;
				aug=-1;
			}

			for(int i=lower;(upper==0&&i>=0)||(upper!=0&&i<upper);i+=aug){
				if(i%1000==0){
					cnt++;
					//add the weight vector after the train of each sentence;
					for(unsigned int j=0;j<this->tSet->feature.size();j++)
						this->weightVec[repeat-1][j]+=this->finalVec[j];
					cout<<"THE ITERTIMES:"<<repeat<<"===============NOW TRAINING THE "<<i<<"th SENTENCE================="<<endl;
				}
				
				//limit the size of the wstring to avoid overflow;
				if(this->tSet->trainSet[i].size()<=4)
					continue;
				//get the tag list using viterbi algorithm;
				wstring tagGot=viterbi(this->finalVec,this->tSet->trainSet[i],false);

				//param count is the times of adjustment when coming across wrong answer;
				//if set too large, the result of each predict can be accurate, however the final result may be overfitting;
				int count=20;
				while(tagGot!=this->tSet->typeTag[i]){
					if((count--)<=0)	break;
					//if viterbi return the false tag list, adjust the weight vector;
					this->adjust(this->tSet->trainSet[i],this->tSet->typeTag[i],tagGot);
					tagGot=viterbi(this->finalVec,this->tSet->trainSet[i],false);
				}

			}

			for(unsigned int i=0;i<this->tSet->feature.size();i++){
				this->weightVec[repeat-1][i]+=this->finalVec[i];
				this->weightVec[repeat-1][i]/=cnt;
			}
			
		}

		cout<<"-----------------GETTING THE AVERAGE VECTOR-----------------"<<endl;

		//get the average of each weightVec to avoid overfitting;
		for(unsigned int i=0;i<this->tSet->feature.size();i++){
			double param=0;
			for(unsigned int j=0;j<iterTimes;j++)
				param+=this->weightVec[j][i];
			param/=iterTimes;
			this->finalVec[i]=param;
		}

	}
	friend class DoTest;
private:
	TrainSet* tSet;
	vector<vector<double> > weightVec;
	vector<double> finalVec;
};

class DoTest{
public:
	DoTest(Perceptron* pct){
            this->pctron=pct;
            this->testSet.clear();
    }
    ~DoTest(){
    	this->testSet.clear();
    	delete this->pctron;
    }
    //input the test set;
    void readIn(string fileName = "test.txt") {
            setlocale(LC_ALL, "");
            cout<<"==========================NOW INPUT THE TEST SET=============================="<<endl;
            ifstream testIfs;
            testIfs.open(fileName);
            char input[10000];
            while (!testIfs.eof()) {
                    testIfs.getline(input, sizeof(input));
                    wstring wstr=c2w(input);
                    //add char L'*' at the beginning of the string;
                    wstr=L'*'+wstr;
                    this->testSet.push_back(wstr);
            }
            testIfs.close();
            cout << "=========================END OF INPUT======================="<<"SIZE OF TESTSET:"<<this->testSet.size()<<endl;
    }
    //Start the word segmentation;
    void doSege(string fileName){
    	for(unsigned int i=0;i<this->testSet.size();i++){
    		if(i%1000==0)
    			cout<<"=================NOW CONDUCTING THE "<<i<<"th SENTENCE================="<<endl;
    		if(this->testSet[i].size()<=4){
    			output(this->testSet[i],fileName);
    			continue;
    		}
    		//use the viterbi algorithm which we define in class Perceptron to get the tag sequence;
    		wstring tags=this->pctron->viterbi(this->pctron->finalVec,this->testSet[i],true);
    		wstring strOut=L"";
    		//the first char '*' that we add before is ignored here and the second char takes the first place;
    		strOut+=this->testSet[i][1];
    		for(unsigned int j=2;j<this->testSet[i].size();j++){
    			//if the tag of word is B|S, add a block before position of the word;
    			if(tags[j]==L'B'||tags[j]==L'S')
    				strOut=strOut+L' '+this->testSet[i][j];
    			else
    				strOut=strOut+this->testSet[i][j];
    		}
    		output(strOut,fileName);
    	}
    }
private:
	vector<wstring> testSet;
	Perceptron* pctron;
};

int main(){
	TrainSet* inputTrain=new TrainSet();
	inputTrain->readIn();
	Perceptron* ptron=new Perceptron(inputTrain);
	ptron->train();
	DoTest doTest(ptron);
	doTest.readIn();
	doTest.doSege("sp_out.txt");
}