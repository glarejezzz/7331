#include <iostream>
#include <string>

int add(int a, int b){

    return a + b;
}
int main(){
    std::string name;
    std::cout <<"Input your name:   ";
    std::cin >> name;

    std::cout << "Greetings, " << name << std::endl;
    return 0;
}