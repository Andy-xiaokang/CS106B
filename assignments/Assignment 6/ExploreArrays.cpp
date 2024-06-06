#include "ExploreArrays.h"
#include "Demos/DataPoint.h"
#include <string>
using namespace std;

void exploreArrays() {
    /* In this section, you'll see how to use the debugger to read the contents
     * of arrays in memory and how to recognize when you're trying to read past
     * the end of an array.
     *
     * Begin by setting a breakpoint on the line of code shown below, then doing
     * execute the "Step Over" command until you're at the next comment.
     */
    DataPoint* elems = new DataPoint[4] {
        { "What",      1 },
        { "a",         2 },
        { "wonderful", 3 },
        { "world!",    4 },
    };

    /* At this point, you now have a nice array of elements in memory. If you
     * try using the debugger to read that array, though, you'll probably only
     * see the first element of that array.
     *
     * To fix this, we'll need to tell the debugger to change the display
     * format for elems. To do so, right-click on "elems" in the debugging
     * window that shows local variable values. You should see two groups of
     * options in the menu. One group be marked "Change Display for Object
     * Named local.elems," and the other will be marked "Change Display for
     * Type DataPoint*." In the group for "Object Named local.elems," choose
     * the option "Array of 10 items." When you do, you should see ten drop-
     * down slots appear underneath elems.
     *
     * Expand out slots [0], [1], [2], and [3] and confirm that you can read
     * the values there and that they match what was created in the array
     * above. You should see the data and weight fields. (You'll also see
     * one called _initializationFlag. This is something extra we added for
     * this assignment to make it easier to spot memory errors, and you can
     * safely ignore it.)
     *
     * Once you've done that, hit "Step Over" to skip this line of code.
     */
    elems[0].weight++; // Just a place for the debugger to rest.

    /* Now, expand out slots [4], [5], [6], ..., and [9]. These slots aren't
     * actually a part of the array, and they're essentially garbage values
     * that appear past the end of the array we allocated up above. Some of
     * the strings you'll see might be marked <not accessible>, while others
     * might be empty, or might be random sequences of symbols. The numbers
     * that you see on the different slots are similarly unpredictable - they
     * might happen to be all zeros, or they might be completely random values
     * with no discernable pattern.
     *
     * Hit "Step Over" to skip this next line and move on to the next section.
     */
    elems[0].weight++; // Just a place for the debugger to rest.

    /* This next section of code allocates space for three DataPoints, but
     * doesn't initialize them. Hit "Step Over" to set that array up.
     *
     * (Yes, we just leaked a bunch of memory. That's not a good thing, but
     * this is purely for educational purposes and so we don't care. ^_^)
     */
    elems = new DataPoint[3];

    /* Now, poke around a bit in the debugger and look at the array contents.
     * Look at slots [0], [1], and [2] (the valid slots in the array.)
     * We didn't initialize any of the elements in this array, so C++ does
     * what it normally does when initializing those elements. In particular,
     * notice that
     *
     * 1. the strings have all been set to the empty string, and
     * 2. the integers are all essentially random.
     *
     * Now, move on and look at slots [3], [4], ..., [9]. Notice that the
     * integers here all look pretty much random as well, but the strings here
     * are likely to be a mix of empty, <not accessible>, and totally random.
     *
     * We wanted you to see this because we want you to understand that seeing
     * garbage values in memory doesn't necessarily mean that you've walked off
     * the end of the array. Rather, it likely means that you're seeing some
     * values in memory that weren't initialized. So don't panic if you see
     * this when writing code that uses arrays - it doesn't mean that your
     * pointer is "bad" or something like that. It could very well mean that
     * you have the space you need, but just forgot to initialize the values
     * there.
     */
    elems[0].weight = 137; // Ah, give a nice value.

    /* To wrap up this section, edit res/ShortAnswers.txt with your answers
     * to the following questions:
     *
     * Q1: What operating system are you using? (Windows, macOS, Linux, etc.)
     *
     * Q2: What are the values you see in elems[3], elems[4], and elems[5]? If
     *     you see garbage strings that run for long periods of time, just give
     *     us the first few characters, or your best approximation of them. :-)
     *
     */
    elems[1].weight = 137; // Just another place for the debugger to chill.
}
