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
    
    std::string dna_copy = dna;
    std::vector< std::size_t > indexes;

    std::size_t new_index = 0;
  
    std::string::iterator it = dna.begin();
    while(it <= dna.end())
    {
        static int iterration_number = 0;
        std::size_t positions = dna_copy.find(user_search);

        if (positions == std::string::npos)
        {
            break;
        }
        new_index += positions;
        indexes.push_back(new_index + 1 + 2*iterration_number);
        std::advance(it, (positions + user_search.size()));
        dna_copy = dna_copy.substr(positions + user_search.size());
        ++iterration_number;
    }
    
    if(indexes.empty())
    {
        std::cout << "No such elements" << std::endl;
        exit(1);
    }
    else
    {
        for(auto & element: indexes)
        {
            std::cout << element << " ";
        }
    }
    
    std::cout << std::endl;
    
    return 0;
}
