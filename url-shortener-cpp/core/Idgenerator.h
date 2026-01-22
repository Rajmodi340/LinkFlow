#ifndef ID_GENRATOR_H
#define ID_GENERATOR_H

#include<atomic>

class Idgenerator {
    private :
           std :: atomic <long long> counter;

public: 
    Idgenerator();

    long long getNextId();



};

#endif