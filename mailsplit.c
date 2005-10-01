begin_unit
begin_comment
comment|/*  * Totally braindamaged mbox splitter program.  *  * It just splits a mbox into a list of files: "0001" "0002" ..  * so you can process them further from there.  */
end_comment
begin_include
include|#
directive|include
file|<unistd.h>
end_include
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
begin_include
include|#
directive|include
file|<fcntl.h>
end_include
begin_include
include|#
directive|include
file|<sys/types.h>
end_include
begin_include
include|#
directive|include
file|<sys/stat.h>
end_include
begin_include
include|#
directive|include
file|<sys/mman.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|<ctype.h>
end_include
begin_include
include|#
directive|include
file|<assert.h>
end_include
begin_function
DECL|function|usage
specifier|static
name|int
name|usage
parameter_list|(
name|void
parameter_list|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"mailsplit<mbox><directory>\n"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|linelen
specifier|static
name|int
name|linelen
parameter_list|(
specifier|const
name|char
modifier|*
name|map
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|int
name|len
init|=
literal|0
decl_stmt|,
name|c
decl_stmt|;
do|do
block|{
name|c
operator|=
operator|*
name|map
expr_stmt|;
name|map
operator|++
expr_stmt|;
name|size
operator|--
expr_stmt|;
name|len
operator|++
expr_stmt|;
block|}
do|while
condition|(
name|size
operator|&&
name|c
operator|!=
literal|'\n'
condition|)
do|;
return|return
name|len
return|;
block|}
end_function
begin_function
DECL|function|is_from_line
specifier|static
name|int
name|is_from_line
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|int
name|len
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|colon
decl_stmt|;
if|if
condition|(
name|len
operator|<
literal|20
operator|||
name|memcmp
argument_list|(
literal|"From "
argument_list|,
name|line
argument_list|,
literal|5
argument_list|)
condition|)
return|return
literal|0
return|;
name|colon
operator|=
name|line
operator|+
name|len
operator|-
literal|2
expr_stmt|;
name|line
operator|+=
literal|5
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
if|if
condition|(
name|colon
operator|<
name|line
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|*
operator|--
name|colon
operator|==
literal|':'
condition|)
break|break;
block|}
if|if
condition|(
operator|!
name|isdigit
argument_list|(
name|colon
index|[
operator|-
literal|4
index|]
argument_list|)
operator|||
operator|!
name|isdigit
argument_list|(
name|colon
index|[
operator|-
literal|2
index|]
argument_list|)
operator|||
operator|!
name|isdigit
argument_list|(
name|colon
index|[
operator|-
literal|1
index|]
argument_list|)
operator|||
operator|!
name|isdigit
argument_list|(
name|colon
index|[
literal|1
index|]
argument_list|)
operator|||
operator|!
name|isdigit
argument_list|(
name|colon
index|[
literal|2
index|]
argument_list|)
condition|)
return|return
literal|0
return|;
comment|/* year */
if|if
condition|(
name|strtol
argument_list|(
name|colon
operator|+
literal|3
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
operator|<=
literal|90
condition|)
return|return
literal|0
return|;
comment|/* Ok, close enough */
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|parse_email
specifier|static
name|int
name|parse_email
parameter_list|(
specifier|const
name|void
modifier|*
name|map
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|unsigned
name|long
name|offset
decl_stmt|;
if|if
condition|(
name|size
operator|<
literal|6
operator|||
name|memcmp
argument_list|(
literal|"From "
argument_list|,
name|map
argument_list|,
literal|5
argument_list|)
condition|)
goto|goto
name|corrupt
goto|;
comment|/* Make sure we don't trigger on this first line */
name|map
operator|++
expr_stmt|;
name|size
operator|--
expr_stmt|;
name|offset
operator|=
literal|1
expr_stmt|;
comment|/* 	 * Search for a line beginning with "From ", and  	 * having something that looks like a date format. 	 */
do|do
block|{
name|int
name|len
init|=
name|linelen
argument_list|(
name|map
argument_list|,
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
name|is_from_line
argument_list|(
name|map
argument_list|,
name|len
argument_list|)
condition|)
return|return
name|offset
return|;
name|map
operator|+=
name|len
expr_stmt|;
name|size
operator|-=
name|len
expr_stmt|;
name|offset
operator|+=
name|len
expr_stmt|;
block|}
do|while
condition|(
name|size
condition|)
do|;
return|return
name|offset
return|;
name|corrupt
label|:
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"corrupt mailbox\n"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|fd
decl_stmt|,
name|nr
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|void
modifier|*
name|map
decl_stmt|;
if|if
condition|(
name|argc
operator|!=
literal|3
condition|)
name|usage
argument_list|()
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
name|perror
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|chdir
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|)
operator|<
literal|0
condition|)
name|usage
argument_list|()
expr_stmt|;
if|if
condition|(
name|fstat
argument_list|(
name|fd
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
block|{
name|perror
argument_list|(
literal|"stat"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
name|size
operator|=
name|st
operator|.
name|st_size
expr_stmt|;
name|map
operator|=
name|mmap
argument_list|(
name|NULL
argument_list|,
name|size
argument_list|,
name|PROT_READ
argument_list|,
name|MAP_PRIVATE
argument_list|,
name|fd
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|map
operator|==
name|MAP_FAILED
condition|)
block|{
name|perror
argument_list|(
literal|"mmap"
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|nr
operator|=
literal|0
expr_stmt|;
do|do
block|{
name|char
name|name
index|[
literal|10
index|]
decl_stmt|;
name|unsigned
name|long
name|len
init|=
name|parse_email
argument_list|(
name|map
argument_list|,
name|size
argument_list|)
decl_stmt|;
name|assert
argument_list|(
name|len
operator|<=
name|size
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|name
argument_list|,
literal|"%04d"
argument_list|,
operator|++
name|nr
argument_list|)
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|name
argument_list|,
name|O_WRONLY
operator||
name|O_CREAT
operator||
name|O_EXCL
argument_list|,
literal|0666
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
name|perror
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|write
argument_list|(
name|fd
argument_list|,
name|map
argument_list|,
name|len
argument_list|)
operator|!=
name|len
condition|)
block|{
name|perror
argument_list|(
literal|"write"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|map
operator|+=
name|len
expr_stmt|;
name|size
operator|-=
name|len
expr_stmt|;
block|}
do|while
condition|(
name|size
operator|>
literal|0
condition|)
do|;
return|return
literal|0
return|;
block|}
end_function
end_unit
