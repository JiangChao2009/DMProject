#ifndef __KMEANS_H__
#define __KMEANS_H__


#include <time.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;
#include "Data.h"

enum KM_METHOD
{
	STD_KMEANS = 1,							//����K��ֵ
	LED_KMEANS								//Leader��ʼ��������
};

class CLeader
{
public:
	friend class CKmeans;
	bool operator<(const CLeader &l)		//for sort
	{
		if(dataNum > l.dataNum)
			return true;
		else
			return false;	
	}
private:
	int index;								//����
	int dataNum;							//�滮������
};

class CKmeans
{
public:
	CKmeans(int k, int method=STD_KMEANS, double thres=1.5);
	~CKmeans();

	void Run(const VECDATA &ds);

private:
	void InitCentroids(const VECDATA &ds);						//�����ʼ������
	void ComputeMeans(const VECDATA &ds);						//�����ֵ��Ϊ������
	double EuDistance(const CData &d1, const CData &d2);		//����ŷʽ����
	void PrintCluster(const VECDATA &ds);						//��ӡ�ִؽ��
	void PrintEffect(const VECDATA &ods, VECDATA &rds);			//��ӡ��������������ʣ�

	KM_METHOD m_method;											//����ѡ��
	double m_leaderThres;										//Leader������ֵ
	int m_clusterNum;											//��������K
	VECDATA m_centroids;										//����
	vector<vector<int>> m_clusters;								//�ִؽ��
public:
	char *GetMethod()const;										//ȡ�÷�����
};

#endif