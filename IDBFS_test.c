#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <emscripten.h>

//-----------------------------------------------------------------------
EMSCRIPTEN_KEEPALIVE 
char *LoadData() {
   int fd;
   int size;
   char *buff;

   fd = open("/data/textfile.txt", O_RDONLY);

   if (fd == -1) return strerror(errno);

   size = lseek(fd, 0, SEEK_END);
   lseek(fd, 0, SEEK_SET);
   buff = (char *) malloc(size+1);
   read(fd, buff, size);
   buff[size] = '\0';
   close(fd); 

   return buff;
}
//-----------------------------------------------------------------------
EMSCRIPTEN_KEEPALIVE
void SaveData(char *data) {
   int fd;
   int size;

   if (data == NULL) return;

   fd = open("/data/textfile.txt", O_CREAT | O_WRONLY, 0666);
   if (fd == -1) {
       printf("ERROR: could not open the file for writing!\n, %s\n", strerror(errno));
       return;
   }
   size = strlen(data);
   printf("saving %i bytes... %s\n", size, data);
   write(fd, data, size);
   ftruncate(fd, size);
   close(fd);

   EM_ASM ( FS.syncfs(false, function (err) {} ); );
}
//-----------------------------------------------------------------------
int main() {
   EM_ASM(
      FS.mkdir('/data');
      FS.mount(IDBFS, {}, '/data');

      FS.syncfs(true, function (err) { 
         // provide the DOM side a way to execute code after directory is mounted
         if (typeof Module.OnDataMounted !== 'undefined') {
            Module.OnDataMounted();
         }
      } );
   );

   emscripten_exit_with_live_runtime();
}
//-----------------------------------------------------------------------

