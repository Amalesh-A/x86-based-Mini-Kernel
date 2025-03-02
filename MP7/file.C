/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
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
#include "file.H"
#include "file_system.C"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR */
/*--------------------------------------------------------------------------*/

File::File(FileSystem *_fs, int _id)  :
        inode(_fs->GetInode(_id)),
        fs(_fs),
        pos(0),
        dirty_cache(false),
        eof(false)
    {
        // Read the first block of the file into the cache
        fs->disk->ReadBlock(inode->data_blocks[0], block_cache);
    }




File::~File() {
        // If the cache is dirty, write it back to disk
        if (dirty_cache) {
            fs->disk->WriteBlock(inode->data_blocks[0], block_cache);
        }
    }


/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char *_buf) {
        // Don't read past the end of the file
        if (pos >= inode->file_size) {
            eof = true;
            return 0;
        }

        // Read up to _n bytes from the cache into _buf
        int bytes_to_read = std::min(_n, inode->file_size - pos);
        std::memcpy(_buf, block_cache + pos % SimpleDisk::BLOCK_SIZE, bytes_to_read);
        pos += bytes_to_read;

        // If we've read to the end of the cache, load the next block
        if (pos % SimpleDisk::BLOCK_SIZE == 0 && pos < inode->file_size) {
            fs->disk->ReadBlock(inode->data_blocks[pos / SimpleDisk::BLOCK_SIZE], block_cache);
            dirty_cache = false;
        }

        return bytes_to_read;
    }


int File::Write(unsigned int _n, const char *_buf) {
        // Write up to _n bytes from _buf to the cache
        int bytes_to_write = std::min(_n, SimpleDisk::BLOCK_SIZE - pos % SimpleDisk::BLOCK_SIZE);
        std::memcpy(block_cache + pos % SimpleDisk::BLOCK_SIZE, _buf, bytes_to_write);
        pos += bytes_to_write;
        dirty_cache = true;

        // If we've written to the end of the cache, write it back to disk
        if (pos % SimpleDisk::BLOCK_SIZE == 0) {
            fs->disk->WriteBlock(inode->data_blocks[pos / SimpleDisk::BLOCK_SIZE - 1], block_cache);
            dirty_cache = false;
        }

        // If we've written past the end of the file, update the file size
        if (pos > inode->file_size) {
            inode->file_size = pos;
        }

        return bytes_to_write;
    }

void File::Reset(){
        pos = 0;
        eof = false;
    }


bool File::EoF() {
        return eof || pos == inode->file_size;
    }

