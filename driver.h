/*
 * driver.h
 *
 *  Created on: 2021年12月31日
 *      Author: nick
 */

#ifndef DRIVER_H_
#define DRIVER_H_
#include "isoc.h"
// Give Flex the prototype of yylex we want ...
# define YY_DECL \
  yy::parser::token_kind_type yylex (yy::parser::value_type* node)
// ... and declare it for the parser's sake.
YY_DECL;



#endif /* DRIVER_H_ */
