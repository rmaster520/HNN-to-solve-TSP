#include <math.h>
#include <ctype.h>
#include <ctime>
#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<cassert>
#include "CityInfo.h"

using namespace std;

#define G(x) ((1.0+tanh(x/u0))/2.0)  //��ڵ㺯��
#define pi 3.1415926
double u0=0.02;
/*
* �������ܣ����еĳ�ʼ�����ã�
			resize(Num)��������ĳ��������ó������飬
			SetCityIndex(i)��������ĳ�����е�����ID��
			SetCoordx���ȡֵ������ĳ������X-Y���ϵ�X����
* ���������Num,���ó��н��ĸ���
* ����ֵ ������һ����ʼ���õĳ�������
*/


//ͨ����������õ�(0,1)֮��ĳ�������
vector<CityInfo>CreateCities(int Num){
	vector<CityInfo> City;
	City.resize(Num);//���ó��еĹ�ģ
	for(int i=0;i<Num;i++){
		City[i].SetCityIndex(i);//���ó���������Ҳ���ǵ�i������
		double t=((rand())%32767)/(float)32767;//�õ�(0,1)֮���һ����
		City[i].SetCoordx(t);//���ó��е�x����
		t=((rand())%32767)/(float)32767;
		City[i].SetCoordy(t);//���ó��е�y����
	}
	return City;
}
/*
* �������ܣ�����ƫ��bias
			
* ���������City,��������
* ����ֵ ������һ��ƫ������
*/
/*
  ���ó��е�ƫ�ñ���
  ��������Ŷ��ķ������ó�ʼ״̬���Ի����ȷ�⣬
  ������Ŷ�ȱ��������������ԣ�������׻�ý�
  �õĽ⣬��������Ч��Ҳ���ߡ��Ľ��ķ����ǲ���
  ����ʼ״̬����һ��ƫ��,ƫ�ù�ʽΪbias(i,j)=cos(atan((y-0.5)/(x-0.5))+2*pi*(j-1)*sqrt((x-0.5)*(x-0.5)+(y-0.5)*(y-0.5))/N)
*/

vector<double> SetBias(vector<CityInfo>City){
	vector<double> Bias;
	double d1=0.0,d2=0.0,d3=0.0,d4=0.0;
	//�������г���
	for(int i=0;i<City.size();i++){
		d1=(City[i].GetCoordy()-0.5)/(City[i].GetCoordx()-0.5);
		d2=atan(d1); //�����к���
		d3=hypot(City[i].GetCoordx()-0.5,City[i].GetCoordy()-0.5);//ƽ����
		for(int j=0;j<City.size();j++){
			d4=d2+(j-1)*2*pi*d3/(float)City.size();//d4����ƫ��ֵ
			Bias.push_back(cos(d4));//��ƫ�������в���˴μ��������ƫ��ֵ
		}
	}
	return Bias;//����ƫ������
}
/*
* �������ܣ���������E������J1,J2,J3,J��E���ǰ��ն�Ӧ�Ĺ�ʽE=A*J1/2+B*J2/2+C*J3/2+D*J/2���������
			
* ���������city�������飬u,v�û�������Ԫ״̬�� ABCDΪ������ʽ����ϵ��
* ����ֵ ����������E
*/
double CompEngery(vector<CityInfo> city,vector<double> & u,vector<double> & v,double A,double B,double C,double D){
	int i,j,x,y;  
	double delt=0,E=0,k=0,h=0.01;
	double J1=0.0,J2=0.0,J3=0.0,J=0.0;
	int Num=city.size();//��ó��е���Ŀ
	///����J1,Ҳ��Լ�����������ڻ�λ�����У�ÿһ������x ���ຬ��һ����1�������඼�ǡ�0����
	for(x=0;x<Num;x++)   
		for(i=0;i<Num-1;i++)
			//j��i+1��ʼ��Ϊ�˱���j=i�����
			for(j=i+1;j<Num;j++)
				J1+=v[x*Num+i]*v[x*Num+j];//J1+=v[x][i]*v[x][j]
    //����J2,Ҳ��Լ�������������û������У�ÿһ������y ���ຬ��һ����1�������඼�ǡ�0����
	for(i=0;i<Num;i++)
		for(x=0;x<Num-1;x++)
			//y��x+1��ʼ��Ϊ�˱���y=x�����
			for(y=x+1;y<Num;y++)
				J2+=v[x*Num+i]*v[y*Num+i];//j2+=v[x][i]*v[y][i]
    //����J3,����K �Ǽ����û�������ܺͣ�J3Ҳ��Լ�������������û�������,ֻ����N��1�����һ��ƽ������Ϊ�˷�ֹ���ָ���
	for(x=0;x<Num;x++)
		for(i=0;i<Num;i++)      
			k+=v[x*Num+i];    //��������ϵ��,k+=v[x][i]
	J3=(k-Num)*(k-Num);
	/*
	����J,��������·�ߵ�·��
	J=min(sum(d[x][y]*v[x][i]*(v[y][i+1]+v[y][i-1]))),y!=x;
	v[x][i]�����±�x�ǳ��б�ţ����±�i��ʾ����x������˳���е�λ�ã��±��Nȡģ����
	*/
	for(x=0;x<Num;x++){
		for(y=0;y<Num;y++){    
			for(i=0;i<Num;i++){  
				if(i==0)  
					//�±��Nȡģ����,����i-1<0,��i��0��ʼȡֵ,����ȡģ��iΪNum-1
					J+=city[x].GetCityDis(city[y])*v[x*Num+i]*(v[y*Num+Num-1]+v[y*Num+i+1]); //J+=dis[x][y]*v[x][i]*(v[y][N-1]+v[y][i+1]
				else if (i==Num-1)   
					J+=city[x].GetCityDis(city[y])*v[x*Num+i]*(v[y*Num+i-1]+v[y*Num]);//J+=dis[x][y]*v[x][i]*(v[y][i-1]+v[y][0])
				else             
					J+=city[x].GetCityDis(city[y])*v[x*Num+i]*(v[y*Num+i-1]+v[y*Num+i+1]);  
			}        
		}    
	}      
	//�õ���������
	E=A*J1/2+B*J2/2+C*J3/2+D*J/2;

	/*
		ȡ��Ԫ��I/O����ΪS�ͺ������������TSP��������緽��
		delt=-u[x][i]-A*Sum(v[x][j])-B*Sum(v[y][j]-C*(Sum(v[x][i])-N)-D*Sum(d[x][y])(v[y][i+1]+v[y][i-1]));
		u[x*Num+i]=h*delt;
		v[x][i]ͨ��G(u[u][i])���
	*/
	for(x=0;x<Num;x++){
		for(i=0;i<Num;i++){
			delt=0-u[x*Num+i];//u[x][i]

			for(j=0;j<Num;j++){
				if(i==j)
					continue;
				delt-=A*v[x*Num+j]; //v[x][j]
			}

			for(y=0;y<Num;y++){
				if(x==y) 
					continue;  
				delt-=B*v[y*Num+i];   //v[y][i]
			} 

			delt-=C*(k-Num);//k=Sum(v[x][i])
			//i���Nȡģ
			for(y=0;y<Num;y++){
				if(i==0)     
					delt-=D*city[x].GetCityDis(city[y])*(v[y*Num+Num-1]+v[y*Num+i+1]);   
				else if (i==Num-1)   
					delt-=D*city[x].GetCityDis(city[y])*(v[y*Num+i-1]+v[y*Num]);    
				else             
					delt-=D*city[x].GetCityDis(city[y])*(v[y*Num+i-1]+v[y*Num+i+1]);
			}
			u[x*Num+i]+=h*delt;//��Сϵ������
			v[x*Num+i]=G(u[x*Num+i]); //v[x][i]=G(u[x][i])
		}
	}
	return E;
}
/*
* �������ܣ���֤·�������У�·���Ƿ������ȷ
			
* ���������Router·�����飬����һά�ģ��������������η��ʵĳ��к�
* ����ֵ ������·���Ƿ���Ч����Ϣ
*/
bool ReVaild(vector<int> Router){
	for(int i=0;i<Router.size();i++){
		for(int j=i+1;j<Router.size();j++){			
			if(Router[i]==Router[j] || Router[i]<0 || Router[i]>Router.size()-1)
				return false;			
		}
	}
	return true;
}
/*
* �������ܣ���������������Ϣ����������е�������X-Y����			
* ���������city��������
*/
void PrintCity(vector<CityInfo> City){
	for(int i=0;i<City.size();i++)
		cout<<City[i].GetCityIndex()<<"	"<<City[i].GetCoordx()<<"	"<<City[i].GetCoordy()<<endl;
}

void HOP_TSP(int Num)
{
	ofstream outfile;//�������ʽ���ļ� 
	string name;
	vector<CityInfo>City;//��������
    vector<int>Router;//·������
	vector<double>InitBias;//��ʼƫ������
	vector<double>InSig,OutSig;
	int CityNum=Num;
	double TotalDis=0.0,E=0.0,u0=0.02;
double MinTotalDis;
vector<int>MinRouter;//��С��·������
	City=CreateCities(CityNum);//����������Ŀ
	
	InitBias=SetBias(City);//����ƫ�ñ���
	Router.resize(CityNum);//·���������ó��й�ģ

	//InSig��������������������еĻ�λ����U������OutSig�������������еĻ�λ����V����
	InSig.resize(CityNum*CityNum);//����һ��CityNum*CityNum���������
	OutSig.resize(CityNum*CityNum);
	
	name="result.txt";
	outfile.open(name.c_str(),ofstream::app);//name.c_str()����const char*����(�ɶ����ɸ�)��ָ���ַ������ָ��
	if (!outfile){
		cout<<"Output file "<<name<<" opening failed!"<<endl;
		exit(1);
	}
	else{
		outfile<<CityNum<<endl;//�����е���ĿCityNum���뵽result.txt��ȥ
		outfile<<CityNum<<"����������"<<endl;
		for(int i=0;i<City.size();i++)
			outfile<<"��"<<City[i].GetCityIndex()<<"����"<<"  "<<City[i].GetCoordx()<<"	"<<City[i].GetCoordy()<<endl;//�����е�(x,y)������Ϣ���뵽�ļ���ȥ
		outfile<<"**********************************************"<<endl;
		//ͨ��forѭ���ı�A,B��C�Ĳ���ֵ
		for(double dd=0.05;dd<0.31;dd+=0.05){
			MinTotalDis=20.0;///++

			cout<<"--------------------------"<<"A=B=C="<<dd<<"	"<<"D=0.1"<<"-------------------------"<<endl;
			outfile<<"--------------------------"<<"A=B=C="<<dd<<"	"<<"D=0.1"<<"-------------------------"<<endl;
			for(int it=0;it<20;it++){
				int ItNum=0;
				
				double u00=0-u0*log(CityNum-1)/2;
				for(i=0;i<CityNum*CityNum;i++){  
					double t=((rand())%32767)/(float)32767;
					//���ʼ��λ����
					InSig[i]=u00+0.001*(t*2-1)*InitBias[i];
					OutSig[i]=G(InSig[i]); //G()�Ƿ�����
				}
				double temp=0;
				assert(InSig.size()==CityNum*CityNum);//����,�쳣�����һ�ָ߼���ʽ,��ʾ�������ʽ������Ϊ��
				assert(OutSig.size()==CityNum*CityNum);
				
				do{
					E=CompEngery(City,InSig,OutSig,dd,dd,dd,0.1);//����������
				if(fabs(E-temp)<1e-20)
					break;
					temp=E;
					ItNum++;
				}while(ItNum<9000);
				cout<<"iterations="<<ItNum<<"	"<<"E="<<E<<endl;//���ص����Ĵ���������ֵ
			
				int i,j,k=0;
				int count=0;
				
				TotalDis=0.0;//TSP�ܾ���
				//ͨ��forѭ���õ���λ���������г��ֵĳ��еı�ţ�����һ�����еı�ţ�������Router[0]��
				for(j=0;j<CityNum;j++){
					if(OutSig[j*CityNum]>=0.2){
						k=j;
						Router[count]=j;//Router���ڱ���˳�����г��еı��
						count+=1;
						break;
					}
				}
				//ͨ��forѭ���õ���λ���������г��ֵĳ��б�ţ�������Router[i]�У�ͬʱ�õ����е������ܾ��룬������TotalDis��
				for(i=1;i<CityNum;i++){
					for(j=0;j<CityNum;j++){
						if(OutSig[j*CityNum+i]>=0.2){//���û������У�����0.2��״̬����Ϊ����ȷ��·�����
							Router[count]=j;
							count+=1;
							//::k��ʾ�û�������ĳһ������ȷ·�������кţ�jΪK������������ȷ·�������кţ��������ڵ��кţ�����ȷ�������ľ���
							TotalDis+=City[k].GetCityDis(City[j]); 	         
							k=j;  
							break;    
						}
					}
				}
				TotalDis+=City[k].GetCityDis(City[Router[0]]);

				if(ReVaild(Router)){
					if(TotalDis<MinTotalDis)////++
					{
						MinTotalDis = TotalDis;
						MinRouter = Router;
					}

					cout<<"right path"<<endl; //���·����Ч�Ļ�
					cout<<"TotaoDis="<<TotalDis<<endl; 
					for(int i=0;i<CityNum;i++)
						cout<<Router[i]<<"	";
						//���뵽�ļ���
						outfile<<endl;
						outfile<<"iterations="<<ItNum<<"	"<<"E="<<E<<endl;//������������������������ļ���
						
						outfile<<it+1<<"	";//������Ĵ������б��
						outfile<<"right path"<<endl; //��ʾ·����Ч
						outfile<<"TotaoDis="<<TotalDis<<endl;//����ܾ���
						
						//ͨ��forѭ�����˳�����г��еı��
						for(i=0;i<CityNum;i++)
							outfile<<Router[i]<<"	";
						outfile<<endl;
					}
				else{
					cout<<"wrong path"<<endl;//��ʾ·���Ƿ�
				}

			}
			if(MinTotalDis!=20.0)
			{
				cout<<endl;
				outfile<<"���·�����ȣ�"<<MinTotalDis;
				outfile<<"	��С·��Ϊ��";
				cout<<"���·�����ȣ�"<<MinTotalDis;
				cout<<"	��С·��Ϊ��";
				for(int ki=0;ki<CityNum;ki++)
				{
					cout<<MinRouter[ki]<<"  ";
					outfile<<MinRouter[ki]<<"  ";
				}
				cout<<endl;
			}
		}
	}
	outfile<<"**********************************************"<<endl;
	outfile<<endl<<endl<<endl;
	outfile.close();
}

void main(){
	clock_t begin,finish;
	srand((unsigned int) time(NULL));//���ڼ�������ʱ��
	begin=clock();//��ȡϵͳ�ĳ�ʼʱ��
	int Num;//���ڱ��������Ŀ
	cout<<"CityNum:	";
	cin>>Num;
	HOP_TSP(Num);//����Hopfild-Tsp�������
	finish = clock();//����ʱ��
}