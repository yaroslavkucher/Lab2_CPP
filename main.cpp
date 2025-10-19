#include <iostream>
#include <random>
#include <vector>


using namespace std;

int main()
{
	mt19937 randomGenerator(random_device{}());
	uniform_int_distribution<int> distribution(0, 100);
	vector<int> sizes = {100, 1'000, 10'000, 100'000};
	int n = 1;
	for(auto vecSize: sizes)
	{
		cout << "Experiment #" << n++ << ":" << endl << "vector size = " << vecSize << endl;
		vector<int> seqOfRandomNumbers(vecSize);
		for (auto& el : seqOfRandomNumbers)
		{
			el = distribution(randomGenerator);
		}
		for (auto& el : seqOfRandomNumbers)
		{
			cout << el << " ";
		}
		cout << endl;
	}
	return 0;
}