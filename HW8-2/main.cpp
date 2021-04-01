//
//  main.cpp
//  HW8-2
//
//  Created by Alyaev Roman on 31.03.2021.
//

#include <iostream>
#include <string>
#include <thread>
#include <random>
#include <vector>
#include <iterator>
#include <mutex>

std::mutex global_mutex;

std::string random_dna(int quantity)
{
    std::string dna;
    std::random_device rd;
    std::mt19937 mersenne(rd());
    std::uniform_int_distribution< int > uni(1, 4);
    
    for(auto i = 0; i < quantity; ++i)
    {
        int code = uni(mersenne);
        if (code == 1)
        {
            dna += 'A';
        }
        else if (code == 2)
        {
            dna += 'G';
        }
        else if (code == 3)
        {
            dna+= 'T';
        }
        else if (code == 4)
        {
            dna += 'C';
        }
    }
    
    return dna;
}

void search_block(std::vector< std::size_t > & indexes, std::string dna, std::string user_search, std::size_t left_edge, std::size_t right_edge)
{
    global_mutex.lock();
    
    int length_of_coincidence = 0;
    
    for (auto i = left_edge; i < right_edge; ++i)
    {
        for (auto j = 0; j < user_search.size(); ++j)
        {
            if (i + j < dna.size())
            {
                (dna[i + j] == user_search[j]) ? length_of_coincidence++ : length_of_coincidence += 0;
            }
        }
        
        if (length_of_coincidence == user_search.size())
        {
            indexes.push_back(i + 1);
        }
        
        length_of_coincidence = 0;
    }
    
    global_mutex.unlock();
}

std::vector< std::size_t > searched_indexes(std::vector< std::size_t > & indexes, std::string dna, std::string user_search)
{
    const std::size_t hardware_threads = std::thread::hardware_concurrency();
    unsigned long block_size = dna.size() / hardware_threads;
    unsigned long retained_block_size = dna.size() % hardware_threads;
    
    std::vector<std::thread> threads(hardware_threads);
    
    std::size_t left_edge = 0;
    std::size_t right_edge = block_size;
    
    for (size_t i = 0; i < threads.size(); i++)
    {
        threads[i] = std::thread(search_block, std::ref(indexes), dna, user_search, left_edge, right_edge);
        
        left_edge += block_size;
        right_edge += block_size;
    }

    if (retained_block_size != 0)
    {
        threads.push_back(std::thread(search_block, std::ref(indexes), dna, user_search, left_edge, left_edge + retained_block_size));
    }
    
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    
    return indexes;
}

int main(int argc, const char * argv[])
{
    std::cout << "Enter the quantity of DNA elements: ";
    int quantity = 0;
    std::cin >> quantity;
    std::string dna = random_dna(quantity);
    std::cout << "Here is your random DNA: " << dna << std::endl;
    
    std::string user_search;
    std::cout << "Insert the part you searching: ";
    std::cin >> user_search;
    
    std::vector< std::size_t > indexes;
    
    indexes = searched_indexes(indexes, dna, user_search);
    
    if(indexes.empty())
    {
        std::cout << "No such elements" << std::endl;
        exit(1);
    }
    else
    {
        std::sort(indexes.begin(), indexes.end());
        std::cout << "Searched elements have positions:" << std::endl;
        for(auto i = 0; i < indexes.size(); ++i)
        {
            std::cout << "Substring №" << i + 1 << ": " << indexes[i] << std::endl;
        }
    }
    
    std::cout << std::endl;
    
    return 0;
}


//ПОСЛЕДОВАТЕЛЬНАЯ РЕАЛИЗАЦИЯ: (мне жалко её выбрасывать)

//    std::string dna_copy = dna;
//    std::size_t new_index = 0;
//
//    std::string::iterator it = dna.begin();
//    while(it <= dna.end())
//    {
//        static int iterration_number = 0;
//
//        std::size_t positions = dna_copy.find(user_search);
//
//        if (positions == std::string::npos)
//        {
//            break;
//        }
//
//        new_index += positions;
//        indexes.push_back(new_index + 1 + user_search.size() * iterration_number);
//
//        std::advance(it, (positions + user_search.size()));
//        dna_copy = dna_copy.substr(positions + user_search.size());
//
//        ++iterration_number;
//    }
