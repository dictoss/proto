/************
 * bit operation demo program
 ************/
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

struct my_bitope {
    /* [0] byte */
    unsigned char h_len : 4;
    unsigned char h_ver : 4;
    /* [1] byte */
    unsigned int h_seq5 : 4;
    unsigned char h_reserved1 : 4;
    /* [2] byte */
    unsigned int h_seq3 : 4;
    unsigned int h_seq4 : 4;
    /* [3] byte */
    unsigned int h_seq1 : 4;
    unsigned int h_seq2 : 4;

    /* [4] byte */
    unsigned char p_c_type : 2;
    unsigned char p_x_type : 2;
    unsigned char p_t_flag1 : 1;
    unsigned char p_reserved : 1;
    unsigned char p_e_flag : 1;
    unsigned char p_bridge : 1;
    /* [5] byte */
    unsigned char p_int_info : 4;
    unsigned char p_dataattr : 4;
    /* [6] byte */
    unsigned char p_datatype : 8;
    /* [7] byte */
    unsigned char p_reserved3 : 8;

    /* [8] byte */
    unsigned char b_type : 4;
    unsigned char b_attr : 3;
    unsigned char b_resend : 1;

    /* [9] byte */
    unsigned char an_len: 8;

    /* [10]-[11] byte */
    unsigned short an_sum : 16;

    /* [12]-[13] byte */
    unsigned char f_type: 2;
    unsigned char f_sys_high: 6;
    unsigned char f_sys_low: 8;

    /* [14]-[15] byte */
    unsigned short f_id : 16;

    /* [16]-[17] byte */
    unsigned char d_type: 2;
    unsigned char d_sys_high: 6;
    unsigned char d_sys_low: 8;

    /* [18]-[19] byte */
    unsigned short d_id : 16;
};


typedef union my_msg_header {
    unsigned char data[20];
    struct my_bitope bitope;
} my_msg_header_t;



void test_bit_print(void);

/*
 main
 */
int main(int argc, char *argv[])
{
    int exitcode = EXIT_SUCCESS;

    test_bit_print();

    return exitcode;
}


void test_bit_print(void)
{
    int i = 0;
    struct my_bitope data = {0};
    my_msg_header_t header = {0};

    printf("sizeof(struct my_bitope) = %ld\n" , sizeof(data));
    printf("sizeof(my_msg_header_t) = %ld\n" , sizeof(header));

    printf("dump byte - 1\n");
    for(i = 0; i < sizeof(my_msg_header_t); i ++){
        printf("  [%02d] = %02x\n", i, header.data[i]);
    }

    /* set data */
    printf("set data\n");
    for(i = 0; i < sizeof(my_msg_header_t); i ++){
        header.data[i] = i + 1;
    }
    printf("dump byte - 2\n");
    for(i = 0; i < sizeof(my_msg_header_t); i ++){
        printf("  [%02d] = %02x\n", i, header.data[i]);
    }

    /*
     read struct membet
    */
    printf("read struct member\n");
    /* 0- byte */
    printf("  .bitope.h_len  = %d\n", header.bitope.h_len);
    printf("  .bitope.h_ver = %d\n", header.bitope.h_ver);
    /* printf("  .bitope.h_reserved1 = %d\n", header.bitope.h_reserved1); */
    {
        unsigned int h_seq = 0;

        h_seq = (header.bitope.h_seq5 * 10000)
            + (header.bitope.h_seq4 * 1000)
            + (header.bitope.h_seq3 * 100)
            + (header.bitope.h_seq2 * 10)
            + (header.bitope.h_seq1 * 1);
        printf("   h_seq  = %d (%d%d%d%d%d)\n",
               h_seq, header.bitope.h_seq5, header.bitope.h_seq4,
               header.bitope.h_seq3, header.bitope.h_seq2,
               header.bitope.h_seq1);
    }

    /* 5- byte */
    printf("  .bitope.p_c_type = %d\n", header.bitope.p_c_type);
    printf("  .bitope.p_x_type = %d\n", header.bitope.p_x_type);
    printf("  .bitope.p_t_flag1 = %d\n", header.bitope.p_t_flag1);
    printf("  .bitope.p_reserved = %d\n", header.bitope.p_reserved);
    printf("  .bitope.p_e_flag = %d\n", header.bitope.p_e_flag);
    printf("  .bitope.p_bridge = %d\n", header.bitope.p_bridge);
    printf("  .bitope.p_int_info = %d\n", header.bitope.p_int_info);
    printf("  .bitope.p_dataattr = %d\n", header.bitope.p_dataattr);
    printf("  .bitope.p_datatype = %d\n", header.bitope.p_datatype);
    printf("  .bitope.p_reserved3 = %d\n", header.bitope.p_reserved3);

    /* 9- byte */
    printf("  .bitope.b_type = %d\n", header.bitope.b_type);
    printf("  .bitope.b_attr = %d\n", header.bitope.b_attr);
    printf("  .bitope.b_resend = %d\n", header.bitope.b_resend);

    printf("  .bitope.an_len = %d (0x%02x)\n", header.bitope.an_len, header.bitope.an_len);

    printf("  .bitope.an_sum = %d (0x%04x)\n", header.bitope.an_sum, header.bitope.an_sum);

    /* 13- byte */
    printf("  .bitope.f_type = %d\n", header.bitope.f_type);
    {
        unsigned short f_sys = 0;
        unsigned short f_id = 0;

        f_sys = (header.bitope.f_sys_high << 8) + (header.bitope.f_sys_low);
        printf("  .bitope.f_sys = %d (0x%04x)\n", f_sys, f_sys);

        f_id = ntohs(header.bitope.f_id);
        printf("  .bitope.f_id = %d (0x%04x)\n", f_id, f_id);
    }

    /* 17- byte */
    printf("  .bitope.d_type = %d\n", header.bitope.d_type);
    {
        unsigned short d_sys = 0;
        unsigned short d_id = 0;

        d_sys = (header.bitope.d_sys_high << 8) + (header.bitope.d_sys_low);
        printf("  .bitope.d_sys = %d (0x%04x)\n", d_sys, d_sys);

        d_id = ntohs(header.bitope.d_id);
        printf("  .bitope.d_id = %d (0x%04x)\n", d_id, d_id);
    }
}
