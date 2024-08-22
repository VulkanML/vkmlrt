#include <iostream>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

struct meta_data {
    size_t size;
    size_t offset;
    bool free;
};

class BuddyAllocator {
    size_t total_size;
    size_t min_block_size;
    std::vector<std::vector<meta_data>> free_blocks;

    void splitBlock(meta_data& block, size_t size) {
        while (block.size > size) {
            block.size /= 2;
            meta_data buddy = {block.size, block.offset ^ block.size), true};
            free_blocks[log2(block.size)].push_back(buddy);
        }
    }

    void mergeBlock(meta_data& block) {
        size_t buddy_offset = block.offset ^ block.size;
        for (auto it = free_blocks[log2(block.size)].begin(); it != free_blocks[log2(block.size)].end(); ++it) {
            if (it->offset == buddy_offset && it->free) {
                it->free = false;
                block.size *= 2;
                block.offset = std::min(block.offset, it->offset);
                free_blocks[log2(block.size / 2)].erase(it);
                mergeBlock(block);
                return;
            }
        }
        free_blocks[log2(block.size)].push_back(block);
    }

public:
    BuddyAllocator(size_t totalSize, size_t minBlockSize) : total_size(totalSize), min_block_size(minBlockSize) {
        size_t levels = log2(total_size) + 1;
        free_blocks.resize(levels);
        free_blocks.back().push_back({total_size, 0, true});
    }

    meta_data allocate(size_t size) {
        size = pow(2, ceil(log2(size)));
        if (size < min_block_size) size = min_block_size;
        for (size_t i = log2(size); i < free_blocks.size(); ++i) {
            if (!free_blocks[i].empty()) {
                meta_data block = free_blocks[i].back();
                free_blocks[i].pop_back();
                block.free = false;
                splitBlock(block, size);
                return block;
            }
        }
        throw std::bad_alloc(); // Out of memory
    }
    
    void print() {
    
        for(size_t i = 0; i < free_blocks.size(); ++i){
            cout << i << ": ";
            for(auto free_block : free_blocks[i]){
                cout << "o:" << free_block.offset << " s:" << free_block.size << ", ";
            }
            cout << endl;
        }
        
        cout << "\n\n";
    }

    void deallocate(meta_data block) {
        block.free = true;
        cout << "deblock " << block.offset << " " << block.size << endl;
        mergeBlock(block);
    }
};

int main() {
    BuddyAllocator allocator(1024, 32);
    meta_data block0 = allocator.allocate(200);
    allocator.print();
    meta_data block1 = allocator.allocate(200);
    allocator.print();
    meta_data block2 = allocator.allocate(200);
    allocator.print();
    meta_data block3 = allocator.allocate(200);
    allocator.print();
    
    // cout << "original" << endl;
    // allocator.print();
    // meta_data block1 = allocator.allocate(64);
    // cout << "block1 " << block1.offset << " " << block1.size << endl;
    // allocator.print();
    // meta_data block2 = allocator.allocate(200);
    // cout << "block2 " << block2.offset << " " << block2.size << endl;
    // allocator.print();
    // meta_data block3 = allocator.allocate(500);
    // cout << "block3 " << block3.offset << " " << block3.size << endl;
    // allocator.print();    
    // meta_data block4 = allocator.allocate(32);
    // cout << "block4 " << block4.offset << " " << block4.size << endl;
    // allocator.print();     
    // meta_data block5 = allocator.allocate(45);
    // cout << "block5 " << block5.offset << " " << block5.size << endl;
    // allocator.print(); 
    // meta_data block6 = allocator.allocate(70);
    // cout << "block6 " << block6.offset << " " << block6.size << endl;
    // allocator.print(); 
    
    // allocator.deallocate(block1);
    // allocator.deallocate(block2);
    // allocator.deallocate(block3);
    // allocator.deallocate(block4);
    // allocator.deallocate(block5);
    

    return 0;
}