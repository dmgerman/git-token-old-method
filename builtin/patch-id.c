begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_function
DECL|function|flush_current_id
specifier|static
name|void
name|flush_current_id
parameter_list|(
name|int
name|patchlen
parameter_list|,
name|unsigned
name|char
modifier|*
name|id
parameter_list|,
name|git_SHA_CTX
modifier|*
name|c
parameter_list|)
block|{
name|unsigned
name|char
name|result
index|[
literal|20
index|]
decl_stmt|;
name|char
name|name
index|[
literal|50
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|patchlen
condition|)
return|return;
name|git_SHA1_Final
argument_list|(
name|result
argument_list|,
name|c
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|name
argument_list|,
name|sha1_to_hex
argument_list|(
name|id
argument_list|)
argument_list|,
literal|41
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|result
argument_list|)
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|git_SHA1_Init
argument_list|(
name|c
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|remove_space
specifier|static
name|int
name|remove_space
parameter_list|(
name|char
modifier|*
name|line
parameter_list|)
block|{
name|char
modifier|*
name|src
init|=
name|line
decl_stmt|;
name|char
modifier|*
name|dst
init|=
name|line
decl_stmt|;
name|unsigned
name|char
name|c
decl_stmt|;
while|while
condition|(
operator|(
name|c
operator|=
operator|*
name|src
operator|++
operator|)
operator|!=
literal|'\0'
condition|)
block|{
if|if
condition|(
operator|!
name|isspace
argument_list|(
name|c
argument_list|)
condition|)
operator|*
name|dst
operator|++
operator|=
name|c
expr_stmt|;
block|}
return|return
name|dst
operator|-
name|line
return|;
block|}
end_function
begin_function
DECL|function|scan_hunk_header
specifier|static
name|int
name|scan_hunk_header
parameter_list|(
specifier|const
name|char
modifier|*
name|p
parameter_list|,
name|int
modifier|*
name|p_before
parameter_list|,
name|int
modifier|*
name|p_after
parameter_list|)
block|{
specifier|static
specifier|const
name|char
name|digits
index|[]
init|=
literal|"0123456789"
decl_stmt|;
specifier|const
name|char
modifier|*
name|q
decl_stmt|,
modifier|*
name|r
decl_stmt|;
name|int
name|n
decl_stmt|;
name|q
operator|=
name|p
operator|+
literal|4
expr_stmt|;
name|n
operator|=
name|strspn
argument_list|(
name|q
argument_list|,
name|digits
argument_list|)
expr_stmt|;
if|if
condition|(
name|q
index|[
name|n
index|]
operator|==
literal|','
condition|)
block|{
name|q
operator|+=
name|n
operator|+
literal|1
expr_stmt|;
name|n
operator|=
name|strspn
argument_list|(
name|q
argument_list|,
name|digits
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|n
operator|==
literal|0
operator|||
name|q
index|[
name|n
index|]
operator|!=
literal|' '
operator|||
name|q
index|[
name|n
operator|+
literal|1
index|]
operator|!=
literal|'+'
condition|)
return|return
literal|0
return|;
name|r
operator|=
name|q
operator|+
name|n
operator|+
literal|2
expr_stmt|;
name|n
operator|=
name|strspn
argument_list|(
name|r
argument_list|,
name|digits
argument_list|)
expr_stmt|;
if|if
condition|(
name|r
index|[
name|n
index|]
operator|==
literal|','
condition|)
block|{
name|r
operator|+=
name|n
operator|+
literal|1
expr_stmt|;
name|n
operator|=
name|strspn
argument_list|(
name|r
argument_list|,
name|digits
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|n
operator|==
literal|0
condition|)
return|return
literal|0
return|;
operator|*
name|p_before
operator|=
name|atoi
argument_list|(
name|q
argument_list|)
expr_stmt|;
operator|*
name|p_after
operator|=
name|atoi
argument_list|(
name|r
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|get_one_patchid
name|int
name|get_one_patchid
parameter_list|(
name|unsigned
name|char
modifier|*
name|next_sha1
parameter_list|,
name|git_SHA_CTX
modifier|*
name|ctx
parameter_list|)
block|{
specifier|static
name|char
name|line
index|[
literal|1000
index|]
decl_stmt|;
name|int
name|patchlen
init|=
literal|0
decl_stmt|,
name|found_next
init|=
literal|0
decl_stmt|;
name|int
name|before
init|=
operator|-
literal|1
decl_stmt|,
name|after
init|=
operator|-
literal|1
decl_stmt|;
while|while
condition|(
name|fgets
argument_list|(
name|line
argument_list|,
sizeof|sizeof
argument_list|(
name|line
argument_list|)
argument_list|,
name|stdin
argument_list|)
operator|!=
name|NULL
condition|)
block|{
name|char
modifier|*
name|p
init|=
name|line
decl_stmt|;
name|int
name|len
decl_stmt|;
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"diff-tree "
argument_list|,
literal|10
argument_list|)
condition|)
name|p
operator|+=
literal|10
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"commit "
argument_list|,
literal|7
argument_list|)
condition|)
name|p
operator|+=
literal|7
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"From "
argument_list|,
literal|5
argument_list|)
condition|)
name|p
operator|+=
literal|5
expr_stmt|;
if|if
condition|(
operator|!
name|get_sha1_hex
argument_list|(
name|p
argument_list|,
name|next_sha1
argument_list|)
condition|)
block|{
name|found_next
operator|=
literal|1
expr_stmt|;
break|break;
block|}
comment|/* Ignore commit comments */
if|if
condition|(
operator|!
name|patchlen
operator|&&
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"diff "
argument_list|,
literal|5
argument_list|)
condition|)
continue|continue;
comment|/* Parsing diff header?  */
if|if
condition|(
name|before
operator|==
operator|-
literal|1
condition|)
block|{
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"index "
argument_list|,
literal|6
argument_list|)
condition|)
continue|continue;
elseif|else
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"--- "
argument_list|,
literal|4
argument_list|)
condition|)
name|before
operator|=
name|after
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|isalpha
argument_list|(
name|line
index|[
literal|0
index|]
argument_list|)
condition|)
break|break;
block|}
comment|/* Looking for a valid hunk header?  */
if|if
condition|(
name|before
operator|==
literal|0
operator|&&
name|after
operator|==
literal|0
condition|)
block|{
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"@@ -"
argument_list|,
literal|4
argument_list|)
condition|)
block|{
comment|/* Parse next hunk, but ignore line numbers.  */
name|scan_hunk_header
argument_list|(
name|line
argument_list|,
operator|&
name|before
argument_list|,
operator|&
name|after
argument_list|)
expr_stmt|;
continue|continue;
block|}
comment|/* Split at the end of the patch.  */
if|if
condition|(
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"diff "
argument_list|,
literal|5
argument_list|)
condition|)
break|break;
comment|/* Else we're parsing another header.  */
name|before
operator|=
name|after
operator|=
operator|-
literal|1
expr_stmt|;
block|}
comment|/* If we get here, we're inside a hunk.  */
if|if
condition|(
name|line
index|[
literal|0
index|]
operator|==
literal|'-'
operator|||
name|line
index|[
literal|0
index|]
operator|==
literal|' '
condition|)
name|before
operator|--
expr_stmt|;
if|if
condition|(
name|line
index|[
literal|0
index|]
operator|==
literal|'+'
operator|||
name|line
index|[
literal|0
index|]
operator|==
literal|' '
condition|)
name|after
operator|--
expr_stmt|;
comment|/* Compute the sha without whitespace */
name|len
operator|=
name|remove_space
argument_list|(
name|line
argument_list|)
expr_stmt|;
name|patchlen
operator|+=
name|len
expr_stmt|;
name|git_SHA1_Update
argument_list|(
name|ctx
argument_list|,
name|line
argument_list|,
name|len
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|found_next
condition|)
name|hashclr
argument_list|(
name|next_sha1
argument_list|)
expr_stmt|;
return|return
name|patchlen
return|;
block|}
end_function
begin_function
DECL|function|generate_id_list
specifier|static
name|void
name|generate_id_list
parameter_list|(
name|void
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|,
name|n
index|[
literal|20
index|]
decl_stmt|;
name|git_SHA_CTX
name|ctx
decl_stmt|;
name|int
name|patchlen
decl_stmt|;
name|git_SHA1_Init
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
name|hashclr
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
while|while
condition|(
operator|!
name|feof
argument_list|(
name|stdin
argument_list|)
condition|)
block|{
name|patchlen
operator|=
name|get_one_patchid
argument_list|(
name|n
argument_list|,
operator|&
name|ctx
argument_list|)
expr_stmt|;
name|flush_current_id
argument_list|(
name|patchlen
argument_list|,
name|sha1
argument_list|,
operator|&
name|ctx
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|sha1
argument_list|,
name|n
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_decl_stmt
DECL|variable|patch_id_usage
specifier|static
specifier|const
name|char
name|patch_id_usage
index|[]
init|=
literal|"git patch-id< patch"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_patch_id
name|int
name|cmd_patch_id
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
if|if
condition|(
name|argc
operator|!=
literal|1
condition|)
name|usage
argument_list|(
name|patch_id_usage
argument_list|)
expr_stmt|;
name|generate_id_list
argument_list|()
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
