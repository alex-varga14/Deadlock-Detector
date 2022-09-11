// Author: Alexander Varga
// date: 26/11/2021
#include "mthread_deadlock_detector.h"
#include "common.h"
#include <iostream>
#include <unordered_map>
#include <vector>

class FastGraph {
private: 
	bool until(int index, bool visited[], bool *stack);
public:
	std::vector<std::vector<int>> adj_list;
	std::vector<int> out_counts;
	void addEdge(int p, int r);
	bool Cycle();
	
	//Result object to store results of deadlock detector
	Result result;
	//Word2Int object to implement FastGraph with unique int keys for node names
	Word2Int dict;
	//int to keep index edge being added from vector input argument
	int edge_index;
	//ints to store Word2Int hash for nodes in graph
	int process;
	int resource;
	//vector of ints to store all nodes in graph with outdegree of 0 as per hint recommendation
	//used to implement efficient topological sort
	std::vector<int> zeros;
	//vector of ints to copy out_counts vector so we can modify it as per hint recommendation
	//used to implement efficient topological sort
	std::vector<int> out;
	//string to hold the node being accessed
	std::string cursor;
	//set to hold keys and node string hashes provided by Word2Int dict
	std::unordered_map<int, std::string> myset;
	//Constant strings to differ between process's and resource's with the same name's
	const std::string p = "PROCESS";
	const std::string r = "RESOURCE";
} graph;
//Function to add edge to graph
void FastGraph::addEdge(int p, int r)
{
	//Add new incoming edge to graph
	graph.adj_list[r].push_back(p);
	graph.out_counts[p]++;
}
//Recursive helper function for Cycle(), determine if index'd node is part of a cycle
bool FastGraph::until(int index, bool visited[], bool *stack)
{
	if(visited[index] == false)
	{
		//Set edge as visited and place on stack
		visited[index] = true;
		stack[index] = true;
		//Loop for all edges adjacent
		int i = 0;
		do{
			i++;
			if (!visited[i] && until(i, visited, stack))
				return true;
			else if (stack[i])
				return true;
		} while(i < (int)adj_list.size());
	}
	//remove the edge from stack
	stack[index] = false;
	return false;
}
//Performs DFS to check if theres a cycle in the directed graph
//returns true if there is a cycle in the graph
bool FastGraph::Cycle()
{
	//Initialize size of visited and stack bool arrays 
	bool* visited = new bool[graph.edge_index];
	bool* stack = new bool[graph.edge_index];
	//Set all edges as not visited or on stack
	for(int i = 0; i < graph.edge_index; i++)
	{
		visited[i] = false;
		stack[i] = false;
	}
	//Recursively call helper function to detect cycle for the number of edges in the directed graph
	for(int i = 0; i < graph.edge_index; i++)
	{
		if(until(i, visited, stack))
			return true;
	}
	return false;
}

Result detect_deadlock(const std::vector<std::string>& edges)
{
    for (graph.edge_index = 0; graph.edge_index < (int)edges.size(); graph.edge_index++) 
	{
		//Get First edge and split into node, request or assignment, and another node, determine which is process
        std::vector<std::string> split_edge = split(simplify(edges[graph.edge_index]));
        int node = graph.dict.get(graph.p + split_edge[0]); 
        int node1 = graph.dict.get(graph.r + split_edge[2]); 
		//Store keys for nodes in myset
		graph.myset[node] = graph.p + split_edge[0];
		graph.myset[node1] = graph.r + split_edge[2];
		//Request edge
        if(split_edge[1] == "->")
		{
            graph.process = node;
            graph.resource = node1;
        } 
		else//Else Assignment edge
		{
            graph.process = node1;
            graph.resource = node;
        } 
		//Resize graph to new edges
        graph.out_counts.resize(graph.myset.size());
        graph.adj_list.resize(graph.myset.size());
		//Add new edge
		graph.addEdge(graph.process, graph.resource);
        //Copy out_counts
        graph.out = graph.out_counts;
		int index = 0;
		//Find all nodes in graph with outdegree 0 and push to zeros vector
		for(auto it: graph.out_counts)
		{
			if(it == 0)
				 graph.zeros.push_back(index);
			index++;
		}
        //While zeros is not empty
		int n2;
        while (!graph.zeros.empty()) 
		{
			//Set n2 to last element in zeros[]
            n2 = graph.zeros.back();
			//Remove one entry from zeros[]
            graph.zeros.pop_back();
			//loop for any n2 in adj_list[n]
            for (auto it : graph.adj_list[n2]) 
			{
                graph.out[it]--;
                if (graph.out[it] == 0) 
                    graph.zeros.push_back(it);
            }
        }
		//Loop through nodes 
		for (int i = 0; i < (int)graph.out.size(); i++) 
		{
			//If outcount is not 0, check for cycle
			if (graph.out[i] != 0) 
			{
				//Access index'd node from myset
				graph.cursor = graph.myset[i];
				//If cycle found, determine if its a process, if not continue
				if(graph.Cycle())
				{
					//Check for process distinction in entire string
					if ((int)graph.cursor.find(graph.p) != -1) {
						graph.cursor.replace(0, 7, "");
						graph.result.dl_procs.push_back(graph.cursor);
					}
					else continue;
				}
			}
		}
		//If result vector is not empty, cycle has been detected
		if(!graph.result.dl_procs.empty())
		{
			graph.result.edge_index = graph.edge_index;
			break;
		}
		else graph.result.edge_index = -1;
    }
    return graph.result;
}