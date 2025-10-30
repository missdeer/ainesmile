#include "stdafx.h"

#include "textutils.h"

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "CodeEdit/textutils.cpp"
#include <hwy/foreach_target.h>
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace TextUtils
{
    namespace HWY_NAMESPACE
    {
        int getLineCountImpl(const char *pData, qint64 length)
        {
            namespace hn = hwy::HWY_NAMESPACE;
            using D      = hn::ScalableTag<uint8_t>;
            const D d;

            int         lineCount = 0;
            const char *pEnd      = pData + length;

            // Create a vector containing the newline character
            const auto newline = hn::Set(d, '\n');

            // Get the number of lanes (bytes) in a vector
            const size_t N = hn::Lanes(d);

            // Main loop processes N bytes at a time
            while (pData + N <= pEnd)
            {
                // Load data into a Highway vector
                const auto chunk = hn::LoadU(d, reinterpret_cast<const uint8_t *>(pData));

                // Compare with newline character
                const auto mask = hn::Eq(chunk, newline);

                // Count matches using PopCount
                lineCount += hn::CountTrue(d, mask);

                pData += N;
            }

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
    } // namespace HWY_NAMESPACE
} // namespace TextUtils
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace TextUtils
{
    HWY_EXPORT(getLineCountImpl);

    int getLineCount(const char *pData, qint64 length)
    {
        return HWY_DYNAMIC_DISPATCH(getLineCountImpl)(pData, length);
    }

    int getLineCount(const QByteArray &data)
    {
        return getLineCount(data.constData(), data.length());
    }

} // namespace TextUtils
#endif // HWY_ONCE