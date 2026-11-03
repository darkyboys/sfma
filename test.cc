#include "sfma.hh"
#include <iostream>

int main(){
    sfma::SFMA_BLOCK_SIZE = 64;
    sfma::MemoryFile file("new.memory");
    file.set(10, "Hello world how are you all ?");
    std::cout<<"Data at index: "<<file.get(10).data<<'\n';
}
