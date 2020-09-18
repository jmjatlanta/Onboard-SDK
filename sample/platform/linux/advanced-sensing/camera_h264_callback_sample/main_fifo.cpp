/*****
 * To demonstrate how to modify the code to use named pipes (FIFO)
 */

#include <cstdint>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

bool open_attempted = false;
int fp; // the file pointer

/****
 * Write stream data to a named pipe
 * @param fileName the name of the pipe
 * @param data the data
 * @param len the length of data
 * @returns 0 on success, -1 on error
 */
int writeStreamData(const char *fileName, const uint8_t *data, uint32_t len) 
{
   if (!open_attempted)
   {
      open_attempted = true;
      std::cerr << "About to open " << fileName << "\n";
      fp = open(fileName, O_WRONLY);
      if (fp < 0)
      {   
         std::cerr << "Unable to open " << fileName << " Errno: " << std::to_string(errno) << "\n";
         return -1;
      }
   }

   if (fp >= 0)
   {
      std::cerr << "About to write the data\n";
      size_t size = write(fp, data, (size_t)len);
      if(size != len)
      {
         std::cerr << "Unable to write the data\n";
         return -1;
      }
   }
   else
   {
      return -1;
   }
   
   return 0;
}

/****
 * The callback that Onboard-SDK calls
 * @param buf the data
 * @param bufLen the length of the buffer
 * @param userData the filename of the named pipe
 */
void liveViewSampleCb(uint8_t* buf, int bufLen, void* userData) 
{
  if (userData) {
    const char *filename = (const char *) userData;
    writeStreamData(filename, buf, bufLen);
  } else {
     std::cerr << "userData is a null value (should be a file name to log h264 stream).\n";
  }
}

/****
 * A simple test to demonstrate the modifications necessary
 * in the methods above to support named pipes
 */
int main(int argc, char** argv) {

   const char *pipeName = "/tmp/MyNamedPipe";

   // Throw some data in the pipe
   std::vector<uint8_t> buf{'H', 'E', 'L', 'L', 'O'};
   liveViewSampleCb(&buf[0], buf.size(), (void*)pipeName);

   // throw some more
   buf = {' ', 'W', 'O','R','L','D' };
   liveViewSampleCb(&buf[0], buf.size(), (void*)pipeName);

   // close the pipe
   if (fp >= 0)
      close(fp);
   return 0;
}
