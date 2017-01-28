/*
----------------------------
NonStructural Perceptron   -
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
    if (destlen ==(size_t)(0))
        return val;
    val = pc;
    delete pc;
    return val;
}
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
	}
	~TrainSet(){
		this->initialize();
	}
	void initialize(){
		this->trainSet.clear();
		this->typeTag.clear();
		this->feature.clear();	
	}
	void readIn(string fname="train.txt"){
		cout<<"===========================NOW INPUT THE TRAINING SET============================="<<endl;
		setlocale(LC_ALL, "");
		ifstream ifs;
		ifs.open(fname);
		char input[10000];
		while (!ifs.eof()) {
			ifs.getline(input, sizeof(input));
			wstring wstr=c2w(input);
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
	void extract(wstring& strToExtr) throw(int){
		wstring typeStr=L"*";
		wstring trainStr=L"*";
		//the char'*' is the start of a wstring
		wchar_t lchar=L'*';
		wchar_t cchar;
		unsigned int index=1;
		unsigned int upper=strToExtr.size();
		while(index<upper&&strToExtr[index]==L' ')
			index++;
		while(index<upper){
			cchar=strToExtr[index];
			trainStr+=cchar;
			//get the next char
			bool isNear=true;
			index++;
			while(index<upper&&strToExtr[index]==L' '){
				index++;
				isNear=false;
			}
			//if reach the end of the string
			if(index>=upper){
				//if the type of last wchar is S|E|*, then the type of cchar is S;
				if(typeStr[typeStr.size()-1]==L'S'||typeStr[typeStr.size()-1]==L'E'||typeStr[typeStr.size()-1]==L'*'){
					typeStr+=L'S';
					insertFeature(L'S',lchar,cchar);
					if(trainStr.size()>=3){
						this->trainSet.push_back(trainStr);
						this->typeTag.push_back(typeStr);
					}
					return;
				}
				//if the type of the last wchar is B|M, then the type of cchar is E;
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
					//the type of cchar is B 
					if(typeStr[typeStr.size()-1]==L'S'||typeStr[typeStr.size()-1]==L'E'||typeStr[typeStr.size()-1]==L'*'){
						typeStr+=L'B';
						insertFeature(L'B',lchar,cchar,strToExtr[index]);
					}
					//the type of cchar is M
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
					//the type of cchar is S
					if(typeStr[typeStr.size()-1]==L'S'||typeStr[typeStr.size()-1]==L'E'||typeStr[typeStr.size()-1]==L'*'){
						typeStr+=L'S';
						insertFeature(L'S',lchar,cchar,strToExtr[index]);
					}
					//the type of cchar is E
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

		for(unsigned int i=0;i<this->tSet->feature.size();i++)
			this->finalVec.push_back(0);
	}
	~Perceptron(){
		delete this->tSet;
		this->weightVec.clear();
		this->finalVec.clear();
	}
	double getScore(wchar_t type,wchar_t lchar,wchar_t cchar,wchar_t nchar){
		double score=getScore(type,lchar,cchar);
		wstring tempStr={lchar,nchar,L'_',type,L'_'};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=this->finalVec[this->tSet->feature[tempStr]];
		tempStr={cchar,type,nchar};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=this->finalVec[this->tSet->feature[tempStr]];
		tempStr={nchar,type,L'_'};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=this->finalVec[this->tSet->feature[tempStr]];
		tempStr={lchar,cchar,type,nchar};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=this->finalVec[this->tSet->feature[tempStr]];
		return score;
	}
	double getScore(wchar_t type,wchar_t lchar,wchar_t cchar){
		double score=0;
		wstring tempStr={lchar,L'_',type};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=this->finalVec[this->tSet->feature[tempStr]];
		tempStr={lchar,cchar,type};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=this->finalVec[this->tSet->feature[tempStr]];
		tempStr={cchar,type};
		if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
			score+=this->finalVec[this->tSet->feature[tempStr]];
		return score;
	}
	void adjust(wchar_t ctag,wchar_t wtag,wchar_t lchar,wchar_t cchar,wchar_t nchar){
		wchar_t types[2]={ctag,wtag};
		for(unsigned int i=0;i<2;i++){
			wchar_t type=types[i];
			double delta=(i==0)?0.5:-0.5;
			wstring tempStr={lchar,L'_',type};
			if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
				this->finalVec[this->tSet->feature[tempStr]]+=delta;
			tempStr={lchar,cchar,type};
			if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
				this->finalVec[this->tSet->feature[tempStr]]+=delta;
			tempStr={cchar,type};
			if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
				this->finalVec[this->tSet->feature[tempStr]]+=delta;
			tempStr={cchar,type,nchar};
			if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
				this->finalVec[this->tSet->feature[tempStr]]+=delta;
			tempStr={lchar,cchar,type,nchar};
			if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
				this->finalVec[this->tSet->feature[tempStr]]+=delta;
			tempStr={lchar,nchar,L'_',type,L'_'};
			if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
				this->finalVec[this->tSet->feature[tempStr]]+=delta;
			tempStr={nchar,type,L'_'};
			if(this->tSet->feature.find(tempStr)!=this->tSet->feature.end())
				this->finalVec[this->tSet->feature[tempStr]]+=delta;
		}
	}

	void trainASent(wstring& wstr,wstring& tags){
		//output(wstr,"show.txt");
		//output(tags,"show.txt");
		wchar_t types[4]={L'B',L'M',L'E',L'S'};
		for(unsigned int i=1;i<wstr.size()-1;i++){
			int count=1;
			while(count--){

				double maxScore=-0x3f3f3f3f;
				int typeNum=0;
				for(unsigned int j=0;j<4;j++){
					double tpScore=this->getScore(types[j],wstr[i-1],wstr[i],wstr[i+1]);
					//cout<<"typeNUM:"<<j<<" SCORE:"<<tpScore<<endl;
					if(tpScore>maxScore){
						maxScore=tpScore;
						typeNum=j;
					}
				}
				//cout<<typeNum;
				if(tags[i]==types[typeNum])
					break;
				this->adjust(tags[i],types[typeNum],wstr[i-1],wstr[i],wstr[i+1]);
			}
		}
	}

	void train(){
		int iterTimes=6;
		int repeat=0;
		while((repeat++)<iterTimes){
			vector<double> initVec;
			for(unsigned int i=0;i<this->tSet->feature.size();i++)
				initVec.push_back(0);
			this->weightVec.push_back(initVec);

			int cnt=1;

			//in the iteration, change the order of sentence trained to get weight vectors with different inclination;
			unsigned int upper=this->tSet->trainSet.size(),lower=0;
			int aug=1;
			
			if(repeat>=iterTimes/2){
				upper=0;
				lower=this->tSet->trainSet.size()-1;
				aug=-1;
			}
			
			for(int i=lower;(upper==0&&i>=0)||(upper!=0&&i<upper);i+=aug){
				if(i%1000==0){
					cnt++;
					for(unsigned int j=0;j<this->tSet->feature.size();j++)
						this->weightVec[repeat-1][j]+=this->finalVec[j];
					cout<<"ITERTIME:"<<repeat<<"================NOW TRAINING THE "<<i<<"TH SENTENCE===================="<<endl;
				}
				trainASent(this->tSet->trainSet[i],this->tSet->typeTag[i]);
			}
			
			for(unsigned int i=0;i<this->tSet->feature.size();i++){
				this->weightVec[repeat-1][i]+=this->finalVec[i];
				this->weightVec[repeat-1][i]/=cnt;
			}
			
		}
		cout<<"-----------------GETTING THE AVERAGE VECTOR-----------------"<<endl;

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
    void readIn(string fileName = "test.txt") {
            setlocale(LC_ALL, "");
            cout<<"==========================NOW INPUT THE TEST SET=============================="<<endl;
            ifstream testIfs;
            testIfs.open(fileName);
            char input[10000];
            while (!testIfs.eof()) {
                    testIfs.getline(input, sizeof(input));
                    wstring wstr=c2w(input);
                    wstr=L'*'+wstr;
                    this->testSet.push_back(wstr);
            }
            testIfs.close();
            cout << "=========================END OF INPUT======================="<<"SIZE OF TESTSET:"<<this->testSet.size()<<endl;
    }
    void doSege(string fileName="output.txt"){
    	for(unsigned int i=0;i<this->testSet.size();i++){
    		if(this->testSet[i].size()<=1)
    			continue;
    		wstring strOut=L"";
    		if(this->testSet[i].size()<=4){
  				strOut=this->testSet[i][1];
  				for(unsigned int j=2;i<this->testSet[i].size()-1;j++)
  					strOut+=L" "+this->testSet[i][j];
  				strOut+=this->testSet[i][this->testSet[i].size()-1];
    			output(this->testSet[i],fileName);
    			continue;
    		}
    		strOut+=this->testSet[i][1];
    		for(unsigned int j=2;j<this->testSet[i].size()-1;j++){
	    		double maxScore=-0x3f3f3f3f;
	    		int typeNum;
	    		wchar_t types[4]={L'B',L'M',L'E',L'S'};
	    		for(unsigned int k=0;k<4;k++){
	    			double tpScore=this->pctron->getScore(types[k],this->testSet[i][j-1],this->testSet[i][j],this->testSet[i][j+1]);
	    			if(tpScore>maxScore){
	    				maxScore=tpScore;
	    				typeNum=k;
	    			}
	    		}
	    		if(typeNum==0||typeNum==3)
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
	doTest.doSege();
}