#include "include/ringbuffer.h"


int ringbuffer_create(RINGBUFFER_TYPE *p_ringbuffer, unsigned int size) {
	
	if (p_ringbuffer == NULL) return ERROR_RING_ALLOC;
	
	p_ringbuffer->memory = (unsigned char *) malloc((unsigned int) size);
	if (p_ringbuffer->memory == NULL) return ERROR_BUF_ALLOC;
	
	p_ringbuffer->fill = 0;
	p_ringbuffer->read_pos = 0;
	p_ringbuffer->write_pos = 0;
	p_ringbuffer->size = (unsigned int) size;
	
	return 0; 
}


void ringbuffer_status(RINGBUFFER_TYPE *p_ringbuffer, RINGBUFFER_VAR *status) {
	status->size = p_ringbuffer->size;
	status->fill = p_ringbuffer->fill;
	status->free = p_ringbuffer->size - p_ringbuffer->fill;
}


void ringbuffer_clear(RINGBUFFER_TYPE *p_ringbuffer) {
	p_ringbuffer->fill = 0;
	p_ringbuffer->read_pos = 0;
	p_ringbuffer->write_pos = 0;
}


int ringbuffer_put(RINGBUFFER_TYPE *p_ringbuffer, void *p_src, unsigned int offset, int lenght) {
	unsigned int end_size, remain_data;
	unsigned char *actual_posit;
	unsigned char *src;

	src = (unsigned char *) p_src;
	
	if(lenght == 0) return 0;
	
	if((p_ringbuffer->size - p_ringbuffer->fill) < lenght) return ERROR_BUF_NO_SPACE;
	
	end_size = (p_ringbuffer->size - p_ringbuffer->write_pos);
	actual_posit = (unsigned char *) (((unsigned int) p_ringbuffer->memory) + p_ringbuffer->write_pos);
	
	if(lenght > end_size)
	{
		memcpy(actual_posit, (const void *) (src + offset), end_size);
		remain_data =  lenght - end_size;
		memcpy(p_ringbuffer->memory, (const void *) (src + offset + end_size), remain_data);
		p_ringbuffer->write_pos = remain_data;
	}
	else
	{
		memcpy(actual_posit, (const void *) (src + offset), lenght);
		p_ringbuffer->write_pos += lenght;
	}
	p_ringbuffer->fill += lenght;
	return lenght;
}


int ringbuffer_get(RINGBUFFER_TYPE *p_ringbuffer, void *p_dst, unsigned int offset, int lenght) {
	unsigned int end_size, remain_data;
	unsigned char *actual_posit;
	unsigned char *dst;

	dst = (unsigned char *) p_dst;
	
	if(lenght == 0) return 0;
	
	if(p_ringbuffer->fill == 0) return ERROR_BUF_EMPTY;
	
	if(lenght > p_ringbuffer->fill) return ERROR_NOT_ENOUGHT;
	
	end_size = (p_ringbuffer->size - p_ringbuffer->read_pos);
	actual_posit = (unsigned char *) (((unsigned int) p_ringbuffer->memory) + p_ringbuffer->read_pos);
	
	if(lenght > end_size)
	{
		memcpy((const void *) (dst + offset), actual_posit, end_size);
		remain_data =  lenght - end_size;
		memcpy((const void *) (dst + offset + end_size), p_ringbuffer->memory, remain_data);
		p_ringbuffer->read_pos = remain_data;
	}
	else
	{
		memcpy((const void *) (dst + offset), actual_posit, lenght);
		p_ringbuffer->read_pos += lenght;
	}
	p_ringbuffer->fill -= lenght;
	return lenght;
}


void ringbuffer_dispose(RINGBUFFER_TYPE *p_ringbuffer) {
	
	free(p_ringbuffer->memory);
	free(p_ringbuffer);
}
