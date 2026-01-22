#include "Idgenerator.h"


Idgenerator :: Idgenerator(){
    counter=1;

}

long long Idgenerator ::getNextId(){
    return counter.fetch_add(1);
}