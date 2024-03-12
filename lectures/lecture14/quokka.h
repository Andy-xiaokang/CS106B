#ifndef QUOKKA_H
#define QUOKKA_H
#include<iostream>


class Quokka
{
public:
    Quokka();
    Quokka(std::string name, int howAdorable, int price);
    ~Quokka();
    std::string getName();
    void setName(std::string name);
    int howAdorable();
    int price();
    void printInfo();
    void discount(float percent);

private:
    std::string _name;
    int _howAdorable;
    int _price;

};

#endif // QUOKKA_H
