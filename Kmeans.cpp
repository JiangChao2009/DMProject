#include "Kmeans.h"

CKmeans::CKmeans(int k, int method, double thres)
{
	//Kֵ���벻С��2��Ĭ��Ϊ2
	m_clusterNum = (k<2) ? 2 : k;
	m_leaderThres = thres;
	//�淶��method
	switch(method)
	{
	case 1:
		m_method = STD_KMEANS;
		break;
	case 2:
		m_method = LED_KMEANS;
		break;
	default:
		m_method = STD_KMEANS;
	}
}
CKmeans::~CKmeans()
{
}

void CKmeans::Run(const VECDATA &ds)
{
	if(ds.empty())
	{
		cout<<"���ݼ�Ϊ�գ�"<<endl;
		return;
	}
	VECDATA cluster = ds;
	int clusterSize = cluster.size();
	//�������ܴ���������
	if(m_clusterNum > clusterSize)
		m_clusterNum = clusterSize;
	cout<<"**************************************"<<endl;
	cout<<"��ʼK��ֵ���ࣺ����="<<clusterSize<<", K="<<m_clusterNum<<", Method="<<GetMethod()<<endl;
	//�����ʼ��K��������m_centroids����
	InitCentroids(cluster);
	//�ظ�����ֱ���ִز��ٱ仯
	cout<<"������..."<<endl;
	int times = 1;
	bool bChange = false;
	while(times<100)
	{
		//ÿ�����������ļ���ŷʽ���룬�����������Ĵ�
		for(int i=0; i!= clusterSize; ++i)
		{
			double dis = EuDistance( cluster[i], m_centroids[0] );
			int c = 0;
			for(int j=1; j!=m_centroids.size(); ++j)
			{
				double dis1 = EuDistance( cluster[i], m_centroids[j] );
				if(dis1 < dis)
				{
					dis = dis1;
					c = j;
				}
			}
			//�ִ��б仯
			if( c != cluster[i].Cluster() )
				bChange = true;
			cluster[i].SetCluster(c);
		}
		//���ִز��䣬��ֹͣ�ظ�����
		if(!bChange)
			break;
		//����ѡ�����ģ����ֵ��
		ComputeMeans(cluster);
		bChange = false;
		times++;
	}
	//�������
	cout<<"��������"<<"times="<<times<<endl;
	PrintCluster(cluster);
	PrintEffect(ds, cluster);
}

void CKmeans::InitCentroids(const VECDATA &ds)
{
	m_centroids.clear();
	m_clusters.clear();
	//�������
	srand((unsigned)time(NULL));
	int dsSize = ds.size();
	vector<CLeader> leaders;
	if(m_method == LED_KMEANS)
	{
		//�������һ��Leader
		CLeader leader;
		leader.index = rand()%ds.size();
		leader.dataNum = 0;
		leaders.push_back(leader);
		//��ÿ��������滮Leader
		for(int i=0; i!=dsSize; ++i)
		{
			double dis = EuDistance(ds[(leaders[0].index)], ds[i]);;
			int leaderSize = leaders.size();
			int ownerIndex = 0;
			for(int j=1; j!=leaderSize; ++j)
			{
				int lIndex = leaders[j].index;
				double dis1 = EuDistance(ds[lIndex], ds[i]);
				if(dis1 < dis)
				{
					dis = dis1;
					ownerIndex = j;
				}
			}
			//��Leader�ľ�����С����ֵ������������Leader�����򴴽�һ����Leader
			if(dis > m_leaderThres)
			{
				CLeader ld;
				ld.index = i;
				ld.dataNum = 1;
				leaders.push_back(ld);
			}
			else
			{
				leaders[ownerIndex].dataNum ++;
			}
		}
		//������������ǰK��Leader��������
		sort(leaders.begin(), leaders.end());
		cout<<"Leaders=";
	}
	int leaderSize = leaders.size();
	for(int i=0; i<m_clusterNum; ++i)
	{
		if(i<leaderSize)	//ʹ��Leader��ʼ������
		{
			int index = leaders[i].index;
			cout<<"["<<ds[index].ID()<<"] ";
			m_centroids.push_back(ds[index]);
			vector<int> tmp;
			m_clusters.push_back(tmp);
		}
		else				//ʹ�������ų�ʼ������
		{
			int ranIndex = rand()%dsSize;
			//�������ı���Ƿ��Ѿ���ѡ��Ϊ����
			int j = 0;
			for(; j<i; ++j)
			{
				if(ds[ranIndex].ID() == m_centroids[j].ID())
					break;
			}
			//�����ظ�����룬������������
			if(j >= i)
			{
				m_centroids.push_back(ds[ranIndex]);
				vector<int> tmp;
				m_clusters.push_back(tmp);
			}
			else
				--i;
		}
	}
	if(m_method == LED_KMEANS) cout<<endl;
}
void CKmeans::ComputeMeans(const VECDATA &ds)
{
	//ÿ���ؼ���
	vector<int> countInClusters;
	int centroidSize = m_centroids.size();
	int dSize = ds.size();
	for(int i=0; i!=centroidSize; ++i)
	{
		countInClusters.push_back(0);
		m_centroids[i].Clear();
	}
	//ͳ�Ƹ�����֮��
	for(int i=0; i!=dSize; ++i)
	{
		int c = ds[i].Cluster();
		m_centroids[c] += ds[i];
		countInClusters[c] ++;
	}
	//�����ֵ
	for(int i=0; i!=centroidSize; ++i)
	{
		m_centroids[i] /= countInClusters[i];
	}
}
double CKmeans::EuDistance(const CData &d1, const CData &d2)
{
	double rd = 0.0;
	int size = d1.Size();
	for(int i=0; i!=size; ++i)
	{
		rd += (d1.Column(i)-d2.Column(i)) * (d1.Column(i)-d2.Column(i));
	}
	return sqrt(rd);
}
void CKmeans::PrintCluster(const VECDATA &cluster)
{
	//���д�ӡ�������ݵĴر�ʶ
	int clusterSize = cluster.size();
	for(int i=0; i!=clusterSize; ++i)
	{
		int id = cluster[i].ID();
		int c = cluster[i].Cluster();
		m_clusters[c].push_back(id);		//�����Ժ��ӡ�ִؽ��
		if( id%25 == 1 )
		{
			cout << "[" << id-1 <<"] ";
		}
		char a = 'A'+c;
		cout << a << " ";
		if( id%25 == 0 )
		{
			cout << endl;
		}
	}
	cout << endl;
	//��ӡ�ִؽ��
	for(int i=0; i!=m_clusterNum; ++i)
	{
		cout<<"Cluster "<<i+1<<":"<<endl;
		int preNum = -1;
		int curNum;
		bool bSkip = false;
		for(int j=0; j != m_clusters[i].size(); ++j)
		{
			curNum = m_clusters[i][j];
			if(curNum != preNum+1)
			{
				if(preNum != -1)
				{
					if(!bSkip)
						cout<<"-"<<preNum<<", "<<curNum;
					else
						cout<<", "<<curNum;
				}
				else
					cout<<curNum;
				bSkip = true;
			}
			else if(!bSkip)
			{
				cout<<"-";
				if(j == m_clusters[i].size()-1)
					cout<<curNum;
			}
			else
			{
				bSkip = false;
			}
			preNum = curNum;
		}
		cout<<endl;
	}
	cout << endl;
}
void CKmeans::PrintEffect(const VECDATA &ods, VECDATA &rds)
{
	//ÿ�صĳɷ�ͳ��
	vector<vector<int>> compos;
	for(int i=0; i<m_clusterNum; ++i)
	{
		vector<int> tmpv;
		compos.push_back(tmpv);
	}

	int size = rds.size();
	for(int i=0; i!=size; ++i)
	{
		int rc = rds[i].Cluster();
		int oc = ods[i].Class()-1;
		//����ÿһ�أ���Ҫͳ�Ƶĳɷֱ�Ŵ���compos[rc]��С����ҪΪcompos[rc]����Ԫ�ء�
		int hastoAppend = oc - compos[rc].size();
		while(hastoAppend >= 0)
		{
			int t = 0;
			compos[rc].push_back(t);
			hastoAppend--;
		}
		compos[rc][oc] ++;
	}
	//��ÿ�����ɷ֣����������
	int hits = 0;
	for(int i=0; i<m_clusterNum; ++i)
	{
		int mainCNum = 0;
		for(int j=0; j != compos[i].size(); ++j)
		{
			if( compos[i][j] > mainCNum )
				mainCNum = compos[i][j];
		}
		hits += mainCNum;
	}
	cout << "���������Average Hits = " << hits*100.0/size <<"%"<< endl << endl;;
}
char *CKmeans::GetMethod()const
{
	switch(m_method)
	{
	case STD_KMEANS:
		return "Standard";
	case LED_KMEANS:
		return "LK-means";
	default:
		return "?";
	}
}