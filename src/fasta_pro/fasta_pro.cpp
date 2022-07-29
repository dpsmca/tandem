// fasta_convertor.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

bool clean_line(const char *_i,char *_o, string strDB,bool _ipi);

int main(int argc, char* argv[])
{
	if(argc < 2)	{
		cout << "version 2006.06.01\n";
		cout << "Usage: \"fasta_pro filename [-i|-u|-n -d]\"\n";
		cout << "-i force IPI file number output\n";
		cout << "-u force Unigene style description line processing\n";
		cout << "-d description line: enclose in quotes if there are spaces in the description\n";
		return 0;
	}
	cout << "version 2006.06.01\n";
	string strPathIn = argv[1];
	cout << strPathIn.c_str()  << "\n";
	string strDB = "ens";
	bool bForceIpi = false;
	bool bForceNr = false;
	bool bForceUnigene = false;
	string strFileDesc;
	strFileDesc.clear();
	if(argc > 2)	{
		string strTest;
		int a = 2;
		while(a < argc)	{
			strTest = argv[a];
			if(strTest == "-i")	{
				bForceIpi = true;
			}
			if(strTest == "-u")	{
				bForceUnigene = true;
			}
			if(strTest == "-n")	{
				bForceNr = true;
			}
			if(strTest.find("-d") == 0 && strTest.size() > 2)	{
				strFileDesc = strTest.substr(2,strTest.size() - 2);
				if(strFileDesc.size()+64 >= 250)	{
					strFileDesc = strFileDesc.substr(0,250-64);
				}
			}
			a++;
		}
	}
	string strPathOut = strPathIn + ".pro";
	ifstream ifIn;
	ifIn.open(strPathIn.c_str());
	if(ifIn.fail())	{
		cout << "Input file path does not exist\n";
		return 1;
	}
	ofstream ofOut;
	cout << "output path = " << strPathOut.c_str() << "\n";
	ofOut.open(strPathOut.c_str(),ios_base::binary|ios_base::out);
	if(ofOut.fail())	{
		cout << "Output file path could not be created.\n";
		return 1;
	}
	unsigned long lSize = 1024*1024 - 1;
	long lEntry = 0;

	char *pLine = new char[lSize+1];
	char *pNew = new char[lSize+1];
	char *pValue = '\0';
	char *pStart = '\0';
	char *pEnd = '\0';
	strcpy(pLine,"xbang-pro-fasta-format\tfasta_pro 2006.06.01");
	size_t a = strlen(pLine);
	while(a < 64)	{
		*(pLine+a) = '\0';
		a++;
	}
	strcpy(pLine+a,strFileDesc.c_str());
	a += strFileDesc.size();
	while(a < 256)	{
		*(pLine+a) = '\0';
		a++;
	}
	ofOut.write(pLine,256);
	ifIn.getline(pLine,lSize);
	strDB = "plain";
	if(strstr(pLine,">gi|") == pLine)	{
		strDB = "nr";
	}
	else if(strstr(pLine,">IPI") != NULL)	{
		strDB = "ipi";
	}
	else if(strstr(pLine,">HIT") != NULL)	{
		strDB = "hit";
	}
	else if(strchr(pLine,':') == NULL || strstr(pLine,">ENS") != NULL || (strchr(pLine,':') > strchr(pLine,' ')))	{
		strDB = "plain";
	}
	if(bForceUnigene)	{
		strDB = "plain";
	}
	else if(bForceNr)	{
		strDB = "nr";
	}
	cout << "db type = " << strDB.c_str() << "\n";
	while(ifIn.good() && !ifIn.eof())	{
		if(pLine[0] == '>')	{
			clean_line(pLine,pNew,strDB,bForceIpi);	
			lEntry = (long)strlen(pNew)+1;
			ofOut.write((char *)&lEntry,sizeof(long));
			ofOut.write(pNew,lEntry);
			pStart = pLine;
			ifIn.getline(pStart,lSize);
			if(strlen(pStart) > 0 && pStart[strlen(pStart)-1] == '*')	{
				pStart[strlen(pStart)-1] = '\0';
			}
			while(pStart[0] != '>' && !ifIn.eof())	{
				pValue = pStart + strlen(pStart);
				while(pValue > pStart && isspace(*pValue))
					pValue--;
				pStart = pValue;
				ifIn.getline(pStart,lSize);
				if(strlen(pStart) > 0 && pStart[strlen(pStart)-1] == '*')	{
					pStart[strlen(pStart)-1] = '\0';
				}
			}
			strcpy(pNew,pStart);
			pStart[0] = '\0';
			lEntry = (long)strlen(pLine) + 1;
			ofOut.write((char *)&lEntry,sizeof(long));			
			ofOut.write(pLine,lEntry);
			strcpy(pLine,pNew);
		}
		else	{
			ifIn.getline(pLine,lSize);
		}
	}
	ifIn.close();
	ofOut.flush();
	if(!ofOut.good())	{
		cout << "crap\n";
	}
	ofOut.close();
	delete pLine;
	delete pNew;
	return 0;
}

bool clean_line(const char *_i,char *_o, string strDB, bool _ipi = false)
{
	size_t tLength = strlen(_i);
	size_t a = 1;
	size_t b = 0;
	if(strDB == "ens"){
		char *pColon = NULL;
		char cValue = '\0';
		pColon = strchr(_i,':');
		if(pColon){
			pColon++;
			cValue = pColon[0];
			while(cValue != '\0' && cValue != ' ' && cValue != '|' && cValue != '\n'){
				_o[b] = pColon[b];
				b++;
				cValue = pColon[b];
			}
		}
		_o[b] = '\0';
	}
	if(strDB == "ipi"){
		char *pColon = NULL;
		char cValue = '\0';
		if(!_ipi)	{
			pColon = strstr(_i,"ENSEMBL:");
		}
		if(pColon == NULL)	{
			pColon = strstr(_i,"IPI:");
		}
		if(pColon)	{
			pColon = strchr(pColon,':');
		}
		if(pColon){
			pColon++;
			cValue = pColon[0];
			while(cValue != '\0' && strchr(" |;\r\n",cValue) == NULL){
				_o[b] = pColon[b];
				b++;
				cValue = pColon[b];
			}
		}
		_o[b] = '\0';
	}
	else if(strDB == "plain"){
		char *pColon = NULL;
		char cValue = '\0';
		pColon = strchr(_i,'>');
		if(pColon){
			pColon++;
			cValue = pColon[0];
			while(cValue != '\0' && cValue != ' ' && cValue != '\n'){
				_o[b] = pColon[b];
				b++;
				cValue = pColon[b];
			}
		}
		_o[b] = '\0';
	}
	else if(strDB == "hit"){
		char *pColon = new char[strlen(_i)+1];
		strcpy(pColon,_i+1);
		char *pValue = strstr(pColon,"|HIX");
		char *pOut = new char[strlen(_i)+1];
		if(pValue)	{
			pValue++;
			*pValue = '\0';
			strcpy(pOut,pColon);
			pValue++;
			char *pStart = strchr(pValue,'|');
			if(pStart)	{
				pStart++;
				pValue = strchr(pStart,'|');
				if(pValue)	{
					pValue++;
					*pValue = '\0';
				}
				strcat(pOut,pStart);
				strcpy(_o,pOut);
			}
		}
		delete pOut;
		delete pColon;
	}
	else{
		char *pGT = NULL;
		char cValue = '\0';
		pGT = strchr(_i,'>');
		if(pGT){
			pGT++;
			cValue = pGT[0];
			while(cValue != '\0' && cValue != '|' && cValue != '\n'){
				_o[b] = pGT[b];
				b++;
				cValue = pGT[b];
			}
			_o[b] = pGT[b];
			b++;
			cValue = pGT[b];
			while(cValue != '\0' && cValue != '|' && cValue != '\n'){
				_o[b] = pGT[b];
				b++;
				cValue = pGT[b];
			}
		}
		_o[b] = pGT[b];
		b++;
		_o[b] = '\0';
	}
	return true;
}
