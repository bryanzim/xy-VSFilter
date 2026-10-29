#ifndef __TEST_INSTRINSICS_MACRO_0E1D7AC0_6386_4E23_97F5_91AFAB395ED7_H__
#define __TEST_INSTRINSICS_MACRO_0E1D7AC0_6386_4E23_97F5_91AFAB395ED7_H__


#include <gtest/gtest.h>
#include "xy_intrinsics.h"
#include <iostream>
#include <cmath>
#include <iomanip>

#define M128I_U8_TO_STR(m128i)  std::setw(3)<<(int)(m128i.m128i_u8[0U])<<" "<<std::setw(3)<<(int)(m128i.m128i_u8[1U])<<" "<<std::setw(3)<<(int)(m128i.m128i_u8[2U])<<" "<<std::setw(3)<<(int)(m128i.m128i_u8[3U])<<" "\
    <<std::setw(3)<<(int)(m128i.m128i_u8[4U])<<" "<<std::setw(3)<<(int)(m128i.m128i_u8[5U])<<" "<<std::setw(3)<<(int)(m128i.m128i_u8[6U])<<" "<<std::setw(3)<<(int)(m128i.m128i_u8[7U])<<" "\
    <<std::setw(3)<<(int)(m128i.m128i_u8[8U])<<" "<<std::setw(3)<<(int)(m128i.m128i_u8[9U])<<" "<<std::setw(3)<<(int)(m128i.m128i_u8[10U])<<" "<<std::setw(3)<<(int)(m128i.m128i_u8[11U])<<" "\
    <<std::setw(3)<<(int)(m128i.m128i_u8[12U])<<" "<<std::setw(3)<<(int)(m128i.m128i_u8[13U])<<" "<<std::setw(3)<<(int)(m128i.m128i_u8[14U])<<" "<<std::setw(3)<<(int)(m128i.m128i_u8[15U])

#define M128I_U8_TO_STR_2(m128i) #m128i<<std::endl<<" "<<M128I_U8_TO_STR(m128i)

#define CMP_M128I(m128i1, m128i2) (m128i1.m128i_i32[0U]==m128i2.m128i_i32[0U] && m128i1.m128i_i32[1U]==m128i2.m128i_i32[1U] && m128i1.m128i_i32[2U]==m128i2.m128i_i32[2U] && m128i1.m128i_i32[3U]==m128i2.m128i_i32[3U] )

class XyIntrinsicsTest : public ::testing::Test 
{
protected:
    virtual void SetUp() 
    {  
    }
};

TEST_F(XyIntrinsicsTest, Check_AVERAGE_4_PIX_INTRINSICS_3)
{   
    __m128i m128_1_in, m128_last_in;
    __m128i m128_1_out, m128_last_out;
    __m128i m128_1_expect, m128_last_expect;

    m128_1_in = _mm_setzero_si128();
    m128_last_in = _mm_setzero_si128();

    m128_1_out = m128_1_in; 
    m128_last_out = m128_last_in;
    m128_1_expect = m128_1_in; 
    m128_last_expect = m128_last_in;

    AVERAGE_4_PIX_INTRINSICS_3(m128_1_out, m128_last_out);
    average_4_pix_intrinsics_3_c(m128_1_expect, m128_last_expect);

    ASSERT_EQ(true, (CMP_M128I(m128_1_expect,m128_1_out) && CMP_M128I(m128_last_expect,m128_last_out)) )
        <<M128I_U8_TO_STR_2(m128_1_in)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_in)<<std::endl
        <<std::endl
        <<M128I_U8_TO_STR_2(m128_1_expect)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_expect)<<std::endl
        <<std::endl
        <<M128I_U8_TO_STR_2(m128_1_out)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_out)<<std::endl;

    m128_1_in = _mm_cmpeq_epi32(m128_1_in, m128_1_in);    
    m128_last_in.m128i_u8[0U] = 0xff;
    m128_last_in.m128i_u8[1U] = 0xff;
    for (int i{2};i<16;++i)
    {
        m128_last_in.m128i_u8[i] = 0;
    }

    m128_1_out = m128_1_in; 
    m128_last_out = m128_last_in;
    m128_1_expect = m128_1_in; 
    m128_last_expect = m128_last_in;

    AVERAGE_4_PIX_INTRINSICS_3(m128_1_out, m128_last_out);
    average_4_pix_intrinsics_3_c(m128_1_expect, m128_last_expect);

    ASSERT_EQ(true, (CMP_M128I(m128_1_expect,m128_1_out) && CMP_M128I(m128_last_expect,m128_last_out)) )
        <<M128I_U8_TO_STR_2(m128_1_in)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_in)<<std::endl
        <<std::endl
        <<M128I_U8_TO_STR_2(m128_1_expect)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_expect)<<std::endl
        <<std::endl
        <<M128I_U8_TO_STR_2(m128_1_out)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_out)<<std::endl;

    for (int i{0};i<100000;++i)
    {
        for (int j{0};j<4;++j)
        {
            m128_1_in.m128i_i32[j] = rand();
            m128_last_in.m128i_i32[j] = rand();
        }
        m128_last_in.m128i_u8[0U] = rand();
        m128_last_in.m128i_u8[1U] = rand();
        for (int i{2};i<16;++i)
        {
            m128_last_in.m128i_u8[i] = 0;
        }

        m128_1_out = m128_1_in; 
        m128_last_out = m128_last_in;
        m128_1_expect = m128_1_in; 
        m128_last_expect = m128_last_in;

        AVERAGE_4_PIX_INTRINSICS_3(m128_1_out, m128_last_out);
        average_4_pix_intrinsics_3_c(m128_1_expect, m128_last_expect);

        ASSERT_EQ(true, (CMP_M128I(m128_1_expect,m128_1_out) && CMP_M128I(m128_last_expect,m128_last_out)) )
            <<M128I_U8_TO_STR_2(m128_1_in)<<std::endl
            <<M128I_U8_TO_STR_2(m128_last_in)<<std::endl
            <<std::endl
            <<M128I_U8_TO_STR_2(m128_1_expect)<<std::endl
            <<M128I_U8_TO_STR_2(m128_last_expect)<<std::endl
            <<std::endl
            <<M128I_U8_TO_STR_2(m128_1_out)<<std::endl
            <<M128I_U8_TO_STR_2(m128_last_out)<<std::endl;
    }
}

TEST_F(XyIntrinsicsTest, Check_AVERAGE_4_PIX_INTRINSICS_4)
{   
    __m128i m128_1_in, m128_last_in;
    __m128i m128_1_out, m128_last_out;
    __m128i m128_1_expect, m128_last_expect;

    m128_1_in = _mm_setzero_si128();
    m128_last_in = _mm_setzero_si128();

    m128_1_out = m128_1_in; 
    m128_last_out = m128_last_in;
    m128_1_expect = m128_1_in; 
    m128_last_expect = m128_last_in;

    AVERAGE_4_PIX_INTRINSICS_4(m128_1_out, m128_last_out);
    average_4_pix_intrinsics_4_c(m128_1_expect, m128_last_expect);

    ASSERT_EQ(true, (CMP_M128I(m128_1_expect,m128_1_out) && CMP_M128I(m128_last_expect,m128_last_out)) )
        <<M128I_U8_TO_STR_2(m128_1_in)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_in)<<std::endl
        <<std::endl
        <<M128I_U8_TO_STR_2(m128_1_expect)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_expect)<<std::endl
        <<std::endl
        <<M128I_U8_TO_STR_2(m128_1_out)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_out)<<std::endl;

    m128_1_in = _mm_cmpeq_epi32(m128_1_in, m128_1_in);    
    m128_last_in.m128i_u8[0U] = 0xff;
    m128_last_in.m128i_u8[1U] = 0xff;
    for (int i{2};i<16;++i)
    {
        m128_last_in.m128i_u8[i] = 0;
    }

    m128_1_out = m128_1_in; 
    m128_last_out = m128_last_in;
    m128_1_expect = m128_1_in; 
    m128_last_expect = m128_last_in;

    AVERAGE_4_PIX_INTRINSICS_4(m128_1_out, m128_last_out);
    average_4_pix_intrinsics_4_c(m128_1_expect, m128_last_expect);

    ASSERT_EQ(true, (CMP_M128I(m128_1_expect,m128_1_out) && CMP_M128I(m128_last_expect,m128_last_out)) )
        <<M128I_U8_TO_STR_2(m128_1_in)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_in)<<std::endl
        <<std::endl
        <<M128I_U8_TO_STR_2(m128_1_expect)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_expect)<<std::endl
        <<std::endl
        <<M128I_U8_TO_STR_2(m128_1_out)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_out)<<std::endl;

    for (int i{0};i<100000;++i)
    {
        for (int j{0};j<4;++j)
        {
            m128_1_in.m128i_i32[j] = rand();
            m128_last_in.m128i_i32[j] = rand();
        }
        m128_last_in.m128i_u8[0U] = rand();
        m128_last_in.m128i_u8[1U] = rand();
        for (int i{2};i<16;++i)
        {
            m128_last_in.m128i_u8[i] = 0;
        }

        m128_1_out = m128_1_in; 
        m128_last_out = m128_last_in;
        m128_1_expect = m128_1_in; 
        m128_last_expect = m128_last_in;

        AVERAGE_4_PIX_INTRINSICS_4(m128_1_out, m128_last_out);
        average_4_pix_intrinsics_4_c(m128_1_expect, m128_last_expect);

        ASSERT_EQ(true, (CMP_M128I(m128_1_expect,m128_1_out) && CMP_M128I(m128_last_expect,m128_last_out)) )
            <<M128I_U8_TO_STR_2(m128_1_in)<<std::endl
            <<M128I_U8_TO_STR_2(m128_last_in)<<std::endl
            <<std::endl
            <<M128I_U8_TO_STR_2(m128_1_expect)<<std::endl
            <<M128I_U8_TO_STR_2(m128_last_expect)<<std::endl
            <<std::endl
            <<M128I_U8_TO_STR_2(m128_1_out)<<std::endl
            <<M128I_U8_TO_STR_2(m128_last_out)<<std::endl;
    }
}

TEST_F(XyIntrinsicsTest, Check_AVERAGE_4_PIX_INTRINSICS_5)
{   
    __m128i m128_1_in, m128_last_in;
    __m128i m128_1_out, m128_last_out;
    __m128i m128_1_expect, m128_last_expect;

    m128_1_in = _mm_setzero_si128();
    m128_last_in = _mm_setzero_si128();

    m128_1_out = m128_1_in; 
    m128_last_out = m128_last_in;
    m128_1_expect = m128_1_in; 
    m128_last_expect = m128_last_in;

    AVERAGE_4_PIX_INTRINSICS_5(m128_1_out, m128_last_out);
    average_4_pix_intrinsics_5_c(m128_1_expect, m128_last_expect);

    ASSERT_EQ(true, (CMP_M128I(m128_1_expect,m128_1_out) && CMP_M128I(m128_last_expect,m128_last_out)) )
        <<M128I_U8_TO_STR_2(m128_1_in)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_in)<<std::endl
        <<std::endl
        <<M128I_U8_TO_STR_2(m128_1_expect)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_expect)<<std::endl
        <<std::endl
        <<M128I_U8_TO_STR_2(m128_1_out)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_out)<<std::endl;

    m128_1_in = _mm_cmpeq_epi32(m128_1_in, m128_1_in);    
    m128_last_in.m128i_u8[0U] = 0xff;
    m128_last_in.m128i_u8[1U] = 0xff;
    for (int i{2};i<16;++i)
    {
        m128_last_in.m128i_u8[i] = 0;
    }

    m128_1_out = m128_1_in; 
    m128_last_out = m128_last_in;
    m128_1_expect = m128_1_in; 
    m128_last_expect = m128_last_in;

    AVERAGE_4_PIX_INTRINSICS_5(m128_1_out, m128_last_out);
    average_4_pix_intrinsics_5_c(m128_1_expect, m128_last_expect);

    ASSERT_EQ(true, (CMP_M128I(m128_1_expect,m128_1_out) && CMP_M128I(m128_last_expect,m128_last_out)) )
        <<M128I_U8_TO_STR_2(m128_1_in)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_in)<<std::endl
        <<std::endl
        <<M128I_U8_TO_STR_2(m128_1_expect)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_expect)<<std::endl
        <<std::endl
        <<M128I_U8_TO_STR_2(m128_1_out)<<std::endl
        <<M128I_U8_TO_STR_2(m128_last_out)<<std::endl;

    for (int i{0};i<100000;++i)
    {
        for (int j{0};j<4;++j)
        {
            m128_1_in.m128i_i32[j] = rand();
            m128_last_in.m128i_i32[j] = rand();
        }
        m128_last_in.m128i_u8[0U] = rand();
        m128_last_in.m128i_u8[1U] = rand();
        for (int i{2};i<16;++i)
        {
            m128_last_in.m128i_u8[i] = 0;
        }

        m128_1_out = m128_1_in; 
        m128_last_out = m128_last_in;
        m128_1_expect = m128_1_in; 
        m128_last_expect = m128_last_in;

        AVERAGE_4_PIX_INTRINSICS_5(m128_1_out, m128_last_out);
        average_4_pix_intrinsics_5_c(m128_1_expect, m128_last_expect);

        ASSERT_EQ(true, (CMP_M128I(m128_1_expect,m128_1_out) && CMP_M128I(m128_last_expect,m128_last_out)) )
            <<M128I_U8_TO_STR_2(m128_1_in)<<std::endl
            <<M128I_U8_TO_STR_2(m128_last_in)<<std::endl
            <<std::endl
            <<M128I_U8_TO_STR_2(m128_1_expect)<<std::endl
            <<M128I_U8_TO_STR_2(m128_last_expect)<<std::endl
            <<std::endl
            <<M128I_U8_TO_STR_2(m128_1_out)<<std::endl
            <<M128I_U8_TO_STR_2(m128_last_out)<<std::endl;
    }
}


#endif // __TEST_INSTRINSICS_MACRO_0E1D7AC0_6386_4E23_97F5_91AFAB395ED7_H__
