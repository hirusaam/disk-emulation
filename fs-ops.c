#include "fs-ops.h"
extern struct filehandle_t file_handle_array[MAX_OPEN_FILES]; // Array for storing opened files

int fs_create(char *filename){
    
	struct inode_t inode;
    struct superblock_t superblock;

    // Check if the file already exists
    for (int i = 0; i < NUM_INODES; i++) {
        fs_readInode(i, &inode);
        if (inode.status == INODE_IN_USE && strcmp(inode.name, filename) == 0) {
            return -1; // File already exists
        }
    }

    // Allocate a free inode
    int inode_number = fs_allocInode();
    if (inode_number == -1) {
        return -1; // No available inodes
    }

    // Initialize the inode
    fs_readInode(inode_number, &inode);
    inode.status = INODE_IN_USE;
    strncpy(inode.name, filename, MAX_NAME_STRLEN);
    inode.file_size = 0;
    for (int i = 0; i < MAX_FILE_SIZE; i++) {
        inode.direct_blocks[i] = -1;
    }
    fs_writeInode(inode_number, &inode);

    return inode_number;

}


void fs_delete(char *filename){

	struct inode_t inode;

    // Locate the file inode
    for (int i = 0; i < NUM_INODES; i++) {
        fs_readInode(i, &inode);
        if (inode.status == INODE_IN_USE && strcmp(inode.name, filename) == 0) {
            // Free associated data blocks
            for (int j = 0; j < MAX_FILE_SIZE; j++) {
                if (inode.direct_blocks[j] != -1) {
                    fs_freeDataBlock(inode.direct_blocks[j]);
                }
            }
            // Free the inode
            fs_freeInode(i);
            return;
        }
    }

}

int fs_open(char *filename){
    
	struct inode_t inode;

    // Locate the file inode
    for (int i = 0; i < NUM_INODES; i++) {
        fs_readInode(i, &inode);
        if (inode.status == INODE_IN_USE && strcmp(inode.name, filename) == 0) {
            // Find a free file handle
            for (int j = 0; j < MAX_OPEN_FILES; j++) {
                if (file_handle_array[j].inode_number == -1) {
                    file_handle_array[j].inode_number = i;
                    file_handle_array[j].offset = 0;
                    return j;
                }
            }
            return -1; // No free file handles
        }
    }
    return -1; // File not found

}

void fs_close(int file_handle){

	if (file_handle >= 0 && file_handle < MAX_OPEN_FILES) {
        file_handle_array[file_handle].inode_number = -1;
        file_handle_array[file_handle].offset = 0;
    }


}

int fs_read(int file_handle, char *buf, int nbytes){
    
	if (file_handle < 0 || file_handle >= MAX_OPEN_FILES) return -1;

    struct filehandle_t *fh = &file_handle_array[file_handle];
    struct inode_t inode;
    fs_readInode(fh->inode_number, &inode);

    if (fh->offset + nbytes > inode.file_size) return -1; // Beyond file size

    int bytes_read = 0;
    int block_index = fh->offset / BLOCKSIZE;
    int block_offset = fh->offset % BLOCKSIZE;

    while (bytes_read < nbytes) {
        int to_read = BLOCKSIZE - block_offset;
        if (to_read > nbytes - bytes_read) {
            to_read = nbytes - bytes_read;
        }

        char block_data[BLOCKSIZE];
        fs_readDataBlock(inode.direct_blocks[block_index], block_data);
        memcpy(buf + bytes_read, block_data + block_offset, to_read);

        bytes_read += to_read;
        block_index++;
        block_offset = 0;
    }
    return 0;

}


int fs_write(int file_handle, char *buf, int nbytes){
    
	if (file_handle < 0 || file_handle >= MAX_OPEN_FILES) return -1;

    struct filehandle_t *fh = &file_handle_array[file_handle];
    struct inode_t inode;
    fs_readInode(fh->inode_number, &inode);

    if (fh->offset + nbytes > MAX_FILE_SIZE * BLOCKSIZE) return -1; // Exceeds max file size

    int bytes_written = 0;
    int block_index = fh->offset / BLOCKSIZE;
    int block_offset = fh->offset % BLOCKSIZE;

    while (bytes_written < nbytes) {
        int to_write = BLOCKSIZE - block_offset;
        if (to_write > nbytes - bytes_written) {
            to_write = nbytes - bytes_written;
        }

        if (inode.direct_blocks[block_index] == -1) {
            int new_block = fs_allocDataBlock();
            if (new_block == -1) return -1; // No free blocks
            inode.direct_blocks[block_index] = new_block;
        }

        char block_data[BLOCKSIZE];
        fs_readDataBlock(inode.direct_blocks[block_index], block_data);
        memcpy(block_data + block_offset, buf + bytes_written, to_write);
        fs_writeDataBlock(inode.direct_blocks[block_index], block_data);

        bytes_written += to_write;
        block_index++;
        block_offset = 0;
    }

    inode.file_size += bytes_written;
    fs_writeInode(fh->inode_number, &inode);

    return 0;

}


int fs_seek(int file_handle, int nseek){

    if (file_handle < 0 || file_handle >= MAX_OPEN_FILES) return -1;

    struct filehandle_t *fh = &file_handle_array[file_handle];
    struct inode_t inode;
    fs_readInode(fh->inode_number, &inode);

    int new_offset = fh->offset + nseek;
    if (new_offset < 0 || new_offset > inode.file_size) return -1;

    fh->offset = new_offset;
    return 0;
}