#include "stdafx.h"

#include "textutils.h"
#include "VectorISA.h"


namespace TextUtils
{
    int getLineCount(const char *pData, qint64 length)
    {
        int         lineCount = 0;
        const char *pEnd      = pData + length;
#if AS_USE_SSE2
        // Prepare a 128-bit register with 16 bytes set to '\n'
        __m128i newline = _mm_set1_epi8('\n');

        // Main loop processes 16 bytes at a time
        while (pData + 15 < pEnd)
        {
            __m128i chunk = _mm_loadu_si128((__m128i const *)pData);

            // Compare 16 bytes to '\n', result is 0 for non-matching bytes, 0xFF for matching bytes
            __m128i result = _mm_cmpeq_epi8(chunk, newline);

            // Use a popcount intrinsic to count the number of set bits in the 128-bit value.
            lineCount += as_popcount(_mm_movemask_epi8(result));

            pData += 16;
        }

#elif AS_USE_AVX2

        // Prepare a 256-bit register with 32 bytes set to '\n'
        __m256i newline = _mm256_set1_epi8('\n');

        // Main loop processes 32 bytes at a time
        while (pData + 31 < pEnd)
        {
            __m256i chunk = _mm256_loadu_si256((__m256i const *)pData);

            // Compare 32 bytes to '\n', result is 0 for non-matching bytes, 0xFF for matching bytes
            __m256i result = _mm256_cmpeq_epi8(chunk, newline);

            // Sum up the set bits from the comparison result
            lineCount += as_popcount(_mm256_movemask_epi8(result));

            pData += 32;
        }
#else
#endif
        // Process any remaining bytes
        while (pData < pEnd)
        {
            if (*pData == '\n')
            {
                ++lineCount;
            }
            ++pData;
        }
        return lineCount + 1;
    }

    int getLineCount(const QByteArray &data)
    {
        return getLineCount(data.constData(), data.length());
    }

} // namespace TextUtils