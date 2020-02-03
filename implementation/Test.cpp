#include<bits/stdc++.h>
using namespace std;

typedef long long int lli;
typedef unsigned long long int ulli;
typedef pair<int,int> pii;
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

const double del1 = 0.9, del2 = 0.99;

map<string,string>attack_class;
vector<string>Features;
vector<string>classes = {"normal", "dos", "u2r", "r2l", "probe"};

map<string, vector<string> >training_data;

vector<map<string,int> >feature_data_count(44);
map<string, vector<map<string,int> > > dicision_data_count; ///attack wise

int data_count = 0;


void Classify_Attack()
{

    ifstream in;

    in.open("attack_type.txt");
    string attack;

    attack_class["normal"] = "normal";

    while(in>>attack)
    {

        string type;
        in>>type;

        attack_class[attack] = type;

    }
    in.close();
}

void Input_features()
{
    ifstream in;

    in.open("feature.txt");
    string f;

    while(in>>f)
    {
        Features.pb(f);
    }
    //cout<<Features.size(); // should be 41
    in.close();
}

void Update_Feature_data(string d, string attack)
{
    string val;
    int fno = 0;

    for(int i=0; i<d.size(); i++)
    {
        if(d[i] == ',' || i == d.size()-1)
        {
            feature_data_count[fno][val]++;
            if(dicision_data_count[attack].size() == fno)
            {
                map<string,int>c;
                dicision_data_count[attack].pb(c);
            }
            dicision_data_count[attack][fno][val]++;
            val.clear();
            fno++;
        }
        else val += d[i];
    }
}

void Input_training_data()
{
    ifstream in;

    in.open("kdd_10_percent.txt");
    string d;

    while(in>>d)
    {

        int comma = 0;
        data_count++;

        for(int i=0; i<d.size(); i++)
        {
            if(d[i] == ',')
                comma = i;
        }
        string attack = d.substr(comma+1);
        attack.pop_back();// pop '.'
        while(d.size()>comma)
            d.pop_back();

        Update_Feature_data(d,attack);

        if(attack_class[attack].size() == 0)
        {
            cout<<"attack is not classified....";
            exit(0);
        }

        training_data[attack].pb(d);
    }

    in.close();
}

double I(string Y, int X)   ///mutual info betwn feature x and decision Y (i.e dos)
{
    vector<string>ys;

    for(auto a : training_data)
    {
        if(attack_class[a.fi] == Y)
            ys.pb(a.fi);
    }
    double ret = 0;

    for(auto x : feature_data_count[X])
    {
        for(string y : ys)
        {
            double denom = x.se;
            double nom = dicision_data_count[y][X][x.fi];
            double fxy = 0;

            if(denom > 0)
                fxy= nom/denom;

            assert(data_count>0);

            double fx = x.se/(double)data_count;
            double fy = training_data[y].size()/(double)data_count;

            assert(fxy<=1 && fx<=1 && fy<=1);

            if(fx*fy && fxy) ret += fxy * log(fxy/(fx*fy));
        }
    }
    //cout<<ret<<endl;
    return ret;
}

double Hy(string Y){ /// i.e dos

    vector<string>ys;
    double cnt = 0;

    for(auto a : training_data)
    {
        if(attack_class[a.fi] == Y){
            ys.pb(a.fi);
            cnt += training_data[a.fi].size();
        }
    }

    double ret = 0;

    for(string y : ys){

        double pi = 1/(double)ys.size();

        if(pi>0) ret += pi * log(1.0/pi);
    }
    return ret;
}

double Qy(int Xi , int Xj , string Y , double h){

    double ret = 0;

    if(h>0){
        double II= I(Y,Xi) + I(Y, Xj) , r = 1/(double)max(50,rand());

        ret = (II-II*r)/h;
    }

    return ret;
}

double e(vector<int>S , string Y){

    if(S.size() == 0) return 0;

    double ret = 0;

    double h = Hy(Y) , SXi = 0;

    for(int xi : S) SXi += I(Y,xi);

    if(h>0) ret = SXi/h;

    for(int xi : S){

        for(int xj : S){

            if(xi == xj) continue;

            ret -= Qy(xi,xj,Y,h);
        }
    }
   // if(S.size() == 1) ret = h;

    return ret;
}

void Ranked_Features(string Y)
{
    vector<int>R; /// relevant feature set;

    for(int Xi=0 ; Xi<Features.size(); Xi++)
    {
        double now = I(Y,Xi);
      //  cout<<now<<endl;

        if(now >= del1){
            R.pb(Xi);
          //  cout<<Xi<<endl;
        }
    }

    vector<int>W; /// working set
    vector<int>G; /// goal set

    W = R;

    //cout<<Y<<" "<<W.size()<<endl;
    double h = Hy(Y);

    while(e(G,Y) < del2){

       // cout<<W.size()<<endl;

        if(W.size() == 0) break;

        //cout<<e(G,Y)<<endl;

        for(int xk : W){

            bool ok = true;

            for(int xl : W){

                if(xl == xk) continue;

                ok = ( I(Y,xk)>=I(Y,xl) );

                if(ok == false) break;
            }

            if(ok == false) continue;

            for(int xn : G){

                for(int xm : W){

                    if(xm == xk) continue;

                    ok = ( Qy(xk,xm,Y,h) <= Qy(xm,xn,Y,h) );

                    if(ok == false) break;
                }
            }

            if(ok){
                vector<int>temp;

                for(int i : W){
                    if(i != xk) temp.pb(i);
                }
                W = temp;
                G.pb(xk);
                break;
            }
        }
    cout<<setprecision(12)<<fixed;
        cout<<" --- "<<e(G,Y)<<endl;
        //cout<<W.size()<<endl;
    }
    cout<<Y<<" "<<G.size()<<endl;
    for(int g : G) cout<<Features[g]<<" ";
    cout<<endl;
}

int main()
{
    ///freopen("output.txt","w",stdout);
    ///freopen("input.txt","r",stdin);

    //FastIO;

    Classify_Attack();
    Input_features();
    Input_training_data();

    while(feature_data_count.size()>Features.size()) feature_data_count.pop_back();

    /*for(string Y : classes)
    {

        if(Y == "normal")
            continue;


        Ranked_Features(Y);

    }*/



    for(int i=0; i<feature_data_count.size() ; i++){

        cout<<Features[i]<<" -> ";

        for(auto it : feature_data_count[i]) cout<<"["<<it.fi<<" "<<it.se<<"] ";

        cout<<endl;
    }



    return 0;

}

///sin and cos expect input in radians not degrees. so use , sin(degrees * pi / 180)
///using bs = bitset<MX>; // how many distinct number can be form?

