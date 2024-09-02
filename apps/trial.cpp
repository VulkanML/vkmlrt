#include <iostream>

#include "./ident.h"
#include "./square.h"

#include "rt.h"



int main(){

    std::cout << "HELLO WORLD" << std::endl;
    std::vector<vkmlrt::vkmlrt_device> devices;
	vkmlrt::init(devices);
	auto buf1 = vkmlrt::allocateBuffer(devices[0], 1024, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	//auto buf2 = vkmlrt::allocateBuffer(devices[1], 1024, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);   

	auto sbuf1 = vkmlrt::allocateBuffer(devices[0], 2048, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	//auto sbuf2 = vkmlrt::allocateBuffer(devices[1], 2048, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	auto dbuf1 = vkmlrt::allocateBuffer(devices[0], 2048, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	//auto dbuf2 = vkmlrt::allocateBuffer(devices[1], 2048, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	

	vkmlrt::vkmlrt_compute_pgrm pgrm;
	vkmlrt::write_shader_pgrm(devices[0], pgrm, square, sizeof(square) / sizeof(uint32_t));

	vkmlrt::write_buffer(devices[0], pgrm, 0, sbuf1);
	vkmlrt::write_buffer(devices[0], pgrm, 1, dbuf1);

	vkmlrt::write_compute_program(devices[0], pgrm);

    vkmlrt::destroy(devices);
    return 0;
}
