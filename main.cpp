#include "library.cpp"
#include <iostream>
#include <unistd.h>
//
// Created by User on 31-08-2019.
//
using namespace TaskParallelize;
void f1(){
    for(int i=0;i<10;i++)
        std::cout<<i<<" ";
}
void f2(){
    for(int i=1;i<=10;i++) {
        std::cout << "A" << i << " ";
        sleep(1);
    }
}
void f3(){
    for(int i=1;i<=10;i++) {
        std::cout << 'B' << i << " ";
        sleep(1);
    }
}
int main(){
    taskUniverse t;
    task t1 = t.createTask(f1);
    task t2 = t.createTask(f2);
    task t3 = t.createTask(f3);
    t1.then(t2);
    t1.then(t3);
    t.runUniverse();

}

