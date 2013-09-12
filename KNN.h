#ifndef __KNN_H__
#define __KNN_H__

#include <math.h>
#include <vector>
#include <iostream>
using namespace std;
#include "Data.h"

enum KNN_METHOD
{
	STD_KNN = 1,				//����KNN
	DWV_KNN						//�����Ȩ���
};

class CNeighbor
{
public:
	friend class CKNN;
private:
	unsigned id;				//���
	double distence;			//����
};

class CKNN
{
public:
	CKNN(int k=0, int method=STD_KNN);
	~CKNN();

	void Run(const VECDATA &train, const VECDATA &test);

private:
	void InitAlgorithm(const VECDATA &train);
	double EuDistance(const CData &d1, const CData &d2)const;
	void PrintClassification(const VECDATA &test);
	void PrintEffect(const VECDATA &ods, const VECDATA &rds);

	int m_k;													//Kֵ
	KNN_METHOD m_method;										//����ѡ��
	vector<double> m_classWeights;								//ѵ����ÿһ��ռ��Ȩ��
	vector<vector<CNeighbor>> m_testDists;						//���Լ���ѵ������������
	vector<vector<unsigned>> m_classes;							//������
public:
	char *GetMethod()const;										//ȡ�÷�����
};

#endif