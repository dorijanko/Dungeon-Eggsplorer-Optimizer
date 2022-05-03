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
#include <chrono>
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
	for (int i=0;i<n;++i) for (int j=0;j<m;++j) if (allo[i][j] && val[i][j]<INF && val[i][j]>0) formed.push_back(val[i][j]);
	vector<pii> poi={start};
	for (int i=0;i<torchc;++i) if (!addran(poi,n,m,allo)) return {start};
	if (!repran(poi,n,m,allo,start)) return poi;
	sort(formed.begin(),formed.end());
	ld med;
	if (formed.empty()) med=1;
	else med=formed[formed.size()/2];
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
		const ld EX=1,DIV=6; //higher EX=more sudden change from exploring to not exploring(in general more exploring), higher DIV=less exploring
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
	vector<pii> annsol=anneal(val,l-1,start,allo);
	vector<pii> optsol=optSol(val,l-1,start,allo);
	ld valann=gettotval(annsol,val);
	ld valopt=gettotval(optsol,val);
	if (valann!=valopt) cout<<valann*100./valopt<<"% correct."<<endl;
	totcor+=valann/valopt;
}

const int MAXGK=10,MAXSK=25,MAXRGK=0,MAXRSK=3;
using muldesc=array<array<array<array<vector<pair<vector<vector<pii>>,ld>>,MAXSK>,MAXSK>,MAXGK>,MAXGK>;
using floordesc=array<array<array<array<vector<pair<vector<pii>,ld>>,MAXSK>,MAXSK>,MAXGK>,MAXGK>;
int curFloorForDebug;

pair<muldesc,array<int,4>> addFloor(const muldesc&prev,const floordesc&newf,array<int,4> dim1,array<int,4> dim2,array<int,2> req,array<int,4> latkl)
{
	muldesc res;
	for (int gk1=0;gk1<dim1[0];++gk1) for (int gk2=max(0,req[0]-gk1);gk2<dim2[0];++gk2)
	{
		for (int gl1=0;gl1<dim1[1];++gl1) for (int gl2=req[0];gl2<dim2[1];++gl2)
		{
			if (gk1>MAXRGK && gl1>0) continue;
			for (int sk1=0;sk1<dim1[2];++sk1) for (int sk2=max(0,req[1]-sk1);sk2<dim2[2];++sk2)
			{
				for (int sl1=0;sl1<dim1[3];++sl1) for (int sl2=req[1];sl2<dim2[3];++sl2)
				{
					if (sk1>MAXRSK && sl1>0) continue;
					if (prev[gk1][gl1][sk1][sl1].empty() || newf[gk2][gl2][sk2][sl2].empty()) continue;
					//clog<<"merging "<<gk1<<' '<<gl1<<' '<<sk1<<' '<<sl1<<" and "<<gk2<<' '<<gl2<<' '<<sk2<<' '<<sl2<<endl;
					int reqs=(int)prev[gk1][gl1][sk1][sl1].size()+(int)newf[gk2][gl2][sk2][sl2].size()-1;
					//clog<<"Expected size: "<<reqs<<' '<<prev[gk1][sl1][sk1][sl1].size()<<' '<<newf[gk2][gl2][sk2][sl2].size()<<endl;
					int ngk=gk1+gk2-req[0],ngl=gl1+gl2-req[0],nsk=sk1+sk2-req[1],nsl=sl1+sl2-req[1];
					if (ngk>ngl+latkl[1]) continue;
					if (ngl>ngk+latkl[0]) continue;
					if (nsk>nsl+latkl[3]) continue;
					if (nsl>nsk+latkl[2]) continue;
					if (ngk>MAXRGK && ngl>0)
					{
						int kol=min(ngk-MAXRGK,ngl);
						ngk-=kol;
						ngl-=kol;
					}
					if (nsk>MAXRSK && nsl>0)
					{
						int kol=min(nsk-MAXRSK,nsl);
						nsk-=kol;
						nsl-=kol;
					}
					while ((int)res[ngk][ngl][nsk][nsl].size()<reqs) res[ngk][ngl][nsk][nsl].push_back({{},-INF*10000});
					//clog<<"Expanded res."<<endl;
					for (int tc1=0;tc1<(int)prev[gk1][gl1][sk1][sl1].size();++tc1) for (int tc2=0;tc2<(int)newf[gk2][gl2][sk2][sl2].size();++tc2)
					{
						ld nv=prev[gk1][gl1][sk1][sl1][tc1].y+newf[gk2][gl2][sk2][sl2][tc2].y;
						if (nv>res[ngk][ngl][nsk][nsl][tc1+tc2].y)
						{
							res[ngk][ngl][nsk][nsl][tc1+tc2].y=nv;
							vector<vector<pii>> no=prev[gk1][gl1][sk1][sl1][tc1].x;
							no.push_back(newf[gk2][gl2][sk2][sl2][tc2].x);
							res[ngk][ngl][nsk][nsl][tc1+tc2].x=no;
						}
					}
				}
			}
		}
	}
	array<int,4> nd;
	for (int i=0;i<4;++i) nd[i]=dim1[i]+dim2[i]-1;
	return {res,nd};
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

pair<floordesc,array<int,4>> getRes(vector<vector<ld>> val,vector<vector<int>> allo,pii start,vector<pair<pii,int>> pl,vector<pii> sk,vector<pii> sl,vector<pii> gk,vector<pii> gl,bool reqExit,pii exitp={-1,-1})
{
	if (reqExit && exitp==make_pair(-1,-1)) exit(420691);
	if (!reqExit && exitp!=make_pair(-1,-1)) exit(420692);
	int maxtorchc=0,n=val.size(),m=val[0].size();
	for (int i=0;i<n;++i) for (int j=0;j<m;++j) maxtorchc+=allo[i][j];
	floordesc res;
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
					int gkc=0,glc=0,skc=0,slc=0;
					for (int i=0;i<(int)gk.size();++i)
					{
						if ((maskGK>>i)&1)
						{
							newval[gk[i].x][gk[i].y]+=INF;
							offset+=INF;
							gkc++;
						}
						else newallo[gk[i].x][gk[i].y]=0;
					}
					for (int i=0;i<(int)gl.size();++i)
					{
						if ((maskGL>>i)&1)
						{
							newval[gl[i].x][gl[i].y]+=INF;
							offset+=INF;
							glc++;
						}
						else newallo[gl[i].x][gl[i].y]=0;
					}
					for (int i=0;i<(int)sk.size();++i)
					{
						if ((maskSK>>i)&1)
						{
							newval[sk[i].x][sk[i].y]+=INF;
							offset+=INF;
							skc++;
						}
						else newallo[sk[i].x][sk[i].y]=0;
					}
					for (int i=0;i<(int)sl.size();++i)
					{
						if ((maskSL>>i)&1)
						{
							newval[sl[i].x][sl[i].y]+=INF;
							offset+=INF;
							slc++;
						}
						else newallo[sl[i].x][sl[i].y]=0;
					}
					++sce;
					auto re=anneal(newval,torchc,start,newallo);
					ld nv=gettotval(re,newval)-offset;
					if (nv<0) continue;
					while ((int)res[gkc][glc][skc][slc].size()<=nettorchc) res[gkc][glc][skc][slc].push_back({{},-INF*10000});
					if (res[gkc][glc][skc][slc][nettorchc].y<nv)
					{
						res[gkc][glc][skc][slc][nettorchc].x=re;
						res[gkc][glc][skc][slc][nettorchc].y=nv;
					}
				}
			}
		}
	}
	return {res,{(int)gk.size()+1,(int)gl.size()+1,(int)sk.size()+1,(int)sl.size()+1}};
}


void ldi(array<int,4> dimens)
{
	clog<<"Dimensions: "<<dimens[0]<<' '<<dimens[1]<<' '<<dimens[2]<<' '<<dimens[3]<<endl;
}

string inSheet(string y)
{
	string x=y.substr(0,2);
	if (x=="GK") return "Gold Key";
	if (x=="GL") return "Gold Lock";
	if (x=="SK") return "Silver Key";
	if (x=="SL") return "Silver Lock";
	if (x=="ST") return "\xE2\x87\xA7";
	if (x=="ED") return "\xE2\x87\xA9";
	if (x=="DI")
	{
		if (y.substr(2)=="0") return "X";
		return y.substr(2)+" dias";
	}
	if (x=="PL")
	{
		string temps=y.substr(2)+" pet";
		if (y.substr(2)!="1") temps.push_back('s');
		return temps;
	}
	if (x=="SP") return y.substr(2)+" SP";
	if (x=="SR") return y.substr(2)+" SP Reset Token";
	if (x=="DU") return y.substr(2)+" dusts";
	if (x=="SH") return y.substr(2)+" shards";
	if (x=="EA") return "Event Avatar";
	if (x=="EF") return "Event Frame";
	if (x=="CE") return "Equip";
	if (x=="RE") return "Rare Equip";
	if (x=="EE") return "Event Equip";
	if (x=="LE") return "Legend Equip";
	if (x=="ME") return "Mythic Equip";
	if (x=="PS") return "PoS Perk";
	if (x=="AR") return "AR Perk";
	if (x=="MR") return "MiR Perk";
	if (x=="MP") return "MP Perk";
	if (x=="DO") return "Doom Perk";
	if (x=="FS") return y.substr(2)+" FS";
	if (x=="FW") return "Fortune Weapon";
	if (x=="XX") return "";
	clog<<x<<endl;
	exit(6969);
}

int main()
{
	ifstream player("resourceValues.txt");
	int torchCount;
	player>>torchCount;
	string filler;
	player>>filler>>filler;
	const int REC=20;
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
	vector<array<int,4>> latkl;
	vector<array<array<string,9>,9>> desc;
	for (int floor=1;floor<=floorCount;++floor)
	{
		int n,m;
		dung>>n>>m;
		array<int,4> cn={0,0,0,0};
		array<array<string,9>,9> cuu;
		for (int i=0;i<n;++i) for (int j=0;j<m;++j)
		{
			string y;
			dung>>y;
			string x=y.substr(0,2);
			if (x=="GK") ++cn[0];
			if (x=="GL") ++cn[1];
			if (x=="SK") ++cn[2];
			if (x=="SL") ++cn[3];
			cuu[i][j]=inSheet(y);
		}
		desc.push_back(cuu);
		latkl.push_back(cn);
	}
	latkl.push_back({0,0,0,0});
	for (int i=floorCount-1;i>=0;--i) for (int j=0;j<4;++j) latkl[i][j]+=latkl[i+1][j];
	dung.close();
	dung.open("dungeon.txt");
	dung>>floorCount;
	muldesc curState;
	curState[0][0][0][0]={{{},0}};
	vector<vector<vector<pii>>> optpoi;
	vector<ld> optval;
	vector<pii> dim,sta;
	array<int,4> dimens={1,1,1,1};
	for (int floor=1;floor<=floorCount;++floor)
	{
		curFloorForDebug=floor;
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
		auto startTime=chrono::high_resolution_clock::now();
		auto resEndEv=getRes(val,allo,start,pl,sk,sl,gk,gl,0);
		chrono::duration<double> diff=chrono::high_resolution_clock::now()-startTime;
		floordesc resEnd=resEndEv.x;
		array<int,4> dimcufl1=resEndEv.y;
		clog<<sce<<" possible scenarios completed."<<endl;
		clog<<"Got results if dungeon "<<floor<<" is the end in "<<diff.count()<<" seconds."<<endl;
		tottim+=diff.count();
		startTime=chrono::high_resolution_clock::now();
		{
			auto totalEndEv=addFloor(curState,resEnd,dimens,dimcufl1,{0,0},{0,0,0,0});
			diff=chrono::high_resolution_clock::now()-startTime;
			const muldesc&totalEnd=totalEndEv.x;
			array<int,4> dimifexno=totalEndEv.y;
			clog<<"Merged results if dungeon "<<floor<<" is the end in "<<diff.count()<<" seconds."<<endl;
			for (int cgk=0;cgk<dimifexno[0];++cgk) for (int cgl=0;cgl<dimifexno[1];++cgl) for (int csk=0;csk<dimifexno[2];++csk) for (int csl=0;csl<dimifexno[3];++csl)
			{
				if (cgl>cgk || csl>csk) continue;
				while (optpoi.size()<totalEnd[cgk][cgl][csk][csl].size()) optpoi.push_back({}),optval.push_back(-INF*20000);
				for (int toru=0;toru<(int)totalEnd[cgk][cgl][csk][csl].size();++toru) if (totalEnd[cgk][cgl][csk][csl][toru].y>optval[toru])
				{
					optval[toru]=totalEnd[cgk][cgl][csk][csl][toru].y;
					optpoi[toru]=totalEnd[cgk][cgl][csk][csl][toru].x;
				}
			}
		}
		if (floor<floorCount)
		{
			sce=0;
			int minGK=minLocks(allo,gl,start,exit);
			int minSK=minLocks(allo,sl,start,exit);
			startTime=chrono::high_resolution_clock::now();
			auto resNoEndEv=getRes(val,allo,start,pl,sk,sl,gk,gl,1,exit);
			diff=chrono::high_resolution_clock::now()-startTime;
			floordesc resNoEnd=resNoEndEv.x;
			array<int,4> dimcufl2=resNoEndEv.y;
			clog<<sce<<" possible scenarios completed."<<endl;
			clog<<"Got results if dungeon "<<floor<<" isn't the end in "<<diff.count()<<" seconds."<<endl;
			tottim+=diff.count();
			startTime=chrono::high_resolution_clock::now();
			auto curStateEv=addFloor(curState,resNoEnd,dimens,dimcufl2,{minGK,minSK},latkl[floor]);
			diff=chrono::high_resolution_clock::now()-startTime;
			curState=curStateEv.x;
			dimens=curStateEv.y;
			clog<<"Merged results if dungeon "<<floor<<" isn't the end in "<<diff.count()<<" seconds."<<endl;
		}
		sta.push_back(start);
	}
	torchCount=min(torchCount,(int)optval.size()-1);
	ofstream result("result.txt");
	ofstream inshee("intoSheet1.txt");
	inshee<<"\xEF\xBB\xBF";
	result<<"The highest obtainable value is "<<fixed<<setprecision(5)<<optval[torchCount]<<endl;
	result<<"You should explore "<<optpoi[torchCount].size()<<" floors."<<endl;
	result<<"Here are the cells you should unlock on each floor(S=start, 1=unlock, 0=don't unlock):"<<endl;
	for (int f=0;f<(int)optpoi[torchCount].size();++f)
	{
		int n=dim[f].x,m=dim[f].y;
		vector<vector<int>> re(n,vector<int>(m));
		for (auto x: optpoi[torchCount][f]) ++re[x.x][x.y];
		inshee<<"; ;A;B;C;D;E;F;G;H;I"<<endl;
		for (int i=0;i<n;++i,result<<endl)
		{
			inshee<<i+1;
			for (int j=0;j<m;++j)
			{
				if (sta[f]==make_pair(i,j))
				{
					if (re[i][j]!=1) exit(69420);
					result<<'S';
				}
				else result<<re[i][j];
				inshee<<";";
				if (re[i][j]) inshee<<desc[f][i][j];
				else inshee<<" ";
			}
			inshee<<";"<<endl;
		}
		result<<endl;
		inshee<<endl;
	}
	ld maxnet=optval[torchCount];
	int howmaxnet=torchCount;
	for (int i=torchCount;i<(int)optval.size();++i) if (optval[i]-(i-torchCount)*30*values["DI"]>maxnet)
	{
		howmaxnet=i;
		maxnet=optval[i]-(i-torchCount)*30*values["DI"];
	}
	if (howmaxnet!=torchCount)
	{
		ofstream insheeA("intoSheet2.txt");
		insheeA<<"\xEF\xBB\xBF";
		result<<"You can also buy "<<howmaxnet-torchCount<<" pet levels to get a value that's "<<optval[howmaxnet]-optval[torchCount]<<" higher, while only losing an equivalent of "<<(howmaxnet-torchCount)*30*values["DI"]<<" value on buying pets."<<endl;
		result<<"You should explore "<<optpoi[howmaxnet].size()<<" floors."<<endl;
		result<<"Here are the cells you should unlock on each floor(S=start, 1=unlock, 0=don't unlock):"<<endl;
		for (int f=0;f<(int)optpoi[howmaxnet].size();++f)
		{
			int n=dim[f].x,m=dim[f].y;
			vector<vector<int>> re(n,vector<int>(m));
			for (auto x: optpoi[howmaxnet][f]) ++re[x.x][x.y];
			insheeA<<"; ;A;B;C;D;E;F;G;H;I"<<endl;
			for (int i=0;i<n;++i,result<<endl)
			{
				insheeA<<i+1;
				for (int j=0;j<m;++j)
				{
					if (sta[f]==make_pair(i,j))
					{
						if (re[i][j]!=1) exit(69421);
						result<<'S';
					}
					else result<<re[i][j];
					insheeA<<";";
					if (re[i][j]) insheeA<<desc[f][i][j];
					else insheeA<<" ";
				}
				insheeA<<";"<<endl;
			}
			result<<endl;
			insheeA<<endl;
		}
	}
	clog<<tottim<<" seconds for annealing."<<endl;
}



