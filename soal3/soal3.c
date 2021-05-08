#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>

// Struct for app config
struct AppManager {
    int fileStart;
    int fileEnd;
    bool fileParse;
    bool starParse;
    bool folderParse;
    char **listOfFiles;
    int listOfFilesCount;
};

// Struct for one file to be moved
struct FileMoveRequest {
    int index;
    char *filename;
    char *folderName;
};

struct FileMoveRequest* NewFileMoveRequest(int index, char * filename, char * folderName);
int parseFilesInput(int argc, char **argv, struct AppManager* app, char *cwd);
char* getfolderName(char *filename);
char* mergeFileWithFolderName(char * filename, char * folderName);
char* getFileName(char *full_path);
void *moveFile(void *ptr);
int parseByFileList(struct AppManager* app, char **listOfFiles, int listOfFilesCount, char *output);
void listFiles(const char *path, const char *beforePath, char** argv, int* pointer);

int main(int argc, char** argv)
{
    //init our app config
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    struct AppManager app;
    app.fileParse = false;
    app.starParse = false;
    app.folderParse = false;
    (&app)->listOfFiles = malloc(argc * sizeof(char*));
    app.listOfFilesCount = 0;

    // read input
    for (int i=0; i<argc; i++) {
        if(strcmp(argv[i], "-f") == 0) {
            app.fileParse = true;

            parseFilesInput(argc, argv, &app, cwd);
            
            break;
        } else if(strcmp(argv[i], "*") == 0) {
            app.starParse = true;
            break;
        } else if(strcmp(argv[i], "-d") == 0) {
            app.folderParse = true;
            strcpy(cwd, argv[2]);
            break;
        }
    }
    
    // preparing parameters for process
    if(app.starParse || app.folderParse) {
        listFiles(cwd, cwd, (&app)->listOfFiles, &app.listOfFilesCount);
    }

    // for(int i =0;i<app.listOfFilesCount;i++) {
    //     printf("%s\n", (&app)->listOfFiles[i]);
    // }

    // run process
    parseByFileList(&app, (&app)->listOfFiles, app.listOfFilesCount, cwd);
  
    return 0;
}

// Create new `file move` request
struct FileMoveRequest* NewFileMoveRequest(int index, char * filename, char * folderName) {
    struct FileMoveRequest *r = malloc(sizeof(struct FileMoveRequest));
    r->index = index;
    r->filename = filename;
    r->folderName = folderName;
    return r;
}

// Parse input for option `-f`
int parseFilesInput(int argc, char **argv, struct AppManager* app, char *cwd) {
    int k = 0;

    char **n = app->listOfFiles;

    for(int a=2; a<argc; a++) {
        // push file to our list
        n[k] = malloc((strlen(argv[a]) + 1) * sizeof(char));

        // This if branch goal is:
        // turn any path to absolute path
        if(argv[a][0] == '/') {
            strcpy(n[k], argv[a]);
        } else {
            char *x = malloc((strlen(argv[a]) + strlen(cwd) + 1) * sizeof(char));
            sprintf(x, "%s/%s", cwd, argv[a]);
            strcpy(n[k], x);
        }
        
        (*app).listOfFilesCount = (*app).listOfFilesCount + 1;
        k++;
    }

    return 1;
}

// Get folder name from a filename. Example abc.txt -> txt, abc.tXt -> txt
char * getfolderName(char *filename) {
    char folderName[] = {};

    char * hidden = "Hidden";
    size_t length = strlen(filename);

    for (int i=0; i < length; i++) {
        if(filename[i] == '.') {
            if(i == 0) return hidden;
            else {
                char * folderName = (char*) malloc((strlen(filename) - i +1)*sizeof(char));
                sprintf(folderName, "%s", filename+i+1);

                for(int i=0;i<strlen(folderName);i++) {
                    *(folderName+i) = tolower(*(folderName+i));
                }

                return folderName;
            }
        }
    }

    return "Unknown";
}

// Merge filename with absolute directory path
char * mergeFileWithFolderName(char * filename, char * folderName) {
    char *result = (char*) malloc((strlen(filename)+strlen(folderName)+1)*sizeof(char));
    sprintf(result, "%s/%s", folderName, filename);
    return result;
}

// Get filename from full path
char *getFileName(char *full_path) {
    for(int i=strlen(full_path)-1;i>=0;i--) {
        if(full_path[i] == '/') {
            return &full_path[i+1];
        } 
    }
}

// Move one file
void *moveFile(void *ptr) {
    struct FileMoveRequest* r;
    r = (struct FileMoveRequest*) ptr;

    char * folderName = getfolderName(r->filename);
    char * absoluteDestination = (char*) malloc((strlen(r->folderName) +strlen(folderName)+1)*sizeof(char));
    sprintf(absoluteDestination, "%s/%s", r->folderName, folderName);
    
    char * dest = mergeFileWithFolderName(getFileName(r->filename), absoluteDestination);

    // make directory for file
    mkdir(folderName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    // move file

    int res = rename(r->filename, dest);
    if(res != 0) printf("File %d : Sad, gagal :(\n", (*r).index + 1);
    else printf("File %d : Berhasil Dikategorikan\n", (*r).index + 1);
}

// Categorize list of files to dependent folders
int parseByFileList(struct AppManager* app, char **listOfFiles, int listOfFilesCount, char *output) {
    pthread_t thread_id[listOfFilesCount];
    int err;

    for(int i=0; i<listOfFilesCount; i++) {
        // prepare parameter of a file
        struct FileMoveRequest* r = NewFileMoveRequest(i, listOfFiles[i], output);

        // create thread to move the file with r as parameter
        err=pthread_create(&(thread_id[i]),NULL,&moveFile, (void *) r);
    }

    for(int i=0; i<listOfFilesCount; i++) {
        int x = pthread_join(thread_id[i], NULL);
        if(x == 0) {
            // printf("Berhasil keluar thread\n");
        } else {
            // printf("Gagal keluar thread\n");
        }
    }

    return 1;
}

// Get all files (absolute path) in a directory *path recursively
void listFiles(const char *path, const char *beforePath, char** argv, int* pointer)
{
    struct dirent *dp;
    DIR *dir = opendir(beforePath);

    // Unable to open directory stream
    if (!dir) 
        return; 

    while ((dp = readdir(dir)) != NULL)
    {
        if(strcmp(dp->d_name, "..") <= 0) continue;

        if(dp->d_type == DT_DIR) {
            char *result = (char*) malloc((strlen(dp->d_name)+strlen(beforePath)+2)*sizeof(char));
            sprintf(result, "%s%s%s", beforePath, "/", dp->d_name);

            argv[(*pointer)] = result;

            listFiles(dp->d_name, result, argv, pointer);
            
            continue;
        }

        char *result = (char*) malloc((strlen(dp->d_name)+strlen(beforePath)+2)*sizeof(char));
        sprintf(result, "%s%s%s", beforePath, "/", dp->d_name);
        
        // skip if it is our program file
        if((strcmp(dp->d_name, "soal3") == 0) || strcmp(dp->d_name, "soal3.c") == 0) continue;

        // push file to our list
        argv[(*pointer)] = malloc((strlen(result) + 1) * sizeof(char));
        strcpy(argv[(*pointer)], result);
        (*pointer) = (*pointer) +1;
    }

    free(dp);

    // Close directory stream
    closedir(dir);
}