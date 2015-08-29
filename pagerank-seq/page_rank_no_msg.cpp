#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <sys/time.h>
using namespace std;

#define DUMP 0.85
#define NUM_VERTICES 1632803 
#define nthreads 240 

#include <omp.h>

double rtclock() //return the clock
{
	struct timezone Tzp;
	struct timeval Tp;
	int stat;
	stat = gettimeofday (&Tp, &Tzp);
	if (stat != 0) printf("Error return from gettimeofday: %d",stat);
	return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}

class graph
{
	size_t num_vertices;
	size_t num_edges;
	vector<int> vedges;	
	vector<int> vvertices;
	int *edges;
	int *vertices;
	float *rank;

	public:
		graph(){num_vertices = NUM_VERTICES; vvertices.resize(NUM_VERTICES, -1);}
		void read_graph(string filename);
		void page_rank();
		~graph();
};

graph::~graph()
{
	delete[] rank;	
}

void graph::read_graph(string filename)
{
	ifstream input(filename.c_str());
	string line;
	string first;
	int edge;
	
	while(getline(input, line))
	{
		//vvertices.push_back(vedges.size());		

		istringstream ss(line);
		ss >> first;
		int vid = atoi(first.substr(0, first.size() - 1).c_str()) - 1;

		vvertices[vid] = vedges.size();

		while(ss >> edge)
		{
			vedges.push_back(edge - 1);	
		}
	}

	num_edges = vedges.size();

	//a dummy last vertex should be appended to notify the end
	vvertices.push_back(num_edges);

	edges = &vedges[0];
	vertices = &vvertices[0];

	for(int i = num_vertices - 1; i >= 0; i--)
	{
		if(vertices[i] == -1)	
		{
			vertices[i] = vertices[i+1];	
		}
	}

	rank = new float[num_vertices];
	for(int i = 0; i < num_vertices; i++)
		rank[i] = 1.0;

	cout<<"read done..."<<endl;
	cout<<"num of vertices: "<<num_vertices<<endl;
	cout<<"num of edges: "<<num_edges<<endl;
	cout<<"memory space taken: "<< num_edges * 4 * 2 / 1048576 <<"MB"<<endl;
}

// For performance test only, and thus only includes a main loop.
void graph::page_rank()
{
	omp_set_num_threads(nthreads);

	omp_lock_t *locks = new omp_lock_t[num_vertices];
	for(int i = 0; i < num_vertices; i++)
		omp_init_lock(&locks[i]);

	int offset = 0;

	cout<<"doing page rank computation..."<<endl;

	// Use direct summation, instead of message storation to improve perf.
	vector<float> sum(num_vertices, 0);
	double before_msg = rtclock();
  #pragma omp parallel for schedule(dynamic,100) num_threads(nthreads)
	for(int i = 0; i < num_vertices; i++)
	{
		int n = vertices[i+1] - vertices[i];

		if(n)
		{
			float message = rank[i]/n;
			for(int j = vertices[i]; j < vertices[i + 1]; j++)
			{
				omp_set_lock(&locks[edges[j]]);
				sum[edges[j]] += message;
				omp_unset_lock(&locks[edges[j]]);
			}
		}
	}

	double after_msg = rtclock();
	cout<<"compute time: "<<after_msg - before_msg<<endl;
	cout<<"compute done..."<<endl;

  delete[] locks;
}

int main()
{
	//convert the input to csr fashion
	string filename("../pagerank1/input/soc-out.txt");
	graph g;
	g.read_graph(filename);
	g.page_rank();
}
