#include "quokka.h"
#include <iostream>
#include "strlib.h"
using namespace std;

Quokka::Quokka() {

}

Quokka::Quokka(string name, int howAdorable, int price) {
    this->_name = name;
    this->_howAdorable = howAdorable;
    this->_price = price;
}

Quokka::~Quokka() {
    cout << "R.I.P. " << _name << endl;
}

string Quokka::getName() {
    return _name;
}

void Quokka::setName(string name) {
    if (stringContains(toLowerCase(name), "kang")) {
        error("name contains my name: kang");
    }
    _name = name;
}

int Quokka::howAdorable() {
    return _howAdorable;
}

int Quokka::price() {
    return _price;
}

void Quokka::printInfo() {
    cout << _name << "(how adorable: " << _howAdorable << ")" << ", price: " << _price << endl;
}

void Quokka::discount(float percent) {
    this->_price = _price * percent;
}
