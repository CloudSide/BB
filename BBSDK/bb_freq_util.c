
#include "bb_freq_util.h"

#include "rscode.h"


static unsigned int frequencies[32];
static double theta = 0;

struct _bb_item_group {
	
	int item;
	int count;
};


/*
static struct _freq_range {
	
	unsigned int start;
	unsigned int end;
	
} freq_range[32];
*/


void freq_init() {
	
	static int flag = 0;
	
	if (flag) {
		
		return;
	}
	
	printf("----------------------\n");
	
	int i, len;
	
	for (i=0, len = strlen(BB_CHARACTERS); i<len; ++i) {
		
		unsigned int freq = (unsigned int)floor(BB_BASEFREQUENCY * pow(BB_SEMITONE, i));
		frequencies[i] = freq;
		
        /*
		if (i>0) {
			
			int spacing = (freq - frequencies[i-1]) / 2;
			
			freq_range[i-1].end = frequencies[i] - spacing - BB_THRESHOLD;
			freq_range[i].start = frequencies[i] - spacing + BB_THRESHOLD;
			
			if (i==1) {
                
				freq_range[0].start = frequencies[0] - spacing + BB_THRESHOLD;
			} else if (i==len-1) {
				
				freq_range[i].end = frequencies[i] + spacing - BB_THRESHOLD;
			}
		}
         */
	}
	
	
	/*
     for (i=0, len = strlen(BB_CHARACTERS); i<len; ++i) {
     
     printf("%d: %d-%d\n", i, freq_range[i].start, freq_range[i].end);
     }
     */
	
	
	flag = 1;
}

int freq_to_num(unsigned int f, int *n) {
	
    frequencies[0] = (unsigned int)floor(BB_BASEFREQUENCY * pow(BB_SEMITONE, 0));
    frequencies[31] = (unsigned int)floor(BB_BASEFREQUENCY * pow(BB_SEMITONE, 31));
    
    if (n != NULL &&
        f >= frequencies[0]-BB_THRESHOLD*pow(BB_SEMITONE, 0) &&
        f <= frequencies[31]+BB_THRESHOLD*pow(BB_SEMITONE, 31)) {
        
        unsigned int i;

        for (i=0; i<32; i++) {
            
            unsigned int freq = (unsigned int)floor(BB_BASEFREQUENCY * pow(BB_SEMITONE, i));
            frequencies[i] = freq;
            
            if (abs(frequencies[i] - f) <= BB_THRESHOLD*pow(BB_SEMITONE, i)) {
                
                *n = i;
                return 0;
            }
        }
    }
     
	
    /*
	if (n!=NULL && f>freq_range[0].start && f<freq_range[31].end) {
        
		unsigned int i;
        
		for (i=0; i<32; i++) {
            
			if (f>freq_range[i].start && f<freq_range[i].end) {
				
				*n = i;
				return 0;
			}
		}		
	}
     */
	
	return -1;
}

int num_to_char(unsigned int n, char *c) {
	
	if (c != NULL && n>=0 && n<32) {
		
		*c = BB_CHARACTERS[n];		
		
		return 0;
	}
		
	return -1;
}

int char_to_num(char c, unsigned int *n) {
	
	if (n == NULL) return -1;
	
	*n = 0;
	
	if (c>=48 && c<=57) {
		
		*n = c - 48;
		
		return 0;
	
	} else if (c>=97 && c<=118) {
		
		*n = c - 87;
		
		return 0;
	}
	
	return -1;
}

int num_to_freq(unsigned int n, unsigned int *f) {
	
	if (f != NULL && n>=0 && n<32) {
		
		*f =  (unsigned int)floor(BB_BASEFREQUENCY * pow(BB_SEMITONE, n));
		
		return 0;
	}
	
	return -1;
}

int char_to_freq(char c, unsigned int *f) {
	
	unsigned int n;
	
	if (f != NULL && char_to_num(c, &n) == 0) {
		
		unsigned int ff;
		
		if (num_to_freq(n, &ff) == 0) {
			
			*f = ff;
			return 0;
		}
	}
	
	return -1;
}

int vote(int *src, int src_len, int *result) {
	
	if (src==NULL || src_len==0 || result==NULL) {
		
		return -1;
	}
		
	int i;
	int map[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int max_value = 0;
	int max_key   = 0;
	int temp      = 0;
	
	for (i=0; i<src_len; i++) {
		
		if (src[i] >= 0 && src[i] < 32) {
			
            temp = ++map[src[i]];
            
            if (temp > max_value) {
                
                max_value = temp;
                max_key   = src[i];
            }
			
		} else if (src[i]==-1) {
            
        } else {
			return -1;
		}
	}
	
	*result = max_key;

	return max_value;
}

int multi_vote(int *src, int src_len, int *result, int res_len, int vote_res) {
	
	if (src==NULL || result==NULL || src_len==0 || res_len==0 || src_len<=res_len || vote_res==0) {
		
		return -1;
	}
	
	int step_len = src_len / res_len;
    
    if (step_len<vote_res) {
        
        return -1;
    }
	
	int i;
	int j = 0;
	
	for (i=0; i<src_len; i+=step_len) {
        
        int vote_count = vote(&src[i], step_len, &result[j++]);
        
        if (vote_count == -1) {
            
            return -1;
        }
        if (vote_res>0 && vote_count<vote_res) {
            
            return 1;
        }
	}
	
	return 0;
}

int multi_vote_accurate(int *src, int src_len, int *result, int res_len, int vote_res) {
	
	if (src==NULL || result==NULL || src_len==0 || res_len==0 || src_len<=res_len || vote_res==0) {
		
		return -1;
	}
	
	int step_len = src_len / res_len;
    
    if (step_len<vote_res) {
        
        return -1;
    }
	
	int i;
	int j = 0;
	
	for (i=0; i<src_len; i+=step_len) {
        
        int vote_count = vote(&src[i], step_len+1, &result[j++]);
        
        if (vote_count == -1) {
            
            return -1;
        }
        if (vote_res>0 && vote_count<vote_res) {
            
            return 1;
        }
	}
	
	return 0;
}

int statistics(int *src, int src_len, int *result, int res_len) {
    
    if (src==NULL || result==NULL || src_len==0 || res_len==0) {
        return -1;
    }
    // Littlebox-XXOO
    printf("111111111111111111\n");
    
    bb_item_group a[32]; // include -1
	set_group(src, src_len, a, 32);
	
    printf("2222222222222222222\n");
    
	process_group(a,32);
    
    printf("33333333333333333333\n");
    
	get_group_data(a, 32, src, src_len);
	
    
    printf("4444444444444444444\n");
    
	bb_item_group b[32]; // without -1
	set_group(src, src_len, b, 32);
    
    printf("55555555555555555555\n");
    
    bb_item_group c[32];
    post_process(b, 32, c, 32);
    
    printf("66666666666666666666\n");
	
	int i;
    
    result[0] = c[0].item;
    result[1] = c[1].item;
    
    if (c[res_len].item != -2) {
        
        for (i=2; i<res_len; i++) {
            
            result[i] = c[i+1].item;
        }
    } else {
        
        for (i=2; i<res_len; i++) {
            
            if (c[i].item == -2) {
                result[i] = 0;
            }else {
                result[i] = c[i].item;
            }
        }
    }
    
    
    printf("77777777777777777777777\n");
    
    return 0;
}

int compose_statistics(int *src_vote, int *src_statics, int src_len, int *result, int res_len) {
	
	if (src_vote==NULL || src_statics==NULL || result==NULL || src_len==0 || res_len==0) {
        return -1;
	}
	
	int i;
	
	if (src_vote[0]==19) {
		
		if (src_statics[0]!=19) {
			
			for (i=0; i<src_len-1; i++) {
				src_vote[i] = src_vote[i+1];
			}
			src_vote[src_len-1] = 0;
		}
	}
	
	for (i=0; i<src_len; i++) {
		
		if (i==0) {
			
			result[0] = src_statics[0];
            
		} else {
			
			if (src_vote[i] != src_statics[i]) {
				
				if (src_vote[i] != result[i-1] && src_statics[i] != result[i-1]) {
                    result[i] = src_statics[i];
                } else if (src_vote[i] != result[i-1] && src_statics[i] == result[i-1]) {
					result[i] = src_vote[i];
				} else {
					result[i] = src_statics[i];
				}
			} else {
				result[i] = src_vote[i];
			}
		}
	}
	
	return 0;
}

int set_group(int *src, int src_len, bb_item_group *result, int res_len) {
	
	if (src==NULL || result==NULL || src_len < res_len) {
		return -1;
	}
	
	int index = 0;
	int i;
	
	for (i=0; i<res_len; i++) {
		result[i].item = -2;
		result[i].count = 0;
	}
	
	result[0].item = src[0];
	result[0].count++;
	
	for (i=1; i<src_len; i++) {
		
		if (src[i] == src[i-1] && result[index].count < 4) {
			
			result[index].count++;
			
		} else {
            
			index++;
			result[index].item = src[i];
			result[index].count++;
		}
	}
    
	return 0;
}

int process_group(bb_item_group *src, int src_len) {
	
	if (src==NULL || src_len == 0) {
		return -1;
	}
	
	int i;
	int twice = 1;
	
	while (twice<3) {
        
		for (i=0; i<src_len; i++) {
            
			if (src[i].count != 0 && src[i].item == -1 && src[i].count < 3) {
				
				if (i == 0) {
                    
					if (src[i+1].count<4) {
                        
						src[i+1].count++;
						src[i].count--;
					}
					
				} else if (src[i+1].item == -2) {
                    
					if (src[i-1].count<4) {
                        
						src[i+1].count++;
						src[i].count--;
					}
					
				} else {
                    
					if (src[i-1].count == 4 && src[i+1].count == 4) {
						
					} else if (src[i-1].count > src[i+1].count) {
                        
						src[i+1].count++;
						src[i].count--;
						
					} else {
						
						src[i-1].count++;
						src[i].count--;
					}
				}
			}
		}
		
		twice++;
	}
    
    
	return 0;
}

int get_group_data(bb_item_group *src, int src_len, int *result, int res_len) {
	
	if (src==NULL || result==NULL || res_len < src_len) {
		return -1;
	}
	
	int i, j;
	int index = 0;
	
	for (i=0; i<32; i++) {
		
		for (j=0; j<src[i].count; j++) {
            
			result[index] = src[i].item;
			index++;
		}
	}
	
	return 0;
}

int post_process(bb_item_group *src, int src_len, bb_item_group *result, int res_len) {
	
	if (src==NULL || result==NULL || src_len < res_len) {
		return -1;
	}
	
	int i;
	int index = 1;
	int x;
	
	for (i=0; i<res_len; i++) {
		result[i].item = -2;
		result[i].count = 0;
	}
	
	result[0].item = src[0].item;
	result[0].count = src[0].count;
	
	for (i=1; i<src_len-1; i++) {
		
		if (src[i].item == -1) {
            
            if (src[i].count == 4 && src[i+1].item == -1) {
				
				result[index].item = src[i].item = 0;
				index++;
				continue;
			}
			
			if (src[i-1].count + src[i].count + src[i+1].count >= 9) {
				
				result[index].item = 0;
				result[index].count = 4;
				result[index-1].count = src[i-1].count = 4;
				src[i+1].count = 4;
				index++;
				
			} else {
				
				x = src[i+1].count + src[i].count;
				src[i+1].count = x > 4 ? 4 : x;
			}
		} else {
			
			result[index].item = src[i].item;
			result[index].count = src[i].count;
			index++;
		}
	}
    
	return 0;
}


/////////////////////

// 一个频率对应的一组PCM的buffer
int encode_sound(unsigned int freq, float buffer[], size_t buffer_length)
{
    const double amplitude = 0.25;
	double theta_increment = 2.0 * PI * freq / SAMPLE_RATE;
	int frame;
    
	for (frame = 0; frame < buffer_length; frame++)
	{
		buffer[frame] = sin(theta) * amplitude;
		
		theta += theta_increment;
		if (theta > 2.0 * PI)
		{
			theta -= 2.0 * PI;
		}
	}
    
    return 1;
}

int create_sending_code(unsigned char *src, unsigned char *result, int res_len) {
    
    if (src==NULL || result==NULL || res_len <= 2) {
        return -1;
    }
    
    int i;
    
    unsigned char data[RS_TOTAL_LEN];
    
    for (i=0; i<RS_TOTAL_LEN; i++) {
        
        if (i<RS_DATA_LEN) {
            char_to_num(src[i], (unsigned int *)(data+i));
        }else {
            data[i] = 0;
        }
    }
    
    unsigned char *code = data + RS_DATA_LEN;
    
    RS *rs = init_rs(RS_SYMSIZE, RS_GFPOLY, RS_FCR, RS_PRIM, RS_NROOTS, RS_PAD);
    encode_rs_char(rs, data, code);
    
    result[0] = BB_HEADER_0;
    result[1] = BB_HEADER_1;
    
    for (i=2; i<res_len; i++) {
        result[i] = data[i-2];
    }
    
    printf("code sending :  ");
    for (i=0; i<res_len; i++) {
        printf("%u-", result[i]);
    }
    printf("\n");
    
    return 0;
}


// 解码音频数据，返回所对应字符
int decode_sound(void *src, int fft_number)
{
    if (!src) {
        return 0;
    }
    
    // 计算fft
    int sound_freq = fft(src, fft_number);
    
    /*
    if (sound_freq>0) {
        int a = -1;
        freq_to_num(sound_freq,&a);
        printf("----%d---  %d\n", sound_freq, a);
    }
     */
    
    //freq_init();
    int num[1] = {-1};
    freq_to_num(sound_freq, num);
    
    return num[0];
}

// fft计算
int fft(void *src_data, int num)
{
    if (!src_data) {
        return 0;
    }
    
    if (num > BB_MAX_FFT_SIZE) {
        num = BB_MAX_FFT_SIZE;
    }
    
    kiss_fft_cpx in_data[BB_MAX_FFT_SIZE];
    kiss_fft_cpx out_data[BB_MAX_FFT_SIZE];
    
    int i;
    
    for (i=0; i<num; i++) {
        
        in_data[i].r = (double)((unsigned char *)src_data)[i];
        in_data[i].i = 0;
    }

    kiss_fft_cfg fft_cfg = kiss_fft_alloc(num, 0, NULL, NULL);
    kiss_fft(fft_cfg, in_data, out_data);
    
    int size = num / 2;
    double maxFreq = 0.0;
    int maxIndx = 0;
    int maxEqual = 0;
    
    for (i=1; i<size; i++)
    {
        
        double out_data_item = sqrt(pow(out_data[i].r, 2) + pow(out_data[i].i, 2));
        
        //printf("%f\n", out_data_item);
        
        if (out_data_item > maxFreq)
        {
            maxFreq = out_data_item;
            maxIndx = i;
        }
        else if (out_data_item == maxFreq)
        {
            maxEqual++;
            
            if (maxFreq > 0.0)
            {
            }
        }
        else
        {
        }
    }
    
    double tmpFreq = (44100 / 2.0) * maxIndx / (size / 2);
    int intFreq = (int) tmpFreq;
    
    /*
    if (intFreq > 40000) {
        
        printf("-----------------------------------------");
        for (i=0; i<num; i++) {
            
            double out_data_item = sqrt(pow(out_data[i].r, 2) + pow(out_data[i].i, 2));
            printf("%f---%d\n", out_data_item, i);
        }
        
        int aaaaaaa = 5;
    }
     */

    kiss_fft_cleanup();
    
    return intFreq;
}
