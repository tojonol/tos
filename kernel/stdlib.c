
#include <kernel.h>


int k_strlen(const char* str)
{
	int strlen = 0;
	while (*str++ != '\0')
	{
		strlen++;
	}
	return (strlen);
}

//void *memcpy(void *str1, const void *str2, size_t n)
void* k_memcpy(void *str1, const void *str2, int n)
{
	int counter = 0;
	char* str1_c = (char*) str1;
	char* str2_c = (char*) str2;

	//iterate through and copy over
	while (counter < n)
	{
		*str1_c++ = *str2_c++;
		counter++;
	}
	return (str1_c);
}

//int memcmp(const void *str1, const void *str2, size_t n)
int k_memcmp(const void *str1, const void *str2, int n)
{
	int counter = 0;
	unsigned char* str1_c = (unsigned char*) str1;
	unsigned char* str2_c = (unsigned char*) str2;
	while (counter < n)
	{
		//if return value < 0 then str1 < str2
		//or
		//if return value > 0 then str1 > str2
		int diff = *str1_c++ - *str2_c++;
		if (diff != 0)
			return (diff);
		counter++;
	}
	//if return value is 0 str1 == str2
	return (0);
}
