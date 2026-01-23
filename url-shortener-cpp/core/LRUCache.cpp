#include "LRUCache.h"

LRUCache::LRUCache(int cap): capacity(cap)
{}

bool LRUCache :: get(const std :: string& key ,std :: string & value){
    auto it =cache.find(key);
    if(it==cache.end()){
        return false;
    }

    value=it->second.first;

    order.erase(it->second.second);

    order.push_front(key);

    it->second.second=order.begin();

   
    return  true;

}




void LRUCache::put(const std :: string& key,const std :: string& value){

    auto it=cache.find(key);

    if(it!=cache.end()){

        order.erase(it->second.second);
        order.push_front(key);
     
        // update map
        it->second = {value, order.begin()};
        return;
    }



    //agar full ho jaye
    if(cache.size()==capacity){
        std :: string lrukey =order.back();
        order.pop_back();
        cache.erase(lrukey);
    }



    order.push_front(key);
    cache[key]={value,order.begin()};
}

