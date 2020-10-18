#include<bits/stdc++.h>
using namespace std;
int main(int argc,char** argv)
{
    int nodes,edges;
    if(argc < 5)
    {
        cerr<<"-n(# of nodes) and -e(# of edges) is to be specified"<<endl;
        abort();
    }
    if(strcmp(argv[1], "-n") == 0)
    {
        nodes = atoi(argv[2]);
        edges = atoi(argv[4]);
    }
    else
    {
        nodes = atoi(argv[4]);
        edges = atoi(argv[2]);
    }
    cout<<"number of nodes in the graph: "<<nodes<<endl;
    cout<<"number of edges in the graph: "<<edges<<endl;
    FILE* fp = fopen("dump","wb");
    // graph generator.
    fwrite(&nodes,1,sizeof(int),fp);
    int (*a)[nodes][nodes];
    a = (int (*)[nodes][nodes])new int[nodes*nodes];
    for(int i=0;i<nodes;i++)
    {
        for(int j=0;j<nodes;j++)
        {
            (*a)[i][j] = 0;
        }
    }
    // random edges generator
    for(int i=0;i<edges;i++)
    {
        int s = rand()%nodes;
        int d = rand()%nodes;
        (*a)[s][d]++;
        (*a)[d][s]++;
    }
    fwrite(a,nodes*nodes,sizeof(int),fp);
    fclose(fp);
    cout<<"graph is dumpped into the binary file: dump"<<endl;
    /*
    fp = fopen("dump","rb");
    int n;
    fread(&n,1,sizeof(int),fp);
    int (*b)[n][n];
    b = (int (*)[n][n]) new int[n*n];
    fread(b,n*n,sizeof(int),fp);
    fclose(fp);
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
        {
            cout<<(*b)[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
    */
    return 0;
}
