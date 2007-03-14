begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  *  * Copyright (C) 2000-2002 Michael R. Elkins<me@mutt.org>  * Copyright (C) 2002-2004 Oswald Buddenhagen<ossi@users.sf.net>  * Copyright (C) 2004 Theodore Y. Ts'o<tytso@mit.edu>  * Copyright (C) 2006 Mike McCormack  * Copyright (C) 2006 Christian Couder  *  *  This program is free software; you can redistribute it and/or modify  *  it under the terms of the GNU General Public License as published by  *  the Free Software Foundation; either version 2 of the License, or  *  (at your option) any later version.  *  *  This program is distributed in the hope that it will be useful,  *  but WITHOUT ANY WARRANTY; without even the implied warranty of  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  *  GNU General Public License for more details.  *  *  You should have received a copy of the GNU General Public License  *  along with this program; if not, write to the Free Software  *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_comment
comment|/* Stolen from "imap-send.c". */
end_comment
begin_function
DECL|function|nfvasprintf
name|int
name|nfvasprintf
parameter_list|(
name|char
modifier|*
modifier|*
name|strp
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
name|va_list
name|ap
parameter_list|)
block|{
name|int
name|len
decl_stmt|;
name|char
name|tmp
index|[
literal|1024
index|]
decl_stmt|;
if|if
condition|(
operator|(
name|len
operator|=
name|vsnprintf
argument_list|(
name|tmp
argument_list|,
sizeof|sizeof
argument_list|(
name|tmp
argument_list|)
argument_list|,
name|fmt
argument_list|,
name|ap
argument_list|)
operator|)
operator|<
literal|0
operator|||
operator|!
operator|(
operator|*
name|strp
operator|=
name|xmalloc
argument_list|(
name|len
operator|+
literal|1
argument_list|)
operator|)
condition|)
name|die
argument_list|(
literal|"Fatal: Out of memory\n"
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|>=
operator|(
name|int
operator|)
sizeof|sizeof
argument_list|(
name|tmp
argument_list|)
condition|)
name|vsprintf
argument_list|(
operator|*
name|strp
argument_list|,
name|fmt
argument_list|,
name|ap
argument_list|)
expr_stmt|;
else|else
name|memcpy
argument_list|(
operator|*
name|strp
argument_list|,
name|tmp
argument_list|,
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
return|return
name|len
return|;
block|}
end_function
begin_function
DECL|function|nfasprintf
name|int
name|nfasprintf
parameter_list|(
name|char
modifier|*
modifier|*
name|str
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
block|{
name|int
name|rc
decl_stmt|;
name|va_list
name|args
decl_stmt|;
name|va_start
argument_list|(
name|args
argument_list|,
name|fmt
argument_list|)
expr_stmt|;
name|rc
operator|=
name|nfvasprintf
argument_list|(
name|str
argument_list|,
name|fmt
argument_list|,
name|args
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|args
argument_list|)
expr_stmt|;
return|return
name|rc
return|;
block|}
end_function
begin_comment
comment|/* Get a trace file descriptor from GIT_TRACE env variable. */
end_comment
begin_function
DECL|function|get_trace_fd
specifier|static
name|int
name|get_trace_fd
parameter_list|(
name|int
modifier|*
name|need_close
parameter_list|)
block|{
name|char
modifier|*
name|trace
init|=
name|getenv
argument_list|(
literal|"GIT_TRACE"
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|trace
operator|||
operator|!
name|strcmp
argument_list|(
name|trace
argument_list|,
literal|""
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|trace
argument_list|,
literal|"0"
argument_list|)
operator|||
operator|!
name|strcasecmp
argument_list|(
name|trace
argument_list|,
literal|"false"
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|trace
argument_list|,
literal|"1"
argument_list|)
operator|||
operator|!
name|strcasecmp
argument_list|(
name|trace
argument_list|,
literal|"true"
argument_list|)
condition|)
return|return
name|STDERR_FILENO
return|;
if|if
condition|(
name|strlen
argument_list|(
name|trace
argument_list|)
operator|==
literal|1
operator|&&
name|isdigit
argument_list|(
operator|*
name|trace
argument_list|)
condition|)
return|return
name|atoi
argument_list|(
name|trace
argument_list|)
return|;
if|if
condition|(
operator|*
name|trace
operator|==
literal|'/'
condition|)
block|{
name|int
name|fd
init|=
name|open
argument_list|(
name|trace
argument_list|,
name|O_WRONLY
operator||
name|O_APPEND
operator||
name|O_CREAT
argument_list|,
literal|0666
argument_list|)
decl_stmt|;
if|if
condition|(
name|fd
operator|==
operator|-
literal|1
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Could not open '%s' for tracing: %s\n"
literal|"Defaulting to tracing on stderr...\n"
argument_list|,
name|trace
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|STDERR_FILENO
return|;
block|}
operator|*
name|need_close
operator|=
literal|1
expr_stmt|;
return|return
name|fd
return|;
block|}
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"What does '%s' for GIT_TRACE means ?\n"
argument_list|,
name|trace
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"If you want to trace into a file, "
literal|"then please set GIT_TRACE to an absolute pathname "
literal|"(starting with /).\n"
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Defaulting to tracing on stderr...\n"
argument_list|)
expr_stmt|;
return|return
name|STDERR_FILENO
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|err_msg
specifier|static
specifier|const
name|char
name|err_msg
index|[]
init|=
literal|"Could not trace into fd given by "
literal|"GIT_TRACE environment variable"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|trace_printf
name|void
name|trace_printf
parameter_list|(
specifier|const
name|char
modifier|*
name|format
parameter_list|,
modifier|...
parameter_list|)
block|{
name|char
modifier|*
name|trace_str
decl_stmt|;
name|va_list
name|rest
decl_stmt|;
name|int
name|need_close
init|=
literal|0
decl_stmt|;
name|int
name|fd
init|=
name|get_trace_fd
argument_list|(
operator|&
name|need_close
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|fd
condition|)
return|return;
name|va_start
argument_list|(
name|rest
argument_list|,
name|format
argument_list|)
expr_stmt|;
name|nfvasprintf
argument_list|(
operator|&
name|trace_str
argument_list|,
name|format
argument_list|,
name|rest
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|rest
argument_list|)
expr_stmt|;
name|write_or_whine_pipe
argument_list|(
name|fd
argument_list|,
name|trace_str
argument_list|,
name|strlen
argument_list|(
name|trace_str
argument_list|)
argument_list|,
name|err_msg
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|trace_str
argument_list|)
expr_stmt|;
if|if
condition|(
name|need_close
condition|)
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|trace_argv_printf
name|void
name|trace_argv_printf
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|int
name|count
parameter_list|,
specifier|const
name|char
modifier|*
name|format
parameter_list|,
modifier|...
parameter_list|)
block|{
name|char
modifier|*
name|argv_str
decl_stmt|,
modifier|*
name|format_str
decl_stmt|,
modifier|*
name|trace_str
decl_stmt|;
name|size_t
name|argv_len
decl_stmt|,
name|format_len
decl_stmt|,
name|trace_len
decl_stmt|;
name|va_list
name|rest
decl_stmt|;
name|int
name|need_close
init|=
literal|0
decl_stmt|;
name|int
name|fd
init|=
name|get_trace_fd
argument_list|(
operator|&
name|need_close
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|fd
condition|)
return|return;
comment|/* Get the argv string. */
name|argv_str
operator|=
name|sq_quote_argv
argument_list|(
name|argv
argument_list|,
name|count
argument_list|)
expr_stmt|;
name|argv_len
operator|=
name|strlen
argument_list|(
name|argv_str
argument_list|)
expr_stmt|;
comment|/* Get the formated string. */
name|va_start
argument_list|(
name|rest
argument_list|,
name|format
argument_list|)
expr_stmt|;
name|nfvasprintf
argument_list|(
operator|&
name|format_str
argument_list|,
name|format
argument_list|,
name|rest
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|rest
argument_list|)
expr_stmt|;
comment|/* Allocate buffer for trace string. */
name|format_len
operator|=
name|strlen
argument_list|(
name|format_str
argument_list|)
expr_stmt|;
name|trace_len
operator|=
name|argv_len
operator|+
name|format_len
operator|+
literal|1
expr_stmt|;
comment|/* + 1 for \n */
name|trace_str
operator|=
name|xmalloc
argument_list|(
name|trace_len
operator|+
literal|1
argument_list|)
expr_stmt|;
comment|/* Copy everything into the trace string. */
name|strncpy
argument_list|(
name|trace_str
argument_list|,
name|format_str
argument_list|,
name|format_len
argument_list|)
expr_stmt|;
name|strncpy
argument_list|(
name|trace_str
operator|+
name|format_len
argument_list|,
name|argv_str
argument_list|,
name|argv_len
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|trace_str
operator|+
name|trace_len
operator|-
literal|1
argument_list|,
literal|"\n"
argument_list|)
expr_stmt|;
name|write_or_whine_pipe
argument_list|(
name|fd
argument_list|,
name|trace_str
argument_list|,
name|trace_len
argument_list|,
name|err_msg
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|argv_str
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|format_str
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|trace_str
argument_list|)
expr_stmt|;
if|if
condition|(
name|need_close
condition|)
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
