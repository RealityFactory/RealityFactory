/*************************************************************************/
/* HASH_TABLE.H             Rex Schilasky                                */
/*                                                                       */
/* HashTable Class combined with std::list                               */
/*                                                                       */
/*************************************************************************/

#ifndef hash_table_h_included
#define hash_table_h_included

#define STL_USING_STRING
#define STL_USING_LIST
#include "stl.h"

#include "hash_fun.h"

/**************************/
/*       StrListT         */
/**************************/
typedef std::list<std::string> StrListT;

/**************************/
/*    StrVoidPtrListT     */
/**************************/
struct SStrVoidPtrPair
{
  std::string key;
  void*       ptr;
};
typedef std::list<struct SStrVoidPtrPair*> StrVoidPtrListT;


/*******************************************/
/*               CHashTable                */
/*******************************************/
template <class T>
class CHashTable : public StrListT
{
public:

  /*****************************************/
  /* Contructor:                           */
  /* ------------------------------------- */
  /*                                       */
  /* tabsize:    estimated size of table   */
  /* VoidPtr:    hash function             */
  /*                                       */
  /*****************************************/
  CHashTable(long tabsize = 10009, long (*VoidPtr)(const std::string& c, const long prime) = hash_fun1)
  {
    prime     = 0;
    map_array = NULL;
    hf        = VoidPtr;
    
    long prime_table[8] = {1009, 5009, 10009, 20011, 50021, 100003, 200003, 500009};
    
    int index = 0;
    while(index < 8)
    {
      prime = prime_table[index];
      if(tabsize <= prime) break;
      index++;
    }
    
    map_array = new void*[prime];
    memset(map_array, 0, sizeof(map_array)*prime);
  };
  
  /*****************************************/
  /* Destructor:                           */
  /*****************************************/
  ~CHashTable()
  {
    RemoveAllKey(false);
    delete[] map_array;
  };
  
  /*****************************************/
  /* AddKey:                               */
  /* ------------------------------------- */
  /*                                       */
  /* key   :    hash key                   */
  /* member:    member pointer to store    */
  /*                                       */
  /*****************************************/
  bool AddKey(std::string key, T* member)
  {
    if(!map_array)  return(false);
    if(!hf)         return(false);
    if(prime <= 0)  return(false);
    if(key.empty()) return(false);
    
    long index = hf(key, prime);
    if((index < 0) || (index >= prime)) return(false);
    
    StrVoidPtrListT* list_ptr = NULL;
    if(map_array[index] == NULL)
    {
      list_ptr = new StrVoidPtrListT;
      map_array[index] = list_ptr;
    }
    else
    {
      list_ptr = (StrVoidPtrListT*)map_array[index];
    }
    if(!list_ptr) return(false);
    
    struct SStrVoidPtrPair* pair_ptr = NULL;
    StrVoidPtrListT::iterator iter = list_ptr->begin();
    while(iter != list_ptr->end())
    {
      pair_ptr = (*iter);
      if(pair_ptr)
      {
        if(pair_ptr->key == key) return(false);
      }
      iter++;
    }
    
    pair_ptr = new SStrVoidPtrPair;
    if(!pair_ptr) return(false);
    pair_ptr->key = key;
    pair_ptr->ptr = member;
    list_ptr->push_back(pair_ptr);
    
    push_back(key);
    return(true);
  };
  
  /*****************************************/
  /* RenameKey:                            */
  /* ------------------------------------- */
  /*                                       */
  /* key    :   old key                    */
  /* new_key:   new key                    */
  /*                                       */
  /*****************************************/
  bool RenameKey(std::string key, std::string new_key)
  {
    if(GetMember(new_key)) return(false);
    T* ptr = GetMember(key);
    if(ptr)
    {
      RemoveKey(key, false);
      AddKey(new_key, ptr);
      return(true);
    }
    return(false);
  };
  
  /*****************************************/
  /* RemoveKey:                            */
  /* ------------------------------------- */
  /*                                       */
  /* key     :  hash key                   */
  /* free_mem:  free allocated memory      */
  /*                                       */
  /*****************************************/
  bool RemoveKey(std::string key, bool free_mem = false)
  {
    if(!map_array)  return(false);
    if(!hf)         return(false);
    if(prime <= 0)  return(false);
    if(key.empty()) return(false);
    
    long index = hf(key, prime);
    if((index < 0) || (index >= prime)) return(false);
    
    StrVoidPtrListT* list_ptr = (StrVoidPtrListT*)map_array[index];
    if(!list_ptr) return(false);
    
    StrVoidPtrListT::iterator iter = list_ptr->begin();
    while(iter != list_ptr->end())
    {
      struct SStrVoidPtrPair* pair_ptr = (*iter);
      if(pair_ptr)
      {
        if(pair_ptr->key == key)
        {
          if(free_mem && pair_ptr->ptr)
          {
            delete (T*)pair_ptr->ptr;
          }
          remove(key);
          
          list_ptr->erase(iter);
          delete pair_ptr;
          
          return(true);
        }
      }
      iter++;
    }
    return(false);
  };
  
  /*****************************************/
  /* RemoveAllKey:                         */
  /* ------------------------------------- */
  /*                                       */
  /* free_mem:  free allocated memory      */
  /*                                       */
  /*****************************************/
  bool RemoveAllKey(bool free_mem = false)
  {
    if(!map_array)  return(false);
    if(!hf)         return(false);
    if(prime <= 0)  return(false);
    
    bool ret_value = false;
    CHashTable::iterator key_iter = begin();
    while(key_iter != end())
    {
      long index = hf((*key_iter), prime);
      if((index < 0) || (index >= prime)) {key_iter++;continue;};
      if(map_array[index] == NULL)  {key_iter++;continue;};
      
      StrVoidPtrListT* list_ptr = (StrVoidPtrListT*)map_array[index];
      StrVoidPtrListT::iterator list_iter = list_ptr->begin();
      while(list_iter != list_ptr->end())
      {
        struct SStrVoidPtrPair* pair_ptr = (*list_iter);
        if(pair_ptr)
        {
          if(free_mem && pair_ptr->ptr)
          {
            delete (T*)pair_ptr->ptr;
          }
          delete pair_ptr;
          ret_value = true;
        }
        list_iter++;
      }
      delete list_ptr;
      map_array[index] = NULL;
      
      key_iter++;
    }
    clear();
    
    return(ret_value);
  };
  
  /*****************************************/
  /* GetMember:                            */
  /* ------------------------------------- */
  /*                                       */
  /* key:       member key                 */
  /*                                       */
  /*****************************************/
  T* GetMember(const std::string& key)
  {
    if(!map_array)  return(NULL);
    if(!hf)         return(NULL);
    if(prime <= 0)  return(NULL);
    if(key.empty()) return(NULL);
    
    long index = hf(key, prime);
    if((index < 0) || (index >= prime)) return(NULL);
    if(map_array[index] == NULL) return(NULL);
    
    StrVoidPtrListT* list_ptr = (StrVoidPtrListT*)map_array[index];
    
    StrVoidPtrListT::iterator iter = list_ptr->begin();
    while(iter != list_ptr->end())
    {
      struct SStrVoidPtrPair* pair_ptr = (*iter);
      if(pair_ptr)
      {
        if(pair_ptr->key == key)
        {
          return((T*)pair_ptr->ptr);
        }
      }
      iter++;
    }
    return(NULL);
  };

protected:
  long   prime;
  long   (*hf)(const std::string& c, const long prime);

  void** map_array;
};


#endif /* hash_table_h_included */
