#include <iostream>
#include <vector>
#include <map>

using namespace std;

void BuildGraph(int n, vector<vector<int>>& graph)
{
	int num;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cin >> num;
			graph[i][j] = num;
		}
	}
}

int main()
{

	int n, start, end;
	cin >> n >> start >> end;

	vector<vector<int>> graph1(n, vector<int>(n));
	vector<vector<int>> graph2(n, vector<int>(n));

	BuildGraph(n, graph1);
	BuildGraph(n, graph2);

	return 0;
}