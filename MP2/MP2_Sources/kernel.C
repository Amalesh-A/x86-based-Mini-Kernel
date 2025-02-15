/*
    File: kernel.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 02/02/17

    This file has the main entry point to the operating system.

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

#define MB (0x1 << 20)
#define KB (0x1 << 10)

#define KERNEL_POOL_START_FRAME ((2 * MB) / (4 * KB))
#define KERNEL_POOL_SIZE ((2 * MB) / (4 * KB))
#define PROCESS_POOL_START_FRAME ((4 * MB) / (4 * KB))
#define PROCESS_POOL_SIZE ((28 * MB) / (4 * KB))

#define MEM_HOLE_START_FRAME ((15 * MB) / (4 * KB))
#define MEM_HOLE_SIZE ((1 * MB) / (4 * KB))

#define TEST_START_ADDR_PROC (4 * MB)
#define TEST_START_ADDR_KERNEL (2 * MB)

#define N_TEST_ALLOCATIONS 32

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "machine.H"     /* LOW-LEVEL STUFF   */
#include "console.H"

#include "assert.H"
#include "cont_frame_pool.H"  /* The physical memory manager */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

void test_memory(ContFramePool * _pool, unsigned int _allocs_to_go);

/*--------------------------------------------------------------------------*/
/* MAIN ENTRY INTO THE OS */
/*--------------------------------------------------------------------------*/

int main() {

    Console::init();

    // Initialize kernel memory pool
    ContFramePool kernel_mem_pool(KERNEL_POOL_START_FRAME,
                                  KERNEL_POOL_SIZE,
                                  0);

    // Initialize process memory pool
    unsigned long n_info_frames = kernel_mem_pool.needed_info_frames(PROCESS_POOL_SIZE);
    unsigned long process_mem_pool_info_frame = kernel_mem_pool.get_frames(n_info_frames);
    
    ContFramePool process_mem_pool(PROCESS_POOL_START_FRAME,
                                   PROCESS_POOL_SIZE,
                                   process_mem_pool_info_frame);
    
    process_mem_pool.mark_inaccessible(MEM_HOLE_START_FRAME, MEM_HOLE_SIZE);

    Console::puts("Frame Manager Initialized!\n");

    // Test memory allocator
    test_memory(&kernel_mem_pool, N_TEST_ALLOCATIONS);

    // Signal end of testing
    Console::puts("Testing DONE. Entering idle loop...\n");
    for (;;);
    return 1;
}

/*--------------------------------------------------------------------------*/
/* MEMORY TEST FUNCTION */
/*--------------------------------------------------------------------------*/

void test_memory(ContFramePool * _pool, unsigned int _allocs_to_go) {
    Console::puts("Allocations left: "); 
    Console::puti(_allocs_to_go); 
    Console::puts("\n");

    if (_allocs_to_go > 0) {
        int n_frames = (_allocs_to_go % 4) + 1;
        if (n_frames > _pool->get_total_frames()) {
            n_frames = _pool->get_total_frames();
        }

        // Allocate contiguous frames
        unsigned long frame = _pool->get_frames(n_frames);
        if (!frame) {
            Console::puts("Frame allocation failed!\n");
            return;
        }

        int *value_array = (int *)(frame * (4 * KB));
        
        // Write test pattern
        for (int i = 0; i < (1 * KB) * n_frames; i++) {
            value_array[i] = _allocs_to_go;
        }

        // Recursive call to stress test allocator
        test_memory(_pool, _allocs_to_go - 1);

        // Verify memory integrity
        for (int i = 0; i < (1 * KB) * n_frames; i++) {
            if (value_array[i] != _allocs_to_go) {
                Console::puts("ERROR: Memory test failed!\n");
                Console::puts("Index: "); Console::puti(i);
                Console::puts(" Value: "); Console::puti(value_array[i]); 
                Console::puts(" Expected: "); Console::puti(_allocs_to_go);
                Console::puts("\n");
                for (;;);
            }
        }

        // Release the frames after verification
        ContFramePool::release_frames(frame, _pool->get_total_frames());
    }
}