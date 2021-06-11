#include<bits/stdc++.h>
#include<fstream>
#define V 26
using namespace std;

struct node
{
    vector<int> path;
    float fitness;
    float prob;
    bool mutate;
    float cost;
};
ifstream fin;

vector<node> population;
vector<node> selected;
vector<node> children;
vector<node> mutated;
node best;
int graph[V][V];
bool compP(node a,node b)
{
    return a.prob>b.prob;
}
bool compF(node a,node b)
{
    return a.fitness>b.fitness;
}

int generateRandomNo(int start,int end)
{
    int r=end-start;
    int no=start + rand() % r;
    return no;
}

bool repeated(int num,vector<int> s)
{
    
    for(int i=1;i<s.size();i++)
    {
        if(num==s[i])
        {
            return true;
        }
    }
    return false;
}

vector<int> randPathGenerator()
{
    //string s="0";
	vector<int> s;
	s.push_back(0);
    while(true)
    {
        if(s.size()==V)
        {
            s.push_back(0);
            break;
        }
        int rnum=generateRandomNo(1,V);
        if(!repeated(rnum,s))
        {
            s.push_back(rnum);
        }
    }
    //cout<<s<<endl;
    return s;
}


float calcFitness(vector<int> s)
{
    float sum=0;
    for(int i=1;i<s.size();i++)
    {
        sum=sum+graph[s[i-1]][s[i]];
    }
    //cout<<sum<<endl;
    float fitness=1/sum;
    if(best.fitness<fitness)
    {
        best.path=s;
        best.fitness=fitness;
        best.cost=sum;
    }
    return fitness;
}


node makenode(vector<int> a)
{
    node tspNode;
    tspNode.path = a;
    tspNode.fitness = calcFitness(tspNode.path);
    tspNode.mutate = false;
    tspNode.cost = 1/(tspNode.fitness);
    return tspNode;
}

void assignProb(float total,vector<node> &vec)
{
    for(int i=0;i<vec.size();i++)
    {
        vec[i].prob = vec[i].fitness / total ;
    }
}


void createPopulation(int popSize)
{
    float sumFitness = 0;
    for(int i=0;i<popSize;i++)
    {
        population.push_back(makenode(randPathGenerator()));
        sumFitness = sumFitness + population[i].fitness;
    }
    
    assignProb(sumFitness,population);
    sort(population.begin(),population.end(),compP);
}


void selectPopulation(int favoured)
{
	
    for(int i=0;i<favoured && i<population.size();i++)
    {
        selected.push_back(population[i]);
        //cout<<population[i].path<<endl;
    }
    
    double r = ((double) rand() / (RAND_MAX));
    
    for(int i=favoured;i<population.size();i++)
    {
        if(r<population[i].prob)
        {
            selected.push_back(population[i]);
            //cout<<population[i].path<<endl;
        }
    }
    population.clear();
}


node crossover(vector<int> a,vector<int> b)
{
    int m = generateRandomNo(1,V-1);
    int n = generateRandomNo(1,V-1);
    while(m==n)
    {
        n = generateRandomNo(1,V-1);
    }
    int start = min(m,n);
    int end = max(m,n);
    //cout<<"Parent : "<<a<<" "<<b;
    //cout<<"Start : "<<start<<endl<<"End : "<<end<<endl;
    vector<int> child = b;
    for(int i=start;i<=end;i++)
    {
        child[i]=a[i];
    }
    int flag=0,c_ptr=1;
    for(int i=1;i<b.size()-1 && c_ptr<b.size()-1;i++)
    {
    	if(c_ptr==start)
        {
            c_ptr=c_ptr+end-start+1;
        }
        flag=0;
        for(int j=start;j<=end;j++)
        {
            if(a[j]==b[i])
            {
                flag=1;
                break;
            }
        }
        if(flag==0)
        {
            child[c_ptr]=b[i];
            c_ptr++;
        }
        
    }
    return makenode(child);
}


void generateChild(int favoured)
{
    int n=selected.size();
    
    for(int i=0;i<favoured && i<n;i++)
    {
        children.push_back(selected[i]);
        //cout<<selected[i].path<<endl;
    }
    //cout<<".";
    for(int i=0;i<n/2;i++)
    {
    	node child = crossover(selected[i].path,selected[n-i-1].path);
        children.push_back(child);
        //cout<<child.path<<endl;
    }
    sort(children.begin(),children.end(),compF);
    selected.clear();
}


node mutation(node child,float mutationRate)
{
    
    for(int i=1;i<child.path.size()-1;i++)
    {
        double r = ((double) rand() / (RAND_MAX));
        //cout<<"Random no : "<<r<<endl;
        if(r<mutationRate)
        {
            int rnum = generateRandomNo(1,V-1);
            char ch = child.path[i];
            child.path[i] = child.path[rnum];
            child.path[rnum] = ch;
            child.mutate=true;
        }
    }
    if(child.mutate)
    {
        child.fitness=calcFitness(child.path);
        //cout<<"Mutated : "<<child.path<<endl;
    }
    
    return child;
}

void mutationSet(float mutationRate,int favoured)
{
    float sumFitness=0;
    for(int i=0;i<favoured && i<children.size();i++)
    {
        mutated.push_back(children[i]);
        sumFitness=sumFitness+children[i].fitness;
    }
    for(int i=0;i<children.size();i++)
    {
        node mutatedChild=mutation(children[i],mutationRate);
        mutated.push_back(mutatedChild);
        sumFitness=sumFitness+mutatedChild.fitness;
    }
    assignProb(sumFitness,mutated);
    sort(mutated.begin(),mutated.end(),compP);
    children.clear();
    population=mutated;
}


void repeatFunction(int gen,int popSize,int favoured,float mutationRate)
{
	for(int i=0;i<gen;i++)
	{
		//cout<<endl<<endl<<"Selected : "<<endl;
		selectPopulation(favoured);
		generateChild(favoured);
		mutationSet(mutationRate,favoured);
		mutated.clear();
		//cout<<endl<<i<<" Cost is : "<<best.cost;
	}
}

int main()
{

	fin.open("input1.txt");
	//int graph[V][V];
	for(int i=0;i<V;i++)
	{
		for(int j=0;j<V;j++)
		{
			fin>>graph[i][j];
		}
	}
	
    int gen,popSize,favoured;
    float mutationRate;
    cout<<"No of generations : ";
    cin>>gen;
    cout<<"Population size : ";
    cin>>popSize;
    cout<<"Size of favoured paths : ";
    cin>>favoured;
    cout<<"Mutation Rate : ";
    cin>>mutationRate;
    best.path.push_back(0);
    best.fitness=0;
    best.mutate=false;
    createPopulation(popSize);
    repeatFunction(gen,popSize,favoured,mutationRate);
    
	cout<<endl<<"Cost is : "<<best.cost<<endl<<"Fitness is : "<<best.fitness<<endl;
	cout<<"Optimal Path : ";
	for(int i=0;i<best.path.size();i++)
	{
		cout<<best.path[i]<<" ";
	}
	cout<<endl;
    cout<<"TIME TAKEN : "<<(float)clock()/CLOCKS_PER_SEC<<" secs "<<endl;
    return 0;
}
