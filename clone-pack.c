begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"pkt-line.h"
end_include
begin_decl_stmt
DECL|variable|clone_pack_usage
specifier|static
specifier|const
name|char
name|clone_pack_usage
index|[]
init|=
literal|"git-clone-pack [--exec=<git-upload-pack>] [<host>:]<directory> [<heads>]*"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|exec
specifier|static
specifier|const
name|char
modifier|*
name|exec
init|=
literal|"git-upload-pack"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|clone_handshake
specifier|static
name|void
name|clone_handshake
parameter_list|(
name|int
name|fd
index|[
literal|2
index|]
parameter_list|,
name|struct
name|ref
modifier|*
name|ref
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
while|while
condition|(
name|ref
condition|)
block|{
name|packet_write
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|,
literal|"want %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|)
argument_list|)
expr_stmt|;
name|ref
operator|=
name|ref
operator|->
name|next
expr_stmt|;
block|}
name|packet_flush
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
comment|/* We don't have nuttin' */
name|packet_write
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|,
literal|"done\n"
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_ack
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|,
name|sha1
argument_list|)
condition|)
name|error
argument_list|(
literal|"Huh! git-clone-pack got positive ack for %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|is_master
specifier|static
name|int
name|is_master
parameter_list|(
name|struct
name|ref
modifier|*
name|ref
parameter_list|)
block|{
return|return
operator|!
name|strcmp
argument_list|(
name|ref
operator|->
name|name
argument_list|,
literal|"refs/heads/master"
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|write_one_ref
specifier|static
name|void
name|write_one_ref
parameter_list|(
name|struct
name|ref
modifier|*
name|ref
parameter_list|)
block|{
name|char
modifier|*
name|path
init|=
name|git_path
argument_list|(
literal|"%s"
argument_list|,
name|ref
operator|->
name|name
argument_list|)
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|char
modifier|*
name|hex
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|ref
operator|->
name|name
argument_list|,
literal|"refs/"
argument_list|,
literal|5
argument_list|)
operator|&&
name|check_ref_format
argument_list|(
name|ref
operator|->
name|name
operator|+
literal|5
argument_list|)
condition|)
block|{
name|error
argument_list|(
literal|"refusing to create funny ref '%s' locally"
argument_list|,
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
return|return;
block|}
if|if
condition|(
name|safe_create_leading_directories
argument_list|(
name|path
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to create leading directory for %s"
argument_list|,
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|path
argument_list|,
name|O_CREAT
operator||
name|O_EXCL
operator||
name|O_WRONLY
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
name|die
argument_list|(
literal|"unable to create ref %s"
argument_list|,
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
name|hex
operator|=
name|sha1_to_hex
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|)
expr_stmt|;
name|hex
index|[
literal|40
index|]
operator|=
literal|'\n'
expr_stmt|;
if|if
condition|(
name|write
argument_list|(
name|fd
argument_list|,
name|hex
argument_list|,
literal|41
argument_list|)
operator|!=
literal|41
condition|)
name|die
argument_list|(
literal|"unable to write ref %s"
argument_list|,
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|write_refs
specifier|static
name|void
name|write_refs
parameter_list|(
name|struct
name|ref
modifier|*
name|ref
parameter_list|)
block|{
name|struct
name|ref
modifier|*
name|head
init|=
name|NULL
decl_stmt|,
modifier|*
name|head_ptr
decl_stmt|,
modifier|*
name|master_ref
decl_stmt|;
name|char
modifier|*
name|head_path
decl_stmt|;
comment|/* Upload-pack must report HEAD first */
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|ref
operator|->
name|name
argument_list|,
literal|"HEAD"
argument_list|)
condition|)
block|{
name|head
operator|=
name|ref
expr_stmt|;
name|ref
operator|=
name|ref
operator|->
name|next
expr_stmt|;
block|}
name|head_ptr
operator|=
name|NULL
expr_stmt|;
name|master_ref
operator|=
name|NULL
expr_stmt|;
while|while
condition|(
name|ref
condition|)
block|{
if|if
condition|(
name|is_master
argument_list|(
name|ref
argument_list|)
condition|)
name|master_ref
operator|=
name|ref
expr_stmt|;
if|if
condition|(
name|head
operator|&&
operator|!
name|memcmp
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|,
name|head
operator|->
name|old_sha1
argument_list|,
literal|20
argument_list|)
operator|&&
operator|!
name|strncmp
argument_list|(
name|ref
operator|->
name|name
argument_list|,
literal|"refs/heads/"
argument_list|,
literal|11
argument_list|)
operator|&&
operator|(
operator|!
name|head_ptr
operator|||
name|ref
operator|==
name|master_ref
operator|)
condition|)
name|head_ptr
operator|=
name|ref
expr_stmt|;
name|write_one_ref
argument_list|(
name|ref
argument_list|)
expr_stmt|;
name|ref
operator|=
name|ref
operator|->
name|next
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|head
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"No HEAD in remote.\n"
argument_list|)
expr_stmt|;
return|return;
block|}
name|head_path
operator|=
name|strdup
argument_list|(
name|git_path
argument_list|(
literal|"HEAD"
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|head_ptr
condition|)
block|{
comment|/* 		 * If we had a master ref, and it wasn't HEAD, we need to undo the 		 * symlink, and write a standalone HEAD. Give a warning, because that's 		 * really really wrong. 		 */
if|if
condition|(
name|master_ref
condition|)
block|{
name|error
argument_list|(
literal|"HEAD doesn't point to any refs! Making standalone HEAD"
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|head_path
argument_list|)
expr_stmt|;
block|}
name|write_one_ref
argument_list|(
name|head
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|head_path
argument_list|)
expr_stmt|;
return|return;
block|}
comment|/* We reset to the master branch if it's available */
if|if
condition|(
name|master_ref
condition|)
return|return;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Setting HEAD to %s\n"
argument_list|,
name|head_ptr
operator|->
name|name
argument_list|)
expr_stmt|;
comment|/* 	 * Uhhuh. Other end didn't have master. We start HEAD off with 	 * the first branch with the same value. 	 */
if|if
condition|(
name|create_symref
argument_list|(
name|head_path
argument_list|,
name|head_ptr
operator|->
name|name
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to link HEAD to %s"
argument_list|,
name|head_ptr
operator|->
name|name
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|head_path
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|clone_pack
specifier|static
name|int
name|clone_pack
parameter_list|(
name|int
name|fd
index|[
literal|2
index|]
parameter_list|,
name|int
name|nr_match
parameter_list|,
name|char
modifier|*
modifier|*
name|match
parameter_list|)
block|{
name|struct
name|ref
modifier|*
name|refs
decl_stmt|;
name|int
name|status
decl_stmt|;
name|get_remote_heads
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|,
operator|&
name|refs
argument_list|,
name|nr_match
argument_list|,
name|match
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|refs
condition|)
block|{
name|packet_flush
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"no matching remote head"
argument_list|)
expr_stmt|;
block|}
name|clone_handshake
argument_list|(
name|fd
argument_list|,
name|refs
argument_list|)
expr_stmt|;
name|status
operator|=
name|receive_keep_pack
argument_list|(
name|fd
argument_list|,
literal|"git-clone-pack"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|status
condition|)
block|{
if|if
condition|(
name|nr_match
operator|==
literal|0
condition|)
name|write_refs
argument_list|(
name|refs
argument_list|)
expr_stmt|;
else|else
while|while
condition|(
name|refs
condition|)
block|{
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|refs
operator|->
name|old_sha1
argument_list|)
argument_list|,
name|refs
operator|->
name|name
argument_list|)
expr_stmt|;
name|refs
operator|=
name|refs
operator|->
name|next
expr_stmt|;
block|}
block|}
return|return
name|status
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
name|ret
decl_stmt|,
name|nr_heads
decl_stmt|;
name|char
modifier|*
name|dest
init|=
name|NULL
decl_stmt|,
modifier|*
modifier|*
name|heads
decl_stmt|;
name|int
name|fd
index|[
literal|2
index|]
decl_stmt|;
name|pid_t
name|pid
decl_stmt|;
name|setup_git_directory
argument_list|()
expr_stmt|;
name|nr_heads
operator|=
literal|0
expr_stmt|;
name|heads
operator|=
name|NULL
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
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|*
name|arg
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"-q"
argument_list|,
name|arg
argument_list|)
condition|)
continue|continue;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
literal|"--exec="
argument_list|,
name|arg
argument_list|,
literal|7
argument_list|)
condition|)
block|{
name|exec
operator|=
name|arg
operator|+
literal|7
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|clone_pack_usage
argument_list|)
expr_stmt|;
block|}
name|dest
operator|=
name|arg
expr_stmt|;
name|heads
operator|=
name|argv
operator|+
name|i
operator|+
literal|1
expr_stmt|;
name|nr_heads
operator|=
name|argc
operator|-
name|i
operator|-
literal|1
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|dest
condition|)
name|usage
argument_list|(
name|clone_pack_usage
argument_list|)
expr_stmt|;
name|pid
operator|=
name|git_connect
argument_list|(
name|fd
argument_list|,
name|dest
argument_list|,
name|exec
argument_list|)
expr_stmt|;
if|if
condition|(
name|pid
operator|<
literal|0
condition|)
return|return
literal|1
return|;
name|ret
operator|=
name|clone_pack
argument_list|(
name|fd
argument_list|,
name|nr_heads
argument_list|,
name|heads
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|finish_connect
argument_list|(
name|pid
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
end_unit
