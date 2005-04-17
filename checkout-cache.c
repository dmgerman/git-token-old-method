begin_unit
begin_comment
comment|/*  * Check-out files from the "current cache directory"  *  * Copyright (C) 2005 Linus Torvalds  *  * Careful: order of argument flags does matter. For example,  *  *	checkout-cache -a -f file.c  *  * Will first check out all files listed in the cache (but not  * overwrite any old ones), and then force-checkout "file.c" a  * second time (ie that one _will_ overwrite any old contents  * with the same filename).  *  * Also, just doing "checkout-cache" does nothing. You probably  * meant "checkout-cache -a". And if you want to force it, you  * want "checkout-cache -f -a".  *  * Intuitiveness is not the goal here. Repeatability is. The  * reason for the "no arguments means no work" thing is that  * from scripts you are supposed to be able to do things like  *  *	find . -name '*.h' -print0 | xargs -0 checkout-cache -f --  *  * which will force all existing *.h files to be replaced with  * their cached copies. If an empty command line implied "all",  * then this would force-refresh everything in the cache, which  * was not the point.  *  * Oh, and the "--" is just a good idea when you know the rest  * will be filenames. Just so that you wouldn't have a filename  * of "-a" causing problems (not possible in the above example,  * but get used to it in scripting!).  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|force
DECL|variable|quiet
specifier|static
name|int
name|force
init|=
literal|0
decl_stmt|,
name|quiet
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|create_directories
specifier|static
name|void
name|create_directories
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|path
argument_list|)
decl_stmt|;
name|char
modifier|*
name|buf
init|=
name|malloc
argument_list|(
name|len
operator|+
literal|1
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|slash
init|=
name|path
decl_stmt|;
while|while
condition|(
operator|(
name|slash
operator|=
name|strchr
argument_list|(
name|slash
operator|+
literal|1
argument_list|,
literal|'/'
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|len
operator|=
name|slash
operator|-
name|path
expr_stmt|;
name|memcpy
argument_list|(
name|buf
argument_list|,
name|path
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|buf
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
name|mkdir
argument_list|(
name|buf
argument_list|,
literal|0755
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|create_file
specifier|static
name|int
name|create_file
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|unsigned
name|int
name|mode
parameter_list|)
block|{
name|int
name|fd
decl_stmt|;
name|mode
operator|=
operator|(
name|mode
operator|&
literal|0100
operator|)
condition|?
literal|0777
else|:
literal|0666
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|path
argument_list|,
name|O_WRONLY
operator||
name|O_TRUNC
operator||
name|O_CREAT
argument_list|,
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|==
name|ENOENT
condition|)
block|{
name|create_directories
argument_list|(
name|path
argument_list|)
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|path
argument_list|,
name|O_WRONLY
operator||
name|O_TRUNC
operator||
name|O_CREAT
argument_list|,
name|mode
argument_list|)
expr_stmt|;
block|}
block|}
return|return
name|fd
return|;
block|}
end_function
begin_function
DECL|function|write_entry
specifier|static
name|int
name|write_entry
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|)
block|{
name|int
name|fd
decl_stmt|;
name|void
modifier|*
name|new
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|long
name|wrote
decl_stmt|;
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|new
operator|=
name|read_sha1_file
argument_list|(
name|ce
operator|->
name|sha1
argument_list|,
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|new
operator|||
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"blob"
argument_list|)
condition|)
block|{
return|return
name|error
argument_list|(
literal|"checkout-cache: unable to read sha1 file of %s (%s)"
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|sha1_to_hex
argument_list|(
name|ce
operator|->
name|sha1
argument_list|)
argument_list|)
return|;
block|}
name|fd
operator|=
name|create_file
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
name|free
argument_list|(
name|new
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"checkout-cache: unable to create %s (%s)"
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
block|}
name|wrote
operator|=
name|write
argument_list|(
name|fd
argument_list|,
name|new
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|new
argument_list|)
expr_stmt|;
if|if
condition|(
name|wrote
operator|!=
name|size
condition|)
return|return
name|error
argument_list|(
literal|"checkout-cache: unable to write %s"
argument_list|,
name|ce
operator|->
name|name
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|checkout_entry
specifier|static
name|int
name|checkout_entry
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
operator|!
name|stat
argument_list|(
name|ce
operator|->
name|name
argument_list|,
operator|&
name|st
argument_list|)
condition|)
block|{
name|unsigned
name|changed
init|=
name|cache_match_stat
argument_list|(
name|ce
argument_list|,
operator|&
name|st
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|changed
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|force
condition|)
block|{
if|if
condition|(
operator|!
name|quiet
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"checkout-cache: %s already exists\n"
argument_list|,
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
comment|/* 		 * We unlink the old file, to get the new one with the 		 * right permissions (including umask, which is nasty 		 * to emulate by hand - much easier to let the system 		 * just do the right thing) 		 */
name|unlink
argument_list|(
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
return|return
name|write_entry
argument_list|(
name|ce
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|checkout_file
specifier|static
name|int
name|checkout_file
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|int
name|pos
init|=
name|cache_name_pos
argument_list|(
name|name
argument_list|,
name|strlen
argument_list|(
name|name
argument_list|)
argument_list|)
decl_stmt|;
if|if
condition|(
name|pos
operator|<
literal|0
condition|)
block|{
if|if
condition|(
operator|!
name|quiet
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"checkout-cache: %s is not in the cache\n"
argument_list|,
name|name
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
return|return
name|checkout_entry
argument_list|(
name|active_cache
index|[
name|pos
index|]
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|checkout_all
specifier|static
name|int
name|checkout_all
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|checkout_entry
argument_list|(
name|ce
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
block|}
return|return
literal|0
return|;
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
name|i
decl_stmt|,
name|force_filename
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|read_cache
argument_list|()
operator|<
literal|0
condition|)
block|{
name|die
argument_list|(
literal|"invalid cache"
argument_list|)
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|force_filename
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-a"
argument_list|)
condition|)
block|{
name|checkout_all
argument_list|()
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|force_filename
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-f"
argument_list|)
condition|)
block|{
name|force
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-q"
argument_list|)
condition|)
block|{
name|quiet
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
block|}
name|checkout_file
argument_list|(
name|arg
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
