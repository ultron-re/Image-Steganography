#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    char image_data[MAX_IMAGE_BUF_SIZE];

    
    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;
    
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    uint stego_image_capacity;
    uint bits_per_pixel;
    char stego_image_data[MAX_IMAGE_BUF_SIZE];

    /*Decoded text info*/
    char *decoded_fname;
    FILE *fptr_decoded;
    char extn_decoded_file[MAX_FILE_SUFFIX];
    char decoded_data[MAX_SECRET_BUF_SIZE];
    long size_decoded_file;


} DecodeInfo;


/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_dec(DecodeInfo *decInfo);

/* check capacity */
//Status check_capacity(DecodeInfo *decInfo);

/* Get image size */
//uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
//uint get_decfile_size(FILE *fptr);

/* Copy bmp image header */
//Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/* DEcode secret file extenstion size */
Status decode_secret_file_extn_size(int size, FILE *file_extn, FILE *fptr_dest_image);

/* Decode secret file extenstion */
Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(long file_size, DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image, FILE *fptr_decoded,DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
Status decode_byte_from_lsb(char *decode_buffer);

/* Decode a size into LSB of image data array */
Status decode_size_from_lsb(char *buffer,int size);
#endif

