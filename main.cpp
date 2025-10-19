#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <chrono>
#include <execution>
#include <thread>
#include <string>
#include <format>
#include <future>

void general_info()
{
	std::cout << std::endl << ':' << std::string(70, '~') << ':' << std::endl;
	std::cout << std::format(":{:^70}:", "The author of this program is Yaroslav Kucher K-27") << std::endl;
	std::cout << std::format(":{:^70}:", "The program measures the time taken by the std::all_of algorithm.") << std::endl;
	std::cout << std::format(":{:^70}:", "It tests different execution policies on vectors of random integers.") << std::endl;
	std::cout << std::format(":{:^70}:", "Computer has " + std::to_string(std::thread::hardware_concurrency()) + " cores.") << std::endl;
	std::cout << ':' << std::string(70, '~')<< ':'  << std::endl << std::endl;
}

void print_policy_time(std::string policy, std::chrono::duration<double, std::milli> duration)
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

	print_policy_time("No policy", duration);
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

	print_policy_time(policy_name<ExecPolicy>(), duration);
}

/*std::chrono::duration<double, std::milli> parallel_all_of(const std::vector<int>& vec, int k)
{

}*/

int main()
{
	std::mt19937 randomGenerator(std::random_device{}());
	std::uniform_int_distribution<int> distribution(0, 100);
	std::vector<int> sizes = {100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000};
	int n = 1;
	general_info();
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
		policy_all_of(seqOfRandomNumbers);
		policy_all_of(seqOfRandomNumbers, std::execution::seq);
		policy_all_of(seqOfRandomNumbers, std::execution::unseq);
		policy_all_of(seqOfRandomNumbers, std::execution::par);
		policy_all_of(seqOfRandomNumbers, std::execution::par_unseq);
		std::cout << std::string(53, '-') << std::endl
			<< std::format("|{:^25}|{:^25}|", "K", "Duration") << std::endl;
		auto best_duration = std::chrono::duration<double, std::milli>::max().count();
		int K = 1;
		for (int k = 1; k <= std::thread::hardware_concurrency() * 2; k = k * 2)
		{
			auto duration = 5;/*parallel_all_of(seqOfRandomNumbers, k);*/
			if (duration < best_duration)
			{
				best_duration = duration;
				K = k;
			}
			std::cout << std::string(53, '-') << std::endl
				<< std::format("|{:^25}|{:^25}|", k, std::to_string(duration)+"ms") << std::endl;
		}
		std::cout << std::string(53, '-') << std::endl;
		std::cout << "The best performance is achieved with " 
			<< K << " part(s) - this is " << best_duration << " ms." << std::endl;
		std::cout << std::endl << ':' << std::string(70, '~') << ':' << std::endl;
	}
	return 0;
}