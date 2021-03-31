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
    std::cout << "Here is your random DNA: " << random_dna(quantity) << std::endl;
    return 0;
}
