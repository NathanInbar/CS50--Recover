#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
/**

Open memory card
Look for beginning of a JPEG
Open new JPEG
Write 512 bytes until new JPEG is found

--JPEG--
byte 1: 0xff
byte 2: 0xd8
byte 3: 0xff
byte 4: 0xe.
@beginning of a 512 byte block = JPEG

--FREAD--
fread(data, size, number, intptr);
data: ptr to where to store data you're reading (array i.e buffer[])
size: size of each element to read (512)
number: number of elements to read (size of raw / 512 ?)
intptr: FILE * to read from (card.raw)

returns number of items of size "size" were read
~condition where if return < "size", it has reached the end (?)

--BUFFER--
is buffer[0] = 0xff;
is buffer[1] = 0xd8;
...

--BITWISE--
turns the last 4 bits of the byte into 0 via bitwise & operation
(buffer[3] & 0xf0) == 0xe0

--NEW FILE--
~jpg names = "000.jpg"
>create filename
sprintf(filename, "%03i.jpg"<-format string, number);
~filename must have enough memory to contain the name (1 byte per char)
>open file name & write
FILE *img = fopen(filename, "w");

--FWRITE--
fwrite(data, size, number, outptr);
data: ptr to bytes that will be written to file
size: size of each element to write
number: number of elements to write
outptr: FILE * to write to (000.jpg)

--MISC--
rm *.jpg to remove all jpg in a directory
**/

/**
Open Memory Card
Repeat until card end:
    Read 512 bytes into buffer

    If start of JPEG:
        If first jpeg
            start writing
        Else
            close previous file
            start writing
    Else:
        If already found JPEG:
            keep writing to it (hit next block of same jpeg)

    Close any remaining files
**/

//problems i encountered and learned from:
/**
 * had fwrite inside the IF such that it would only write blocks with JPEG header
 *
 * didnt init img ptr to NULL
 *
 * didnt except lack of arguments
 *
 *
**/

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    int jpgCount = -1;
    const int block_size = 512;

    if (argc < 2)
    {
        printf("Usage: ./recover *.raw");
        return 1;
    }


    //create buffer that each block will be read into
    BYTE buffer[block_size];

    //Open the memory card
    FILE *file = fopen(argv[1], "r");

    FILE *img = NULL;

    //read 1 block into buffer
    while (fread(buffer, block_size, 1, file))
    {
        //check if (new) JPG via the Header bytes
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {

            jpgCount++;

            //if not the first jpg
            if (jpgCount != 0)
            {
                //close the previously written JPG file
                fclose(img);
            }

            //create the appropriate JPG file name
            char filename[8]; //"000.jpg\0" = 8B
            sprintf(filename, "%03i.jpg", jpgCount);

            //create a new .JPG
            img = fopen(filename, "w");

        }

        if (img != NULL)
        {
            fwrite(buffer, block_size, 1, img); //write buffer to the jpg file
        }
    }

    //close the memory card's .raw & opened .jpg
    fclose(img);
    fclose(file);
}