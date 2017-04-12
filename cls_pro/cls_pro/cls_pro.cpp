#include "stdafx.h"
#include <iostream>   
#include <windows.h>   
#include <ctime>
#include <stdio.h>
#include<fstream>
#include<iomanip>
#include<string>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include<bitset>
using namespace std; 

string Data; //read from file
string DataBuffer=""; //add data from each data update
LONGLONG start;

//////////////// information read from input file////////////////////////
string MD1_MTU; 
string MD2_MTU;
string ND1_IF1_IP;   //IP of interface1 of node1
string ND1_IF1_MASK;  //mask of interface1 of node1
string ND1_IF1_ETHER;//ehter  of interface1 of node1
string ND1_IF1_BANDWIDTH;//bandwidth of interface1 of node1
string ND1_IF1_CONNECTION;//connection node of interface1 of node1
string ND2_IF1_IP;
string ND2_IF1_MASK;
string ND2_IF1_ETHER;
string ND2_IF1_BANDWIDTH;
string ND2_IF1_CONNECTION;
string ND2_IF2_IP;
string ND2_IF2_MASK;
string ND2_IF2_ETHER;
string ND2_IF2_BANDWIDTH;
string ND2_IF2_CONNECTION;
string ND3_IF1_IP;
string ND3_IF1_MASK;
string ND3_IF1_ETHER;
string ND3_IF1_BANDWIDTH;
string ND3_IF1_CONNECTION;
//////////////// information read from input file////////////////////////

struct ND_Input //struct node information needed to input to routing table
{
	string name;
	string Network_address;
	string Net_Mask;
}nodein[4];

typedef struct IP_Header    //Identify the data striation of IP header
{
	int Version;
	int HeadLen;
	int ServiceType;
	int TotalLen;
	int Identifier;
	int Flags;
	int FragOffset; 
	int TimeToLive; 
	int Protocol;
	int HeadChecksum;
	string SourceAddr;
	string DestinAddr;
}; 

//***************************************************************************************************//
class Physical  //struct physical layer
{	
public:
	void Run(int, char*);
};
class Data_link  //struct data_link layer
{	
public:
	void Run(int, char*);
	bool isbusy;
};
class Network  //struct network layer
{	
public:
	void Run(int, char*);
};
class Transport  //struct transport layer
{	
public:
	void Run(int, char*);
	void Stop();
	bool confliction;
};
struct NODE  //struct node-information all from input file
{
	int label;
	Physical layer1;
	Data_link layer2;
	Network layer3;
	Transport layer4;
	int connection_num;
}*node;

struct Routing_table_row  //struct routing table
{
	int label;
	string Name;
	string Network_address;
	string Net_Mask;
	string Next_hop_IP;
	int Metric;
};


//HANDLE isbusy;   
LPCWSTR Medium_busy_check; 
bool allset = false;  // All data have been transmitted and recieved
const int MTU = 10;
const char* public_message="";

struct DATA  //struct data-information needed from input file
{
	int label; 
	int Time_stamp_ms;
	string Src_node;
	string Dest_node;
	string message; //MTU

}data_in[3];  //set 3 masks in the input file

class Media  //define media struct
{	
public:
	HANDLE isbusy;
	void Open_connection(int);
	void Transmit(char*);
	void Close_connection(int);
private:
	int ID;
	char Message[MTU];
}Medium;

void Media::Open_connection(int id)  //media function-open define
{
	int ID=id;

}
void Media::Close_connection(int id)  //media function-close define
{
	int ID=id;
}

/////////function (d)/////////
void RT_input(int i, string Next_hop_IP)
{
	int k=i;
	string Next_Hop_IP= Next_hop_IP;
	cout<<k<<"\t"<<nodein[k].name<<"\t\t"<<nodein[k].Network_address<<"\t"<<nodein[k].Net_Mask<<"\t"<<Next_Hop_IP<<"\t"<<"2"<<endl;
}

void Routing_tables_es(NODE Node)
{
	cout<<"Node "<<Node.label <<" routing table:"<<endl;
	cout<<"----------------------------------------------------------------------------------"<<endl;
	cout<<"Label"<<" | "<<"Node_name:"<<" | "<<"Network_address:"<<" | "<<"Net_Mask:"<<" | "<<"Next_hop_IP:"<<" | "<<"Metric:"<<endl;
	cout<<"----------------------------------------------------------------------------------"<<endl;
	string Next_hop_IP;
	switch ( Node.label )
	{
	case 1:
		Next_hop_IP="10.10.20.10";
		RT_input(1,Next_hop_IP);
		RT_input(3,Next_hop_IP);
		cout<<"----------------------------------------------------------------------------------\n\n"<<endl;
		break;
	case 2:
		Next_hop_IP="10.10.10.10";
		RT_input(0,Next_hop_IP);
		Next_hop_IP="10.10.40.10";
		RT_input(3,Next_hop_IP);
		cout<<"----------------------------------------------------------------------------------\n\n"<<endl;
		break;
	case 3:
		Next_hop_IP="10.10.30.10";
		RT_input(0,Next_hop_IP);
		RT_input(2,Next_hop_IP);

		cout<<"----------------------------------------------------------------------------------\n\n"<<endl;
		break;

	}
}


void app(string recmsg){ //user receive message and output it
	cout<<"user receive message"<<'"'<<recmsg<<'"'<<endl;
}
//*************************receive 5 to 2 **********************************//
void rec5(string recmsg,string node){ // send received message to user
	app(recmsg);
}

void rec4(string recmsg,string node){ //send received message to layer5
	rec5(recmsg, node);
}


void rec3(string node,string msg){ //send received message to layer4
	rec4(msg,node);
}

void rec2(string node,string msg ,string i){  // send received and processed message to layer3
	string str;
	stringstream stream;
	stream << msg[1];
	str = stream.str(); // change char into string
	if (node == str)
	{
		msg=msg.erase (0,4); // erase the mac ip address
		msg=msg.erase(msg.length(),1); //erase checksum
		rec3(node,msg);  //keep the messge only
	}else 
		cout<<"\nAt "<<GetTickCount()-start<<" Milliseconds Node "<<i<<" throw the data file to node "<<node<<"\n"<<endl;

}

void Receive(string node,string message, string i)
{
	rec2(node,message,i);
}



char* message_t1;
char* message_t2;
char* message_t3;
//*************************send 2 to 5 **********************************//
string send2(string msg,string src,string des){ //ad mac address to message

	///////////////////////////////////----------------ether checksum calculation and add to message start------------------------------///////////////////////////////////
	string echecksum;
	int ec1=atoi(msg.substr(0,1).c_str());
	int ec2=atoi(msg.substr(1,2).c_str());
	int ec3=atoi(msg.substr(2,3).c_str());
	int ec4=atoi(msg.substr(3,4).c_str());
	int ec5=atoi(msg.substr(4,5).c_str());
	int ec6=atoi(msg.substr(5,6).c_str());
	int ec=ec1+ec2+ec3+ec4+ec5+ec6;
	if (ec%2==0)
		echecksum="0";
	else echecksum="1";
	msg=msg+echecksum;
	///////////////////////////////////----------------ether checksum start calculation and add to message -----------------------------///////////////////////////////////
	msg=src+des+msg;
	return msg;
}

string send3(string msg,string src, string des){ //ad ip adress to message
	msg=src+des+msg;
	return send2(msg,src,des);
}

std::vector<std::string> split(std::string str) //split the message into equal size packet
{   
	std::vector<std::string> result;  // set up vector to store the result
	int size=str.size(); 
	int i;
	for(i=0; i<size; i++)  
	{  
		int MTU=100;
		std::string s=str.substr(i,MTU-4); // The new packet size is set to (MTU minus the length of two IP headers and two MAC headers), and we set the length of two headers are 2 respectively
		result.push_back(s);
		i=i+95; //The position of next i is set to the packet body length minus 1
	}  
	return result;  
}

string send5(string msg, string src, string des){
	std::vector<std::string> result=split(msg);  // receive the message from user and call split funtion 
	for(unsigned int i=0; i<result.size(); i++)  
	{  
		return send3(result[i],src,des);  //output the split processed result with full address
	} 
}


//////////////////////---------------------------file print start--------------------//////////////////////////////////////////
void printfile()
{
	cout<<"Tasks Pool:"<<endl;
	cout<<"----------------------------------------------------------------------------------"<<endl;
	cout<<"TASK_NUM"<<"\t"<<"TIME_STAMP_MS"<<"\t"<<"SRC_NODE"<<"\t"<<"MESSAGE"<<"\t\t"<<"DEST_NODE"<<endl; 
	for(int i=0;i<3;i++)//out put the array of structures 
	{
		cout<<data_in[i].label<<"\t\t"<<data_in[i].Time_stamp_ms<<"\t\t"<<data_in[i].Src_node<<"\t\t"<<data_in[i].message<<"\t\t"<<data_in[i].Dest_node<<endl;
	}
	cout<<"----------------------------------------------------------------------------------"<<endl;
}
//////////////////////---------------------------file print end--------------------//////////////////////////////////////////



long hdchecksumt;
///////////////////////////////////----------------ipHeader define&print start------------------------------///////////////////////////////////
void hdprint(DATA data){
	IP_Header ip;
	int srcnode=atoi(data.Src_node.c_str());  //read information from thread 
	int desnode=atoi(data.Dest_node.c_str());
	ip.Version=3; //4 digits
	ip.HeadLen=3; //4 digits
	ip.ServiceType=4; //8 digits
	ip.TotalLen=5; //16 digits
	ip.Identifier=3; //16 digits
	ip.Flags=2; //4 digits
	ip.FragOffset=5; //12 digits
	ip.TimeToLive=2; //8 digits
	ip.Protocol=3; //8 digits
	ip.HeadChecksum=0;
	ip.SourceAddr=nodein[srcnode].Network_address;
	ip.DestinAddr=nodein[desnode].Network_address;  

	bitset<4> Version2(3);  //convert normal information to binary to print
	bitset<4> HeadLen2(3);
	bitset<8> ServiceType2(3);
	bitset<16> TotalLen2(3);
	bitset<16> Identifier2(3);
	bitset<4> Flags2(3);
	bitset<12> FragOffset2(3);
	bitset<8> TimeToLive2(3);
	bitset<8> Protocol2(3);

	string pp1 = ip.SourceAddr; //convert ip source address to binary coding
	string ppp1[4];
	for (int j=0;j<4;j++){
		ppp1[j]=pp1.substr(j*3,2);
	}
	int ips[4]={0,0,0,0};
	for(int i=0;i<4;i++){
		ips[i]=atoi(ppp1[i].c_str());
	}
	bitset<8> bss1=ips[0];
	bitset<8> bss2=ips[1];
	bitset<8> bss3=ips[2];
	bitset<8> bss4=ips[3];

	string pp2 = ip.DestinAddr;//convert ip destination address to binary coding
	string ppp2[4];
	for (int j=0;j<4;j++){
		ppp2[j]=pp2.substr(j*3,2);
	}
	int ipd[4]={0,0,0,0};
	for(int i=0;i<4;i++){
		ipd[i]=atoi(ppp2[i].c_str());
	}
	bitset<8> bsd1=ipd[0];
	bitset<8> bsd2=ipd[1];
	bitset<8> bsd3=ipd[2];
	bitset<8> bsd4=ipd[3];

	////////////////////////////-------------calculate IP_Header Checksum START----------------------------------////////////////////////////

	bitset<16> a=((ip.Version<<12)|(ip.HeadLen<<8)|ip.ServiceType);  //connect header coding in binary to 16-digits
	bitset<16> b=TotalLen2;
	bitset<16> c=Identifier2;
	bitset<16> d=((ip.Flags<<12)|ip.FragOffset);
	bitset<16> e=((ip.TimeToLive<<8)|ip.Protocol);
	bitset<16> f=((ips[0]<<8)|ips[1]);
	bitset<16> g=((ips[2]<<8)|ips[3]);
	bitset<16> h=((ipd[0]<<8)|ipd[1]);
	bitset<16> i=((ipd[2]<<8)|ipd[3]);

	hdchecksumt=a.to_ulong()+b.to_ulong()+c.to_ulong()+d.to_ulong()+e.to_ulong()+f.to_ulong()+g.to_ulong()+h.to_ulong()+i.to_ulong();
	bitset<16> hdchecksum(hdchecksumt);
	////////////////////////////-------------calculate IP_Header Checksum END----------------------------------///////////////////////////

	cout<<"Version:"<<Version2<<endl;  //print header
	cout<<"HeadLen:"<<HeadLen2<<endl;
	cout<<"ServiceType:"<<ServiceType2 <<endl;
	cout<<"TotalLen:"<<TotalLen2 <<endl;
	cout<<"Identifier:"<<Identifier2 <<endl;
	cout<<"Flags:"<<Flags2 <<endl;
	cout<<"FragOffset:"<<FragOffset2<<endl;
	cout<<"TimeToLive:"<<TimeToLive2<<endl;
	cout<<"Protocol:"<<Protocol2 <<endl;
	cout<<"Headerchecksum:"<<hdchecksum<<endl;
	cout<<"SourceAddr:" <<ip.SourceAddr <<endl;
	cout<<"DestinAddress:"<<ip.DestinAddr<<endl;

}
///////////////////////////////////----------------ipHeader define&print END------------------------------///////////////////////////////////

string msg2to11;
string msg2to12;
string msg2to13;
int Tasknum=4;
//******************************thread**********************************//
DWORD WINAPI Node1(LPVOID lpParamter)   
{
	msg2to11=data_in[0].Src_node+data_in[0].Dest_node+data_in[0].Src_node+data_in[0].Dest_node+data_in[0].message;//for receive sample test
	msg2to12=data_in[1].Src_node+data_in[1].Dest_node+data_in[1].Src_node+data_in[1].Dest_node+data_in[1].message;//for receive sample test
	msg2to13=data_in[2].Src_node+data_in[2].Dest_node+data_in[2].Src_node+data_in[2].Dest_node+data_in[2].message;//for receive sample test

	while(Tasknum>1)
	{  
		if (WaitForSingleObject(Medium.isbusy,0)==WAIT_OBJECT_0)  //Medium is not busy Node 1 take it!
		{ 
			if(Tasknum<=0)
			{
				cin.get();
				exit(0);
			}
			cout<<"\nAt "<<GetTickCount()-start<<" Milliseconds Node 1 occupies medium now!>>>>>\n"<<endl; 

			//*************Task begin at here!****************//
			string str;
			char *cstr = new char[msg2to11.length() + 1];
			stringstream stream;
			stream << msg2to11[1];
			str = stream.str(); // change char into string
			if ("1" == str)
			{
				strcpy(message_t1, msg2to11.c_str());
			};
			Medium.Open_connection(1);  // Node 1 begin to transmit
			hdprint(data_in[0]);
			msg2to11=send5(data_in[0].message,data_in[0].Src_node,data_in[0].Dest_node); // user calls layer5 and send the message layer by layer
			cout<<"user sends message with NODEIP and ETHER CHECKSUM:"<<msg2to11<<endl;
			for (int i=0;i<3;i++){
				Receive(data_in[i].Dest_node,msg2to13,"1");
			}
			Medium.Close_connection(1);  // Task over
			Tasknum=Tasknum-1;
			ReleaseMutex(Medium.isbusy);  
			Sleep(3000);
		}
		else
		{ 
			cout<<"\nAt "<<GetTickCount()-start<<" Milliseconds Node 1 attempt has failed!\n"<<endl;
			Sleep(113);  // Time interval to check the medium

		}
	} 
	return 0;
}   

DWORD WINAPI Node2(LPVOID lpParamter)   
{
	msg2to11=data_in[0].Src_node+data_in[0].Dest_node+data_in[0].Src_node+data_in[0].Dest_node+data_in[0].message;  //for receive sample test
	msg2to12=data_in[1].Src_node+data_in[1].Dest_node+data_in[1].Src_node+data_in[1].Dest_node+data_in[1].message;//for receive sample test
	msg2to13=data_in[2].Src_node+data_in[2].Dest_node+data_in[2].Src_node+data_in[2].Dest_node+data_in[2].message;//for receive sample test

	while(Tasknum>1)
	{  
		if (WaitForSingleObject(Medium.isbusy,0)==WAIT_OBJECT_0)  //Medium is not busy Node 1 take it!
		{ 
			if(Tasknum<=0)
			{
				cin.get();
				exit(0);
			}
			cout<<"\nAt "<<GetTickCount()-start<<" Milliseconds Node 2 occupies medium now!>>>>>\n"<<endl;

			//*************Task begin at here!****************//
			string str;
			char *cstr = new char[msg2to11.length() + 1];
			stringstream stream;
			stream << msg2to12[1];
			str = stream.str(); // change char into string
			//str="2";
			if ("2" == str)
			{
				strcpy(message_t2, msg2to12.c_str());
			};
			Medium.Open_connection(2);  // Node 2 begin to transmit

			hdprint(data_in[1]);
			msg2to12=send5(data_in[1].message,data_in[1].Src_node,data_in[1].Dest_node); // user calls layer5 and send the message layer by layer
			cout<<"user sends message with NODEIP and ETHER CHECKSUM:"<<msg2to12<<endl;
			for (int i=0;i<3;i++){
				Receive(data_in[i].Dest_node,msg2to11,"2");
			}
			Medium.Close_connection(2);  // Task over
			Tasknum=Tasknum-1;
			ReleaseMutex(Medium.isbusy);  
			Sleep(3000);
		}
		else
		{ 
			cout<<"\nAt "<<GetTickCount()-start<<" Milliseconds Node 2 attempt has failed!\n"<<endl;
			Sleep(111);  // Time interval to check the medium
		}
	} 
	return 0;
} 

DWORD WINAPI Node3(LPVOID lpParamter)   
{
	msg2to11=data_in[0].Src_node+data_in[0].Dest_node+data_in[0].Src_node+data_in[0].Dest_node+data_in[0].message;//for receive sample test
	msg2to12=data_in[1].Src_node+data_in[1].Dest_node+data_in[1].Src_node+data_in[1].Dest_node+data_in[1].message;//for receive sample test
	msg2to13=data_in[2].Src_node+data_in[2].Dest_node+data_in[2].Src_node+data_in[2].Dest_node+data_in[2].message;//for receive sample test

	while(Tasknum>1)
	{  
		if (WaitForSingleObject(Medium.isbusy,0)==WAIT_OBJECT_0)  //Medium is not busy Node 1 take it!
		{ 
			if(Tasknum<=0)
			{
				cin.get();
				exit(0);
			}
			hdprint(data_in[2]);
			cout<<"\nAt "<<GetTickCount()-start<<" Milliseconds Node 3 occupies medium now!>>>>>\n"<<endl; 

			//*************Task begin at here!****************//
			string str;
			char *cstr = new char[msg2to13.length() + 1];
			stringstream stream;
			stream << msg2to13[1];
			str = stream.str(); // change char into string
			//str="3";
			if ("3" == str)
			{
				strcpy(message_t3, msg2to13.c_str());
			};
			Medium.Open_connection(3);  // Node 1 begin to transmit

			hdprint(data_in[2]);
			msg2to13=send5(data_in[2].message,data_in[2].Src_node,data_in[2].Dest_node); // user calls layer5 and send the message layer by layer
			cout<<"user sends message with NODEIP and ETHER CHECKSUM:"<<msg2to13<<endl;
			for (int i=0;i<3;i++){
				Receive(data_in[i].Dest_node,msg2to12,"3");
			}
			Medium.Close_connection(3);  // Task over
			Tasknum=Tasknum-1;
			ReleaseMutex(Medium.isbusy);  
			Sleep(3000);
		}
		else
		{ 
			cout<<"\nAt "<<GetTickCount()-start<<" Milliseconds Node 3 attempt has failed!\n"<<endl;
			Sleep(123);  // Time interval to check the medium
		}
	} 
	return 0;
}  

int _tmain(int argc, _TCHAR* argv[])  
{   

	///////////////////////////////------------------read file start------------------//////////////////////////////////////////////
	ifstream fin("inputfile.txt"); //read file to stream
	if( ! fin )  //in case of file not existing
	{
		cerr<<"Can not open datain1 file."<<endl;
		cin.get();
		return 1;
	};
	while( ! fin.eof() ) //read data from file
	{

		fin>>Data;
		DataBuffer=DataBuffer+Data;
	};
	///////////////////////////////------------------read file end------------------//////////////////////////////////////////////


	///////////////////////////////------------------read medias MTU from file begin------------------//////////////////////////////////////////////
	int pos_MD1_MTU = DataBuffer.find("MD1_MTU", 0);// find the position of MD1_MTU
	MD1_MTU = DataBuffer.substr(pos_MD1_MTU+sizeof("MD1_MTU"), 4); //fetch MD1_MTU Size

	int pos_MD2_MTU = DataBuffer.find("MD2_MTU", 0);// find the position of MD2_MTU
	MD2_MTU = DataBuffer.substr(pos_MD2_MTU+sizeof("MD2_MTU"), 4); //fetch MD2_MTU Size
	///////////////////////////////------------------read medias MTU from file end------------------//////////////////////////////////////////////



	///////////////////////////////------------------read informations about nodes from file begin------------------//////////////////////////////////////////////
	int pos_ND1_IF1_IP = DataBuffer.find("ND1_IF1_IP", 0);// find the position of IP
	int pos_ND2_IF1_IP = DataBuffer.find("ND2_IF1_IP", 0);
	int pos_ND2_IF2_IP = DataBuffer.find("ND2_IF2_IP", 0); 
	int pos_ND3_IF1_IP = DataBuffer.find("ND3_IF1_IP", 0);
	ND1_IF1_IP = DataBuffer.substr(pos_ND1_IF1_IP+sizeof("ND1_IF1_IP"), 11); //fetch IP Address
	ND2_IF1_IP = DataBuffer.substr(pos_ND2_IF1_IP+sizeof("ND2_IF1_IP"), 11);
	ND2_IF2_IP = DataBuffer.substr(pos_ND2_IF2_IP+sizeof("ND2_IF2_IP"), 11);
	ND3_IF1_IP = DataBuffer.substr(pos_ND3_IF1_IP+sizeof("ND3_IF1_IP"), 11);

	int pos_ND1_IF1_MASK = DataBuffer.find("ND1_IF1_MASK", 0);// find the position of MASK
	int pos_ND2_IF1_MASK = DataBuffer.find("ND2_IF1_MASK", 0);
	int pos_ND2_IF2_MASK = DataBuffer.find("ND2_IF2_MASK", 0); 
	int pos_ND3_IF1_MASK = DataBuffer.find("ND3_IF1_MASK", 0);
	ND1_IF1_MASK = DataBuffer.substr(pos_ND1_IF1_MASK+sizeof("ND1_IF1_MASK"), 13); //fetch MASK Address
	ND2_IF1_MASK = DataBuffer.substr(pos_ND2_IF1_MASK+sizeof("ND2_IF1_MASK"), 13);
	ND2_IF2_MASK = DataBuffer.substr(pos_ND2_IF2_MASK+sizeof("ND2_IF2_MASK"), 13); 
	ND3_IF1_MASK = DataBuffer.substr(pos_ND3_IF1_MASK+sizeof("ND3_IF1_MASK"), 13);


	int pos_ND1_IF1_ETHER = DataBuffer.find("ND1_IF1_ETHER", 0);// find the position of ETHER
	int pos_ND2_IF1_ETHER = DataBuffer.find("ND2_IF1_ETHER", 0);
	int pos_ND2_IF2_ETHER = DataBuffer.find("ND2_IF2_ETHER", 0);
	int pos_ND3_IF1_ETHER = DataBuffer.find("ND3_IF1_ETHER", 0);
	ND1_IF1_ETHER = DataBuffer.substr(pos_ND1_IF1_ETHER+sizeof("ND1_IF1_ETHER"), 10); //fetch ETHER Address
	ND2_IF1_ETHER = DataBuffer.substr(pos_ND2_IF1_ETHER+sizeof("ND2_IF1_ETHER"), 10);
	ND2_IF2_ETHER = DataBuffer.substr(pos_ND2_IF2_ETHER+sizeof("ND2_IF2_ETHER"), 10);
	ND3_IF1_ETHER = DataBuffer.substr(pos_ND3_IF1_ETHER+sizeof("ND3_IF1_ETHER"), 10);

	int pos_ND1_IF1_BANDWIDTH = DataBuffer.find("ND1_IF1_BANDWIDTH", 0);// find the position of BANDWIDTH
	int pos_ND2_IF1_BANDWIDTH = DataBuffer.find("ND2_IF1_BANDWIDTH", 0);
	int pos_ND2_IF2_BANDWIDTH = DataBuffer.find("ND2_IF2_BANDWIDTH", 0);
	int pos_ND3_IF1_BANDWIDTH = DataBuffer.find("ND3_IF1_BANDWIDTH", 0);
	ND1_IF1_BANDWIDTH = DataBuffer.substr(pos_ND1_IF1_BANDWIDTH+sizeof("ND1_IF1_BANDWIDTH"), 2); //fetch BANDWIDTH Address
	ND2_IF1_BANDWIDTH = DataBuffer.substr(pos_ND2_IF1_BANDWIDTH+sizeof("ND2_IF1_BANDWIDTH"), 2);
	ND2_IF2_BANDWIDTH = DataBuffer.substr(pos_ND2_IF2_BANDWIDTH+sizeof("ND2_IF2_BANDWIDTH"), 2);
	ND3_IF1_BANDWIDTH = DataBuffer.substr(pos_ND3_IF1_BANDWIDTH+sizeof("ND3_IF1_BANDWIDTH"), 2);

	int pos_ND1_IF1_CONNECTION = DataBuffer.find("ND1_IF1_CONNECTION", 0);// find the position of CONNECTION
	int pos_ND2_IF1_CONNECTION = DataBuffer.find("ND2_IF1_CONNECTION", 0);
	int pos_ND2_IF2_CONNECTION = DataBuffer.find("ND2_IF2_CONNECTION", 0);
	int pos_ND3_IF1_CONNECTION = DataBuffer.find("ND3_IF1_CONNECTION", 0);
	ND1_IF1_CONNECTION = DataBuffer.substr(pos_ND1_IF1_CONNECTION+sizeof("ND1_IF1_CONNECTION")+sizeof("MD"), 1); //fetch CONNECTION Address
	ND2_IF1_CONNECTION = DataBuffer.substr(pos_ND2_IF1_CONNECTION+sizeof("ND2_IF1_CONNECTION")+sizeof("MD"), 1);
	ND2_IF2_CONNECTION = DataBuffer.substr(pos_ND2_IF2_CONNECTION+sizeof("ND2_IF2_CONNECTION")+sizeof("MD"), 1);
	ND3_IF1_CONNECTION = DataBuffer.substr(pos_ND3_IF1_CONNECTION+sizeof("ND3_IF1_CONNECTION")+sizeof("MD"), 1);
	///////////////////////////////------------------read informations about nodes from file end ------------------//////////////////////////////////////////////


	///////////////////////////////------------------struct node array begin------------------//////////////////////////////////////////////
	nodein[0].name="NODE1"; //initial node1 according to inputfile
	nodein[0].Network_address=ND1_IF1_IP;
	nodein[0].Net_Mask=ND1_IF1_MASK;

	nodein[1].name="NODE2";//initial node2 according to inputfile
	nodein[1].Network_address=ND2_IF1_IP;
	nodein[1].Net_Mask=ND2_IF1_MASK;

	nodein[2].name="NODE2";//initial node2 according to inputfile
	nodein[2].Network_address=ND2_IF2_IP;
	nodein[2].Net_Mask=ND2_IF2_MASK;

	nodein[3].name="NODE3";//initial node3 according to inputfile
	nodein[3].Network_address=ND3_IF1_IP;
	nodein[3].Net_Mask=ND3_IF1_MASK;
	///////////////////////////////------------------struct node array end------------------//////////////////////////////////////////////


	///////////////////////////////------------------read tasks from file begin------------------//////////////////////////////////////////////
	int pos_FILE_MESSAGES = DataBuffer.find("FILE_MESSAGES", 0);// find the position of FILE_SIZE (KB)
	for (int i=0;i<3;i++){
		data_in[i].label=i;
		data_in[i].Time_stamp_ms=atoi(DataBuffer.substr(pos_FILE_MESSAGES+sizeof("FILE_MESSAGES")+i*16,3).c_str());
		data_in[i].Src_node=DataBuffer.substr(pos_FILE_MESSAGES+sizeof("FILE_MESSAGES")+5+i*16,1);
		data_in[i].Dest_node=DataBuffer.substr(pos_FILE_MESSAGES+sizeof("FILE_MESSAGES")+8+i*16,1);
		data_in[i].message=DataBuffer.substr(pos_FILE_MESSAGES+sizeof("FILE_MESSAGES")+9+i*16,6);
	}
	///////////////////////////////------------------read tasks from file end------------------//////////////////////////////////////////////

	/////////////////***********************************////////////////////////////
	NODE *node=new NODE[3]; 
	for (int i=0;i<3;i++)
	{
		node[i].label=i+1;
		node[i].connection_num=2;   //Need value in
		Routing_tables_es(node[i]);
	}	
	/////////////////***************************************///////////////////////
	printfile();
	start=GetTickCount();
	HANDLE Node_1 = CreateThread(NULL, 0, Node1, NULL, 0, NULL);   
	HANDLE Node_2 = CreateThread(NULL, 0, Node2, NULL, 0, NULL);
	HANDLE Node_3 = CreateThread(NULL, 0, Node3, NULL, 0, NULL);
	Medium.isbusy = CreateMutex(NULL, FALSE, Medium_busy_check); //equivalent to de-confliction (medium access control) based on the ¡°busy¡± signal from the medium.
	CloseHandle(Node_1);  
	CloseHandle(Node_2);  
	CloseHandle(Node_3);   

	cin.get();
	return 0;   
}    