#include "helperfuns.h"

bool in_vector(vector<string> vec, string value)
{
    return count(vec.begin(), vec.end(), value);
}