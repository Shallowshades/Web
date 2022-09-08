#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

using namespace std;

int main() {

    int fd = open("a.txt", O_RDWR);
    if (fd == -1) {
        perror("open");
        return -1;
    }
    cout << "file open successfully!" << endl;
    return 0;
}
