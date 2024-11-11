//Riabichenko Yevgieniia, K-24;
//LR_02, task 6;
//work in CodeBlocks

#include <iostream>
#include <vector>
#include <numeric>
#include <execution>
#include <chrono>
#include <thread>

template <typename Func>
auto measure(Func&& func)
{
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();

    return
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

void inner_product_test(const std::vector<int>& data)
{
    auto result = measure([&]()
    {
        std::inner_product(data.begin(), data.end(), data.begin(), 0);
    });

    std::cout << "Without policy: " << result << " ms" << '\n';
}

void transform_reduce_test(const std::vector<int>& data, auto policy, const std::string& policy_name)
{
    auto result = measure([&]()
    {
        std::transform_reduce(policy, data.begin(), data.end(), data.begin(), 0);
    });

    std::cout << "With policy " << policy_name << ": " << result << " ms" << '\n';
}

void custom_parallel_inner_product(const std::vector<int>& data, int K)
{
    int chunk_size = data.size() / K;
    std::vector<std::thread> threads;
    std::vector<int> partial_sums(K, 0);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < K; i++)
    {
        threads.emplace_back([&data, &partial_sums, i, chunk_size]()
                             {
                                    partial_sums[i] = std::inner_product(data.begin() + i * chunk_size,
                                                                         data.begin() + (i + 1) * chunk_size,
                                                                         data.begin() + i * chunk_size, 0);
                             });
    }

    for (auto& t : threads)
    {
        t.join();
    }

    int result = std::accumulate(partial_sums.begin(), partial_sums.end(), 0);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Custom algorithm with K=" << K << ": " << duration << " ms" << '\n';
}

int main()
{
    std::vector<int> data(1e8, 1);

    inner_product_test(data);
    transform_reduce_test(data, std::execution::seq, "seq");
    transform_reduce_test(data, std::execution::par, "par");
    transform_reduce_test(data, std::execution::par_unseq, "par_unseq");

    for (int K = 1; K <= std::thread::hardware_concurrency(); K++)
    {
        custom_parallel_inner_product(data, K);
    }

    return 0;
}
