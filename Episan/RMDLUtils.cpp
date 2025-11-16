/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLUtils.cpp            +++     +++           **/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 09/09/2025 14:50:38      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "RMDLUtils.hpp"

void *ft_memcpy(void *dst, const void *src, size_t n)
{
    unsigned char       *c_dst;
    const unsigned char *c_src;

    c_dst = (unsigned char *)dst;
    c_src = (const unsigned char *)src;
    if (dst == NULL && src == NULL)
        return (dst);
    while (n--)
        *c_dst++ = *c_src++;
    return (dst);
}

void *ft_memset(void *s, int c, size_t n)
{
    unsigned char   *ptr;
    unsigned char   value;

    ptr = (unsigned char *)s;
    value = (unsigned char)c;
    while (n --)
        *ptr++ = value;
    return (s);
}

std::vector<uint8_t> readBytecode( const std::string& path )
{
    std::ifstream in(path);
    if (in)
    {
        in.seekg(0, std::ios::end);
        size_t siz = in.tellg();
        in.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> bytecode(siz);
        in.read((char *)bytecode.data(), siz);
        
        return (bytecode);
    }
    return {};
}
