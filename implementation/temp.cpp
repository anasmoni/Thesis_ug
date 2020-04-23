
#include<bits/stdc++.h>
using namespace std;

typedef long long int lli;
typedef unsigned long long int ulli;
typedef pair<int,int> pii;
typedef pair<string,string> pss;
typedef priority_queue< int, vector< int >, greater< int > >  minHeap;

#define ones(x) __builtin_popcount(x)
#define onesl(x) __builtin_popcountl(x)
#define onesll(x) __builtin_popcountll(x)

#define pb push_back
#define eb emplace_back
#define mp make_pair
#define mt make_tuple
#define scn(n) scanf("%d",&n)
#define scnll(n) scanf("%lld",&n)
#define scn2(n,m) scanf("%d%d",&n,&m)
#define scn3(n,m,w) scanf("%d%d%d",&n,&m,&w)
#define scn2ll(n,m) scanf("%lld%lld",&n,&m)
#define atoz(v) v.begin(),v.end()
#define ratoz(v) v.rbegin(),v.rend()
#define Fill(a,v) memset(a,v,sizeof(a))
#define sz(v) v.size()
#define fi first
#define se second
#define inf 1e9
#define sqr(x) x*x
#define max3(a,b,c) max(a,max(b,c))
#define min3(a,b,c) min(a,min(b,c))
#define ten(n) (int)1e##n
#define tenll(n) (lli)1e##n
#define mod 1000000007
#define FastIO ios_base::sync_with_stdio(false); cin.tie(0); cout.tie(0);

int Set(int N,int pos)
{
    return N=N | (1<<pos);
}
int reset(int N,int pos)
{
    return N = N & ~(1<<pos);
}
bool check(int N,int pos)
{
    return (bool)(N & (1<<pos));
}


inline int addmod(int x,int y)
{
    return (x%mod + y%mod)%mod;
}
inline int submod(int x,int y)
{
    return (x%mod - y%mod + mod)%mod;
}
inline int mulmod(int x,int y)
{
    return (x%mod *1LL* y%mod)%mod;
}
inline int nextSubMask(int i, int mask)
{
    return (i-1)&mask;   /// returns next smallest submask
}

void we_r_done()
{
    cout<<"NO";
    exit(0);
}

void debug(bool ok, string mssg)
{
    if(ok)
        return;
    cout<<mssg;
    exit(0);
}
map<string,string>attack_class;

void Classify_Attack()
{

    ifstream in;
    attack_class["normal"] = "normal";
    in.open("attack_type.txt");
    string attack;

    while(in>>attack)
    {

        string type;
        in>>type;

        attack_class[attack] = type;

    }
    in.close();
}


void fun()
{
    ifstream in;

    in.open("kdd_10_percent.txt");
    string d;

    //ofstream out;
    //out.open("kdd_10_attack_classified.txt");

    map<string,int>cnt;
    while(in>>d)
    {
        string now ;
        vector<string>they;

        for(int i=0; i<d.size(); i++)
        {
            if(d[i] == ',' ){
                now.clear();
            }
            else now += d[i];
        }
        now.pop_back();
        //cout<<now<<" "<<attack_class[now]<<endl;
        while(d.back() != ',') d.pop_back();
        d += attack_class[now];
        cnt[attack_class[now]]++;
       // out<<d<<endl;

    }
    lli tot  = 0;

    for(auto it : cnt) tot += it.se;
    cout<<"KDD 10%\n\nTotal data : "<<tot<<"\n\n";
    cout<<"class\tcount\tpercentage\n\n";
    for(auto it : cnt) cout<<it.fi<<"\t"<<it.se<<"\t"<<(100.0*it.se)/(1.0*tot)<<"%\n\n";
    in.close();
    //out.close();
}


int main()
{
    ///freopen("output.txt","w",stdout);
    ///freopen("input.txt","r",stdin);

    //FastIO;
    Classify_Attack();
    fun();


    return 0;

}

///sin and cos expect input in radians not degrees. so use , sin(degrees * pi / 180)
///using bs = bitset<MX>; // how many distinct number can be form?

