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
//unsigned int ContFramePool* ContFramePool::head = NULL;

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no)
{
    // TODO: IMPLEMENTATION NEEEDED!

    
    beg = _base_frame_no;
    frame_size =_n_frames;
    number_Free_Frames = _n_frames;
    ffo = _info_frame_no;
    
    //check if the ffo is zero
    if(ffo == 0)
    {
    	//if ffo is zero then store it in the first frame
    	bitmapframepool = (unsigned char *) (beg * FRAME_SIZE);
    }
    else
    {
    	//if ffo is not zero then store it in the provided frame
    	bitmapframepool = (unsigned char *) (ffo * FRAME_SIZE);
    }
    
    //the given bitmap should be accomodated by the given number of frames
    assert ((frame_size % 8)== 0);
    
    for(int zeta=0; zeta < frame_size; zeta++)
    {
    	bitmapframepool[zeta] = 0xFF;
    }
    
    if(ffo == 0)
    {
    	bitmapframepool[0] = 0x7F;
    	number_Free_Frames--; 
    }
    if(head == NULL)
    {
    	head = this;
    	head->next = NULL;
    }
    else{
    ContFramePool *v = NULL;
    for(v = head; v->next != NULL; v = v->next);
    v->next = this;
    v = this;
    v->next = NULL;
    }
    Console::puts("Frame Pool has been initialized!\n");
    //assert(false);

}

unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    assert(_n_frames > 0);
    
    unsigned int frmno = beg;
    
    for(int i = 0; i < _n_frames; i++)
    {
    	while(get_state(frmno) == FrameState::Used)
    	{
	    	set_state(frmno, FrameState::Used);
    		frmno++;
    		number_Free_Frames = number_Free_Frames - 1;
    	}
    }
    return (frmno + beg);
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    for(int fno = _base_frame_no; fno < _base_frame_no + _n_frames; fno++)
    	ms(fno);

}

void ContFramePool::ms(unsigned long _frame_no)
{
	unsigned long fm = _frame_no ;
	unsigned int bin = fm - beg;
	unsigned char st = 0xC0;
	if(!((fm >= beg) && (fm < beg + frame_size)))
	{
		Console::puts("\nOutside limit");
		assert(false);
	}
	assert((bitmapframepool[bin] & st)!=0);
	bitmapframepool[bin] ^= st;
}

void ContFramePool::sbm(unsigned int x, unsigned int _n_frames)
{
	unsigned int m = _n_frames;
	unsigned int n = _n_frames;
	unsigned char st = 0x80;
	while(m!=0)
	{
		if(m == n)
		{
			bitmapframepool[x] = bitmapframepool[x]^0xC0;
		}
		else{
		bitmapframepool[x] = bitmapframepool[x]^st;
		}
		number_Free_Frames = number_Free_Frames - 1;
		x=x+1;
		m = m - 1;
	}
}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    // TODO: IMPLEMENTATION NEEEDED!
    ContFramePool *xyz = NULL;
    unsigned long n = _first_frame_no;
    for(xyz = head; xyz != NULL; xyz = xyz->next)
    {
    	if((n >= xyz->beg) && (n <= (xyz->beg + xyz->frame_size - 1)))
    	{
    		xyz->r_singleframe(_first_frame_no);
    		break;
    	}
    }
}

void ContFramePool::r_singleframe(unsigned long _first_frame_no)
{
	unsigned long n = _first_frame_no;
	//unsigned long xy;
	//xy = (n - beg);
	if(!(bitmapframepool[n - beg] == 0x3F))
	{
		Console::puts("\nCannot release non-head frames\n");
		assert(false);
	}
	
	for(unsigned long xy = (n - beg); xy < frame_size;)
	{
		bitmapframepool[xy] |= 0xC0;
		xy = xy + 1;
		number_Free_Frames = number_Free_Frames + 1;
		if((bitmapframepool[xy] ^ 0xFF) == 0x80)
			continue;
		else
			break;
	}
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    unsigned long nf = _n_frames;
    unsigned long nif;
    nif = ((nf * 8) /(4*1024*8)) + (((nf * 8)%(4*1024*8) >0 ? 1: 0));
    return nif;
}
