begin_unit
begin_comment
comment|/*  * This merges the file listing in the directory cache index  * with the actual working directory list, and shows different  * combinations of the two.  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
begin_include
include|#
directive|include
file|<dirent.h>
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|show_deleted
specifier|static
name|int
name|show_deleted
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_cached
specifier|static
name|int
name|show_cached
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_others
specifier|static
name|int
name|show_others
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_ignored
specifier|static
name|int
name|show_ignored
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_stage
specifier|static
name|int
name|show_stage
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_unmerged
specifier|static
name|int
name|show_unmerged
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|line_terminator
specifier|static
name|int
name|line_terminator
init|=
literal|'\n'
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|dir
specifier|static
specifier|const
name|char
modifier|*
modifier|*
name|dir
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|nr_dir
specifier|static
name|int
name|nr_dir
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|dir_alloc
specifier|static
name|int
name|dir_alloc
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|add_name
specifier|static
name|void
name|add_name
parameter_list|(
specifier|const
name|char
modifier|*
name|pathname
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|char
modifier|*
name|name
decl_stmt|;
if|if
condition|(
name|cache_name_pos
argument_list|(
name|pathname
argument_list|,
name|len
argument_list|)
operator|>=
literal|0
condition|)
return|return;
if|if
condition|(
name|nr_dir
operator|==
name|dir_alloc
condition|)
block|{
name|dir_alloc
operator|=
name|alloc_nr
argument_list|(
name|dir_alloc
argument_list|)
expr_stmt|;
name|dir
operator|=
name|realloc
argument_list|(
name|dir
argument_list|,
name|dir_alloc
operator|*
sizeof|sizeof
argument_list|(
name|char
operator|*
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|name
operator|=
name|malloc
argument_list|(
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|name
argument_list|,
name|pathname
argument_list|,
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
name|dir
index|[
name|nr_dir
operator|++
index|]
operator|=
name|name
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Read a directory tree. We currently ignore anything but  * directories and regular files. That's because git doesn't  * handle them at all yet. Maybe that will change some day.  *  * Also, we currently ignore all names starting with a dot.  * That likely will not change.  */
end_comment
begin_function
DECL|function|read_directory
specifier|static
name|void
name|read_directory
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|)
block|{
name|DIR
modifier|*
name|dir
init|=
name|opendir
argument_list|(
name|path
argument_list|)
decl_stmt|;
if|if
condition|(
name|dir
condition|)
block|{
name|struct
name|dirent
modifier|*
name|de
decl_stmt|;
name|char
name|fullname
index|[
name|MAXPATHLEN
operator|+
literal|1
index|]
decl_stmt|;
name|memcpy
argument_list|(
name|fullname
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|de
operator|=
name|readdir
argument_list|(
name|dir
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|int
name|len
decl_stmt|;
if|if
condition|(
name|de
operator|->
name|d_name
index|[
literal|0
index|]
operator|==
literal|'.'
condition|)
continue|continue;
name|len
operator|=
name|strlen
argument_list|(
name|de
operator|->
name|d_name
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|fullname
operator|+
name|baselen
argument_list|,
name|de
operator|->
name|d_name
argument_list|,
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|de
operator|->
name|d_type
condition|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
default|default:
continue|continue;
case|case
name|DT_UNKNOWN
case|:
if|if
condition|(
name|lstat
argument_list|(
name|fullname
argument_list|,
operator|&
name|st
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
break|break;
if|if
condition|(
operator|!
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
continue|continue;
comment|/* fallthrough */
case|case
name|DT_DIR
case|:
name|memcpy
argument_list|(
name|fullname
operator|+
name|baselen
operator|+
name|len
argument_list|,
literal|"/"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|read_directory
argument_list|(
name|fullname
argument_list|,
name|fullname
argument_list|,
name|baselen
operator|+
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
continue|continue;
case|case
name|DT_REG
case|:
break|break;
block|}
name|add_name
argument_list|(
name|fullname
argument_list|,
name|baselen
operator|+
name|len
argument_list|)
expr_stmt|;
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|cmp_name
specifier|static
name|int
name|cmp_name
parameter_list|(
specifier|const
name|void
modifier|*
name|p1
parameter_list|,
specifier|const
name|void
modifier|*
name|p2
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|n1
init|=
operator|*
operator|(
specifier|const
name|char
operator|*
operator|*
operator|)
name|p1
decl_stmt|;
specifier|const
name|char
modifier|*
name|n2
init|=
operator|*
operator|(
specifier|const
name|char
operator|*
operator|*
operator|)
name|p2
decl_stmt|;
name|int
name|l1
init|=
name|strlen
argument_list|(
name|n1
argument_list|)
decl_stmt|,
name|l2
init|=
name|strlen
argument_list|(
name|n2
argument_list|)
decl_stmt|;
return|return
name|cache_name_compare
argument_list|(
name|n1
argument_list|,
name|l1
argument_list|,
name|n2
argument_list|,
name|l2
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|show_files
specifier|static
name|void
name|show_files
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
comment|/* For cached/deleted files we don't need to even do the readdir */
if|if
condition|(
name|show_others
operator||
name|show_ignored
condition|)
block|{
name|read_directory
argument_list|(
literal|"."
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|qsort
argument_list|(
name|dir
argument_list|,
name|nr_dir
argument_list|,
sizeof|sizeof
argument_list|(
name|char
operator|*
argument_list|)
argument_list|,
name|cmp_name
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|show_others
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_dir
condition|;
name|i
operator|++
control|)
name|printf
argument_list|(
literal|"%s%c"
argument_list|,
name|dir
index|[
name|i
index|]
argument_list|,
name|line_terminator
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|show_cached
operator||
name|show_stage
condition|)
block|{
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
name|show_unmerged
operator|&&
operator|!
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
continue|continue;
if|if
condition|(
operator|!
name|show_stage
condition|)
name|printf
argument_list|(
literal|"%s%c"
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|line_terminator
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
comment|/* "%06o %s %d %10d %s%c", */
literal|"%06o %s %d %s%c"
argument_list|,
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
argument_list|,
name|sha1_to_hex
argument_list|(
name|ce
operator|->
name|sha1
argument_list|)
argument_list|,
name|ce_stage
argument_list|(
name|ce
argument_list|)
argument_list|,
comment|/* ntohl(ce->ce_size), */
name|ce
operator|->
name|name
argument_list|,
name|line_terminator
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|show_deleted
condition|)
block|{
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
continue|continue;
name|printf
argument_list|(
literal|"%s%c"
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|line_terminator
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|show_ignored
condition|)
block|{
comment|/* We don't have any "ignore" list yet */
block|}
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
decl_stmt|;
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
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-z"
argument_list|)
condition|)
block|{
name|line_terminator
operator|=
literal|0
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
literal|"--cached"
argument_list|)
condition|)
block|{
name|show_cached
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
literal|"--deleted"
argument_list|)
condition|)
block|{
name|show_deleted
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
literal|"--others"
argument_list|)
condition|)
block|{
name|show_others
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
literal|"--ignored"
argument_list|)
condition|)
block|{
name|show_ignored
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
literal|"--stage"
argument_list|)
condition|)
block|{
name|show_stage
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
literal|"--unmerged"
argument_list|)
condition|)
block|{
comment|// There's no point in showing unmerged unless you also show the stage information
name|show_stage
operator|=
literal|1
expr_stmt|;
name|show_unmerged
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
literal|"show-files [-z] (--[cached|deleted|others|ignored|stage])*"
argument_list|)
expr_stmt|;
block|}
comment|/* With no flags, we default to showing the cached files */
if|if
condition|(
operator|!
operator|(
name|show_stage
operator||
name|show_deleted
operator||
name|show_others
operator||
name|show_ignored
operator||
name|show_unmerged
operator|)
condition|)
name|show_cached
operator|=
literal|1
expr_stmt|;
name|read_cache
argument_list|()
expr_stmt|;
name|show_files
argument_list|()
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
