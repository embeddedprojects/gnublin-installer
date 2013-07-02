/*  md5.cpp
 *
 *  Author:    Manuel Liebert
 *
 * Functions to generate md5 checksum of given file
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "md5.cpp"
#include "installer.h"

char* calc_md5(char* filename) {
  MD5 md5;
  FILE* file;

  int len;
  unsigned char buffer[1024] ;

  if((file = fopen(filename, "rb")) == NULL) {
    printf("%s can't be opened\n", filename) ;
  } else {
    while((len = fread(buffer, 1, 1024, file))) {
      md5.update(buffer, len) ;
    }
    md5.finalize();

    fclose(file);
  }
  return (char*)md5.hexdigest().c_str() ;
}

