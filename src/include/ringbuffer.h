#ifndef RINGBUFFER_H
#define RINGBUFFER_H
		
#include "common.h"
		
#define ERROR_RING_ALLOC     -1
#define ERROR_BUF_ALLOC      -2
#define ERROR_BUF_NO_SPACE   -3
#define ERROR_BUF_EMPTY   	 -4
#define ERROR_NOT_ENOUGHT 	 -5
	
typedef struct {
    unsigned char *memory;     // memory used for the buffer itself
    int size;                  // byte used for the buffer
    unsigned int fill;         // number of bytes currently in the buffer
    unsigned int read_pos;     // reading position
    unsigned int write_pos;    // writing position
} RINGBUFFER_TYPE;

//for ringbuffer_status() use
typedef struct {
    int size;                  // byte used for the buffer
    unsigned int fill;         // number of bytes currently in the buffer
	unsigned int free;         // number of bytes free in the buffer
} RINGBUFFER_VAR;


/**
 * Create ringbuffer.
 * param: buffer        [out] pointer to the ringbuffer
 * param: size          [in]  ringbuffer size wanted
 * return: error code
 */
int ringbuffer_create(RINGBUFFER_TYPE *p_ringbuffer, unsigned int size);

/**
 * Get all variables of the ringbuffer.
 * param: buffer        [in]  the ringbuffer pointer
 * param: status        [out] pointer to the ringbuffer status struct
 */
void ringbuffer_status(RINGBUFFER_TYPE *p_ringbuffer, RINGBUFFER_VAR *status);

/**
 * Clear the ring buffer (setting its length to zero).
 * param: buffer        [in] the ringbuffer pointer
 */
void ringbuffer_clear(RINGBUFFER_TYPE *p_ringbuffer);

/**
 * Put n bytes in the ringbuffer. 
 * param: buffer        [in] the ringbuffer pointer 
 * param: src     		[in] source buffer
 * param: offset  		[in] source buffer offset
 * param: lenght     	[in] source buffer lenght to be put
 * return:              error code or number of bytes put
 *
 * E.g.
 *
 *       src ------------------------------------------
 *               |                      |
 *             offset					|
 *				 >......................<length
 *
 */
int ringbuffer_put(RINGBUFFER_TYPE *p_ringbuffer, void *p_src, unsigned int offset, int lenght);

/**
 * Get n bytes from the ringbuffer.
 *
 * param: buffer        [in]  the ringbuffer pointer
 * param: dst     		[out] destination buffer
 * param: offset  		[in]  destination buffer offset
 * param: lenght     	[in]  destination buffer lenght to be written
 * return:              error code or bytes gotten
 */
int ringbuffer_get(RINGBUFFER_TYPE *p_ringbuffer, void *p_dst, unsigned int offset, int lenght);

/**
 * Dispose ringbuffer.
 * 
 * param: buffer        [out] pointer to the ringbuffer
 */
void ringbuffer_dispose(RINGBUFFER_TYPE *p_ringbuffer);


#endif
