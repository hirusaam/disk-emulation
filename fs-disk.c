#include "fs-disk.h"

int DISK_FD;   // pointer to fs.txt
struct filehandle_t file_handle_array[MAX_OPEN_FILES]; // Array for storing opened files

void fs_readSuperBlock(struct superblock_t *superblock){

    char tempBuf[BLOCKSIZE];
    lseek(DISK_FD, 0, SEEK_SET);
    int ret = read(DISK_FD ,tempBuf, BLOCKSIZE);
    assert(ret == BLOCKSIZE);
    memcpy(superblock, tempBuf, sizeof(struct superblock_t));
}

void fs_writeSuperBlock(struct superblock_t *superblock){

    char tempBuf[BLOCKSIZE];
    memcpy(tempBuf, superblock, sizeof(struct superblock_t));
    lseek(DISK_FD, 0, SEEK_SET);
    int ret = write(DISK_FD ,tempBuf, BLOCKSIZE);
    assert(ret == BLOCKSIZE);
}

void fs_formatDisk(){

    FILE* fp;
    fp = fopen("fs", "w+");
    DISK_FD = fileno(fp);

    // Setting up superblock
    struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
    memcpy(superblock->name, "fs", 8);
    for(int i=0; i<NUM_INODES; i++)
        superblock->inode_freelist[i] = INODE_FREE;
    for(int i=0; i<NUM_DATA_BLOCKS; i++){
        superblock->datablock_freelist[i] = DATA_BLOCK_FREE;
    }
    fs_writeSuperBlock(superblock);
    free(superblock);
    
    // Setting up inode structure
    struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
    memcpy(inode->name, "", 1);
    inode->status = INODE_FREE;
    inode->file_size = 0;
    for(int i=0; i<MAX_FILE_SIZE; i++)
        inode->direct_blocks[i] = -1;
    for(int i=0; i<NUM_INODES; i++)
        fs_writeInode(i, inode);
    free(inode);

    // Formatting file handler array
    for(int i=0; i<MAX_OPEN_FILES; i++){
        file_handle_array[i].inode_number = -1;
        file_handle_array[i].offset = 0;
    }
}

int fs_allocInode(){

    struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
    fs_readSuperBlock(superblock);
    for(int i=0; i<NUM_INODES; i++){
        if(superblock->inode_freelist[i] == INODE_FREE){
            superblock->inode_freelist[i] = INODE_IN_USE;
            fs_writeSuperBlock(superblock);
            free(superblock);
            return i;
        }
    }
    free(superblock);
    return -1;
}

void fs_freeInode(int inodenum){

    assert(inodenum < NUM_INODES);
    struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
    struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
    fs_readSuperBlock(superblock);
    fs_readInode(inodenum, inode);
    assert(superblock->inode_freelist[inodenum] == INODE_IN_USE);
    superblock->inode_freelist[inodenum] = INODE_FREE;
    inode->status = INODE_FREE;
    inode->file_size = 0;
    for (int i = 0; i < MAX_FILE_SIZE; i++)
        inode->direct_blocks[i] = -1;
    fs_writeSuperBlock(superblock);
    fs_writeInode(inodenum, inode);
    free(inode);
    free(superblock);
}

void fs_readInode(int inodenum, struct inode_t *inodeptr){

    assert(inodenum < NUM_INODES);
    char tempBuf[BLOCKSIZE / NUM_INODES_PER_BLOCK];
    lseek(DISK_FD, BLOCKSIZE + inodenum * sizeof(struct inode_t), SEEK_SET);
    int ret = read(DISK_FD, tempBuf, sizeof(struct inode_t));
    assert(ret == sizeof(struct inode_t));
    memcpy(inodeptr, tempBuf, sizeof(struct inode_t));
}

void fs_writeInode(int inodenum, struct inode_t *inodeptr){

    assert(inodenum < NUM_INODES);
    char tempBuf[BLOCKSIZE / NUM_INODES_PER_BLOCK];
    memcpy(tempBuf, inodeptr, sizeof(struct inode_t));
    lseek(DISK_FD, BLOCKSIZE + inodenum * sizeof(struct inode_t), SEEK_SET);
    int ret = write(DISK_FD, tempBuf, sizeof(struct inode_t));
    assert(ret == sizeof(struct inode_t));
}

int fs_allocDataBlock(){

    struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
    fs_readSuperBlock(superblock);
    for (int i = 0; i < NUM_DATA_BLOCKS; i++){
        if (superblock->datablock_freelist[i] == DATA_BLOCK_FREE){
            superblock->datablock_freelist[i] = DATA_BLOCK_USED;
            fs_writeSuperBlock(superblock);
            free(superblock);
            return i;
        }
    }
    free(superblock);   
    return -1;
}

void fs_freeDataBlock(int blocknum){
 
    struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
    fs_readSuperBlock(superblock);
    assert(superblock->datablock_freelist[blocknum] == DATA_BLOCK_USED);
    superblock->datablock_freelist[blocknum] = DATA_BLOCK_FREE;
    fs_writeSuperBlock(superblock);
    free(superblock);
}

void fs_readDataBlock(int blocknum, char *buf){

    assert(blocknum < NUM_DATA_BLOCKS);
    char tempBuf[BLOCKSIZE];
    lseek(DISK_FD, BLOCKSIZE*(5 + blocknum), SEEK_SET);
    int ret = read(DISK_FD, tempBuf, BLOCKSIZE);
    // assert(ret == BLOCKSIZE);
    memcpy(buf, tempBuf, BLOCKSIZE);
}

void fs_writeDataBlock(int blocknum, char *buf){

    assert(blocknum < NUM_DATA_BLOCKS);
    char tempBuf[BLOCKSIZE];
    lseek(DISK_FD, BLOCKSIZE * (5 + blocknum), SEEK_SET);
    memcpy(tempBuf, buf, BLOCKSIZE); 
    int ret = write(DISK_FD, tempBuf, BLOCKSIZE);
    assert(ret == BLOCKSIZE);
}

void fs_dump(){

    printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<DISK STATE>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
    fs_readSuperBlock(superblock);
    char buf[MAX_NAME_STRLEN + 1];
    buf[MAX_NAME_STRLEN] = '\0';
    memcpy(buf, superblock->name, sizeof(buf) - 1);
    printf("DISK NAME: %s\nINODE FREELIST:\t", buf);
    for(int i=0; i<NUM_INODES; i++)
        printf("%c\t", superblock->inode_freelist[i]);
    printf("\nDATA BLOCK FREELIST:\t");
    for(int i=0; i<NUM_DATA_BLOCKS; i++)
        printf("%c\t", superblock->datablock_freelist[i]);
    printf("\n");

    struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
    for(int i=0; i<NUM_INODES; i++){
        fs_readInode(i, inode);
        if(inode->status == INODE_IN_USE){
            printf("INODE %d\nSTATUS:\t%c\tNAME\t%s\tSIZE\t%d\tDATABLOCK\t", i, inode->status, inode->name, inode->file_size);
            for (int j = 0; j < MAX_FILE_SIZE; j++)
                printf("%d\t", inode->direct_blocks[j]);
            printf("\n");
            for (int j = 0; j < MAX_FILE_SIZE; j++){
                if (inode->direct_blocks[j] != -1 ){
                    char tempBuf[BLOCKSIZE+1];
                    tempBuf[BLOCKSIZE] = '\0';
                    fs_readDataBlock(inode->direct_blocks[j], tempBuf);
                    printf("DATA BLOCK %d: %s\n", j, tempBuf);
                }
            }
            printf("\n");
        }     
    }
    free(inode);
    free(superblock);
    printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}
