#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <set>
#include <ctime>
#include <array>
#include <fstream>
#include <map>
#include <iomanip>
#include <queue>
#define x first
#define y second

using namespace std;

using pii=pair<int,int>;
using ld=long double;
mt19937 rng(0);

vector<int> dx={0,1,0,-1},dy={1,0,-1,0};
const ld INF=1e12;
ld tottim,totcor;

void pr(vector<pii> v,int n,int m)
{
	vector<vector<int>> re(n,vector<int>(m));
	for (auto x: v) ++re[x.x][x.y];
	for (int i=0;i<n;++i,cout<<endl) for (int j=0;j<m;++j) cout<<re[i][j];
	cout<<endl;
}

void dfs(int n,int m,int i,int j,vector<vector<int>>&vis)
{
	if (i<0 || j<0 || i>=n || j>=m) return;
	if (vis[i][j]) return;
	vis[i][j]=1;
	for (int k=0;k<4;++k) dfs(n,m,i+dx[k],j+dy[k],vis);
}

bool connected(vector<pii> poi)
{
	if (poi.size()<=1) return 1;
	int n=0,m=0;
	for (auto x: poi) n=max(n,x.x+1),m=max(m,x.y+1);
	vector<vector<int>> vis(n,vector<int>(m,1));
	for (auto x: poi) vis[x.x][x.y]=0;
	dfs(n,m,poi[0].x,poi[0].y,vis);
	for (int i=0;i<n;++i) for (int j=0;j<m;++j) if (!vis[i][j]) return 0;
	return 1;
}

bool addran(vector<pii>&poi,int n,int m,vector<vector<int>> allo,vector<pii> avoi={})
{
	vector<vector<int>> dob(n,vector<int>(m));
	for (auto x: poi) for (int k=0;k<4;++k)
	{
		int x1=x.x+dx[k],y1=x.y+dy[k];
		if (x1<0 || y1<0 || x1>=n || y1>=m) continue;
		if (!allo[x1][y1]) continue;
		//cout<<x.x<<' '<<x.y<<' '<<x1<<' '<<y1<<endl;
		dob[x1][y1]=1;
	}
	for (auto x: poi) dob[x.x][x.y]=0;
	int kol=0;
	for (int i=0;i<n;++i) for (int j=0;j<m;++j) kol+=dob[i][j];
	if (kol==0) return 0;
	vector<int> ordtry;
	for (int i=1;i<=kol;++i) ordtry.push_back(i);
	shuffle(ordtry.begin(),ordtry.end(),rng);
	sort(avoi.begin(),avoi.end());
	for (auto whi: ordtry)
	{
		kol=0;
		for (int i=0;i<n;++i) for (int j=0;j<m;++j) if (dob[i][j])
		{
			++kol;
			if (kol==whi)
			{
				auto poi1=poi;
				poi1.push_back({i,j});
				sort(poi1.begin(),poi1.end());
				if (poi1!=avoi && connected(poi1))
				{
					poi=poi1;
					return 1;
				}
			}
		}
	}
	return 0;
}

bool repran(vector<pii>&poi,int n,int m,vector<vector<int>> allo,pii start)
{
	vector<int> ordtry;
	for (int i=0;i<(int)poi.size();++i) ordtry.push_back(i);
	shuffle(ordtry.begin(),ordtry.end(),rng);
	for (auto whi: ordtry)
	{
		if (poi[whi]==start) continue;
		auto poi1=poi;
		poi1.erase(poi1.begin()+whi);
		int u=addran(poi1,n,m,allo,poi);
		if (u)
		{
			poi=poi1;
			return 1;
		}
	}
	return 0;
}

ld gettotval(vector<pii> poi,vector<vector<ld>> val)
{
	ld res=0;
	for (auto x: poi) res+=val[x.x][x.y];
	return res;
}

vector<pii> anneal(vector<vector<ld>> val,int torchc,pii start,vector<vector<int>> allo)
{
	vector<ld> formed;
	int n=val.size(),m=val[0].size();
	for (int i=0;i<n;++i) for (int j=0;j<m;++j) if (allo[i][j] && val[i][j]<INF) formed.push_back(val[i][j]);
	vector<pii> poi={start};
	for (int i=0;i<torchc;++i) if (!addran(poi,n,m,allo)) return {start};
	if (!repran(poi,n,m,allo,start)) return poi;
	sort(formed.begin(),formed.end());
	if (formed.empty()) exit(69);
	ld med=formed[formed.size()/2];
	const int ITERCOUNT=5e3;
	uniform_real_distribution<ld> dis(0,1);
	ld maxval=gettotval(poi,val);
	vector<pii> maxpoi=poi;
	for (int curit=0;curit<ITERCOUNT;++curit)
	{
		vector<pii> prevpoi=poi;
		ld prevval=gettotval(poi,val);
		repran(poi,n,m,allo,start);
		ld newval=gettotval(poi,val);
		if (newval>maxval)
		{
			maxval=newval;
			maxpoi=poi;
		}
		const ld EX=1,DIV=1; //higher EX=more sudden change from exploring to not exploring(in general more exploring), higher DIV=less exploring
		ld T=med*pow(ITERCOUNT/(curit+1),EX)/DIV;
		if (newval<prevval-INF/2 || (newval<prevval && exp((newval-prevval)/T)<dis(rng))) poi=prevpoi;
	}
	return maxpoi;
}

vector<pii> optSol(vector<vector<ld>> val,int torchc,pii start,vector<vector<int>> allo) //exponential time complexity, just for testing
{
	int n=val.size(),m=val[0].size();
	vector<pii> alp;
	for (int i=0;i<n;++i) for (int j=0;j<m;++j) if (allo[i][j]) alp.push_back({i,j});
	ld maxval=-INF;
	vector<pii> maxpoi;
	for (int b=0;b<(1<<(alp.size()));++b)
	{
		if (__builtin_popcount(b)!=torchc+1) continue;
		vector<pii> poi;
		bool uzs=0;
		for (int i=0;i<(int)alp.size();++i) if ((b>>i)&1)
		{
			if (alp[i]==start) uzs=1;
			poi.push_back(alp[i]);
		}
		if (uzs && connected(poi))
		{
			ld va=gettotval(poi,val);
			if (va>maxval)
			{
				maxval=va;
				maxpoi=poi;
			}
		}
	}
	return maxpoi;
}

void genRandTest(int n,int m,int k,int l) //just for testing
{
	vector<pii> dob;
	while (1)
	{
		set<pii> s;
		while ((int)s.size()<k)
		{
			int a=rand()%n;
			int b=rand()%m;
			s.insert({a,b});
		}
		vector<pii> v(s.begin(),s.end());
		if (!connected(v)) continue;
		dob=v;
		break;
	}
	pii start=dob[rand()%k];
	vector<vector<int>> allo(n,vector<int>(m));
	vector<vector<ld>> val(n,vector<ld>(m));
	for (auto x: dob)
	{
		allo[x.x][x.y]=1;
		val[x.x][x.y]=(rand()%101)/100.;
	}
	ld sta=clock();
	vector<pii> annsol=anneal(val,l-1,start,allo);
	ld end=clock();
	tottim+=(end-sta)/CLOCKS_PER_SEC;
	vector<pii> optsol=optSol(val,l-1,start,allo);
	ld valann=gettotval(annsol,val);
	ld valopt=gettotval(optsol,val);
	if (valann!=valopt) cout<<valann*100./valopt<<"% correct."<<endl;
	totcor+=valann/valopt;
}

array<array<vector<pair<vector<vector<pii>>,ld>>,100>,100> addFloor(array<array<vector<pair<vector<vector<pii>>,ld>>,100>,100> prev,array<array<vector<pair<vector<pii>,ld>>,100>,100> newf)
{
	array<array<vector<pair<vector<vector<pii>>,ld>>,100>,100> res;
	for (int gk1=0;gk1<100;++gk1) for (int gk2=max(0,50-gk1);gk2<min(100,150-gk1);++gk2)
	{
		for (int sk1=0;sk1<100;++sk1) for (int sk2=max(0,50-sk1);sk2<min(100,150-sk1);++sk2)
		{
			if (prev[gk1][sk1].empty() || newf[gk2][sk2].empty()) continue;
			int reqs=(int)prev[gk1][sk1].size()+(int)newf[gk2][sk2].size()-1;
			while ((int)res[gk1+gk2-50][sk1+sk2-50].size()<reqs) res[gk1+gk2-50][sk1+sk2-50].push_back({{},-INF*10000});
			for (int tc1=0;tc1<(int)prev[gk1][sk1].size();++tc1) for (int tc2=0;tc2<(int)newf[gk2][sk2].size();++tc2)
			{
				ld nv=prev[gk1][sk1][tc1].y+newf[gk2][sk2][tc2].y;
				if (nv>res[gk1+gk2-50][sk1+sk2-50][tc1+tc2].y)
				{
					res[gk1+gk2-50][sk1+sk2-50][tc1+tc2].y=nv;
					vector<vector<pii>> no=prev[gk1][sk1][tc1].x;
					no.push_back(newf[gk2][sk2][tc2].x);
					res[gk1+gk2-50][sk1+sk2-50][tc1+tc2].x=no;
				}
			}
		}
	}
	return res;
}

int minLocks(vector<vector<int>> allo,vector<pii> lockp,pii start,pii exitp)
{
	int n=allo.size(),m=allo[0].size();
	vector<vector<int>> lockc(n,vector<int>(m));
	for (auto x: lockp) ++lockc[x.x][x.y];
	vector<vector<int>> vis(n,vector<int>(m));
	priority_queue<pair<int,pii>> pq;
	pq.push({0,start});
	while (pq.size())
	{
		int lc=-pq.top().x,px=pq.top().y.x,py=pq.top().y.y;
		pq.pop();
		if (vis[px][py]) continue;
		vis[px][py]=1;
		if (exitp==make_pair(px,py)) return lc;
		for (int k=0;k<4;++k)
		{
			int x1=px+dx[k],y1=py+dy[k];
			if (x1<0 || y1<0 || x1>=n || y1>=m) continue;
			if (!allo[x1][y1]) continue;
			pq.push({-lc-lockc[x1][y1],{x1,y1}});
		}
	}
	return 1000;
}

int sce=0;

array<array<vector<pair<vector<pii>,ld>>,100>,100> getRes(vector<vector<ld>> val,vector<vector<int>> allo,pii start,vector<pair<pii,int>> pl,vector<pii> sk,vector<pii> sl,vector<pii> gk,vector<pii> gl,bool reqExit,pii exitp={-1,-1})
{
	if (reqExit && exitp==make_pair(-1,-1)) exit(420691);
	if (!reqExit && exitp!=make_pair(-1,-1)) exit(420692);
	int maxtorchc=0,n=val.size(),m=val[0].size();
	for (int i=0;i<n;++i) for (int j=0;j<m;++j) maxtorchc+=allo[i][j];
	array<array<vector<pair<vector<pii>,ld>>,100>,100> res;
	int minGK=0,minSK=0;
	if (reqExit)
	{
		minGK=minLocks(allo,gl,start,exitp)+50;
		minSK=minLocks(allo,sl,start,exitp)+50;
	}
	for (int torchc=0;torchc<maxtorchc;++torchc)
	{
		for (int maskPL=0;maskPL<(1<<pl.size());++maskPL)
		{
			for (int maskGK=0;maskGK<(1<<gk.size());++maskGK) for (int maskGL=0;maskGL<(1<<gl.size());++maskGL)
			{
				for (int maskSK=0;maskSK<(1<<sk.size());++maskSK) for (int maskSL=0;maskSL<(1<<sl.size());++maskSL)
				{
					vector<vector<ld>> newval=val;
					vector<vector<int>> newallo=allo;
					ld offset=0;
					if (reqExit)
					{
						newval[exitp.x][exitp.y]+=INF;
						offset+=INF;
					}
					int nettorchc=torchc;
					for (int i=0;i<(int)pl.size();++i)
					{
						if ((maskPL>>i)&1)
						{
							newval[pl[i].x.x][pl[i].x.y]+=INF;
							offset+=INF;
							nettorchc-=pl[i].y;
						}
						else newallo[pl[i].x.x][pl[i].x.y]=0;
					}
					if (nettorchc<0) continue;
					int netgkc=50,netskc=50;
					for (int i=0;i<(int)gk.size();++i)
					{
						if ((maskGK>>i)&1)
						{
							newval[gk[i].x][gk[i].y]+=INF;
							offset+=INF;
							netgkc++;
						}
						else newallo[gk[i].x][gk[i].y]=0;
					}
					if (netgkc<minGK) continue;
					for (int i=0;i<(int)gl.size();++i)
					{
						if ((maskGL>>i)&1)
						{
							newval[gl[i].x][gl[i].y]+=INF;
							offset+=INF;
							netgkc--;
						}
						else newallo[gl[i].x][gl[i].y]=0;
					}
					for (int i=0;i<(int)sk.size();++i)
					{
						if ((maskSK>>i)&1)
						{
							newval[sk[i].x][sk[i].y]+=INF;
							offset+=INF;
							netskc++;
						}
						else newallo[sk[i].x][sk[i].y]=0;
					}
					if (netskc<minSK) continue;
					for (int i=0;i<(int)sl.size();++i)
					{
						if ((maskSL>>i)&1)
						{
							newval[sl[i].x][sl[i].y]+=INF;
							offset+=INF;
							netskc--;
						}
						else newallo[sl[i].x][sl[i].y]=0;
					}
					++sce;
					auto re=anneal(newval,torchc,start,newallo);
					ld nv=gettotval(re,newval)-offset;
					if (nv<0) continue;
					while ((int)res[netgkc][netskc].size()<=nettorchc) res[netgkc][netskc].push_back({{},-INF*10000});
					if (res[netgkc][netskc][nettorchc].y<nv)
					{
						res[netgkc][netskc][nettorchc].x=re;
						res[netgkc][netskc][nettorchc].y=nv;
					}
				}
			}
		}
	}
	return res;
}

int main()
{
	ifstream player("resourceValues.txt");
	int torchCount;
	player>>torchCount;
	string filler;
	player>>filler>>filler;
	const int REC=11;
	map<string,ld> values;
	for (int i=0;i<REC;++i)
	{
		string nam;
		player>>nam;
		nam=nam.substr(0,2);
		while (player>>filler)
		{
			if (filler[0]>='0' && filler[0]<='9')
			{
				values[nam]=stold(filler);
				break;
			}
		}
	}
	ifstream dung("dungeon.txt");
	int floorCount;
	dung>>floorCount;
	array<array<vector<pair<vector<vector<pii>>,ld>>,100>,100> curState;
	curState[50][50]={{{},0}};
	vector<vector<pii>> optpoi;
	ld optval=-INF*20000;
	vector<pii> dim,sta;
	for (int floor=1;floor<=floorCount;++floor)
	{
		vector<pair<pii,int>> pl;
		vector<pii> sk,sl,gk,gl;
		pii start={-1,-1},exit={-1,-1};
		int n,m;
		dung>>n>>m;
		dim.push_back({n,m});
		vector<vector<ld>> val(n,vector<ld>(m));
		vector<vector<int>> allo(n,vector<int>(m));
		for (int i=0;i<n;++i) for (int j=0;j<m;++j)
		{
			string x;
			dung>>x;
			string nam=x.substr(0,2);
			int qu=stoi(x.substr(2));
			val[i][j]=values[nam]*qu;
			allo[i][j]=(nam!="XX");
			if (nam=="ST") start={i,j};
			if (nam=="ED") exit={i,j};
			if (nam=="GL") gl.push_back({i,j});
			if (nam=="SL") sl.push_back({i,j});
			if (nam=="GK") gk.push_back({i,j});
			if (nam=="SK") sk.push_back({i,j});
			if (nam=="PL") pl.push_back({{i,j},qu});
		}
		sce=0;
		clog<<"Read dungeon "<<floor<<endl;
		auto resEnd=getRes(val,allo,start,pl,sk,sl,gk,gl,0);
		clog<<sce<<" possible scenarios completed."<<endl;
		clog<<"Got results if dungeon "<<floor<<" is the end."<<endl;
		auto totalEnd=addFloor(curState,resEnd);
		clog<<"Merged results if dungeon "<<floor<<" is the end."<<endl;
		for (int cgk=50;cgk<100;++cgk) for (int csk=50;csk<100;++csk) for (int toru=0;toru<=min(torchCount,(int)totalEnd[cgk][csk].size()-1);++toru) if (totalEnd[cgk][csk][toru].y>optval)
		{
			optval=totalEnd[cgk][csk][toru].y;
			optpoi=totalEnd[cgk][csk][toru].x;
		}
		if (floor<floorCount)
		{
			sce=0;
			auto resNoEnd=getRes(val,allo,start,pl,sk,sl,gk,gl,1,exit);
			clog<<sce<<" possible scenarios completed."<<endl;
			clog<<"Got results if dungeon "<<floor<<" isn't the end."<<endl;
			curState=addFloor(curState,resNoEnd);
			clog<<"Merged results if dungeon "<<floor<<" isn't the end."<<endl;
		}
		sta.push_back(start);
	}
	ofstream result("result.txt");
	result<<"The highest obtainable value is "<<fixed<<setprecision(5)<<optval<<endl;
	result<<"You should explore "<<optpoi.size()<<" floors."<<endl;
	result<<"Here are the cells you should unlock on each floor(S=start, 1=unlock, 0=don't unlock):"<<endl;
	for (int f=0;f<(int)optpoi.size();++f)
	{
		int n=dim[f].x,m=dim[f].y;
		vector<vector<int>> re(n,vector<int>(m));
		for (auto x: optpoi[f]) ++re[x.x][x.y];
		for (int i=0;i<n;++i,result<<endl) for (int j=0;j<m;++j)
		{
			if (sta[f]==make_pair(i,j))
			{
				if (re[i][j]!=1) exit(69420);
				result<<'S';
			}
			else result<<re[i][j];
		}
		result<<endl;
	}
}




