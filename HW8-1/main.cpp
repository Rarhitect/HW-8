//
//  main.cpp
//  HW8-1
//
//  Created by Alyaev Roman on 31.03.2021.
//

#include <iostream>
#include <thread>
#include <random>
#include <mutex>

void thread_func_for_pi(int number_of_points, int & counter_included, double start_of_xrange, double end_of_xrange)
{
    std::random_device rd;
    std::mt19937 mersenne(rd());
    std::uniform_real_distribution<double> uni_of_y(0.0, 1.0);
    std::uniform_real_distribution<double> uni_of_x(start_of_xrange, end_of_xrange);
    
    int counter = 0; //счетчик попавших в окружность точек
    
    for(auto i = 0; i < number_of_points; ++i)
    {
        double x = uni_of_x(mersenne);
        double y = uni_of_y(mersenne);
        
        if( (x * x + y * y) <= 1 )
            counter++;
    }
    
    std::mutex mutex;
    std::lock_guard< std::mutex > lock(mutex);
    counter_included += counter;
}

double parallel_pi(int number_of_points)
{
    double pi;
    
    int counter = 0;
    
    const std::size_t number_of_cores = std::thread::hardware_concurrency();
    std::size_t hardware_threads = (number_of_cores != 0) ? number_of_cores : 8;
    
    std::vector<std::thread> threads(hardware_threads);
    
    for(auto i = 0; i < threads.size(); ++i)
    {
        threads[i] = std::thread(thread_func_for_pi, number_of_points, std::ref(counter), static_cast<double>(i) / hardware_threads, static_cast<double>(i + 1) / hardware_threads);
    }
    
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    
    pi = static_cast<double>(counter) / (2 * static_cast<double>(number_of_points));
    
    return pi;
}

double not_parallel_pi(int number_of_points)
{
    double pi;
    
    std::random_device rd;
    std::mt19937 mersenne(rd());
    std::uniform_real_distribution<double> uni(0.0, 1.0);
    
    int couter_included = 0; //счетчик попавших в окружность точек
    
    for(auto i = 0; i < number_of_points; ++i)
    {
        double x = uni(mersenne);
        double y = uni(mersenne);
        
        if( (x * x + y * y) <= 1 )
            couter_included++;
    }
    
    pi = 4 * static_cast<double>(couter_included) / static_cast<double>(number_of_points);
    
    return pi;
}

int main(int argc, const char * argv[])
{
    int number_of_points;
    std::cout << "Insert the number of points: ";
    std::cin >> number_of_points;
    
    std::cout << "Not parallel pi: " << not_parallel_pi(number_of_points) << std::endl;
    std::cout << "Parallel pi: " << parallel_pi(number_of_points) << std::endl;
    
    return 0;
}
