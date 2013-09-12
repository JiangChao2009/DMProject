#include "KNN.h"

CKNN::CKNN(int k, int method)
{
	//Kֵ���벻С��0��Ĭ��Ϊ0
	m_k = (k<0) ? 0 : k;
	//�淶��method
	switch(method)
	{
	case 1:
		m_method = STD_KNN;
		break;
	case 2:
		m_method = DWV_KNN;
		break;
	default:
		m_method = STD_KNN;
	}
}
CKNN::~CKNN()
{
}

void CKNN::Run(const VECDATA &train, const VECDATA &test)
{
	if(train.empty())
	{
		cout<<"ѵ����Ϊ�գ�"<<endl;
		return;
	}
	if(test.empty())
	{
		cout<<"���Լ�Ϊ�գ�"<<endl;
		return;
	}
	//��ʼ��һЩ�㷨����
	InitAlgorithm(train);
	int trainSize = train.size();
	int testSize = test.size();
	cout<<"**************************************"<<endl;
	cout<<"��ʼKNN���ࣺѵ��="<<trainSize<<", ����="<<testSize<<", K="<<m_k<<", Method="<<GetMethod()<<endl;
	cout<<"������..."<<endl;
	//ÿ������������ѵ�����ݼ���ŷ�Ͼ��룬�������������
	m_testDists.reserve(testSize);
	for(int i=0; i!=testSize; ++i)
	{
		vector<CNeighbor> everyTest;
		everyTest.reserve(testSize);
		for(int j=0; j!=trainSize; ++j)
		{
			CNeighbor nb;
			nb.id = j;
			nb.distence = EuDistance(test[i], train[j]);
			//����
			vector<CNeighbor>::iterator iter = everyTest.begin();
			vector<CNeighbor>::iterator itE = everyTest.end();
			for(; iter != itE; ++iter)
			{
				if(nb.distence < (*iter).distence)
					break;
			}
			everyTest.insert(iter, nb);
		}
		m_testDists.push_back(everyTest);
	}
	//ÿ����������ȡ���������K��ѵ�����ݣ�ͳ��ռ��������
	VECDATA testSet = test;
	for(int i=0; i!=testSize; ++i)
	{
		vector<double> classStat;
		for(int j=0; j!=trainSize; ++j)
		{
			if(j>=m_k)		//ǰk��
				break;
			int trainID = m_testDists[i][j].id;
			double dist = m_testDists[i][j].distence;
			int c = train[trainID].Class();
			int hastoAppend = c - classStat.size();
			while(hastoAppend >= 0)
			{
				int tmp = 0;
				classStat.push_back(tmp);
				hastoAppend--;
			}
			//�����Ȩ���
			if(m_method == STD_KNN)
				classStat[c] += 1.0;
			else if(m_method == DWV_KNN)
				classStat[c] = classStat[c] + 1.0/dist/dist;
		}
		//���������Ҫ������Ȩֵ��
		int maxClass = 0;
		double maxClassStat = 0.0;
		for(int j=1; j!=classStat.size(); ++j)
		{
			classStat[j] = classStat[j]*m_classWeights[j];
			if(classStat[j] > maxClassStat)
			{
				maxClassStat = classStat[j];
				maxClass = j;
			}
		}
		//���÷���
		testSet[i].Classify(maxClass);
	}
	//�������
	cout<<"��������"<<endl;
	PrintClassification(testSet);
	PrintEffect(test, testSet);
}
void CKNN::InitAlgorithm(const VECDATA &train)
{
	int size = train.size();
	//Kֵ����С��ѵ����
	if(m_k > size)
		m_k = size;
	m_classes.clear();
	m_classWeights.clear();
	for(int i=0; i!=size; ++i)
	{
		//ͳ��ÿ���Ȩ��
		int c = train[i].Class();
		int hastoAppend = c - m_classWeights.size();
		while(hastoAppend >= 0)
		{
			vector<unsigned> tmp;
			m_classes.push_back(tmp);
			double d = 0.0;
			m_classWeights.push_back(d);
			hastoAppend--;
		}
		m_classWeights[c] += 1.0;
	}

	//��Ȩֵ
	int minc = size;
	for(int i=0; i!=m_classWeights.size(); ++i)
	{
		if(m_classWeights[i] < 1.0)
			continue;
		if(m_classWeights[i] < minc)
			minc = (int)m_classWeights[i];
		m_classWeights[i] = size/m_classWeights[i];
	}
	//��KΪ0����ʼ��һ������Kֵ��ѡռ������С���������ΪKֵ
	if(m_k == 0)
		m_k = minc;
}
double CKNN::EuDistance(const CData &d1, const CData &d2)const
{
	double rd = 0.0;
	int size = d1.Size();
	for(int i=0; i!=size; ++i)
	{
		rd += (d1.Column(i)-d2.Column(i)) * (d1.Column(i)-d2.Column(i));
	}
	return sqrt(rd);
}
void CKNN::PrintClassification(const VECDATA &test)
{
	//������
	int testSize = test.size();
	for(int i=0; i!=testSize; ++i)
	{
		int c = test[i].Class();
		m_classes[c].push_back(test[i].ID());
	}
	//��ӡ
	for(int i=1; i!=m_classes.size(); ++i)
	{
		int count = m_classes[i].size();
		if(count == 0)
			continue;
		cout<<"Classification "<<i<<":"<<endl;
		for(int j=0; j!=count; ++j)
		{
			cout<<m_classes[i][j]<<" ";
			if(j%5 == 4 || j == count-1)
				cout<<endl;
		}
		cout<<endl;
	}
}
void CKNN::PrintEffect(const VECDATA &ods, const VECDATA &rds)
{
	//�ȽϷ����ʶ
	int size = ods.size();
	int hits = 0;
	for(int i=0; i!=size; ++i)
	{
		if(ods[i].Class() == rds[i].Class())
		{
			hits++;
		}
	}
	cout<<"���������Accuracy = "<<hits*100.0/size<<"%"<<endl<<endl;
}
char *CKNN::GetMethod()const
{
	switch(m_method)
	{
	case STD_KNN:
		return "Standard";
	case DWV_KNN:
		return "Distance-Weighted Voting";
	default:
		return "?";
	}
}