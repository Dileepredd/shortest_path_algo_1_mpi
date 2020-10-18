#include<bits/stdc++.h>
#include<mpi/mpi.h>
using namespace std;
int main(int argc,char** argv)
{
    /*
    componnt 1:
    input: argc, argv;
    output: 
    nodes - number of nodes.
    procs - number of processes.
    rank - rank of the current process.
    source - source node.
    destination - destination node.
    */
    int procs, nodes, rank, source, destination;
    // mpi environtment initialization.
    MPI_Init(&argc, &argv);
    // initializing rank of the current matrix(rank).
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // initializing the number of processes(procs)
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    // verifiying the input is correct.(argc)
    if(argc < 5)
    {
        cout<<"-s(source), -d(destination) are not specified"<<endl;
        abort();
    }
    // using input (argv) to generate (nodes), (source), (destination).
    for(int i=1;i<argc;i+=2)
    {
        if(strcmp(argv[i],"-s") == 0)
        {
            source = atoi(argv[i+1]);
        }
        if(strcmp(argv[i],"-d") == 0)
        {
            destination = atoi(argv[i+1]);
        }
    }
    FILE* fp = fopen("dump","rb");
    if(fp == nullptr)
    {
        cout<<"fp is null"<<endl;
        abort();
    }
    fread(&nodes,1,sizeof(int),fp);
    
    // presenting the information to the user.
    if(rank == 0)
    {
        cout<<"number of processes: "<<procs<<endl;
        cout<<"number of nodes in graph: "<<nodes<<endl;
        cout<<"source node in the graph: "<<source<<endl;
        cout<<"destination node in the graph: "<<destination<<endl;
    }
    // bounds check on the source and destination.
    if(source < 0 || source >= nodes)
    {
        cout<<"Error: source out of bounds"<<endl;
        abort();
    }
    if(destination < 0 || destination >= nodes)
    {
        cout<<"Error: destination out of bounds"<<endl;
        abort();
    }
    if(nodes%procs != 0)
    {
        cout<<"Error: nodes ("<<nodes<<") in the graph are not muitple of processes("<<procs<<")"<<endl;
        abort();
    }
    /*
    component 2:
    input: filename
    filename - contains dump of graph.(dump - binary file)
    output: matrix a, matrix b, matrix c.
    matrix a - should contain actual graph.
    matrix b - contains SP(n-1).
    matrix c - is a buffer that contains SP(n).
    */
    // file open to load graph in it.
    // matrix a initialization.
    int (*a)[nodes][nodes];
    a = (int (*)[nodes][nodes])new int[nodes*nodes];
    fread(a,nodes*nodes,sizeof(int),fp);
    fclose(fp);
    // matrix b,c declaration and initialization.
    int (*b)[nodes][nodes];
    int (*c)[nodes][nodes];
    b = (int (*)[nodes][nodes])new int[nodes*nodes];
    c = (int (*)[nodes][nodes])new int[nodes*nodes];
    for(int i=0;i<nodes;i++)
    {
        for(int j=0;j<nodes;j++)
        {
            (*b)[i][j] = (*a)[i][j];
            (*c)[i][j] = (*a)[i][j];
        }
    }
    /*
    component 3:
    input : matrix a.
    output : shortest path between source and destination initial check.
    */
    if((*a)[source][destination] > 0)
    {
        if(rank == 0)
        cout<<"shortest path length from source "<<source<<" to destination "<<destination<<" is: "<<"1"<<endl;
        return 0;
    }
    /*
    component 4:
    input: start,end,source,destination.
    start: row from where the process starts multiplying(including).
    end: row untill where the process multiplies(excluding).
    output: shortest path length.
    */
    int start = rank*(nodes/procs);
    int end = start + nodes/procs;
    // from SP(2) to SP(nodes-1).
    // a = SP(1); b = SP(i-1); c = SP(i);
    // SP(i) = SP(i-1)*SP(1);
    for(int i=2;i<nodes;i++)
    {
        for(int j=start;j<end;j++)
        {
            for(int k=0;k<nodes;k++)
            {
                (*c)[j][k] = 0;
                for(int l=0;l<nodes;l++)
                {
                    (*c)[j][k] += (*b)[j][l]*(*a)[l][k];
                }
            }
        }
        if(rank == 0)
        {
            for(int j=1;j<procs;j++)
            {
                MPI_Status status;
                int *ptr = &((*c)[j*(end-start)][0]);
                MPI_Recv(ptr,nodes*(end-start),MPI_INT,j,i,MPI_COMM_WORLD,&status);
            }
        }
        else
        {
            // send
            int *ptr = &((*c)[start][0]);
            MPI_Send(ptr,nodes*(end-start),MPI_INT,0,i,MPI_COMM_WORLD);
        }
        //
        if(rank == 0)
        {
            if((*c)[source][destination] > 0)
            {
                cout<<"shortest path length from source "<<source<<" to destination "<<destination<<" is: "<<i<<endl;
                return 0;
            }
        }
        
        int (*temp)[nodes][nodes] = b;
        b = c;
        c = temp;
    }
    if(rank == 0)
    {
        cout<<"shortest path length from source "<<source<<" to destination "<<destination<<" is: "<<"infinite"<<endl;
    }
    MPI_Finalize();
    return 0;
}