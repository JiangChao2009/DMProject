#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
#include "Data.h"
#include "Kmeans.h"
#include "KNN.h"

VECDATA g_dataset;			//���ݼ�
VECDATA g_trainingSet;		//ѵ����
VECDATA g_testSet;			//���Լ�

class CInitDataset
{
public:
	static bool LoadFile(const char *fileName);
	static void InitTrainingSet(double ratio, int method);
};

int main()
{
	cout<<"**** DM Project by ֣Խ S11102035 ****"<<endl;
	//��ȡ�ļ�
	if(!CInitDataset::LoadFile("iris.txt"))
	{
		return -1;
	}
	while(1)
	{
		cout<<"**************************************"<<endl;
		cout<<"����������ѡ��"<<endl;
		cout<<"1. ����(KNN)(Ĭ��)"<<endl;
		cout<<"2. ����(K-means)"<<endl;
		cout<<"0. �˳�"<<endl;
		int menu = 1;
		cin.unsetf(ios::skipws);
		cin>>menu;
		cin.clear();	cin.sync();
		if(menu == 1)
		{
			cout<<"����������ѵ��������(Ĭ��0.5)��";
			double trainRatio = 0.5;
			cin>>trainRatio;
			cin.clear();	cin.sync();
			if(trainRatio>1 || trainRatio<0)
			{
				cout<<"�������󣬽�ʹ��Ĭ�ϱ���0.5��"<<endl;
				trainRatio = 0.5;
			}
			cout<<endl;
			int tr = (int)(100 * trainRatio);
			cout<<"��ѡ��ѵ�������ɷ�ʽ(Ĭ��ǰ"<<tr<<"%)��"<<endl;
			cout<<"1. ������ǰ"<<tr<<"%"<<endl;
			cout<<"2. �����ݺ�"<<tr<<"%"<<endl;
			cout<<"3. �������Լ"<<tr<<"%"<<endl;
			int initMethod = 1;
			cin>>initMethod;
			cin.clear();	cin.sync();
			if(initMethod!=1 && initMethod!=2 && initMethod!=3)
			{
				cout<<"�������󣬽�Ĭ��ʹ�������ݵ�ǰ"<<tr<<"%��"<<endl<<endl;;
				initMethod = 1;
			}
			CInitDataset::InitTrainingSet(trainRatio, initMethod);
			cin.clear();	cin.sync();
			cout<<"������K(Ĭ��Ϊ�������������)��";
			int k = 0;
			cin>>k;
			cin.clear();	cin.sync();
			cout<<endl;
			cout<<"��ѡ��������(Ĭ��Ϊ��׼)��"<<endl;
			cout<<"1. ��׼KNN"<<endl;
			cout<<"2. ʹ�þ����Ȩ���"<<endl;
			int method = STD_KNN;
			cin>>method;
			cin.clear();	cin.sync();
			cout<<endl;
			//����
			CKNN knn(k, method);
			knn.Run(g_trainingSet, g_testSet);
			system("pause");
		}
		else if(menu == 2)
		{
			cout<<"������K(Ĭ��Ϊ3)��";
			int k = 3;
			cin>>k;
			cin.clear();	cin.sync();
			cout<<endl;
			cout<<"��ѡ�����ĳ�ʼ������(Ĭ��Ϊ��׼)��"<<endl;
			cout<<"1. ��׼K-means"<<endl;
			cout<<"2. Leader K-means"<<endl;
			int method = STD_KMEANS;
			cin>>method;
			cin.clear();	cin.sync();
			cout<<endl;
			double thres = 1.5;
			if(method == LED_KMEANS)
			{
				cout<<"������Leader������ֵ(Ĭ��Ϊ1.5)��";
				cin>>thres;
				cin.clear();	cin.sync();
				cout<<endl;
			}
			//����
			CKmeans kmeans(k, method, thres);
			kmeans.Run(g_dataset);
			system("pause");
		}
		else if(menu == 0)
		{
			break;
		}
	
	}
	system("pause");
	return 0;
}

bool CInitDataset::LoadFile(const char *fileName)
{
	ifstream file(fileName, ios::in);
	if(file.fail())
	{
		cout<<"���ļ�ʧ�ܣ�"<<endl;
		system("pause");
		return false;
	}
	string tmpStr;
	int columnNum = 0, colTest = 0;
	bool bFirst = true;
	while(getline(file, tmpStr))
	{
		istringstream line(tmpStr);
		CData data;
		//��һ���Ǳ��
		line >> data.m_id;
		//�������ֶΣ��ж��������ݵ��ֶ����Ƿ�һ��
		double x;
		while(line >> x)
		{
			data.m_column.push_back(x);
			//��¼��һ�����ݵ��ֶ���
			if(bFirst)
				columnNum++;
			else
				colTest++;
		}
		if(!bFirst && colTest != columnNum)
		{
			cout<<"�����ֶ�����һ�£�"<<endl;
			system("pause");
			return false;
		}
		g_dataset.push_back(data);
		bFirst = false;
		colTest = 0;
	}
	if(g_dataset.size() == 0 || columnNum == 0)
	{
		cout<<"δ��ȡ���κ����ݣ�"<<endl;
		system("pause");
		return false;
	}
	int datasetSize = g_dataset.size();
	cout<<"��"<<fileName<<"��ȡ"<<datasetSize<<"�����ݣ��ֶ���="<<columnNum<<endl;
	file.close();

	//����Ĭ�Ϸ���
	for(int i=0; i!=datasetSize; ++i)
	{
		int id = g_dataset[i].m_id;
		if( (id>0 && id<26) || (id>75 && id<101) )
			g_dataset[i].m_class = 1;
		if( (id>25 && id<51) || (id>100 && id<126) )
			g_dataset[i].m_class = 2;
		if( (id>50 && id<76) || (id>125 && id<151) )
			g_dataset[i].m_class = 3;
	}
	return true;
} 

void CInitDataset::InitTrainingSet(double ratio, int method)
{
	//��ʼ��ѵ���������Լ�
	int datasetSize = g_dataset.size();
	int trainNum = (int)(datasetSize*ratio);
	if(method == 3)
		srand((unsigned)time(NULL));
		
	g_trainingSet.clear();
	g_testSet.clear();
	for(int i=0; i!=datasetSize; ++i)
	{
		int id = g_dataset[i].m_id;
		if(method == 1)
		{
			if(id < trainNum+1)
			{
				g_trainingSet.push_back(g_dataset[i]);
			}
			else
			{
				g_testSet.push_back(g_dataset[i]);
			}
		}
		if(method == 2)
		{
			if(id > (datasetSize-trainNum) )
			{
				g_trainingSet.push_back(g_dataset[i]);
			}
			else
			{
				g_testSet.push_back(g_dataset[i]);
			}
		}
		if(method == 3)
		{
			if(rand()%datasetSize < trainNum)
			{
				g_trainingSet.push_back(g_dataset[i]);
			}
			else
			{
				g_testSet.push_back(g_dataset[i]);
			}
		}
	}
}


