#include "multiboot.h"

#include "inc/fs.h"
#include "inc/timer.h"
#include "inc/error.h"
#include "inc/paging.h"
#include "inc/initrd.h"
#include "inc/kbDetect.h"
#include "inc/descriptorTables.h"
#include "inc/thread.h"

extern uint32 placement_address;

void listTree();
void launchShell();
void catFile(fs_node_t*);
uint32 findInitrd(struct multiboot*);

int kmain(struct multiboot* mboot_ptr)
{
    clearScreen();
    init_descriptor_tables();
    __asm__ __volatile__("sti");

    uint32 initrd_location = findInitrd(mboot_ptr);
    initialize_paging();

    // Initialize the initial ramdisk, and set it as the filesystem root.
    fs_root = initialize_initrd(initrd_location);
    
    print("================================================================================", 0x3F);
    print("                             Welcome to Q OS                                    ", 0x3F);
    print("================================================================================", 0x3F);

    init_scheduler();
    
    launchShell();
    return 0;
}

void catFile(fs_node_t* fsnode)
{
    printch('\n', 0x0F);
    if ((fsnode->flags & 0x7) == FS_FILE)
    {
        const uint32 rbuff = fsnode->length;
        char buf[rbuff];
        uint32 sz = read_fs(fsnode, 0, rbuff, (uint8*) buf);
        uint32 j;
        for (j = 0; j < sz; j++)
            printch(buf[j], 0x0F);
        printch('\n', 0x0F);
    }
}

void listTree() {
    // list the contents of
    int i = 0;
    struct dirent *node = 0;
    while ((node = readdir_fs(fs_root, i)) != 0)
    {
        fs_node_t *fsnode = finddir_fs(fs_root, node->name);

        if ((fsnode->flags & 0x7) == FS_DIRECTORY)
        {
            print("dir \t", 0x0F);
            print(node->name, 0x0F);
            printch('/', 0x0F);
        }
        else
        {
            print("file\t", 0x0F);
            print(node->name, 0x0F);
        }
        printch('\n', 0x0F);
        i++;
    }
}

uint32 findInitrd(struct multiboot* mboot_ptr)
{
    // Find the location of our initial ramdisk.
    ASSERT(mboot_ptr->mods_count > 0);
    uint32 initrd_location = *((uint32*)mboot_ptr->mods_addr);
    uint32 initrd_end = *(uint32*)(mboot_ptr->mods_addr+4);
    // Don't trample our module with placement accesses, please!
    placement_address = initrd_end;
    return initrd_location;
}

void kbHelp()
{
    print("\nKeybindings in Q OS:", 0x0F);
    print("\n\tCtrl-b -> left", 0x0F);
    print("\n\tCtrl-f -> right", 0x0F);
    print("\n\tCtrl-a -> home", 0x0F);
    print("\n\tCtrl-p -> up", 0x0F);
    print("\n\tCtrl-n -> down", 0x0F);
    print("\n\tCtrl-z -> quit", 0x0F);
    print("\n\tCtrl-l -> clear", 0x0F);
    print("\n", 0x0F);
}

void cat() {
    char bufStr[MAX_FNAME_LEN];
    print("\nFile Name>  ", 0x0F);
    readStr(bufStr, MAX_FNAME_LEN);
    ASSERT(strlength(bufStr) < MAX_FNAME_LEN);
    for(int i = 0; i < 100; i++)
    catFile(finddir_fs(fs_root, bufStr));
    exit_thread();
}

void launchShell() {
    print(PRO_TIP, 0x0F);
    kbHelp();
    //allocate some memory for command string buffer. 1kB should be enough for now
    const int bufSize = 128;
    const int editorBufSize = 1024;
    char bufStr[bufSize];
    char writerContents[editorBufSize];
    writerContents[0] = 0;
    while (true)
    {
        print("\nQ-Kernel>  ", 0x08);
        typingCmd = true;
        newCmd = true;
        readStr(bufStr, bufSize);
        typingCmd = false;

        if (strEql(strTrim(bufStr), ""))
        {
            print(COMMAND_HELP, 0x0F);
        }
        else if(strEql(bufStr, "help"))
        {
            kbHelp();
            print(PRO_TIP, 0x0F);
            print(COMMAND_HELP, 0x0F);
        }
        else if(strEql(bufStr, "skip"))
        {
            // It literally does nothing... (Useful at callback) 
        }
        else if(strEql(bufStr, "hi"))
        {
            print("\nHello!", 0x3F);
        }
        else if(strEql(bufStr, "files"))
        {
            print("\n", 0x0F);
            listTree();
        }
        else if(strEql(bufStr, "cat"))
        {
            uint32 pid = start_thread(&cat);
            wait_for_thread(pid);
        }
        else if(strEql(bufStr,"execute"))
        {
            print("\ntype>  ", 0x0F);
            readStr(bufStr, bufSize);
            if(strEql(bufStr,"repeat"))
            {
	            print("\nrepeat>  ", 0x0F);
	            readStr(bufStr, bufSize);
	            writing = 1;
	            while(true)
	            {
	                printch('\n', 0x0F);
	                print(bufStr, 0x0F);
	            }
            } 
            else if(strEql(bufStr,"c"))
            {
	            print("\nc>  ",0x0F);
	            readStr(bufStr, bufSize);
            }
            else
            {
	            print("\nThe 'execute' command does not support the command you entered or it does not exist ", 0x0F);
            }
        }
        else if(strEql(bufStr,"switch"))
        {
            print("\nThe specified directory was not found ", 0x0F);
        }
        else if(strEql(bufStr,"writer"))
        {
            clearScreen();
            print("================================================================================", 0x3F);
            print("                      Q OS Text Editor Version 0.2                              ", 0x3F);
            print("================================================================================", 0x3F);
            writing = true;
            readStr(writerContents, editorBufSize);
            writing = false;
        }
        else if(strEql(bufStr, "clear"))
        {
            clearScreen();
            cursorX = 0;
            cursorY = 0;
            updateCursor();
        }
        else if(strEql(bufStr, "clear -i"))
        {
            clearScreen();
            print("================================================================================", 0x3F);
            print("                             Welcome to Q OS                                    ", 0x3F);
            print("================================================================================", 0x3F);
        }
        else if(strEql(bufStr, "newdir"))
        {
            print("\nReserved", 0x0F);
        }
        else if(strEql(bufStr, "erase"))
        {
            print("\nReserved", 0x0F);
        }
        else
        {
            print("\nCommand Not Found ", 0x0F);
        }
        print("\n", 0x0F);
    }
}
