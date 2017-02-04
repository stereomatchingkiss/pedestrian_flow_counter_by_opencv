#include "people_count_test.hpp"

int main(int argc, char *argv[])
{    
    if(argc != 2){
        std::cerr<<"please enter the video file you want to process\n";
        return -1;
    }

    people_count_test().test_00(argv[1]);

    return 0;
}
