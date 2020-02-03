#include<bits/stdc++.h>
using namespace std;

typedef long long int lli;
typedef unsigned long long int ulli;
typedef pair<int,int> pii;
typedef pair<string,string> pss;
typedef pair<string,double> psd;
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

double del1 = 0.7, del2 = 0.6;

vector<string>classes = {"dos", "u2r", "r2l", "probe"};

map<string, double> attackProb;
map<string,string>attack_class;
map<string, map<pss,double> >fxy;
map<string,vector<psd> >feature_x;
map<string , vector<string> > sub_attacks;

void Classify_Attack()
{

    ifstream in;

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

void Input_Attack_Prob(){

    ifstream in;

    in.open("attackprob.txt");

    string attack;
    double p;

    while(in>>attack)
    {
        in>>p;
      //  cout<<attack<<" "<<p<<endl;
        if(attack.back() == '.') attack.pop_back();

        attackProb[attack] =  p/100.0;

    }
    in.close();
}

void Input_Feature_Prob(){

    ifstream in;

    in.open("fxy.txt");
	int cnt;
    string feature;
    while(in>>feature)
    {
        in>>cnt;
        string atr, attack;
        double prob;
        while(cnt--){
            in>>atr>>attack>>prob;

            fxy[feature][pss(atr,attack)] = prob;
        }
    }

    in.close();
}

void Input_Feature_atr(){

    ifstream in;

    in.open("attribut_featureprob.txt");
	int cnt;
    string feature;
    while(in>>feature)
    {
        in>>cnt;
        string atr;
        double prob;
        while(cnt--){
            in>>atr>>prob;
            feature_x[feature].pb(psd(atr,prob));
        }
    }

    in.close();
}

void Store_sub_attack(){

    for(auto a : attack_class)
    {
        sub_attacks[a.se].pb(a.fi);
    }

}


double I(string Y, string X)   ///mutual info betwn feature X and decision Y (i.e dos)
{


    double ret = 0;

    for(auto x : feature_x[X])
    {
        for(string y : sub_attacks[Y])
        {
            double f_x_y = fxy[X][pss(x.fi,y)];
            double fx = x.se;
            double fy = attackProb[y];

            if(fx*fy > 0 && f_x_y>0)ret += f_x_y * log(f_x_y/(fx*fy));
        }
    }
    //cout<<ret<<endl;
    return ret;
}

double Hy(string Y){ /// i.e dos

    double ret = 0;

    for(string y : sub_attacks[Y]){

        double pi = attackProb[y];

        if(pi>0) ret += pi * log(1.0/pi);
    }
    return ret;
}

double Qy(string Xi , string Xj , string Y , double hy){

    double ret = 0;

    if(hy>0){
        double II= I(Y,Xi) + I(Y, Xj); /// ??? I(Y:Xi,Xj)

        ret = II/hy;
    }

    return ret;
}

double e(vector<string>S , string Y){

    if(S.size() == 0) return 0;

    double ret = 0;

    double h = Hy(Y) , SXi = 0;

    for(string xi : S) SXi += I(Y,xi);

    if(h>0) ret = SXi/h;

    for(string xi : S){

        for(string xj : S){

            if(xi == xj) continue;

            ret -= Qy(xi,xj,Y,h);
        }
    }
   // if(S.size() == 1) ret = h;

    return ret;
}

void Ranked_Features(string Y)
{
    vector<string>R ; /// relevant feature set;

    for(auto it : feature_x)
    {
        string Xi = it.fi;
        //cout<<I(Y,Xi)<<" "<<endl;
        if(I(Y,Xi)<=del1){
            R.pb(Xi);
        }
    }

    vector<string>W; /// working set
    vector<string>G; /// goal set

    W = R;


    double h = Hy(Y);

    //cout<<(e(G,Y)<del2)<<" "<<W.size()<<endl;
    while( e(G,Y)<del2){

        if(W.size() == 0) break;

        for(string xk : W){

            bool ok = true;

            for(string xl : W){

                if(xl == xk) continue;

                ok = ( I(Y,xk)>=I(Y,xl) );

                if(ok == false) break;
            }

            if(ok == false) continue;

            for(string xn : G){

                for(string xm : W){

                    if(xm == xk) continue;

                    ok = ( Qy(xk,xm,Y,h) <= Qy(xm,xn,Y,h) );

                    if(ok == false) break;
                }
            }

            if(ok){
                vector<string>temp;

                for(string i : W){
                    if(i != xk) temp.pb(i);
                }
                W = temp;
                G.pb(xk);
                break;
            }
        }
        //cout<<setprecision(12)<<fixed;
        //cout<<" --- "<<e(G,Y)<<endl;
        //cout<<W.size()<<endl;
    }

    cout<<Y<<" "<<G.size()<<endl;
    for(string g : G) cout<<g<<" ";
    cout<<endl<<endl;
}

int main()
{
    ///freopen("output.txt","w",stdout);
    ///freopen("input.txt","r",stdin);

    //FastIO;
    Classify_Attack();
    Input_Attack_Prob();
    Input_Feature_Prob();
    Input_Feature_atr();
    Store_sub_attack();


    for(string a : classes){
        Ranked_Features(a);
    }


    return 0;

}

///sin and cos expect input in radians not degrees. so use , sin(degrees * pi / 180)
///using bs = bitset<MX>; // how many distinct number can be form?

