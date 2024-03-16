// Quokka class with geocities-inspired profile rendering.
//
// Author: Sean Szumlanski
// Date: Autumn 2023
//
// This program renders little Quokka profile pages in geocities-inspired themes
// with tiled backgrounds and bright Comic Sans font.

#include <iostream>
#include "console.h"
#include "quokka.h"  // for Quokka class
#include "vector.h"
using namespace std;

int main()
{
    Vector<Quokka> v;

    v.add(Quokka("Muffinface", 5, "muffinface.jpg"));
    v.add(Quokka("Hemmy", 4, "hemmy.jpg"));
    v.add(Quokka("Percival", 5, "percival.jpg"));
    v.add(Quokka("Fred", 5, "04.jpg"));
    v.add(Quokka("Lovelace", 5, "05.jpg"));
    v.add(Quokka("Night Terror", 5, "06.jpg"));
    v.add(Quokka("Glen", 5, "07.jpg"));

    for (Quokka q : v)
    {
        q.renderProfile();
    }

    return 0;
}
