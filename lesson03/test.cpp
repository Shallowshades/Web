#include <iostream>

using namespace std;

int main(){
    int a = 10;

#ifdef DEBUG
    cout<<"DEBUG...\n";
#endif

    for(int i = 0; i < 5; ++i) cout <<"Hello, GCC" <<endl;
    return 0;
}