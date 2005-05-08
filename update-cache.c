begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
begin_include
include|#
directive|include
file|<signal.h>
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_comment
comment|/*  * Default to not allowing changes to the list of files. The  * tool doesn't actually care, but this makes it harder to add  * files to the revision control by mistake by doing something  * like "update-cache *" and suddenly having all the object  * files be revision controlled.  */
end_comment
begin_decl_stmt
DECL|variable|allow_add
DECL|variable|allow_remove
DECL|variable|allow_replace
DECL|variable|not_new
specifier|static
name|int
name|allow_add
init|=
literal|0
decl_stmt|,
name|allow_remove
init|=
literal|0
decl_stmt|,
name|allow_replace
init|=
literal|0
decl_stmt|,
name|not_new
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* Three functions to allow overloaded pointer return; see linux/err.h */
end_comment
begin_function
DECL|function|ERR_PTR
specifier|static
specifier|inline
name|void
modifier|*
name|ERR_PTR
parameter_list|(
name|long
name|error
parameter_list|)
block|{
return|return
operator|(
name|void
operator|*
operator|)
name|error
return|;
block|}
end_function
begin_function
DECL|function|PTR_ERR
specifier|static
specifier|inline
name|long
name|PTR_ERR
parameter_list|(
specifier|const
name|void
modifier|*
name|ptr
parameter_list|)
block|{
return|return
operator|(
name|long
operator|)
name|ptr
return|;
block|}
end_function
begin_function
DECL|function|IS_ERR
specifier|static
specifier|inline
name|long
name|IS_ERR
parameter_list|(
specifier|const
name|void
modifier|*
name|ptr
parameter_list|)
block|{
return|return
operator|(
name|unsigned
name|long
operator|)
name|ptr
operator|>
operator|(
name|unsigned
name|long
operator|)
operator|-
literal|1000L
return|;
block|}
end_function
begin_comment
comment|/*  * This only updates the "non-critical" parts of the directory  * cache, ie the parts that aren't tracked by GIT, and only used  * to validate the cache.  */
end_comment
begin_function
DECL|function|fill_stat_cache_info
specifier|static
name|void
name|fill_stat_cache_info
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|struct
name|stat
modifier|*
name|st
parameter_list|)
block|{
name|ce
operator|->
name|ce_ctime
operator|.
name|sec
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_ctime
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_mtime
operator|.
name|sec
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_mtime
argument_list|)
expr_stmt|;
ifdef|#
directive|ifdef
name|NSEC
name|ce
operator|->
name|ce_ctime
operator|.
name|nsec
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_ctim
operator|.
name|tv_nsec
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_mtime
operator|.
name|nsec
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_mtim
operator|.
name|tv_nsec
argument_list|)
expr_stmt|;
endif|#
directive|endif
name|ce
operator|->
name|ce_dev
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_dev
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_ino
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_ino
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_uid
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_uid
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_gid
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_gid
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_size
operator|=
name|htonl
argument_list|(
name|st
operator|->
name|st_size
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_file_to_cache
specifier|static
name|int
name|add_file_to_cache
parameter_list|(
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|size
decl_stmt|,
name|namelen
decl_stmt|,
name|option
decl_stmt|,
name|status
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|char
modifier|*
name|target
decl_stmt|;
name|status
operator|=
name|lstat
argument_list|(
name|path
argument_list|,
operator|&
name|st
argument_list|)
expr_stmt|;
if|if
condition|(
name|status
operator|<
literal|0
operator|||
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
block|{
comment|/* When we used to have "path" and now we want to add 		 * "path/file", we need a way to remove "path" before 		 * being able to add "path/file".  However, 		 * "git-update-cache --remove path" would not work. 		 * --force-remove can be used but this is more user 		 * friendly, especially since we can do the opposite 		 * case just fine without --force-remove. 		 */
if|if
condition|(
name|status
operator|==
literal|0
operator|||
operator|(
name|errno
operator|==
name|ENOENT
operator|||
name|errno
operator|==
name|ENOTDIR
operator|)
condition|)
block|{
if|if
condition|(
name|allow_remove
condition|)
return|return
name|remove_file_from_cache
argument_list|(
name|path
argument_list|)
return|;
block|}
return|return
operator|-
literal|1
return|;
block|}
name|namelen
operator|=
name|strlen
argument_list|(
name|path
argument_list|)
expr_stmt|;
name|size
operator|=
name|cache_entry_size
argument_list|(
name|namelen
argument_list|)
expr_stmt|;
name|ce
operator|=
name|xmalloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|ce
argument_list|,
literal|0
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|path
argument_list|,
name|namelen
argument_list|)
expr_stmt|;
name|fill_stat_cache_info
argument_list|(
name|ce
argument_list|,
operator|&
name|st
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_mode
operator|=
name|create_ce_mode
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_flags
operator|=
name|htons
argument_list|(
name|namelen
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|st
operator|.
name|st_mode
operator|&
name|S_IFMT
condition|)
block|{
case|case
name|S_IFREG
case|:
name|fd
operator|=
name|open
argument_list|(
name|path
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
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|index_fd
argument_list|(
name|ce
operator|->
name|sha1
argument_list|,
name|fd
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
break|break;
case|case
name|S_IFLNK
case|:
name|target
operator|=
name|xmalloc
argument_list|(
name|st
operator|.
name|st_size
operator|+
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|readlink
argument_list|(
name|path
argument_list|,
name|target
argument_list|,
name|st
operator|.
name|st_size
operator|+
literal|1
argument_list|)
operator|!=
name|st
operator|.
name|st_size
condition|)
block|{
name|free
argument_list|(
name|target
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|write_sha1_file
argument_list|(
name|target
argument_list|,
name|st
operator|.
name|st_size
argument_list|,
literal|"blob"
argument_list|,
name|ce
operator|->
name|sha1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|free
argument_list|(
name|target
argument_list|)
expr_stmt|;
break|break;
default|default:
return|return
operator|-
literal|1
return|;
block|}
name|option
operator|=
name|allow_add
condition|?
name|ADD_CACHE_OK_TO_ADD
else|:
literal|0
expr_stmt|;
name|option
operator||=
name|allow_replace
condition|?
name|ADD_CACHE_OK_TO_REPLACE
else|:
literal|0
expr_stmt|;
return|return
name|add_cache_entry
argument_list|(
name|ce
argument_list|,
name|option
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|match_data
specifier|static
name|int
name|match_data
parameter_list|(
name|int
name|fd
parameter_list|,
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
while|while
condition|(
name|size
condition|)
block|{
name|char
name|compare
index|[
literal|1024
index|]
decl_stmt|;
name|int
name|ret
init|=
name|read
argument_list|(
name|fd
argument_list|,
name|compare
argument_list|,
sizeof|sizeof
argument_list|(
name|compare
argument_list|)
argument_list|)
decl_stmt|;
if|if
condition|(
name|ret
operator|<=
literal|0
operator|||
name|ret
operator|>
name|size
operator|||
name|memcmp
argument_list|(
name|buffer
argument_list|,
name|compare
argument_list|,
name|ret
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|size
operator|-=
name|ret
expr_stmt|;
name|buffer
operator|+=
name|ret
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|compare_data
specifier|static
name|int
name|compare_data
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|unsigned
name|long
name|expected_size
parameter_list|)
block|{
name|int
name|match
init|=
operator|-
literal|1
decl_stmt|;
name|int
name|fd
init|=
name|open
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|O_RDONLY
argument_list|)
decl_stmt|;
if|if
condition|(
name|fd
operator|>=
literal|0
condition|)
block|{
name|void
modifier|*
name|buffer
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|char
name|type
index|[
literal|10
index|]
decl_stmt|;
name|buffer
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
name|buffer
condition|)
block|{
if|if
condition|(
name|size
operator|==
name|expected_size
operator|&&
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"blob"
argument_list|)
condition|)
name|match
operator|=
name|match_data
argument_list|(
name|fd
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
return|return
name|match
return|;
block|}
end_function
begin_function
DECL|function|compare_link
specifier|static
name|int
name|compare_link
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|unsigned
name|long
name|expected_size
parameter_list|)
block|{
name|int
name|match
init|=
operator|-
literal|1
decl_stmt|;
name|char
modifier|*
name|target
decl_stmt|;
name|void
modifier|*
name|buffer
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|char
name|type
index|[
literal|10
index|]
decl_stmt|;
name|int
name|len
decl_stmt|;
name|target
operator|=
name|xmalloc
argument_list|(
name|expected_size
argument_list|)
expr_stmt|;
name|len
operator|=
name|readlink
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|target
argument_list|,
name|expected_size
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|!=
name|expected_size
condition|)
block|{
name|free
argument_list|(
name|target
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|buffer
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
name|buffer
condition|)
block|{
name|free
argument_list|(
name|target
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|size
operator|==
name|expected_size
condition|)
name|match
operator|=
name|memcmp
argument_list|(
name|buffer
argument_list|,
name|target
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|target
argument_list|)
expr_stmt|;
return|return
name|match
return|;
block|}
end_function
begin_comment
comment|/*  * "refresh" does not calculate a new sha1 file or bring the  * cache up-to-date for mode/content changes. But what it  * _does_ do is to "re-match" the stat information of a file  * with the cache, so that you can refresh the cache for a  * file that hasn't been changed but where the stat entry is  * out of date.  *  * For example, you'd want to do this after doing a "read-tree",  * to link up the stat cache details with the proper files.  */
end_comment
begin_function
DECL|function|refresh_entry
specifier|static
name|struct
name|cache_entry
modifier|*
name|refresh_entry
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
name|struct
name|cache_entry
modifier|*
name|updated
decl_stmt|;
name|int
name|changed
decl_stmt|,
name|size
decl_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|ce
operator|->
name|name
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
return|return
name|ERR_PTR
argument_list|(
operator|-
name|errno
argument_list|)
return|;
name|changed
operator|=
name|cache_match_stat
argument_list|(
name|ce
argument_list|,
operator|&
name|st
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|changed
condition|)
return|return
name|ce
return|;
comment|/* 	 * If the mode or type has changed, there's no point in trying 	 * to refresh the entry - it's not going to match 	 */
if|if
condition|(
name|changed
operator|&
operator|(
name|MODE_CHANGED
operator||
name|TYPE_CHANGED
operator|)
condition|)
return|return
name|ERR_PTR
argument_list|(
operator|-
name|EINVAL
argument_list|)
return|;
switch|switch
condition|(
name|st
operator|.
name|st_mode
operator|&
name|S_IFMT
condition|)
block|{
case|case
name|S_IFREG
case|:
if|if
condition|(
name|compare_data
argument_list|(
name|ce
argument_list|,
name|st
operator|.
name|st_size
argument_list|)
condition|)
return|return
name|ERR_PTR
argument_list|(
operator|-
name|EINVAL
argument_list|)
return|;
break|break;
case|case
name|S_IFLNK
case|:
if|if
condition|(
name|compare_link
argument_list|(
name|ce
argument_list|,
name|st
operator|.
name|st_size
argument_list|)
condition|)
return|return
name|ERR_PTR
argument_list|(
operator|-
name|EINVAL
argument_list|)
return|;
break|break;
default|default:
return|return
name|ERR_PTR
argument_list|(
operator|-
name|EINVAL
argument_list|)
return|;
block|}
name|size
operator|=
name|ce_size
argument_list|(
name|ce
argument_list|)
expr_stmt|;
name|updated
operator|=
name|xmalloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|updated
argument_list|,
name|ce
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|fill_stat_cache_info
argument_list|(
name|updated
argument_list|,
operator|&
name|st
argument_list|)
expr_stmt|;
return|return
name|updated
return|;
block|}
end_function
begin_function
DECL|function|refresh_cache
specifier|static
name|int
name|refresh_cache
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|has_errors
init|=
literal|0
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
decl_stmt|,
modifier|*
name|new
decl_stmt|;
name|ce
operator|=
name|active_cache
index|[
name|i
index|]
expr_stmt|;
if|if
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
block|{
name|printf
argument_list|(
literal|"%s: needs merge\n"
argument_list|,
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
name|has_errors
operator|=
literal|1
expr_stmt|;
while|while
condition|(
operator|(
name|i
operator|<
name|active_nr
operator|)
operator|&&
operator|!
name|strcmp
argument_list|(
name|active_cache
index|[
name|i
index|]
operator|->
name|name
argument_list|,
name|ce
operator|->
name|name
argument_list|)
condition|)
name|i
operator|++
expr_stmt|;
name|i
operator|--
expr_stmt|;
continue|continue;
block|}
name|new
operator|=
name|refresh_entry
argument_list|(
name|ce
argument_list|)
expr_stmt|;
if|if
condition|(
name|IS_ERR
argument_list|(
name|new
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
operator|(
name|not_new
operator|&&
name|PTR_ERR
argument_list|(
name|new
argument_list|)
operator|==
operator|-
name|ENOENT
operator|)
condition|)
block|{
name|printf
argument_list|(
literal|"%s: needs update\n"
argument_list|,
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
name|has_errors
operator|=
literal|1
expr_stmt|;
block|}
continue|continue;
block|}
name|active_cache_changed
operator|=
literal|1
expr_stmt|;
name|active_cache
index|[
name|i
index|]
operator|=
name|new
expr_stmt|;
block|}
return|return
name|has_errors
return|;
block|}
end_function
begin_comment
comment|/*  * We fundamentally don't like some paths: we don't want  * dot or dot-dot anywhere, and in fact, we don't even want  * any other dot-files (.git or anything else). They  * are hidden, for chist sake.  *  * Also, we don't want double slashes or slashes at the  * end that can make pathnames ambiguous.  */
end_comment
begin_function
DECL|function|verify_path
specifier|static
name|int
name|verify_path
parameter_list|(
name|char
modifier|*
name|path
parameter_list|)
block|{
name|char
name|c
decl_stmt|;
goto|goto
name|inside
goto|;
for|for
control|(
init|;
condition|;
control|)
block|{
if|if
condition|(
operator|!
name|c
condition|)
return|return
literal|1
return|;
if|if
condition|(
name|c
operator|==
literal|'/'
condition|)
block|{
name|inside
label|:
name|c
operator|=
operator|*
name|path
operator|++
expr_stmt|;
if|if
condition|(
name|c
operator|!=
literal|'/'
operator|&&
name|c
operator|!=
literal|'.'
operator|&&
name|c
operator|!=
literal|'\0'
condition|)
continue|continue;
return|return
literal|0
return|;
block|}
name|c
operator|=
operator|*
name|path
operator|++
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|add_cacheinfo
specifier|static
name|int
name|add_cacheinfo
parameter_list|(
name|char
modifier|*
name|arg1
parameter_list|,
name|char
modifier|*
name|arg2
parameter_list|,
name|char
modifier|*
name|arg3
parameter_list|)
block|{
name|int
name|size
decl_stmt|,
name|len
decl_stmt|,
name|option
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
if|if
condition|(
name|sscanf
argument_list|(
name|arg1
argument_list|,
literal|"%o"
argument_list|,
operator|&
name|mode
argument_list|)
operator|!=
literal|1
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|arg2
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|verify_path
argument_list|(
name|arg3
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|len
operator|=
name|strlen
argument_list|(
name|arg3
argument_list|)
expr_stmt|;
name|size
operator|=
name|cache_entry_size
argument_list|(
name|len
argument_list|)
expr_stmt|;
name|ce
operator|=
name|xmalloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|ce
argument_list|,
literal|0
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ce
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|arg3
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_flags
operator|=
name|htons
argument_list|(
name|len
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_mode
operator|=
name|create_ce_mode
argument_list|(
name|mode
argument_list|)
expr_stmt|;
name|option
operator|=
name|allow_add
condition|?
name|ADD_CACHE_OK_TO_ADD
else|:
literal|0
expr_stmt|;
name|option
operator||=
name|allow_replace
condition|?
name|ADD_CACHE_OK_TO_REPLACE
else|:
literal|0
expr_stmt|;
return|return
name|add_cache_entry
argument_list|(
name|ce
argument_list|,
name|option
argument_list|)
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|lockfile_name
specifier|static
specifier|const
name|char
modifier|*
name|lockfile_name
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|remove_lock_file
specifier|static
name|void
name|remove_lock_file
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|lockfile_name
condition|)
name|unlink
argument_list|(
name|lockfile_name
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|remove_lock_file_on_signal
specifier|static
name|void
name|remove_lock_file_on_signal
parameter_list|(
name|int
name|signo
parameter_list|)
block|{
name|remove_lock_file
argument_list|()
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
name|i
decl_stmt|,
name|newfd
decl_stmt|,
name|entries
decl_stmt|,
name|has_errors
init|=
literal|0
decl_stmt|;
name|int
name|allow_options
init|=
literal|1
decl_stmt|;
specifier|static
name|char
name|lockfile
index|[
name|MAXPATHLEN
operator|+
literal|1
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|indexfile
init|=
name|get_index_file
argument_list|()
decl_stmt|;
name|snprintf
argument_list|(
name|lockfile
argument_list|,
sizeof|sizeof
argument_list|(
name|lockfile
argument_list|)
argument_list|,
literal|"%s.lock"
argument_list|,
name|indexfile
argument_list|)
expr_stmt|;
name|newfd
operator|=
name|open
argument_list|(
name|lockfile
argument_list|,
name|O_RDWR
operator||
name|O_CREAT
operator||
name|O_EXCL
argument_list|,
literal|0600
argument_list|)
expr_stmt|;
if|if
condition|(
name|newfd
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to create new cachefile"
argument_list|)
expr_stmt|;
name|signal
argument_list|(
name|SIGINT
argument_list|,
name|remove_lock_file_on_signal
argument_list|)
expr_stmt|;
name|atexit
argument_list|(
name|remove_lock_file
argument_list|)
expr_stmt|;
name|lockfile_name
operator|=
name|lockfile
expr_stmt|;
name|entries
operator|=
name|read_cache
argument_list|()
expr_stmt|;
if|if
condition|(
name|entries
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"cache corrupted"
argument_list|)
expr_stmt|;
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
name|path
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|allow_options
operator|&&
operator|*
name|path
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|allow_options
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
name|path
argument_list|,
literal|"--add"
argument_list|)
condition|)
block|{
name|allow_add
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
name|path
argument_list|,
literal|"--replace"
argument_list|)
condition|)
block|{
name|allow_replace
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
name|path
argument_list|,
literal|"--remove"
argument_list|)
condition|)
block|{
name|allow_remove
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
name|path
argument_list|,
literal|"--refresh"
argument_list|)
condition|)
block|{
name|has_errors
operator||=
name|refresh_cache
argument_list|()
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--cacheinfo"
argument_list|)
condition|)
block|{
if|if
condition|(
name|i
operator|+
literal|3
operator|>=
name|argc
operator|||
name|add_cacheinfo
argument_list|(
name|argv
index|[
name|i
operator|+
literal|1
index|]
argument_list|,
name|argv
index|[
name|i
operator|+
literal|2
index|]
argument_list|,
name|argv
index|[
name|i
operator|+
literal|3
index|]
argument_list|)
condition|)
name|die
argument_list|(
literal|"update-cache: --cacheinfo<mode><sha1><path>"
argument_list|)
expr_stmt|;
name|i
operator|+=
literal|3
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--force-remove"
argument_list|)
condition|)
block|{
if|if
condition|(
name|argc
operator|<=
name|i
operator|+
literal|1
condition|)
name|die
argument_list|(
literal|"update-cache: --force-remove<path>"
argument_list|)
expr_stmt|;
if|if
condition|(
name|remove_file_from_cache
argument_list|(
name|argv
index|[
name|i
operator|+
literal|1
index|]
argument_list|)
condition|)
name|die
argument_list|(
literal|"update-cache: --force-remove cannot remove %s"
argument_list|,
name|argv
index|[
name|i
operator|+
literal|1
index|]
argument_list|)
expr_stmt|;
name|i
operator|++
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"--ignore-missing"
argument_list|)
condition|)
block|{
name|not_new
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|die
argument_list|(
literal|"unknown option %s"
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|verify_path
argument_list|(
name|path
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Ignoring path %s\n"
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|add_file_to_cache
argument_list|(
name|path
argument_list|)
condition|)
name|die
argument_list|(
literal|"Unable to add %s to database"
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|write_cache
argument_list|(
name|newfd
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|)
operator|||
name|rename
argument_list|(
name|lockfile
argument_list|,
name|indexfile
argument_list|)
condition|)
name|die
argument_list|(
literal|"Unable to write new cachefile"
argument_list|)
expr_stmt|;
name|lockfile_name
operator|=
name|NULL
expr_stmt|;
return|return
name|has_errors
condition|?
literal|1
else|:
literal|0
return|;
block|}
end_function
end_unit
