#include<stdio.h>
#include <fcntl.h>
#include <stdlib.h>


void displayMenu(){
    printf("Choose an option\n");
    printf("1. Press r for Read\n");
    printf("2. Press w for Write\n");
    printf("3. Press s for Seek\n");
}
int main(){
    int openDriver = open("/dev/simple_character_device", O_RDWR);
    if(openDriver<0){printf("Cannot open device file \n"); exit(1);}
    int quit = 0;
    while(!quit){
        displayMenu(); 
        char input;
        scanf(" %c", &input);
        if(feof(stdin)){
            printf("Terminating...\n");
            return 0;
        } 
        switch(input) {
            case 'r': {
                printf("Enter the number of bytes you want to read:\n");
                int bytesToRead;
                scanf(" %d", &bytesToRead);
                if(feof(stdin)){
                    printf("Terminating...\n");
                    return 0;
                }
                char * mem = (char*)malloc(bytesToRead);
                int res = read(openDriver, mem, bytesToRead);
                //printf("Read returned: %d\n", res);
                printf("Bytes read: %s\n", mem);
                free(mem);
                break;
            }
            case 'w': {
                printf("Enter the string you want to write: \n");
                char stringToWrite[756];
                scanf(" %[^\n]%*c", &stringToWrite);
                if(feof(stdin)){
                    printf("Terminating...\n");
                    return 0;
                }
                write(openDriver, stringToWrite, strlen(stringToWrite));
                break;
            }

            case 's': {
                off_t offset;
                printf("Enter an offset value: \n");
                scanf(" %d", &offset);
                if(feof(stdin)){
                    printf("Terminating...");
                    return 0;
                }
                int whence;
                printf("Enter a value for whence (0 for SEEK_SET, 1 for SEEK_CUR, 2 for SEEK_END): \n");
                scanf(" %d", &whence);
                if(feof(stdin)){
                    printf("Terminating...");
                    return 0;
                }
                int ret = llseek(openDriver, offset, whence);
                printf("Seek returned: %d\n", ret);
                break;

            }
            case 'q': {
                quit = 1;
                printf("Quitting\n");
                break;
            }
            default: {
                printf("Invalid input. Please enter a valid option (r/w/s)\n");
                break;
            }
        }
    }
    close(openDriver);
    return 0;

}