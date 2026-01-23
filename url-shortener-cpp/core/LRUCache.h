#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include<unordered_map>
#include<list>
#include<string>

//namespace islea nhi le rhae kyuki koi humare header file importa krega to garbage vlaue bhi a skte h


// lru - fast lookup 
// limited size_t
// recently used 


class LRUCache {
    private : int capacity;
    std :: list<std ::string>order;


        std::unordered_map<
        std::string,
        std::pair<std::string, std::list<std::string>::iterator>
    > cache;


    // map k structure:
// 




public: 
   LRUCache(int cap);

   bool get(const std :: string& key ,std :: string& value);
   //agar key cache me hai
   //value fill krenge aur true return krenge

   void put( const std :: string& key, const std:: string& value);
};


#endif