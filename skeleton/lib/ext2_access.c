// ext2 definitions from the real driver in the Linux kernel.
#include "ext2fs.h"

// This header allows your project to link against the reference library. If you
// complete the entire project, you should be able to remove this directive and
// still compile your code.
#include "reference_implementation.h"

// Definitions for ext2cat to compile against.
#include "ext2_access.h"



///////////////////////////////////////////////////////////
//  Accessors for the basic components of ext2.
///////////////////////////////////////////////////////////

// Return a pointer to the primary superblock of a filesystem.
struct ext2_super_block * get_super_block(void * fs) {
    // FIXME: Uses reference implementation.
    return (struct ext2_super_block *) ((char*)fs + SUPERBLOCK_OFFSET);

    //return _ref_get_super_block(fs);
}


// Return the block size for a filesystem.
__u32 get_block_size(void * fs) {
    // FIXME: Uses reference implementation.
    //block size = 1024 << s_log_block_size;
    return EXT2_BLOCK_SIZE(get_super_block(fs));
    //return _ref_get_block_size(fs);
}


// Return a pointer to a block given its number.
// get_block(fs, 0) == fs;
void * get_block(void * fs, __u32 block_num) {
    // FIXME: Uses reference implementation.
    //return fs + block_num * block_size
    
    return (void*) ((char*)fs + block_num*get_block_size(fs) );
    //return _ref_get_block(fs, block_num);
}


// Return a pointer to the first block group descriptor in a filesystem. Real
// ext2 filesystems will have several of these, but, for simplicity, we will
// assume there is only one.
struct ext2_group_desc * get_block_group(void * fs, __u32 block_group_num) {
    // FIXME: Uses reference implementation.
    
    // it's the first block after superblock
    return (struct ext2_group_desc *) ((char*)(get_super_block(fs)) + SUPERBLOCK_SIZE);
    
    //return _ref_get_block_group(fs, block_group_num);
}


// Return a pointer to an inode given its number. In a real filesystem, this
// would require finding the correct block group, but you may assume it's in the
// first one.
struct ext2_inode * get_inode(void * fs, __u32 inode_num) {
    // FIXME: Uses reference implementation.
    
    //get to the block group 
    struct ext2_group_desc * blockGroup = get_block_group(fs,(__u32) 0 );
    
    //get to the itable
    struct ext2_inode* iNodeTablePtr = (struct ext2_inode*) (get_block(fs,(blockGroup->bg_inode_table)));
    
    struct ext2_inode* iNodePtr = iNodeTablePtr + inode_num-1;
    return iNodePtr;
    
    //get to the inode_num element = inode_num*
    
    //return _ref_get_inode(fs, inode_num);
}



///////////////////////////////////////////////////////////
//  High-level code for accessing filesystem components by path.
///////////////////////////////////////////////////////////

// Chunk a filename into pieces.
// split_path("/a/b/c") will return {"a", "b", "c"}.
//
// This one's a freebie.
char ** split_path(char * path) {
    int num_slashes = 0;
    for (char * slash = path; slash != NULL; slash = strchr(slash + 1, '/')) {
        num_slashes++;
    }

    // Copy out each piece by advancing two pointers (piece_start and slash).
    char ** parts = (char **) calloc(num_slashes, sizeof(char *));
    char * piece_start = path + 1;
    int i = 0;
    for (char * slash = strchr(path + 1, '/');
         slash != NULL;
         slash = strchr(slash + 1, '/')) {
        int part_len = slash - piece_start;
        parts[i] = (char *) calloc(part_len + 1, sizeof(char));
        strncpy(parts[i], piece_start, part_len);
        piece_start = slash + 1;
        i++;
    }
    // Get the last piece.
    parts[i] = (char *) calloc(strlen(piece_start) + 1, sizeof(char));
    strncpy(parts[i], piece_start, strlen(piece_start));
    return parts;
}


// Convenience function to get the inode of the root directory.
struct ext2_inode * get_root_dir(void * fs) {
    return get_inode(fs, EXT2_ROOT_INO);
}


// Given the inode for a directory and a filename, return the inode number of
// that file inside that directory, or 0 if it doesn't exist there.
// 
// name should be a single component: "foo.txt", not "/files/foo.txt".
__u32 get_inode_from_dir(void * fs, struct ext2_inode * dir, 
        char * name) {
    // FIXME: Uses reference implementation.
    
    //first, go through all the data blocks 
        //for each data block:
        //  while (ptr<(block+blockSize):
        //      if find name, return True, 
        //      else, find size and ptr + size of that struct
        //return False
        
    struct ext2_dir_entry* firstEntry = (struct ext2_dir_entry*) (get_block(fs,dir->i_block[0]));
    
    struct ext2_dir_entry* current = firstEntry;
    //while ((__u32)(current - firstEntry) < (__u32)(dir->i_size))
    while (current->inode!=0)
    {
        //strcmp of name and input
        if (strncmp(current->name,name,strlen(name))==0)
        {
            return current->inode;
        }
        else
        {
            current = (struct ext2_dir_entry*) ((char*)(current) + (current->rec_len));
        }
        //if yes, return inode number
        //else, go to next one (plus size)
        
    }
    //return 0
    return 0;
    
    
    //return _ref_get_inode_from_dir(fs, dir, name);
}



// Find the inode number for a file by its full path.
// This is the functionality that ext2cat ultimately needs.
__u32 get_inode_by_path(void * fs, char * path) {
    // FIXME: Uses reference implementation.
    
    int num_slashes = 0;
    for (char * slash = path; slash != NULL; slash = strchr(slash + 1, '/')) {
        num_slashes++;
    }
    //how many parts do we have? put in array

    char ** parts = split_path(path);
    
    //now we go through each path (go into the directory)
     
    struct ext2_inode * currentDir = get_root_dir(fs);
    __u32 iNodeNumber;
    
    int i;
    for (i=0;i<num_slashes;i++)
    {
        //find the file (or directory at that level)
        iNodeNumber = get_inode_from_dir(fs,currentDir,parts[i]);
        
        if (iNodeNumber==0)
        {
            return 0;
        }
        
        //otherwise, currentDir = get_inode
        if (i<(num_slashes-1))
        {
            currentDir = get_inode(fs,iNodeNumber);
        }
    }
    
    return iNodeNumber;
    
    
    
    
    
    
    //return _ref_get_inode_by_path(fs, path);
}

