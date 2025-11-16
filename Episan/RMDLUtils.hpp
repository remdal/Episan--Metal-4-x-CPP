/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLUtils.hpp            +++     +++			**/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 09/09/2025 14:50:41      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef RMDLUTILS_HPP
# define RMDLUTILS_HPP

# include <chrono>
# include <time.h>
# include <iostream>
# include <sys/sysctl.h>
# include <stdlib.h>
# include <fstream>

void *ft_memcpy(void *dst, const void *src, size_t n);
void *ft_memset(void *s, int c, size_t n);
std::vector<uint8_t> readBytecode(const std::string& path);

#endif /* RMDLUTILS_HPP */
