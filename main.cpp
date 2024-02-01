#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <tuple>
#include <unordered_set>
#include <fstream>
#include <string>
#include <format>
#include <unordered_map>

using namespace std;

bool GetBit(int num, int i)
{
	return ((num & (1 << i)) != 0);
}

int SetBit(int num, int i)
{
	return num | (1 << i);
}

class Node
{
public:
	Node(int node)
	{
		this->node = node;
		path = { node };
		visited = SetBit(visited, node);
	}

	vector<int> path;
	int node;
	int possible_cost = 0;
	int accumulated_cost = 0;
	int visited = 0;
};

class Compare
{
public:
	bool operator()(Node u, Node v)
	{
		return u.possible_cost > v.possible_cost;
	}
};

class CompareTuple
{
public:
	bool operator()(tuple<int, int, int> u, tuple<int, int, int> v)
	{
		return get<2>(u) > get<2>(v);
	}
};

struct HashFunction
{
	size_t operator()(const pair<int, int>& x) const
	{
		return x.first ^ x.second;
	}
};

void BuildGraph(int n, vector<vector<int>>& graph, istream& input)
{
	int num;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			input >> num;
			graph[i][j] = num;
		}
	}
}

int PossibleCost(vector<vector<int>> graph, int visited, int node, int last, int start)
{	
	int possibleCost = 0;
	for (int i = 0; i < graph.size(); i++)
	{	

		if (GetBit(visited, i) && i != node) continue;

		int min = -1;
		for (int j = 0; j < graph.size(); j++)
		{
			if ((!GetBit(visited, j) || j == start) &&
				(min == -1 || min > graph[i][j]) &&
				i != j && (j != last || i != node))
				min = graph[i][j];
		}

		if (min != -1) possibleCost += min;
	}

	return possibleCost;
}

void HamiltonianCycle(vector<vector<int>>& graph, int start)
{
	vector<int> optimal_path;
	int optimal_cost = -1;
	int n = graph.size();
	priority_queue<Node, vector<Node>, Compare> pq;
	Node u(start);
	u.possible_cost = PossibleCost(graph, u.visited, start, -1, start);
	pq.push(u);


	while (!pq.empty())
	{	
		u = pq.top();
		pq.pop();
		if (optimal_cost == -1 || optimal_cost > u.possible_cost)
		{
			for (int i = 0; i < graph.size(); i++)
			{
				if (!GetBit(u.visited, i) && graph[u.node][i] != -1)
				{
					Node v(u);
					v.node = i;
					v.visited = SetBit(v.visited, v.node);
					v.path.push_back(v.node);
					v.accumulated_cost += graph[u.node][v.node];
					v.possible_cost = PossibleCost(graph, v.visited, v.node, u.node, start) + v.accumulated_cost;

					if (v.path.size() == n && graph[v.node][start] != -1)
					{
						v.accumulated_cost += graph[v.node][start];
						v.path.push_back(start);

						if (v.accumulated_cost < optimal_cost || optimal_cost == -1)
						{
							optimal_path = v.path;
							optimal_cost = v.accumulated_cost;
						}
					}
					else
					{
						pq.push(v);
					}
				}
			}
		}
	}

	cout << "Minimum cost: " << optimal_cost << endl;
	cout << "Path: ";

	for (int i = 0; i <= n; i++)
	{
		cout << optimal_path[i] << " ";
	}

	cout << endl;
}

void BFS(vector<vector<int>>& graph, unordered_set<pair<int, int>, HashFunction>& valid_edges, vector<int>& levels, int start, int end)
{
	queue<int> q;
	int curr_level = 0;
	int parents = 1;
	q.push(start);
	levels[start] = curr_level;

	while (!q.empty())
	{
		int node = q.front();
		q.pop();

		if (node == end) continue;

		for (int i = 0; i < graph.size(); i++)
		{
			if (graph[node][i] > 0 && levels[i] == -1)
			{
				levels[i] = curr_level + 1;
				q.push(i);
				valid_edges.insert({ node, i });
			}
		}

		parents--;
		if (parents == 0)
		{
			parents = q.size();
			curr_level++;
		}
	} 

	return;
}

int DFS(vector<vector<int>>& graph, unordered_set<pair<int, int>, HashFunction> valid_edges, int node, int end, int flow)
{
	if (node == end) return flow;

	for (int i = 0; i < graph.size(); i++)
	{
		int cap = graph[node][i];
		if (cap > 0 && valid_edges.find({ node, i }) != valid_edges.end())
		{
			int minFlow = DFS(graph, valid_edges, i, end, (flow < cap && flow != -1) ? flow : cap);
			if (minFlow > 0)
			{
				graph[node][i] -= minFlow;
				graph[i][node] += minFlow;

				return minFlow;
			}
		}
	}

	return 0; 
}

void MaxFlow(vector<vector<int>>& graph, int start, int end)
{
	int n = graph.size();
	int max_flow = 0;
	vector<int> levels(n, -1);
	unordered_set<pair<int, int>, HashFunction> valid_edges;

	BFS(graph, valid_edges, levels, start, end);

	while (levels[end] != -1)
	{
		max_flow +=  DFS(graph, valid_edges, start, end, -1);
		
		valid_edges.clear();
		levels = vector<int>(n, -1);
		BFS(graph, valid_edges, levels, start, end);
	}

	cout << "Maximum flow from " << start << " to " << end << " is " << max_flow << endl;
}

void FillQueue(vector<vector<int>>& graph, priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareTuple>& pq)
{
	for (int i = 0; i < graph.size(); i++)
		for (int j = i + 1; j < graph.size(); j++)
			if (graph[i][j] != -1)
				pq.push(make_tuple(i, j, graph[i][j]));
}

int FindParent(unordered_map<int, int>& parents, int node)
{
	if (parents[node] == node)
		return node;

	return FindParent(parents, parents[node]);
}

void Kruskal(vector<vector<int>>& graph)
{
	int edges = 0;
	unordered_map<int, int> parents;
	priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, CompareTuple> pq;

	FillQueue(graph, pq);
	for (int i = 0; i < graph.size(); i++)
		parents[i] = i;

	while (edges < graph.size() - 1)
	{
		tuple<int, int, int> edge = pq.top();
		int source = get<0>(edge), end = get<1>(edge);
		int parent_source = FindParent(parents, source);
		int parent_end = FindParent(parents, end);

		if (parent_end != parent_source)
		{
			parents[end] = parent_source;
			edges++;

			cout << "(" << source << ", " << end << ")" << endl;
		}

		pq.pop();
	}
}

int main()
{
	int n, start, end;
	vector<vector<int>> graph1;
	vector<vector<int>> graph2;

	for (int i = 0; i < 5; i++)
	{
		ifstream file_stream("input" + to_string(i) + ".txt");
		file_stream >> n >> start >> end;

		graph1 = vector<vector<int>>(n, vector<int>(n));
		graph2 = vector<vector<int>>(n, vector<int>(n));

		BuildGraph(n, graph1, file_stream);
		BuildGraph(n, graph2, file_stream);

		cout << "input file " << i << endl;

		cout << "-------------------------------------" << endl << "Problem 1" << endl;
		Kruskal(graph1);
		
		cout << "-------------------------------------" << endl << "Problem 2" << endl;
		HamiltonianCycle(graph1, 0);

		cout << "-------------------------------------" << endl << "Problem 3" << endl;
		MaxFlow(graph2, start, end);

		cout << "-------------------------------------" << endl << endl;
	}

	return 0;
}