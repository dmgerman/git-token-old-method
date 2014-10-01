begin_unit
begin_comment
comment|/*  * Copyright (c) 2005, Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"sigchain.h"
end_include
begin_comment
comment|/*  * File write-locks as used by Git.  *  * For an overview of how to use the lockfile API, please see  *  *     Documentation/technical/api-lockfile.txt  *  * This module keeps track of all locked files in lock_file_list for  * use at cleanup. This list and the lock_file objects that comprise  * it must be kept in self-consistent states at all time, because the  * program can be interrupted any time by a signal, in which case the  * signal handler will walk through the list attempting to clean up  * any open lock files.  *  * A lockfile is owned by the process that created it. The lock_file  * object has an "owner" field that records its owner. This field is  * used to prevent a forked process from closing a lockfile created by  * its parent.  *  * A lock_file object can be in several states:  *  * - Uninitialized.  In this state the object's on_list field must be  *   zero but the rest of its contents need not be initialized.  As  *   soon as the object is used in any way, it is irrevocably  *   registered in the lock_file_list, and on_list is set.  *  * - Locked, lockfile open (after hold_lock_file_for_update(),  *   hold_lock_file_for_append(), or reopen_lock_file()). In this  *   state, the lockfile exists, filename holds the filename of the  *   lockfile, fd holds a file descriptor open for writing to the  *   lockfile, and owner holds the PID of the process that locked the  *   file.  *  * - Locked, lockfile closed (after close_lock_file()).  Same as the  *   previous state, except that the lockfile is closed and fd is -1.  *  * - Unlocked (after commit_lock_file(), rollback_lock_file(), or a  *   failed attempt to lock).  In this state, filename[0] == '\0' and  *   fd is -1.  The object is left registered in the lock_file_list,  *   and on_list is set.  */
end_comment
begin_decl_stmt
DECL|variable|lock_file_list
specifier|static
name|struct
name|lock_file
modifier|*
name|lock_file_list
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
name|pid_t
name|me
init|=
name|getpid
argument_list|()
decl_stmt|;
while|while
condition|(
name|lock_file_list
condition|)
block|{
if|if
condition|(
name|lock_file_list
operator|->
name|owner
operator|==
name|me
operator|&&
name|lock_file_list
operator|->
name|filename
index|[
literal|0
index|]
condition|)
block|{
if|if
condition|(
name|lock_file_list
operator|->
name|fd
operator|>=
literal|0
condition|)
name|close
argument_list|(
name|lock_file_list
operator|->
name|fd
argument_list|)
expr_stmt|;
name|unlink_or_warn
argument_list|(
name|lock_file_list
operator|->
name|filename
argument_list|)
expr_stmt|;
block|}
name|lock_file_list
operator|=
name|lock_file_list
operator|->
name|next
expr_stmt|;
block|}
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
name|sigchain_pop
argument_list|(
name|signo
argument_list|)
expr_stmt|;
name|raise
argument_list|(
name|signo
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * p = absolute or relative path name  *  * Return a pointer into p showing the beginning of the last path name  * element.  If p is empty or the root directory ("/"), just return p.  */
end_comment
begin_function
DECL|function|last_path_elm
specifier|static
name|char
modifier|*
name|last_path_elm
parameter_list|(
name|char
modifier|*
name|p
parameter_list|)
block|{
comment|/* r starts pointing to null at the end of the string */
name|char
modifier|*
name|r
init|=
name|strchr
argument_list|(
name|p
argument_list|,
literal|'\0'
argument_list|)
decl_stmt|;
if|if
condition|(
name|r
operator|==
name|p
condition|)
return|return
name|p
return|;
comment|/* just return empty string */
name|r
operator|--
expr_stmt|;
comment|/* back up to last non-null character */
comment|/* back up past trailing slashes, if any */
while|while
condition|(
name|r
operator|>
name|p
operator|&&
operator|*
name|r
operator|==
literal|'/'
condition|)
name|r
operator|--
expr_stmt|;
comment|/* 	 * then go backwards until I hit a slash, or the beginning of 	 * the string 	 */
while|while
condition|(
name|r
operator|>
name|p
operator|&&
operator|*
operator|(
name|r
operator|-
literal|1
operator|)
operator|!=
literal|'/'
condition|)
name|r
operator|--
expr_stmt|;
return|return
name|r
return|;
block|}
end_function
begin_comment
comment|/* We allow "recursive" symbolic links. Only within reason, though */
end_comment
begin_define
DECL|macro|MAXDEPTH
define|#
directive|define
name|MAXDEPTH
value|5
end_define
begin_comment
comment|/*  * p = path that may be a symlink  * s = full size of p  *  * If p is a symlink, attempt to overwrite p with a path to the real  * file or directory (which may or may not exist), following a chain of  * symlinks if necessary.  Otherwise, leave p unmodified.  *  * This is a best-effort routine.  If an error occurs, p will either be  * left unmodified or will name a different symlink in a symlink chain  * that started with p's initial contents.  *  * Always returns p.  */
end_comment
begin_function
DECL|function|resolve_symlink
specifier|static
name|char
modifier|*
name|resolve_symlink
parameter_list|(
name|char
modifier|*
name|p
parameter_list|,
name|size_t
name|s
parameter_list|)
block|{
name|int
name|depth
init|=
name|MAXDEPTH
decl_stmt|;
while|while
condition|(
name|depth
operator|--
condition|)
block|{
name|char
name|link
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|int
name|link_len
init|=
name|readlink
argument_list|(
name|p
argument_list|,
name|link
argument_list|,
sizeof|sizeof
argument_list|(
name|link
argument_list|)
argument_list|)
decl_stmt|;
if|if
condition|(
name|link_len
operator|<
literal|0
condition|)
block|{
comment|/* not a symlink anymore */
return|return
name|p
return|;
block|}
elseif|else
if|if
condition|(
name|link_len
operator|<
sizeof|sizeof
argument_list|(
name|link
argument_list|)
condition|)
comment|/* readlink() never null-terminates */
name|link
index|[
name|link_len
index|]
operator|=
literal|'\0'
expr_stmt|;
else|else
block|{
name|warning
argument_list|(
literal|"%s: symlink too long"
argument_list|,
name|p
argument_list|)
expr_stmt|;
return|return
name|p
return|;
block|}
if|if
condition|(
name|is_absolute_path
argument_list|(
name|link
argument_list|)
condition|)
block|{
comment|/* absolute path simply replaces p */
if|if
condition|(
name|link_len
operator|<
name|s
condition|)
name|strcpy
argument_list|(
name|p
argument_list|,
name|link
argument_list|)
expr_stmt|;
else|else
block|{
name|warning
argument_list|(
literal|"%s: symlink too long"
argument_list|,
name|p
argument_list|)
expr_stmt|;
return|return
name|p
return|;
block|}
block|}
else|else
block|{
comment|/* 			 * link is a relative path, so I must replace the 			 * last element of p with it. 			 */
name|char
modifier|*
name|r
init|=
operator|(
name|char
operator|*
operator|)
name|last_path_elm
argument_list|(
name|p
argument_list|)
decl_stmt|;
if|if
condition|(
name|r
operator|-
name|p
operator|+
name|link_len
operator|<
name|s
condition|)
name|strcpy
argument_list|(
name|r
argument_list|,
name|link
argument_list|)
expr_stmt|;
else|else
block|{
name|warning
argument_list|(
literal|"%s: symlink too long"
argument_list|,
name|p
argument_list|)
expr_stmt|;
return|return
name|p
return|;
block|}
block|}
block|}
return|return
name|p
return|;
block|}
end_function
begin_comment
comment|/* Make sure errno contains a meaningful value on error */
end_comment
begin_function
DECL|function|lock_file
specifier|static
name|int
name|lock_file
parameter_list|(
name|struct
name|lock_file
modifier|*
name|lk
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|flags
parameter_list|)
block|{
comment|/* 	 * subtract LOCK_SUFFIX_LEN from size to make sure there's 	 * room for adding ".lock" for the lock file name: 	 */
specifier|static
specifier|const
name|size_t
name|max_path_len
init|=
sizeof|sizeof
argument_list|(
name|lk
operator|->
name|filename
argument_list|)
operator|-
name|LOCK_SUFFIX_LEN
decl_stmt|;
if|if
condition|(
operator|!
name|lock_file_list
condition|)
block|{
comment|/* One-time initialization */
name|sigchain_push_common
argument_list|(
name|remove_lock_file_on_signal
argument_list|)
expr_stmt|;
name|atexit
argument_list|(
name|remove_lock_file
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|lk
operator|->
name|on_list
condition|)
block|{
comment|/* Initialize *lk and add it to lock_file_list: */
name|lk
operator|->
name|fd
operator|=
operator|-
literal|1
expr_stmt|;
name|lk
operator|->
name|owner
operator|=
literal|0
expr_stmt|;
name|lk
operator|->
name|filename
index|[
literal|0
index|]
operator|=
literal|0
expr_stmt|;
name|lk
operator|->
name|next
operator|=
name|lock_file_list
expr_stmt|;
name|lock_file_list
operator|=
name|lk
expr_stmt|;
name|lk
operator|->
name|on_list
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|strlen
argument_list|(
name|path
argument_list|)
operator|>=
name|max_path_len
condition|)
block|{
name|errno
operator|=
name|ENAMETOOLONG
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|strcpy
argument_list|(
name|lk
operator|->
name|filename
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|flags
operator|&
name|LOCK_NODEREF
operator|)
condition|)
name|resolve_symlink
argument_list|(
name|lk
operator|->
name|filename
argument_list|,
name|max_path_len
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|lk
operator|->
name|filename
argument_list|,
name|LOCK_SUFFIX
argument_list|)
expr_stmt|;
name|lk
operator|->
name|fd
operator|=
name|open
argument_list|(
name|lk
operator|->
name|filename
argument_list|,
name|O_RDWR
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
name|lk
operator|->
name|fd
operator|<
literal|0
condition|)
block|{
name|lk
operator|->
name|filename
index|[
literal|0
index|]
operator|=
literal|0
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|lk
operator|->
name|owner
operator|=
name|getpid
argument_list|()
expr_stmt|;
if|if
condition|(
name|adjust_shared_perm
argument_list|(
name|lk
operator|->
name|filename
argument_list|)
condition|)
block|{
name|int
name|save_errno
init|=
name|errno
decl_stmt|;
name|error
argument_list|(
literal|"cannot fix permission bits on %s"
argument_list|,
name|lk
operator|->
name|filename
argument_list|)
expr_stmt|;
name|rollback_lock_file
argument_list|(
name|lk
argument_list|)
expr_stmt|;
name|errno
operator|=
name|save_errno
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
return|return
name|lk
operator|->
name|fd
return|;
block|}
end_function
begin_function
DECL|function|unable_to_lock_message
name|void
name|unable_to_lock_message
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|err
parameter_list|,
name|struct
name|strbuf
modifier|*
name|buf
parameter_list|)
block|{
if|if
condition|(
name|err
operator|==
name|EEXIST
condition|)
block|{
name|strbuf_addf
argument_list|(
name|buf
argument_list|,
literal|"Unable to create '%s.lock': %s.\n\n"
literal|"If no other git process is currently running, this probably means a\n"
literal|"git process crashed in this repository earlier. Make sure no other git\n"
literal|"process is running and remove the file manually to continue."
argument_list|,
name|absolute_path
argument_list|(
name|path
argument_list|)
argument_list|,
name|strerror
argument_list|(
name|err
argument_list|)
argument_list|)
expr_stmt|;
block|}
else|else
name|strbuf_addf
argument_list|(
name|buf
argument_list|,
literal|"Unable to create '%s.lock': %s"
argument_list|,
name|absolute_path
argument_list|(
name|path
argument_list|)
argument_list|,
name|strerror
argument_list|(
name|err
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|unable_to_lock_error
name|int
name|unable_to_lock_error
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|err
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|unable_to_lock_message
argument_list|(
name|path
argument_list|,
name|err
argument_list|,
operator|&
name|buf
argument_list|)
expr_stmt|;
name|error
argument_list|(
literal|"%s"
argument_list|,
name|buf
operator|.
name|buf
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|unable_to_lock_die
name|NORETURN
name|void
name|unable_to_lock_die
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|err
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|unable_to_lock_message
argument_list|(
name|path
argument_list|,
name|err
argument_list|,
operator|&
name|buf
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"%s"
argument_list|,
name|buf
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* This should return a meaningful errno on failure */
end_comment
begin_function
DECL|function|hold_lock_file_for_update
name|int
name|hold_lock_file_for_update
parameter_list|(
name|struct
name|lock_file
modifier|*
name|lk
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|flags
parameter_list|)
block|{
name|int
name|fd
init|=
name|lock_file
argument_list|(
name|lk
argument_list|,
name|path
argument_list|,
name|flags
argument_list|)
decl_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
operator|&&
operator|(
name|flags
operator|&
name|LOCK_DIE_ON_ERROR
operator|)
condition|)
name|unable_to_lock_die
argument_list|(
name|path
argument_list|,
name|errno
argument_list|)
expr_stmt|;
return|return
name|fd
return|;
block|}
end_function
begin_function
DECL|function|hold_lock_file_for_append
name|int
name|hold_lock_file_for_append
parameter_list|(
name|struct
name|lock_file
modifier|*
name|lk
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|flags
parameter_list|)
block|{
name|int
name|fd
decl_stmt|,
name|orig_fd
decl_stmt|;
name|fd
operator|=
name|lock_file
argument_list|(
name|lk
argument_list|,
name|path
argument_list|,
name|flags
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
name|flags
operator|&
name|LOCK_DIE_ON_ERROR
condition|)
name|unable_to_lock_die
argument_list|(
name|path
argument_list|,
name|errno
argument_list|)
expr_stmt|;
return|return
name|fd
return|;
block|}
name|orig_fd
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
name|orig_fd
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|ENOENT
condition|)
block|{
if|if
condition|(
name|flags
operator|&
name|LOCK_DIE_ON_ERROR
condition|)
name|die
argument_list|(
literal|"cannot open '%s' for copying"
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|rollback_lock_file
argument_list|(
name|lk
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"cannot open '%s' for copying"
argument_list|,
name|path
argument_list|)
return|;
block|}
block|}
elseif|else
if|if
condition|(
name|copy_fd
argument_list|(
name|orig_fd
argument_list|,
name|fd
argument_list|)
condition|)
block|{
if|if
condition|(
name|flags
operator|&
name|LOCK_DIE_ON_ERROR
condition|)
name|exit
argument_list|(
literal|128
argument_list|)
expr_stmt|;
name|rollback_lock_file
argument_list|(
name|lk
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
return|return
name|fd
return|;
block|}
end_function
begin_function
DECL|function|close_lock_file
name|int
name|close_lock_file
parameter_list|(
name|struct
name|lock_file
modifier|*
name|lk
parameter_list|)
block|{
name|int
name|fd
init|=
name|lk
operator|->
name|fd
decl_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
literal|0
return|;
name|lk
operator|->
name|fd
operator|=
operator|-
literal|1
expr_stmt|;
return|return
name|close
argument_list|(
name|fd
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|reopen_lock_file
name|int
name|reopen_lock_file
parameter_list|(
name|struct
name|lock_file
modifier|*
name|lk
parameter_list|)
block|{
if|if
condition|(
literal|0
operator|<=
name|lk
operator|->
name|fd
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"BUG: reopen a lockfile that is still open"
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|lk
operator|->
name|filename
index|[
literal|0
index|]
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"BUG: reopen a lockfile that has been committed"
argument_list|)
argument_list|)
expr_stmt|;
name|lk
operator|->
name|fd
operator|=
name|open
argument_list|(
name|lk
operator|->
name|filename
argument_list|,
name|O_WRONLY
argument_list|)
expr_stmt|;
return|return
name|lk
operator|->
name|fd
return|;
block|}
end_function
begin_function
DECL|function|commit_lock_file
name|int
name|commit_lock_file
parameter_list|(
name|struct
name|lock_file
modifier|*
name|lk
parameter_list|)
block|{
name|char
name|result_file
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|size_t
name|i
decl_stmt|;
if|if
condition|(
name|close_lock_file
argument_list|(
name|lk
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|strcpy
argument_list|(
name|result_file
argument_list|,
name|lk
operator|->
name|filename
argument_list|)
expr_stmt|;
name|i
operator|=
name|strlen
argument_list|(
name|result_file
argument_list|)
operator|-
name|LOCK_SUFFIX_LEN
expr_stmt|;
comment|/* .lock */
name|result_file
index|[
name|i
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|rename
argument_list|(
name|lk
operator|->
name|filename
argument_list|,
name|result_file
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|lk
operator|->
name|filename
index|[
literal|0
index|]
operator|=
literal|0
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|hold_locked_index
name|int
name|hold_locked_index
parameter_list|(
name|struct
name|lock_file
modifier|*
name|lk
parameter_list|,
name|int
name|die_on_error
parameter_list|)
block|{
return|return
name|hold_lock_file_for_update
argument_list|(
name|lk
argument_list|,
name|get_index_file
argument_list|()
argument_list|,
name|die_on_error
condition|?
name|LOCK_DIE_ON_ERROR
else|:
literal|0
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|rollback_lock_file
name|void
name|rollback_lock_file
parameter_list|(
name|struct
name|lock_file
modifier|*
name|lk
parameter_list|)
block|{
if|if
condition|(
operator|!
name|lk
operator|->
name|filename
index|[
literal|0
index|]
condition|)
return|return;
name|close_lock_file
argument_list|(
name|lk
argument_list|)
expr_stmt|;
name|unlink_or_warn
argument_list|(
name|lk
operator|->
name|filename
argument_list|)
expr_stmt|;
name|lk
operator|->
name|filename
index|[
literal|0
index|]
operator|=
literal|0
expr_stmt|;
block|}
end_function
end_unit
