#include <iostream>
#include "skiplist.h"
#define FILE_PATH "./store/dumpFile"

void print()
{
    std::cout << std::endl;
    std::cout << "********************************************************************************************************" << std::endl;
    std::cout << "* 1.insert  2.search  3.delete  4.display_list  5.List_size  6.dump_file  7.load_file  8.clear_list *" << std::endl;
    std::cout << "********************************************************************************************************" << std::endl;
    std::cout << "Please enter the number to start the operation:  ";
}

bool keyIsValid(const std::string &str)
{
    if (str.size() > 5)
        return false;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] < '0' || str[i] > '9')
            return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "usage: ./main1 10" << std::endl;
        return 1;
    }
    zskiplist<int, std::string> skipList(atoi(argv[1]));

    std::cout << "\n*****************   A tiny KV storage based on skiplist   ********************"
              << "\n";

    print();
    std::string flagstr = "";
    int flag = 0;
    while (std::cin >> flagstr)
    {
        if (flagstr == "exit")
        {
            break;
        }
        if (!keyIsValid(flagstr))
        {
            std::cout << "The enter is invalid,  please enter again:" << std::endl;
            continue;
        }
        flag = stoi(flagstr);
        std::cout << std::endl;
        switch (flag)
        {
        case 1:
        {
            int key;
            std::string keystr;
            std::string value;
            std::cout << "Please enter key:  ";

            std::cin >> keystr;
            if (!keyIsValid(keystr))
            {
                std::cout << "The key is invalid,  please check again" << std::endl;
                break;
            }
            key = stoi(keystr);
            std::cout << "Please enter value:  ";
            std::cin >> value;
            skipList.insert_element(key, value);
            break;
        }
        case 2:
        {
            int key;
            std::string keystr;
            std::cout << "Please enter search_key:  ";
            std::cin >> keystr;
            if (!keyIsValid(keystr))
            {
                std::cout << "The key is invalid,  please check again" << std::endl;
                break;
            }
            key = stoi(keystr);
            skipList.search_element(key);
            break;
        }
        case 3:
        {
            int key;
            std::string keystr;
            std::cout << "Please enter delete_key:  ";
            std::cin >> keystr;
            if (!keyIsValid(keystr))
            {
                std::cout << "The key is invalid,  please check again" << std::endl;
                break;
            }
            key = stoi(keystr);
            skipList.delete_element(key);
            break;
        }
        case 4:
        {
            skipList.display_list();
            break;
        }
        case 5:
        {
            std::cout << "skipList size:" << skipList.size() << std::endl;
            break;
        }
        case 6:
        {
            skipList.dump_file();
            break;
        }
        case 7:
        {
            skipList.load_file();
            break;
        }
        case 8:
        {
            skipList.clear_list();
            break;
        }
        case 888:
        {
            return 0;
        }
        default:
            std::cout << "your input is not right,  please input again" << std::endl;
            break;
        }
        print();
    }

    system("pause");
    return 0;
}

    // 键值中的key用int型，如果用其他类型，需要自定义比较函数
    // 而且如果修改key的类型，同时需要修改skipList.load_file函数
    // zskiplist<int, std::string> list;
	// list.insert_element(1, "学"); 
	// list.insert_element(3, "算法"); 
	// list.insert_element(7, "认准"); 
	// list.insert_element(8, "微信公众号：代码随想录"); 
	// list.insert_element(9, "学习"); 
	// list.insert_element(18, "算法不迷路"); 
	// list.insert_element(19, "赶快关注吧你会发现详见很晚！"); 

    // std::cout << "list size:" << list.size() << std::endl;

    // list.dump_file();

    // // list.load_file();

    // list.search_element(9);
    // list.search_element(18);


    // list.display_list();

    // list.delete_element(3);
    // list.delete_element(7);

    // std::cout << "list size:" << list.size() << std::endl;

//     list.display_list();
// }
