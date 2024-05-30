#include "Permutations.h"
using namespace std;

/* Returns all permutations of the given input string.
 *
 * NOTE: THE INITIAL IMPLEMENTATION PROVIDED HERE IS INCORRECT. You will need
 * to isolate the bug by using the debugger, then answer some questions about
 * why this code is incorrect. Remember that the error consists purely of the
 * addition of a single extra character; you shouldn't need to do any major
 * surgery to repair this function.
 *
 * Should we have used pass-by-const-reference here? Probably. That itself
 * isn't the error, but it is related to what went wrong here.
 */
Set<string> permutationsRec(string str, string chosen) {
    /* Base Case: If there are no remaining characters left to consider, then
     * the only permutation possible is the single permutation consisting of
     * what we already committed to.
     */
    if (str == "") {
        return { chosen };
    }
    /* Recursive Case: Some character has to be the next one in the permutation.
     * Which one should it be? Let's try all possible options and see what we
     * find.
     */
    else {
        /* Hold all permutations we find. */
        Set<string> result;
        for (int i = 0; i < str.size(); i++) {
            /* Form a new string of all the remaining letters by grabbing everything
             * up to but not including the current character, then everything after
             * the current character.
             */
            char ch = str[i];
            string remaining = str.substr(0, i) + str.substr(i + 1);

            /* Find all permutations we can make with this choice and add them into
             * the result.
             */
            Set<string> thisOption = permutationsRec(remaining, chosen + ch);
            result += thisOption;
        }

        /* We've now tried all options, so let's return what we came up with. */
        return result;
    }
}

Set<string> permutationsOf(const string& str) {
    return permutationsRec(str, "");
}
