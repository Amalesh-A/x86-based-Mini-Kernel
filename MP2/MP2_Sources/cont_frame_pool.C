/*
 File: ContFramePool.C
 
 Author:
 Date  : 
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no)
{
    this->base_frame_no = _base_frame_no;
    this->n_frames = _n_frames;

    // Calculate how many info frames we need
    unsigned long info_frames = needed_info_frames(_n_frames);
    this->info_frame_no = _info_frame_no;

    // Initialize bitmap in the first info_frames
    char *bitmap = (char *)(_base_frame_no * FRAME_SIZE);
    for (unsigned long i = 0; i < n_frames; i++) {
        bitmap[i] = 0; // Mark all frames as free initially
    }

    // Mark info frames as inaccessible
    for (unsigned long i = 0; i < info_frames; i++) {
        bitmap[i] = 3; // 3 = Inaccessible
    }

    Console::puts("ContFramePool initialized.\n");
}

unsigned long ContFramePool::get_total_frames() const {
    return n_frames;
}

unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    char *bitmap = (char *)(base_frame_no * FRAME_SIZE);

    // Search for _n_frames contiguous frames
    for (unsigned long i = 0; i <= n_frames - _n_frames; i++) {
        bool available = true;

        // Check if the next _n_frames frames are free
        for (unsigned int j = 0; j < _n_frames; j++) {
            if (bitmap[i + j] != 0) {
                available = false;
                break;
            }
        }

        // If available, allocate the frames
        if (available) {
            bitmap[i] = 1; // Head-of-sequence
            for (unsigned int j = 1; j < _n_frames; j++) {
                bitmap[i + j] = 2; // Allocated
            }

            unsigned long allocated_frame = base_frame_no + i;
            Console::puts("Allocated frames at frame number: ");
            Console::puti(allocated_frame);
            Console::puts("\n");
            return allocated_frame;
        }
    }
    Console::puts("Failed to allocate contiguous frames.\n");
    return 0; // Allocation failed
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    char *bitmap = (char *)(base_frame_no * FRAME_SIZE);
    unsigned long start = _base_frame_no - base_frame_no;

    for (unsigned long i = 0; i < _n_frames; i++) {
        bitmap[start + i] = 3; // Inaccessible
    }

    Console::puts("Marked frames as inaccessible.\n");
}

void ContFramePool::release_frames(unsigned long _first_frame_no, unsigned long base_frame_no)
{
    unsigned long index = _first_frame_no - base_frame_no;
    char *bitmap = (char *)(base_frame_no * FRAME_SIZE);

    // Validate head-of-sequence
    if (bitmap[index] != 1) {
        Console::puts("Error: Attempt to release a non-head frame!\n");
        return;
    }

    // Free the sequence
    while (bitmap[index] == 1 || bitmap[index] == 2) {
        bitmap[index] = 0; // Free frame
        index++;
    }

    Console::puts("Released frames starting at frame number: ");
    Console::puti((int)_first_frame_no);
    Console::puts("\n");
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    // Each frame can track FRAME_SIZE frames (since we store 1 byte per frame)
    unsigned long frames_needed = (_n_frames + FRAME_SIZE - 1) / FRAME_SIZE;
    return frames_needed;
}
