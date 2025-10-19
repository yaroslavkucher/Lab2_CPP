#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <chrono>
#include <execution>

void wfile(std::string policy, std::chrono::duration<double, std::milli> duration)
{
	std::cout << "Policy: " << policy << ", Time taken: " << duration.count() << " ms" << std::endl;
}

bool pred(int x)
{
	for (int i = 0; i < 1000; i++)
	{
		x = (x * x - x + 1)%86;
	}
	return true;
}

void policy_all_of(const std::vector<int>& vec)
{

	auto start = std::chrono::high_resolution_clock::now();
	std::all_of(vec.begin(), vec.end(), pred);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;

	wfile("No policy", duration);
}

template<typename ExecPolicy>
constexpr const std::string policy_name() {
	if constexpr (std::is_same_v<std::decay_t<ExecPolicy>, std::execution::sequenced_policy>)
		return "sequential";
	else if constexpr (std::is_same_v<std::decay_t<ExecPolicy>, std::execution::parallel_policy>)
		return "parallel";
	else if constexpr (std::is_same_v<std::decay_t<ExecPolicy>, std::execution::unsequenced_policy>)
		return "unsequenced";
	else if constexpr (std::is_same_v<std::decay_t<ExecPolicy>, std::execution::parallel_unsequenced_policy>)
		return "parallel unsequenced";
}

template<typename ExecPolicy>
void policy_all_of(const std::vector<int>& vec, ExecPolicy&& policy)
{

	auto start = std::chrono::high_resolution_clock::now();
	std::all_of(std::forward<ExecPolicy>(policy), vec.begin(), vec.end(), pred);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;

	wfile(policy_name<ExecPolicy>(), duration);
}

int main()
{
	std::mt19937 randomGenerator(std::random_device{}());
	std::uniform_int_distribution<int> distribution(0, 100);
	std::vector<int> sizes = {100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000};
	int n = 1;
	for(auto vecSize: sizes)
	{
		std::cout << "Experiment #" << n++ << ":" << std::endl
				  << "vector size = " << vecSize << std::endl;
		std::vector<int> seqOfRandomNumbers(vecSize);
		for (auto& el : seqOfRandomNumbers)
		{
			el = distribution(randomGenerator);
		}
		/*for (auto& el : seqOfRandomNumbers)
		{
			std::cout << el << " ";
		}*/
		std::cout << std::endl;
		policy_all_of(seqOfRandomNumbers);
		policy_all_of(seqOfRandomNumbers, std::execution::seq);
		policy_all_of(seqOfRandomNumbers, std::execution::unseq);
		policy_all_of(seqOfRandomNumbers, std::execution::par);
		policy_all_of(seqOfRandomNumbers, std::execution::par_unseq);
	}
	return 0;
}