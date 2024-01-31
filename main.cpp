#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <tuple>
#include <unordered_set>
#include <fstream>
#include <string>
#include <format>

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
					v.visited = SetBit(v.visited, i);
					v.path.push_back(i);
					v.accumulated_cost += graph[u.node][v.node];
					v.possible_cost = PossibleCost(graph, v.visited, v.node, u.node, start) + v.accumulated_cost;

					if (v.path.size() == n && graph[v.node][start] != -1)
					{
						v.accumulated_cost += graph[v.node][start];
						v.path.push_back(start);

						if (v.accumulated_cost <= optimal_cost || optimal_cost == -1)
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

	for (int i = 0; i <= n; i++)
	{
		cout << optimal_path[i] << " ";
	}

	cout << endl << optimal_cost << endl;
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

		HamiltonianCycle(graph1, 0);
	}

	return 0;
}