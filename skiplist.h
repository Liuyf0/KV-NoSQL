#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
#include <random>

#define ZSKIPLIST_P 0.25
#define STORE_FILE "store/dumpFile"

std::mutex mtx;     // mutex for critical section
std::string delimiter = ":";

//Class template to implement zskiplistNode
template<typename K, typename V> 
class zskiplistNode {

public:
    
    zskiplistNode() {} 

    zskiplistNode(K k, V v, int); 

    ~zskiplistNode();

    K get_key() const;

    V get_value() const;

    void set_value(V);
    
    // Linear array to hold pointers to next zskiplistNode of different level
    // level array
    zskiplistNode<K, V> **forward;

    int node_level;

private:
    K key;
    V value;
};

template<typename K, typename V> 
zskiplistNode<K, V>::zskiplistNode(const K k, const V v, int level) : key (k), value(v), node_level(level) {
    // level + 1, because array index is from 0 - level
    this->forward = new zskiplistNode<K, V>*[level+1];
    
	// Fill forward array with 0(NULL) 
    memset(this->forward, 0, sizeof(zskiplistNode<K, V>*)*(level+1));
};

template<typename K, typename V> 
zskiplistNode<K, V>::~zskiplistNode() {
    delete []forward;
};

template<typename K, typename V> 
K zskiplistNode<K, V>::get_key() const {
    return key;
};

template<typename K, typename V> 
V zskiplistNode<K, V>::get_value() const {
    return value;
};
template<typename K, typename V> 
void zskiplistNode<K, V>::set_value(V value) {
    this->value=value;
};

// Class template for Skip list
template <typename K, typename V> 
class zskiplist {

public: 
    zskiplist(int max_level = 32);
    ~zskiplist();
    int get_random_level();
    zskiplistNode<K, V>* create_node(K, V, int);
    int insert_element(K, V);
    void display_list();
    bool search_element(K);
    void delete_element(K);
    void dump_file();
    void load_file();
    void clear_list();
    int size();

private:
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
    bool is_valid_string(const std::string& str);

private:    
    // Maximum level of the skip list 
    int _max_level;

    // current level of skip list 
    int _skip_list_level;

    // pointer to header zskiplistNode 
    zskiplistNode<K, V> *_header;

    // file operator
    std::ofstream _file_writer;
    std::ifstream _file_reader;

    // zskiplist current element count
    int _element_count;
};

// create new zskiplistNode 
template<typename K, typename V>
zskiplistNode<K, V>* zskiplist<K, V>::create_node(const K k, const V v, int level) {
    zskiplistNode<K, V> *n = new zskiplistNode<K, V>(k, v, level);
    return n;
}

// Insert given key and value in skip list 
// return 1 means element exists  
// return 0 means insert successfully
/* 
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+

*/
template<typename K, typename V>
int zskiplist<K, V>::insert_element(const K key, const V value) {
    
    mtx.lock();
    zskiplistNode<K, V> *current = this->_header;

    // create update array and initialize it 
    // update is array which put zskiplistNode that the zskiplistNode->forward[i] should be operated later
    zskiplistNode<K, V> *update[_max_level+1];
    memset(update, 0, sizeof(zskiplistNode<K, V>*)*(_max_level+1));  

    // start form highest level of skip list 
    for(int i = _skip_list_level; i >= 0; i--) {
        while(current->forward[i] != NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i]; 
        }
        update[i] = current;
    }

    // reached level 0 and forward pointer to right zskiplistNode, which is desired to insert key.
    current = current->forward[0];

    // if current zskiplistNode have key equal to searched key, we get it
    // 判断key是否重复
    if (current != NULL && current->get_key() == key) {
        std::cout << "key: " << key << ", exists" << std::endl;
        mtx.unlock();
        return 1;
    }

    // if current is NULL that means we have reached to end of the level 
    // if current's key is not equal to key that means we have to insert zskiplistNode between update[0] and current zskiplistNode 
    if (current == NULL || current->get_key() != key ) {
        
        // Generate a random level for zskiplistNode
        int random_level = get_random_level();

        // If random level is greater thar skip list's current level, initialize update value with pointer to header
        if (random_level > _skip_list_level) {
            for (int i = _skip_list_level+1; i < random_level+1; i++) {
                update[i] = _header;
            }
            _skip_list_level = random_level;
        }

        // create new zskiplistNode with random level generated 
        zskiplistNode<K, V>* inserted_node = create_node(key, value, random_level);
        
        // insert zskiplistNode 
        for (int i = 0; i <= random_level; i++) {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        // std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        _element_count++;
    }
    mtx.unlock();
    return 0;
}

// Display skip list 
template<typename K, typename V> 
void zskiplist<K, V>::display_list() {

    std::cout << "\n*****Print Skip List*****"<<"\n"; 
    for (int i = 0; i <= _skip_list_level; i++) {
        zskiplistNode<K, V> *zskiplistNode = this->_header->forward[i]; 
        std::cout << "Level " << i << ": ";
        while (zskiplistNode != NULL) {
            std::cout << zskiplistNode->get_key() << ":" << zskiplistNode->get_value() << ";";
            zskiplistNode = zskiplistNode->forward[i];
        }
        std::cout << std::endl;
    }
}

// Dump data in memory to file 
template<typename K, typename V> 
void zskiplist<K, V>::dump_file() {

    std::cout << "dump_file-----------------" << std::endl;
    _file_writer.open(STORE_FILE);
    zskiplistNode<K, V> *zskiplistNode = this->_header->forward[0]; 

    while (zskiplistNode != NULL) {
        _file_writer << zskiplistNode->get_key() << ":" << zskiplistNode->get_value() << "\n";
        std::cout << zskiplistNode->get_key() << ":" << zskiplistNode->get_value() << ";\n";
        zskiplistNode = zskiplistNode->forward[0];
    }

    _file_writer.flush();
    _file_writer.close();
    std::cout << "dump_file ok" << std::endl;
    return ;
}

// Load data from disk
template<typename K, typename V> 
void zskiplist<K, V>::load_file() {

    _file_reader.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string* key = new std::string();
    std::string* value = new std::string();
    while (getline(_file_reader, line)) {
        get_key_value_from_string(line, key, value);
        if (key->empty() || value->empty()) {
            continue;
        }
        // Define key as int type
        insert_element(stoi(*key), *value);
        std::cout << "key:" << *key << "value:" << *value << std::endl;
    }
    delete key;
    delete value;
    _file_reader.close();
}

// Get current zskiplist size 
template<typename K, typename V> 
int zskiplist<K, V>::size() { 
    return _element_count;
}

template<typename K, typename V>
void zskiplist<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value) {

    if(!is_valid_string(str)) {
        return;
    }
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1, str.length());
}

template<typename K, typename V>
bool zskiplist<K, V>::is_valid_string(const std::string& str) {

    if (str.empty()) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
}

// Delete element from skip list 
template<typename K, typename V> 
void zskiplist<K, V>::delete_element(K key) {

    mtx.lock();
    zskiplistNode<K, V> *current = this->_header; 
    zskiplistNode<K, V> *update[_max_level+1];
    memset(update, 0, sizeof(zskiplistNode<K, V>*)*(_max_level+1));

    // start from highest level of skip list
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] !=NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    if (current != NULL && current->get_key() == key) {
       
        // start for lowest level and delete the current zskiplistNode of each level
        for (int i = 0; i <= _skip_list_level; i++) {

            // if at level i, next zskiplistNode is not target zskiplistNode, break the loop.
            if (update[i]->forward[i] != current) 
                break;

            update[i]->forward[i] = current->forward[i];
        }

        // Remove levels which have no elements
        while (_skip_list_level > 0 && _header->forward[_skip_list_level] == 0) {
            _skip_list_level --; 
        }

        std::cout << "Successfully deleted key "<< key << std::endl;
        delete current;
        _element_count --;
    }
    mtx.unlock();
    return;
}

// Search for element in skip list 
/*
                           +------------+
                           |  select 60 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |
level 3         1+-------->10+------------------>50+           70       100
                                                   |
                                                   |
level 2         1          10         30         50|           70       100
                                                   |
                                                   |
level 1         1    4     10         30         50|           70       100
                                                   |
                                                   |
level 0         1    4   9 10         30   40    50+-->60      70       100
*/
template<typename K, typename V> 
bool zskiplist<K, V>::search_element(K key) {

    std::cout << "search_element-----------------" << std::endl;
    zskiplistNode<K, V> *current = _header;

    // start from highest level of skip list
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
    }

    //reached level 0 and advance pointer to right zskiplistNode, which we search
    current = current->forward[0];

    // if current zskiplistNode have key equal to searched key, we get it
    if (current and current->get_key() == key) {
        std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
        return true;
    }

    std::cout << "Not Found Key:" << key << std::endl;
    return false;
}

//clear the list

template <typename K, typename V>
void zskiplist<K, V>::clear_list()
{
    zskiplistNode<K, V> *current = _header->forward[0];

    while (current != nullptr)
    {
        zskiplistNode<K, V> *tmp = current->forward[0];
        delete current;
        current = tmp;
    }

    //若不清空，_header->forward[0]会保存非法值，插入时调用_header->forward[0]->get_key出错
    memset(_header->forward, 0, sizeof(zskiplistNode<K, V> *) * (_skip_list_level + 1));
    _skip_list_level = 0;
    _element_count = 0;
}

// construct skip list
template<typename K, typename V> 
zskiplist<K, V>::zskiplist(int max_level) {

    this->_max_level = max_level;
    this->_skip_list_level = 0;
    this->_element_count = 0;

    // create header zskiplistNode and initialize key and value to null
    K k;
    V v;
    this->_header = new zskiplistNode<K, V>(k, v, _max_level);
};

template<typename K, typename V> 
zskiplist<K, V>::~zskiplist() {

    if (_file_writer.is_open()) {
        _file_writer.close();
    }
    if (_file_reader.is_open()) {
        _file_reader.close();
    }
    delete _header;
}

template<typename K, typename V>
int zskiplist<K, V>::get_random_level(){

    int k = 1;

    while ((random()&0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
        k += 1;
    
    k = (k < _max_level) ? k : _max_level;
    return k;
};
// vim: et tw=100 ts=4 sw=4 cc=120
