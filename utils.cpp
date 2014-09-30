#include "utils.h"
#include "addresses.h"
#include "gettime.h"

string Utils::ExplodeAndReturn(string stringToExplode, int field, string delimiter) {
	Utils expPointer;
	vector<string> v = expPointer.explode(delimiter, stringToExplode);
	std::string xcontent = v[field];
	char myArray[MAX_STRING_LEN+1];
	return strcpy(myArray, xcontent.c_str());
}
vector<string> Utils::explode( const string &delimiter, const string &str) {
    vector<string> arr;
    int strleng = str.length();
    int delleng = delimiter.length();
    if (delleng==0)
        return arr;//no change
 
    int i=0;
    int k=0;
    while( i<strleng ) {
        int j=0;
        while (i+j<strleng && j<delleng && str[i+j]==delimiter[j])
            j++;
        if (j==delleng) { //found delimiter
            arr.push_back(  str.substr(k, i-k) );
            i+=delleng;
            k=i;
        } else {
            i++;
        }
    }
    arr.push_back(  str.substr(k, i-k) );
    return arr;
}
float Utils::GetPointDistance(float px, float py, float pz, float X, float Y, float Z) {
	return sqrt( ((px-X)*(px-X))+((py-Y)*(py-Y))+((pz-Z)*(pz-Z)) );
}