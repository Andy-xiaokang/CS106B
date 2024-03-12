// Quokka class.
//
// Author: Sean Szumlanski
// Date: Autumn 2023
//
// This library implements a basic Quokka class as an introduction to object-
// oriented programming concepts.

#include <iostream>
#include "geocities.h"
#include "lexicon.h"
#include "quokka.h"
#include "strlib.h"
using namespace std;

Quokka::Quokka()
{
}

Quokka::Quokka(string name, int howAdorable, string profilePic)
{
    _name = name;
    _howAdorable = howAdorable;
    _profilePic = profilePic;

    _location = "Australia";
}

Quokka::~Quokka()
{
    cout << "R.I.P. " << _name << endl;
}

string Quokka::getName()
{
    return _name;
}

void Quokka::setName(string name)
{
    Lexicon lex("bad_words.txt");

    for (string naughtyWord : lex)
    {
        if (stringContains(toLowerCase(name), toLowerCase(naughtyWord)))
        {
            error("Name contains bad word: " + naughtyWord);
        }
    }

	_name = name;
}

int Quokka::howAdorable()
{
    return _howAdorable;
}

string Quokka::location()
{
    return _location;
}

string Quokka::profilePic()
{
    return _profilePic;
}

void Quokka::printInfo()
{
    cout << _name << " (how adorable: " << _howAdorable
         << ", loc: " << _location << ")" << endl;
}

void Quokka::renderProfile()
{
    // The keyword 'this' refers to the object we're inside right now -- the
    // object from which we called .renderProfile(), which exists back in main().
    renderGeocitiesPage(this);
}
