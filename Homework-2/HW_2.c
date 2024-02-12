// printCurrentDirectory https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
// https://www.quora.com/How-do-I-print-type-of-variable-in-c
// https://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
// https://man7.org/linux/man-pages/man3/opendir.3.html
// https://cboard.cprogramming.com/c-programming/72671-displaying-file-permissions.html
// https://www.geeksforgeeks.org/function-pointer-in-c/


# include <stdio.h>
# include <string.h>
# include <dirent.h>
# include <unistd.h> 
# include <sys/stat.h>
# include <sys/types.h>
# include <libgen.h>
# include <stdlib.h>
# include <time.h>

void SymboliclinkHandling(char* PathForInnerDir, char* d_name){

    char targFilePath[1000];
    char filepath[1024];
   
    ssize_t size_target;
    struct stat targetData, symblinkData;

    snprintf(filepath, sizeof(filepath), "%s/%s", PathForInnerDir, d_name);
    printf("1. %s, %s, %s\n", PathForInnerDir, d_name, filepath);

    //check if symbolink present F_OK = existence check
    if(access(filepath, F_OK) == 0){
        
        printf("%s", filepath);
        // read the target 
        size_target = readlink(PathForInnerDir, targFilePath, sizeof(targFilePath) - 1);
        // printf("size = %ld" , size_target);

        switch(size_target){
            case -1:
                printf("Error");
                break;
            
            default:
                targFilePath[size_target] = '\0';  // end with null
                printf("%s -> (%s)\n ", d_name, targFilePath);
                break;
        }
    }

   
}
void printCurrentDirectory(char* Directory, int depth){

    // pointer to read the directory
    DIR *directory;
    struct dirent *readptr;
    struct stat element;
    ssize_t size;
    char buf[100], target_buffer[100];
    int i;
 
    directory = opendir(Directory); 

    // open the directory path
    if(directory == NULL){
        printf("Error in opening directory");
    }

    // depth = 0 for starting directory. 1 for the directory inside 0. 2 for dir inside 1
    if (depth == 0) {
        char *startdirectoryName = basename(getcwd(buf, sizeof(buf)));
        printf("%s\n", startdirectoryName);
        depth = depth +1;
    } 
    

    // read contents until its not NULL
    // printf("Directory = %s", Directory);
    while ((readptr = readdir(directory)) != NULL) {    // readdir is incrementing the pointer 
        
        
        if(lstat(readptr->d_name, &element) == 0)   // cheeck if the file/dir exists or not
        {
            
            char PathForInnerDir[1024];
            
            if(depth != 0 && (strcmp(readptr->d_name, ".") != 0) && strcmp(readptr->d_name, ".."))
                snprintf(PathForInnerDir, sizeof(PathForInnerDir), "%s/%s", Directory, readptr->d_name);         
            
            if(depth == 0){
                snprintf(PathForInnerDir, sizeof(PathForInnerDir), "%s/%s", Directory, readptr->d_name); 
            }
            
            for (i = 0; i < depth; i++) {
                printf("\t");
            }
            if(S_ISDIR(element.st_mode)){

                if(strcmp(".", readptr->d_name) == 0 || strcmp("..", readptr->d_name) == 0)
                    continue;

                printf("%s\n", readptr->d_name);
                printCurrentDirectory(PathForInnerDir, depth+1);
            } 

            else if(S_ISLNK(element.st_mode)){ 
                
                printf("symbolic link");
                
                  
                SymboliclinkHandling(getcwd(buf, sizeof(buf)), readptr->d_name);
                continue;

            }

            else if(S_ISREG(element.st_mode)){

                printf("%s\n", readptr->d_name);
                
            }
            
        } 
    }


    closedir(directory);

}




void funcForv(char * fileDirectory, int depth){      // -v
                  
    struct stat ele;
    DIR *Directory = opendir(fileDirectory);
    struct dirent *readptr;
    char tbuffer[100];
    

    printf("\n ***list Directory Contents and Attributes*** \n");

    readptr = readdir(Directory);
    
    // keep reading the directory until NULL
    while(readptr != NULL){

        //1. determine whether the content is a file or a directory. 
        //2. If it is a directory print contents of the directory
        //3. If it is a file then use filepointer and inbuilt funcs to print attributes

        if(S_ISREG(ele.st_mode)){

            // st_size gives size in bytes
            printf("\n %s, %ld, %o, (%s)", (char *)readptr->d_name, (long)ele.st_size, ele.st_mode, ctime(&ele.st_atime));
        }
        if(S_ISDIR(ele.st_mode)){
            funcForv(readptr->d_name, depth);
        }

        
    }
}

void funcForL(char * fileDirectory, int size){                 // -L
    printf("\n -L listFilesMoreThanSpecifiedSize for = %s", fileDirectory);
    struct stat L_ele;
    DIR *LDirectory = opendir(fileDirectory);
    struct dirent *Lreadptr;
    

    if(LDirectory == NULL){
        printf("No such directory");
    }
    
    else{
        do{
            if(strcmp(".", Lreadptr->d_name) == 0 || strcmp("..", Lreadptr->d_name) == 0)
                    continue;
            else if(S_ISREG(L_ele.st_mode) && (L_ele.st_size) >= 1024){
                printf("%s %ld", Lreadptr->d_name, L_ele.st_size);
            }   

        }while((Lreadptr = readdir(LDirectory)) != NULL);
    }


}

void funcFors(char * fileDirectory, char * substring, int depth){                 // -s
    printf("\n -s SearchString for = %s", fileDirectory);

}

void funcFort(char * fileDirectory){                 // -t
    printf("\n -t  for = %s", fileDirectory);

}

int main(int argc, char *argv[]) {

    int cmdOption, i, count=0;
    char currentDirectoryfromPath[1024];
    char cwd[1024];
    typedef void (*fp)(char *, int);
    int size, depth, vdepth;
    char *substring;


    
    // printf("%s", getcwd(cwd, sizeof(cwd)));
    // option 1
    if(argc == 1){
        
        printCurrentDirectory(getcwd(cwd, sizeof(cwd)), 0);
       
    }

    // The getopt() function is a builtin function in C and is used to parse command line arguments.-v in any Linux command means verbose.
    
    while((cmdOption = getopt(argc, argv, "vL:s:t:")) != -1)
    {
        printf("Entered while loop");
        switch(cmdOption)
        {

            case 'v':  // same args , same return type and same type of args
                    printf("Inside case v");
                    vdepth = atoi(optarg);
                    funcForv("/", vdepth);
                    break;

            case 'L': 
                    
                    size = atoi(optarg);   // it willl give args for L option
                    funcForL("/", size);
                    break;

            case 's': 
                    
                    substring = optarg;
                    depth = atoi(optarg++);
                    funcFors("/", substring, depth);
                    break;

            case 't': 
                    funcFort("/");  // -t f = files and -t d = directories print
                    break;

            case ':':  
                printf("Error! value needed for the option\n");  
                break; 

            case '?':  
                printf("Error! unknown option "); 
                break;   
            

        }
    }

    return 0;
}