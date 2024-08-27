#include <iostream>
#include "./rt.h"
#include "./ident.h"
#include "./square.h"


int main(){
    auto r = rt::instance();
    auto buffer = r->allocateBuffer(0,1024);
    auto buffer2 = r->allocateBuffer(0,1024);
    auto tranfer = r->allocateTransferBuffer(0, 1024);
    
    auto cmd = r->buildFunction(0, (unsigned char*)square, sizeof(square), {buffer, buffer2});
    r->execute(0, cmd);
    std::cout << "HELLO WORLD" << std::endl;
    
    return 0;
}
