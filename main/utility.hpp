#pragma once

#include <cstddef>
#include <cstring>

namespace zsf::utility
{

/**************************************************************************************************
 * @brief 固定长度字符串，长度在编译器确定，不进行堆分配
 *
 * @tparam N 指定要存储的字符个数，不包含 '\0'
 * @note
 *
 * @example
 * `zsf::utility::FixedString<64> name("button");
 *************************************************************************************************/
template <std::size_t N = 10>
class FixedString
{
public:
    /**************************************************************************************************
     * @brief Construct a new Fixed String object
     *
     * @param src
     * @date 2026-10-06
     * @details
     * @Call 该函数调用的其他函数
     * @Caller 调用该函数的其他函数
     * @author freerealmshn@gmail.com
     * @note
     *************************************************************************************************/
    FixedString(const char *src = "") { set(src); }

    void set(const char *src)
    {
        if (!src)
        {
            _buf[0] = '\0';
            return;
        }
        size_t len = std::strlen(src);
        if (len >= N)
        {
            // 运行期提示：程序终止并打印文件名/行号
            assert(len < N && "FixedString: source string too long");
            len = N - 1;
        }
        std::memcpy(_buf, src, len);
        _buf[len] = '\0';
    }

    const char *c_str() const { return _buf; }
    /**************************************************************************************************
     * @brief 返回不包含 '\0' 的字符数量
     *
     * @return size_t 返回字符的数量
     * @date 2026-10-06
     * @details
     * @Call 该函数调用的其他函数
     * @Caller 调用该函数的其他函数
     * @author freerealmshn@gmail.com
     * @note
     *************************************************************************************************/
    size_t size() const { return std::strlen(_buf); }
    bool append(const char *src)
    {
        std::size_t cur = size();
        std::size_t add = std::strlen(src);
        if (cur + add > N - 1) return false;
        std::memcpy(_buf + cur, src, add);
        _buf[cur + add] = '\0';
        return true;
    }

private:
    char _buf[N];
};

}  // namespace zsf::utility