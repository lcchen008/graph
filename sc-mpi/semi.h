#ifndef SEMI_H_
#define SEMI_H_

//max num of clusters
#define CMAX 4 

//max num of vertices in each cluster
#define VMAX 4 
#define MMAX 4 
#define SORT_MAX 10
#define fB 0.5

#define CLUSTER_PER_MSG 5 

//used as a vertex list for each cluster
struct cluster 
{
	float score;
	int num_vertices;
	int vertices[VMAX];		

	cluster()
	{
		num_vertices = 0;
	}

	void insert(size_t vertex_id)
	{
		if(num_vertices == VMAX)
			return;
		vertices[num_vertices] = vertex_id;		
		num_vertices++;
	}

	int &operator[](int idx)
	{
		return vertices[idx];	
	}
	
	int size(){return num_vertices;}
};

//used as vertex value: a list of clusters
struct cluster_list
{
	int num_clusters;
	cluster clusters[CMAX];

	cluster_list()
	{
		num_clusters = 0;
	}

	void insert(cluster &c)
	{
		if(num_clusters == CMAX)
			return;
		clusters[num_clusters] = c;		
		num_clusters++;
	}

	void clear()
	{
		num_clusters = 0;	
	}

	cluster &operator[](int idx)
	{
		return clusters[idx];	
	}

	int size(){return num_clusters;}
};

struct msg_cluster_list
{
	int num_clusters;
	cluster clusters[CLUSTER_PER_MSG];

	msg_cluster_list()
	{
		num_clusters = 0;
	}

	void insert(cluster &c)
	{
		if(num_clusters == CLUSTER_PER_MSG)
			return;
		clusters[num_clusters] = c;		
		num_clusters++;
	}

	cluster &operator[](int idx)
	{
		return clusters[idx];	
	}

	int size(){return num_clusters;}
};

#endif
