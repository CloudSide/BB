//
//  bb_freq_util.h
//  BBSDK
//
//  Created by Littlebox on 13-5-6.
//  Copyright (c) 2013年 Littlebox. All rights reserved.
//

#ifndef BBSDK_bb_freq_util_h
#define BBSDK_bb_freq_util_h




#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "_kiss_fft_guts.h"


#define PI                      3.1415926535897932384626433832795028841971               //定义圆周率值
#define SAMPLE_RATE             44100                                                    //采样频率

#define BB_SEMITONE 			1.05946311
#define BB_BASEFREQUENCY		1760
#define BB_CHARACTERS			"0123456789abcdefghijklmnopqrstuv"

#define BB_FREQUENCIES          {1479.978, 1760, 2093.005, 2217.461, 2349.318, 2489.016, 2637.021, 2793.826, 2959.956, 3049, 3135.964, 3322.438, 3729.31, 4186.009, 4698.637, 5919.912, 6271.928, 6644.876, 7000, 7458.62, 7900, 8372.019, 8869.845, 9300, 9956.064, 10548.083, 11000, 11839.823, 12543.855, 12900, 13289.752, 14000}

#define BB_THRESHOLD            20

#define BB_HEADER_0             17
#define BB_HEADER_1             19

typedef struct _bb_item_group bb_item_group;

typedef int element;

//void freq_init();

int freq_to_num(unsigned int f, int *n);

int num_to_char(int n, char *c);

int char_to_num(char c, unsigned int *n);

int num_to_freq(int n, unsigned int *f);

int char_to_freq(char c, unsigned int *f);

int vote(int *src, int src_len, int *result);

int multi_vote(int *src, int src_len, int *result, int res_len, int vote_res);

int multi_vote_accurate(int *src, int src_len, int *result, int res_len, int vote_res);

int statistics(int *src, int src_len, int *result, int res_len);

int compose_statistics(int *src_vote, int *src_statics, int src_len, int *result, int res_len);

/////////
int set_group(int *src, int src_len, bb_item_group *result, int res_len);

int process_group(bb_item_group *src, int src_len);

int get_group_data(bb_item_group *src, int src_len, int *result, int res_len);

int post_process(bb_item_group *src, int src_len, bb_item_group *result, int res_len);

/////////
int encode_sound(unsigned int freq, float buffer[], size_t buffer_length);

int create_sending_code(unsigned char *src, unsigned char *result, int res_len);

int decode_sound(void *src, int fft_number);

int fft(void *src, int num);

/////////

int statistics_2(int *src, int src_len, int *result, int res_len);
int process_group_2(bb_item_group *src, int src_len);
int post_process_2(bb_item_group *src, int src_len, bb_item_group *result, int res_len);

void _medianfilter(const element* signal, element* result, int N);

#endif /* BBSDK_bb_freq_util_h */


