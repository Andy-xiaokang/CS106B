// Quokka class.
//
// Author: Sean Szumlanski
// Date: Autumn 2023
//
// This library implements a basic Quokka class as an introduction to object-
// oriented programming concepts.

#ifndef QUOKKA_H
#define QUOKKA_H

#include <iostream>

class Quokka
{
public:
    Quokka();
    Quokka(std::string name, int howAdorable, std::string profilePic);
    ~Quokka();
    void printInfo();
    std::string getName();
    void setName(std::string name);
    int howAdorable();
    std::string location();
    std::string profilePic();
    void renderProfile();

private:
    std::string _name;
    int _howAdorable;  // 1 through 5
    std::string _location;
    std::string _profilePic;
};

#endif
