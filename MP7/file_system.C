/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file_system.H"

/*--------------------------------------------------------------------------*/
/* CLASS Inode */
/*--------------------------------------------------------------------------*/

/* You may need to add a few functions, for example to help read and store 
   inodes from and to disk. */

/*--------------------------------------------------------------------------*/
/* CLASS FileSystem */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem()  : disk(nullptr), size(0), inodes(nullptr), free_blocks(nullptr) {}

FileSystem::~FileSystem() {
    //Console::puts("unmounting file system\n");
    /* Make sure that the inode list and the free list are saved. */
    //assert(false);
    if (disk) {
        disk->Unmount();
    }
    if (inodes) {
        delete[] inodes;
    }
    if (free_blocks) {
        delete[] free_blocks;
    }
}



/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/


bool FileSystem::Mount(SimpleDisk * _disk) {
    //Console::puts("mounting file system from disk\n");

    /* Here you read the inode list and the free list into memory */
    
    //assert(false);
    if (_disk->Mount()) 
    {
        disk = _disk;
        size = disk->GetSize();
        inodes = new Inode[MAX_INODES];
        unsigned int num_blocks = size / SimpleDisk::BLOCK_SIZE;
        free_blocks = new unsigned char[num_blocks];
        for (unsigned int i = 0; i < num_blocks; ++i) {
            free_blocks[i] = 1;
        }
        return true;
    }
    return false;
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) { // static!
    //Console::puts("formatting disk\n");
    /* Here you populate the disk with an initialized (probably empty) inode list
       and a free list. Make sure that blocks used for the inodes and for the free list
       are marked as used, otherwise they may get overwritten. */
    //assert(false);
    if (_disk->Mount()) {
        unsigned int num_blocks = _size / SimpleDisk::BLOCK_SIZE;
        unsigned int free_blocks_offset = 1;
        for (unsigned int i = 0; i < free_blocks_offset; ++i) {
            _disk->WriteBlock(i, reinterpret_cast<char*>(free_blocks));
        }
        for (unsigned int i = free_blocks_offset; i < num_blocks; ++i) {
            _disk->ClearBlock(i);
        }
        _disk->Unmount();
        return true;
    }
    return false;
}


Inode * FileSystem::LookupFile(int _file_id) {
    //Console::puts("looking up file with id = "); Console::puti(_file_id); Console::puts("\n");
    /* Here you go through the inode list to find the file. */
    //assert(false);
        for (unsigned int i = 0; i < MAX_INODES; ++i) {
        if (inodes[i].GetFileId() == _file_id) {
            return &inodes[i];
        }
    }
    return nullptr;
}

bool FileSystem::CreateFile(int _file_id) {
    //Console::puts("creating file with id:"); Console::puti(_file_id); Console::puts("\n");
    /* Here you check if the file exists already. If so, throw an error.
       Then get yourself a free inode and initialize all the data needed for the
       new file. After this function there will be a new file on disk. */
    //assert(false);
        if (LookupFile(_file_id)) {
        return false;
    }
    short inode_idx = GetFreeInode();
    if (inode_idx == -1) {
        return false;
    }
    inodes[inode_idx].Initialize(_file_id);
    return true;
}

bool FileSystem::DeleteFile(int _file_id) {
//    Console::puts("deleting file with id:"); Console::puti(_file_id); Console::puts("\n");
    /* First, check if the file exists. If not, throw an error. 
       Then free all blocks that belong to the file and delete/invalidate 
       (depending on your implementation of the inode list) the inode. */
    Inode* inode = LookupFile(_file_id);
    if (!inode) {
        return false;
    }
    inode->Delete();
    return true;
}

}
