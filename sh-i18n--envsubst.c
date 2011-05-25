begin_unit
begin_comment
comment|/*  * sh-i18n--envsubst.c - a stripped-down version of gettext's envsubst(1)  *  * Copyright (C) 2010 Ãvar ArnfjÃ¶rÃ° Bjarmason  *  * This is a modified version of  * 67d0871a8c:gettext-runtime/src/envsubst.c from the gettext.git  * repository. It has been stripped down to only implement the  * envsubst(1) features that we need in the git-sh-i18n fallbacks.  *  * The "Close standard error" part in main() is from  * 8dac033df0:gnulib-local/lib/closeout.c. The copyright notices for  * both files are reproduced immediately below.  */
end_comment
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_comment
comment|/* Substitution of environment variables in shell format strings.    Copyright (C) 2003-2007 Free Software Foundation, Inc.    Written by Bruno Haible<bruno@clisp.org>, 2003.     This program is free software; you can redistribute it and/or modify    it under the terms of the GNU General Public License as published by    the Free Software Foundation; either version 2, or (at your option)    any later version.     This program is distributed in the hope that it will be useful,    but WITHOUT ANY WARRANTY; without even the implied warranty of    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    GNU General Public License for more details.     You should have received a copy of the GNU General Public License    along with this program; if not, write to the Free Software Foundation,    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */
end_comment
begin_comment
comment|/* closeout.c - close standard output and standard error    Copyright (C) 1998-2007 Free Software Foundation, Inc.     This program is free software; you can redistribute it and/or modify    it under the terms of the GNU General Public License as published by    the Free Software Foundation; either version 2, or (at your option)    any later version.     This program is distributed in the hope that it will be useful,    but WITHOUT ANY WARRANTY; without even the implied warranty of    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    GNU General Public License for more details.     You should have received a copy of the GNU General Public License    along with this program; if not, write to the Free Software Foundation,    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */
end_comment
begin_include
include|#
directive|include
file|<errno.h>
end_include
begin_include
include|#
directive|include
file|<getopt.h>
end_include
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_comment
comment|/* If true, substitution shall be performed on all variables.  */
end_comment
begin_decl_stmt
DECL|variable|all_variables
specifier|static
name|unsigned
name|short
name|int
name|all_variables
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* Forward declaration of local functions.  */
end_comment
begin_function_decl
specifier|static
name|void
name|print_variables
parameter_list|(
specifier|const
name|char
modifier|*
name|string
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|static
name|void
name|note_variables
parameter_list|(
specifier|const
name|char
modifier|*
name|string
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|static
name|void
name|subst_from_stdin
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function
name|int
DECL|function|main
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
name|argv
index|[]
parameter_list|)
block|{
comment|/* Default values for command line options.  */
comment|/* unsigned short int show_variables = 0; */
switch|switch
condition|(
name|argc
condition|)
block|{
case|case
literal|1
case|:
name|error
argument_list|(
literal|"we won't substitute all variables on stdin for you"
argument_list|)
expr_stmt|;
comment|/* 	  all_variables = 1;       subst_from_stdin (); 	  */
case|case
literal|2
case|:
comment|/* echo '$foo and $bar' | git sh-i18n--envsubst --variables '$foo and $bar' */
name|all_variables
operator|=
literal|0
expr_stmt|;
name|note_variables
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|subst_from_stdin
argument_list|()
expr_stmt|;
break|break;
case|case
literal|3
case|:
comment|/* git sh-i18n--envsubst --variables '$foo and $bar' */
if|if
condition|(
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--variables"
argument_list|)
condition|)
name|error
argument_list|(
literal|"first argument must be --variables when two are given"
argument_list|)
expr_stmt|;
comment|/* show_variables = 1; */
name|print_variables
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
break|break;
default|default:
name|error
argument_list|(
literal|"too many arguments"
argument_list|)
expr_stmt|;
break|break;
block|}
comment|/* Close standard error.  This is simpler than fwriteerror_no_ebadf, because      upon failure we don't need an errno - all we can do at this point is to      set an exit status.  */
name|errno
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|ferror
argument_list|(
name|stderr
argument_list|)
operator|||
name|fflush
argument_list|(
name|stderr
argument_list|)
condition|)
block|{
name|fclose
argument_list|(
name|stderr
argument_list|)
expr_stmt|;
name|exit
argument_list|(
name|EXIT_FAILURE
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|fclose
argument_list|(
name|stderr
argument_list|)
operator|&&
name|errno
operator|!=
name|EBADF
condition|)
name|exit
argument_list|(
name|EXIT_FAILURE
argument_list|)
expr_stmt|;
name|exit
argument_list|(
name|EXIT_SUCCESS
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Parse the string and invoke the callback each time a $VARIABLE or    ${VARIABLE} construct is seen, where VARIABLE is a nonempty sequence    of ASCII alphanumeric/underscore characters, starting with an ASCII    alphabetic/underscore character.    We allow only ASCII characters, to avoid dependencies w.r.t. the current    encoding: While "${\xe0}" looks like a variable access in ISO-8859-1    encoding, it doesn't look like one in the BIG5, BIG5-HKSCS, GBK, GB18030,    SHIFT_JIS, JOHAB encodings, because \xe0\x7d is a single character in these    encodings.  */
end_comment
begin_function
specifier|static
name|void
DECL|function|find_variables
name|find_variables
parameter_list|(
specifier|const
name|char
modifier|*
name|string
parameter_list|,
name|void
function_decl|(
modifier|*
name|callback
function_decl|)
parameter_list|(
specifier|const
name|char
modifier|*
name|var_ptr
parameter_list|,
name|size_t
name|var_len
parameter_list|)
parameter_list|)
block|{
for|for
control|(
init|;
operator|*
name|string
operator|!=
literal|'\0'
condition|;
control|)
if|if
condition|(
operator|*
name|string
operator|++
operator|==
literal|'$'
condition|)
block|{
specifier|const
name|char
modifier|*
name|variable_start
decl_stmt|;
specifier|const
name|char
modifier|*
name|variable_end
decl_stmt|;
name|unsigned
name|short
name|int
name|valid
decl_stmt|;
name|char
name|c
decl_stmt|;
if|if
condition|(
operator|*
name|string
operator|==
literal|'{'
condition|)
name|string
operator|++
expr_stmt|;
name|variable_start
operator|=
name|string
expr_stmt|;
name|c
operator|=
operator|*
name|string
expr_stmt|;
if|if
condition|(
operator|(
name|c
operator|>=
literal|'A'
operator|&&
name|c
operator|<=
literal|'Z'
operator|)
operator|||
operator|(
name|c
operator|>=
literal|'a'
operator|&&
name|c
operator|<=
literal|'z'
operator|)
operator|||
name|c
operator|==
literal|'_'
condition|)
block|{
do|do
name|c
operator|=
operator|*
operator|++
name|string
expr_stmt|;
do|while
condition|(
operator|(
name|c
operator|>=
literal|'A'
operator|&&
name|c
operator|<=
literal|'Z'
operator|)
operator|||
operator|(
name|c
operator|>=
literal|'a'
operator|&&
name|c
operator|<=
literal|'z'
operator|)
operator|||
operator|(
name|c
operator|>=
literal|'0'
operator|&&
name|c
operator|<=
literal|'9'
operator|)
operator|||
name|c
operator|==
literal|'_'
condition|)
do|;
name|variable_end
operator|=
name|string
expr_stmt|;
if|if
condition|(
name|variable_start
index|[
operator|-
literal|1
index|]
operator|==
literal|'{'
condition|)
block|{
if|if
condition|(
operator|*
name|string
operator|==
literal|'}'
condition|)
block|{
name|string
operator|++
expr_stmt|;
name|valid
operator|=
literal|1
expr_stmt|;
block|}
else|else
name|valid
operator|=
literal|0
expr_stmt|;
block|}
else|else
name|valid
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|valid
condition|)
name|callback
argument_list|(
name|variable_start
argument_list|,
name|variable_end
operator|-
name|variable_start
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_comment
comment|/* Print a variable to stdout, followed by a newline.  */
end_comment
begin_function
specifier|static
name|void
DECL|function|print_variable
name|print_variable
parameter_list|(
specifier|const
name|char
modifier|*
name|var_ptr
parameter_list|,
name|size_t
name|var_len
parameter_list|)
block|{
name|fwrite
argument_list|(
name|var_ptr
argument_list|,
name|var_len
argument_list|,
literal|1
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Print the variables contained in STRING to stdout, each one followed by a    newline.  */
end_comment
begin_function
specifier|static
name|void
DECL|function|print_variables
name|print_variables
parameter_list|(
specifier|const
name|char
modifier|*
name|string
parameter_list|)
block|{
name|find_variables
argument_list|(
name|string
argument_list|,
operator|&
name|print_variable
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Type describing list of immutable strings,    implemented using a dynamic array.  */
end_comment
begin_typedef
DECL|typedef|string_list_ty
typedef|typedef
name|struct
name|string_list_ty
name|string_list_ty
typedef|;
end_typedef
begin_struct
DECL|struct|string_list_ty
struct|struct
name|string_list_ty
block|{
DECL|member|item
specifier|const
name|char
modifier|*
modifier|*
name|item
decl_stmt|;
DECL|member|nitems
name|size_t
name|nitems
decl_stmt|;
DECL|member|nitems_max
name|size_t
name|nitems_max
decl_stmt|;
block|}
struct|;
end_struct
begin_comment
comment|/* Initialize an empty list of strings.  */
end_comment
begin_function
specifier|static
specifier|inline
name|void
DECL|function|string_list_init
name|string_list_init
parameter_list|(
name|string_list_ty
modifier|*
name|slp
parameter_list|)
block|{
name|slp
operator|->
name|item
operator|=
name|NULL
expr_stmt|;
name|slp
operator|->
name|nitems
operator|=
literal|0
expr_stmt|;
name|slp
operator|->
name|nitems_max
operator|=
literal|0
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Append a single string to the end of a list of strings.  */
end_comment
begin_function
specifier|static
specifier|inline
name|void
DECL|function|string_list_append
name|string_list_append
parameter_list|(
name|string_list_ty
modifier|*
name|slp
parameter_list|,
specifier|const
name|char
modifier|*
name|s
parameter_list|)
block|{
comment|/* Grow the list.  */
if|if
condition|(
name|slp
operator|->
name|nitems
operator|>=
name|slp
operator|->
name|nitems_max
condition|)
block|{
name|size_t
name|nbytes
decl_stmt|;
name|slp
operator|->
name|nitems_max
operator|=
name|slp
operator|->
name|nitems_max
operator|*
literal|2
operator|+
literal|4
expr_stmt|;
name|nbytes
operator|=
name|slp
operator|->
name|nitems_max
operator|*
sizeof|sizeof
argument_list|(
name|slp
operator|->
name|item
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|slp
operator|->
name|item
operator|=
operator|(
specifier|const
name|char
operator|*
operator|*
operator|)
name|xrealloc
argument_list|(
name|slp
operator|->
name|item
argument_list|,
name|nbytes
argument_list|)
expr_stmt|;
block|}
comment|/* Add the string to the end of the list.  */
name|slp
operator|->
name|item
index|[
name|slp
operator|->
name|nitems
operator|++
index|]
operator|=
name|s
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Compare two strings given by reference.  */
end_comment
begin_function
specifier|static
name|int
DECL|function|cmp_string
name|cmp_string
parameter_list|(
specifier|const
name|void
modifier|*
name|pstr1
parameter_list|,
specifier|const
name|void
modifier|*
name|pstr2
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|str1
init|=
operator|*
operator|(
specifier|const
name|char
operator|*
operator|*
operator|)
name|pstr1
decl_stmt|;
specifier|const
name|char
modifier|*
name|str2
init|=
operator|*
operator|(
specifier|const
name|char
operator|*
operator|*
operator|)
name|pstr2
decl_stmt|;
return|return
name|strcmp
argument_list|(
name|str1
argument_list|,
name|str2
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/* Sort a list of strings.  */
end_comment
begin_function
specifier|static
specifier|inline
name|void
DECL|function|string_list_sort
name|string_list_sort
parameter_list|(
name|string_list_ty
modifier|*
name|slp
parameter_list|)
block|{
if|if
condition|(
name|slp
operator|->
name|nitems
operator|>
literal|0
condition|)
name|qsort
argument_list|(
name|slp
operator|->
name|item
argument_list|,
name|slp
operator|->
name|nitems
argument_list|,
sizeof|sizeof
argument_list|(
name|slp
operator|->
name|item
index|[
literal|0
index|]
argument_list|)
argument_list|,
name|cmp_string
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Test whether a string list contains a given string.  */
end_comment
begin_function
specifier|static
specifier|inline
name|int
DECL|function|string_list_member
name|string_list_member
parameter_list|(
specifier|const
name|string_list_ty
modifier|*
name|slp
parameter_list|,
specifier|const
name|char
modifier|*
name|s
parameter_list|)
block|{
name|size_t
name|j
decl_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|slp
operator|->
name|nitems
condition|;
operator|++
name|j
control|)
if|if
condition|(
name|strcmp
argument_list|(
name|slp
operator|->
name|item
index|[
name|j
index|]
argument_list|,
name|s
argument_list|)
operator|==
literal|0
condition|)
return|return
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/* Test whether a sorted string list contains a given string.  */
end_comment
begin_function
specifier|static
name|int
DECL|function|sorted_string_list_member
name|sorted_string_list_member
parameter_list|(
specifier|const
name|string_list_ty
modifier|*
name|slp
parameter_list|,
specifier|const
name|char
modifier|*
name|s
parameter_list|)
block|{
name|size_t
name|j1
decl_stmt|,
name|j2
decl_stmt|;
name|j1
operator|=
literal|0
expr_stmt|;
name|j2
operator|=
name|slp
operator|->
name|nitems
expr_stmt|;
if|if
condition|(
name|j2
operator|>
literal|0
condition|)
block|{
comment|/* Binary search.  */
while|while
condition|(
name|j2
operator|-
name|j1
operator|>
literal|1
condition|)
block|{
comment|/* Here we know that if s is in the list, it is at an index j 	     with j1<= j< j2.  */
name|size_t
name|j
init|=
operator|(
name|j1
operator|+
name|j2
operator|)
operator|>>
literal|1
decl_stmt|;
name|int
name|result
init|=
name|strcmp
argument_list|(
name|slp
operator|->
name|item
index|[
name|j
index|]
argument_list|,
name|s
argument_list|)
decl_stmt|;
if|if
condition|(
name|result
operator|>
literal|0
condition|)
name|j2
operator|=
name|j
expr_stmt|;
elseif|else
if|if
condition|(
name|result
operator|==
literal|0
condition|)
return|return
literal|1
return|;
else|else
name|j1
operator|=
name|j
operator|+
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|j2
operator|>
name|j1
condition|)
if|if
condition|(
name|strcmp
argument_list|(
name|slp
operator|->
name|item
index|[
name|j1
index|]
argument_list|,
name|s
argument_list|)
operator|==
literal|0
condition|)
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/* Set of variables on which to perform substitution.    Used only if !all_variables.  */
end_comment
begin_decl_stmt
DECL|variable|variables_set
specifier|static
name|string_list_ty
name|variables_set
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* Adds a variable to variables_set.  */
end_comment
begin_function
specifier|static
name|void
DECL|function|note_variable
name|note_variable
parameter_list|(
specifier|const
name|char
modifier|*
name|var_ptr
parameter_list|,
name|size_t
name|var_len
parameter_list|)
block|{
name|char
modifier|*
name|string
init|=
name|xmalloc
argument_list|(
name|var_len
operator|+
literal|1
argument_list|)
decl_stmt|;
name|memcpy
argument_list|(
name|string
argument_list|,
name|var_ptr
argument_list|,
name|var_len
argument_list|)
expr_stmt|;
name|string
index|[
name|var_len
index|]
operator|=
literal|'\0'
expr_stmt|;
name|string_list_append
argument_list|(
operator|&
name|variables_set
argument_list|,
name|string
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Stores the variables occurring in the string in variables_set.  */
end_comment
begin_function
specifier|static
name|void
DECL|function|note_variables
name|note_variables
parameter_list|(
specifier|const
name|char
modifier|*
name|string
parameter_list|)
block|{
name|string_list_init
argument_list|(
operator|&
name|variables_set
argument_list|)
expr_stmt|;
name|find_variables
argument_list|(
name|string
argument_list|,
operator|&
name|note_variable
argument_list|)
expr_stmt|;
name|string_list_sort
argument_list|(
operator|&
name|variables_set
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
specifier|static
name|int
DECL|function|do_getc
name|do_getc
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|c
init|=
name|getc
argument_list|(
name|stdin
argument_list|)
decl_stmt|;
if|if
condition|(
name|c
operator|==
name|EOF
condition|)
block|{
if|if
condition|(
name|ferror
argument_list|(
name|stdin
argument_list|)
condition|)
name|error
argument_list|(
literal|"error while reading standard input"
argument_list|)
expr_stmt|;
block|}
return|return
name|c
return|;
block|}
end_function
begin_function
specifier|static
specifier|inline
name|void
DECL|function|do_ungetc
name|do_ungetc
parameter_list|(
name|int
name|c
parameter_list|)
block|{
if|if
condition|(
name|c
operator|!=
name|EOF
condition|)
name|ungetc
argument_list|(
name|c
argument_list|,
name|stdin
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Copies stdin to stdout, performing substitutions.  */
end_comment
begin_function
specifier|static
name|void
DECL|function|subst_from_stdin
name|subst_from_stdin
parameter_list|(
name|void
parameter_list|)
block|{
specifier|static
name|char
modifier|*
name|buffer
decl_stmt|;
specifier|static
name|size_t
name|bufmax
decl_stmt|;
specifier|static
name|size_t
name|buflen
decl_stmt|;
name|int
name|c
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|c
operator|=
name|do_getc
argument_list|()
expr_stmt|;
if|if
condition|(
name|c
operator|==
name|EOF
condition|)
break|break;
comment|/* Look for $VARIABLE or ${VARIABLE}.  */
if|if
condition|(
name|c
operator|==
literal|'$'
condition|)
block|{
name|unsigned
name|short
name|int
name|opening_brace
init|=
literal|0
decl_stmt|;
name|unsigned
name|short
name|int
name|closing_brace
init|=
literal|0
decl_stmt|;
name|c
operator|=
name|do_getc
argument_list|()
expr_stmt|;
if|if
condition|(
name|c
operator|==
literal|'{'
condition|)
block|{
name|opening_brace
operator|=
literal|1
expr_stmt|;
name|c
operator|=
name|do_getc
argument_list|()
expr_stmt|;
block|}
if|if
condition|(
operator|(
name|c
operator|>=
literal|'A'
operator|&&
name|c
operator|<=
literal|'Z'
operator|)
operator|||
operator|(
name|c
operator|>=
literal|'a'
operator|&&
name|c
operator|<=
literal|'z'
operator|)
operator|||
name|c
operator|==
literal|'_'
condition|)
block|{
name|unsigned
name|short
name|int
name|valid
decl_stmt|;
comment|/* Accumulate the VARIABLE in buffer.  */
name|buflen
operator|=
literal|0
expr_stmt|;
do|do
block|{
if|if
condition|(
name|buflen
operator|>=
name|bufmax
condition|)
block|{
name|bufmax
operator|=
literal|2
operator|*
name|bufmax
operator|+
literal|10
expr_stmt|;
name|buffer
operator|=
name|xrealloc
argument_list|(
name|buffer
argument_list|,
name|bufmax
argument_list|)
expr_stmt|;
block|}
name|buffer
index|[
name|buflen
operator|++
index|]
operator|=
name|c
expr_stmt|;
name|c
operator|=
name|do_getc
argument_list|()
expr_stmt|;
block|}
do|while
condition|(
operator|(
name|c
operator|>=
literal|'A'
operator|&&
name|c
operator|<=
literal|'Z'
operator|)
operator|||
operator|(
name|c
operator|>=
literal|'a'
operator|&&
name|c
operator|<=
literal|'z'
operator|)
operator|||
operator|(
name|c
operator|>=
literal|'0'
operator|&&
name|c
operator|<=
literal|'9'
operator|)
operator|||
name|c
operator|==
literal|'_'
condition|)
do|;
if|if
condition|(
name|opening_brace
condition|)
block|{
if|if
condition|(
name|c
operator|==
literal|'}'
condition|)
block|{
name|closing_brace
operator|=
literal|1
expr_stmt|;
name|valid
operator|=
literal|1
expr_stmt|;
block|}
else|else
block|{
name|valid
operator|=
literal|0
expr_stmt|;
name|do_ungetc
argument_list|(
name|c
argument_list|)
expr_stmt|;
block|}
block|}
else|else
block|{
name|valid
operator|=
literal|1
expr_stmt|;
name|do_ungetc
argument_list|(
name|c
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|valid
condition|)
block|{
comment|/* Terminate the variable in the buffer.  */
if|if
condition|(
name|buflen
operator|>=
name|bufmax
condition|)
block|{
name|bufmax
operator|=
literal|2
operator|*
name|bufmax
operator|+
literal|10
expr_stmt|;
name|buffer
operator|=
name|xrealloc
argument_list|(
name|buffer
argument_list|,
name|bufmax
argument_list|)
expr_stmt|;
block|}
name|buffer
index|[
name|buflen
index|]
operator|=
literal|'\0'
expr_stmt|;
comment|/* Test whether the variable shall be substituted.  */
if|if
condition|(
operator|!
name|all_variables
operator|&&
operator|!
name|sorted_string_list_member
argument_list|(
operator|&
name|variables_set
argument_list|,
name|buffer
argument_list|)
condition|)
name|valid
operator|=
literal|0
expr_stmt|;
block|}
if|if
condition|(
name|valid
condition|)
block|{
comment|/* Substitute the variable's value from the environment.  */
specifier|const
name|char
modifier|*
name|env_value
init|=
name|getenv
argument_list|(
name|buffer
argument_list|)
decl_stmt|;
if|if
condition|(
name|env_value
operator|!=
name|NULL
condition|)
name|fputs
argument_list|(
name|env_value
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
block|}
else|else
block|{
comment|/* Perform no substitution at all.  Since the buffered input 		     contains no other '$' than at the start, we can just 		     output all the buffered contents.  */
name|putchar
argument_list|(
literal|'$'
argument_list|)
expr_stmt|;
if|if
condition|(
name|opening_brace
condition|)
name|putchar
argument_list|(
literal|'{'
argument_list|)
expr_stmt|;
name|fwrite
argument_list|(
name|buffer
argument_list|,
name|buflen
argument_list|,
literal|1
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
name|closing_brace
condition|)
name|putchar
argument_list|(
literal|'}'
argument_list|)
expr_stmt|;
block|}
block|}
else|else
block|{
name|do_ungetc
argument_list|(
name|c
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'$'
argument_list|)
expr_stmt|;
if|if
condition|(
name|opening_brace
condition|)
name|putchar
argument_list|(
literal|'{'
argument_list|)
expr_stmt|;
block|}
block|}
else|else
name|putchar
argument_list|(
name|c
argument_list|)
expr_stmt|;
block|}
block|}
end_function
end_unit
